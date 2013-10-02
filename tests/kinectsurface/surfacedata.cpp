/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
**
** Licensees holding valid Qt Enterprise licenses may use this file in
** accordance with the Qt Enterprise License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
**
****************************************************************************/

#define NOMINMAX

#include "surfacedata.h"
#include "QKinectWrapper.h"
#include <QtDataVisualization/Q3DValueAxis>
#include <QScrollBar>
#include <QSize>
#include <QImage>

QT_DATAVISUALIZATION_USE_NAMESPACE

SurfaceData::SurfaceData(Q3DSurface *surface, Q3DScatter *scatter, Q3DBars *bars,
                         QTextEdit *statusArea) :
    m_surface(surface),
    m_scatter(scatter),
    m_bars(bars),
    m_statusArea(statusArea),
    m_resize(true),
    m_resolution(QSize(320, 240)),
    m_resolutionLevel(0),
    m_mode(Surface)
{
    // Initialize surface
    m_surface->setTheme(QDataVis::ThemeIsabelle);
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::black);
    gradient.setColorAt(0.33, Qt::blue);
    gradient.setColorAt(0.67, Qt::red);
    gradient.setColorAt(1.0, Qt::yellow);
    m_surface->setSelectionMode(QDataVis::SelectionModeNone);
    m_surface->setGridVisible(false);
    m_surface->setGradient(gradient);
    m_surface->axisY()->setMax(255);
    m_surface->setSurfaceGridEnabled(false);
    m_surface->setBackgroundVisible(false);
    m_surface->setSmoothSurfaceEnabled(false);
    m_surface->setActiveDataProxy(new QHeightMapSurfaceDataProxy());
    m_surface->setCameraPosition(0.0, 90.0, 105);

    // Initialize scatter
    m_scatter->setTheme(QDataVis::ThemeStoneMoss);
    m_scatter->setSelectionMode(QDataVis::SelectionModeNone);
    m_scatter->setGridVisible(false);
    m_scatter->setObjectType(QDataVis::MeshStyleDots, false);
    m_scatter->setShadowQuality(QDataVis::ShadowQualitySoftLow);
    m_scatter->setCameraPosition(0.0, 85.0, 110);
    m_scatter->axisY()->setMin(-128);
    m_scatter->axisY()->setMax(128);
    m_scatter->axisX()->setMin(-m_resolution.width() / 2);
    m_scatter->axisX()->setMax(m_resolution.width() / 2);
    m_scatter->axisZ()->setMin(-m_resolution.height() / 2);
    m_scatter->axisZ()->setMax(m_resolution.height() / 2);

    // Initialize bars
    m_bars->setTheme(QDataVis::ThemeQt);
    m_bars->setSelectionMode(QDataVis::SelectionModeNone);
    m_bars->setGridVisible(false);
    m_bars->setBarType(QDataVis::MeshStyleBars, true);
#if 1
    m_bars->setShadowQuality(QDataVis::ShadowQualityLow);
#else
    m_bars->setBarSpacing(QSizeF(0.0, 0.0));
#endif
    m_bars->setCameraPosition(0.0, 75.0);
    m_bars->valueAxis()->setMax(255);

    // Hide scroll bar
    m_statusArea->verticalScrollBar()->setVisible(false);

    // Connect Kinect signals
    connect(&m_kinect, &QKinect::QKinectWrapper::dataNotification, this, &SurfaceData::updateData);
    connect(&m_kinect, &QKinect::QKinectWrapper::statusNotification, this,
            &SurfaceData::updateStatus);
}

SurfaceData::~SurfaceData()
{   // HACK: The current context needs to be destroyed last
    // TODO: Fix properly in datavis code somehow
    if (m_mode == Surface) {
        delete m_scatter;
        delete m_bars;
        delete m_surface;
    } else if (m_mode == MeshStyleBars) {
        delete m_scatter;
        delete m_surface;
        delete m_bars;
    } else {
        delete m_bars;
        delete m_surface;
        delete m_scatter;
    }
}

void SurfaceData::updateData()
{
    QImage depthMap = m_kinect.getDepth();
    if (m_resize) // Resize for better performance
        depthMap = depthMap.scaled(m_resolution);
    if (m_mode != Surface)
        setData(depthMap);
    else
        static_cast<QHeightMapSurfaceDataProxy *>(m_surface->activeDataProxy())->setHeightMap(
                depthMap);
}

void SurfaceData::updateStatus(QKinect::KinectStatus status)
{
    switch (status) {
    case QKinect::Idle: {
        m_statusArea->append(QStringLiteral("<b>Kinect:</b> Stopped"));
        m_statusArea->append(QStringLiteral("<br><b>Ready</b><br>"));
        break;
    }
    case QKinect::Initializing: {
        m_statusArea->append(QStringLiteral("<b>Kinect:</b> Initializing"));
        break;
    }
    case QKinect::OkRun: {
        m_statusArea->append(QStringLiteral("<b>Kinect:</b> Running"));
        m_statusArea->append(QString(QStringLiteral("<i> - resolution: %1 x %2</i>")).arg(
                                 m_resolution.width()).arg(m_resolution.height()));
        break;
    }
    default: {
        m_statusArea->append(QStringLiteral("<b>Kinect:</b> Error"));
        break;
    }
    };
}

void SurfaceData::start()
{
    if (m_kinect.isStopped())
        m_kinect.start();
    else
        m_statusArea->append(QStringLiteral("<b>Kinect:</b> Already running"));
}

void SurfaceData::stop()
{
    if (m_kinect.isRunning())
        m_kinect.stop();
    else
        m_statusArea->append(QStringLiteral("<b>Kinect:</b> Already stopped"));
}

void SurfaceData::setDistance(int distance)
{
    m_kinect.setMaxDist(unsigned int(distance * 100));
}

void SurfaceData::setResolution(int selection)
{
    m_resolutionLevel = selection;
    switch (selection) {
    case 0: {
        m_resize = true;
        m_resolution = QSize(320, 240);
        break;
    }
    case 1: {
        m_resize = true;
        m_resolution = QSize(384, 288);
        break;
    }
    case 2: {
        m_resize = true;
        m_resolution = QSize(512, 384);
        break;
    }
    case 3: {
        m_resize = false;
        m_resolution = QSize(640, 480);
        break;
    }
    };
    if (m_mode == Scatter) {
        m_resize = true;
        m_resolution /= 4;
        m_scatter->axisX()->setMin(-m_resolution.width() / 2);
        m_scatter->axisX()->setMax(m_resolution.width() / 2);
        m_scatter->axisZ()->setMin(-m_resolution.height() / 2);
        m_scatter->axisZ()->setMax(m_resolution.height() / 2);
        m_scatterDataArray = new QScatterDataArray;
        m_scatterDataArray->resize(m_resolution.width() * m_resolution.height());
    } else if (m_mode == MeshStyleBars) {
        m_resize = true;
        m_resolution /= 4;
        m_barDataArray = new QBarDataArray;
        m_barDataArray->reserve(m_resolution.height());
        for (int i = 0; i < m_resolution.height(); i++) {
            QBarDataRow *newProxyRow = new QBarDataRow(m_resolution.width());
            m_barDataArray->append(newProxyRow);
        }
    }

    m_statusArea->append(QString(QStringLiteral("<b>Resolution:</b> %1 x %2")).arg(
                             m_resolution.width()).arg(m_resolution.height()));
    if (m_kinect.isStopped())
        m_statusArea->append(QStringLiteral("<i> - change takes effect once Kinect is running</i>"));
}

void SurfaceData::scrollDown()
{
    QScrollBar *scrollbar = m_statusArea->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void SurfaceData::useGradientOne()
{
    m_surface->setTheme(QDataVis::ThemeIsabelle);
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::black);
    gradient.setColorAt(0.33, Qt::blue);
    gradient.setColorAt(0.67, Qt::red);
    gradient.setColorAt(1.0, Qt::yellow);
    m_surface->setGradient(gradient);
    m_statusArea->append(QStringLiteral("<b>Colors:</b> Thermal image imitation"));
}

void SurfaceData::useGradientTwo()
{
    m_surface->setTheme(QDataVis::ThemeQt);
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(0.8, Qt::red);
    gradient.setColorAt(1.0, Qt::green);
    m_surface->setGradient(gradient);
    m_statusArea->append(QStringLiteral("<b>Colors:</b> Highlight foreground"));
}

void SurfaceData::setData(const QImage &image)
{
    QImage heightImage = image;

    uchar *bits = heightImage.bits();

    int imageHeight = heightImage.height();
    int imageWidth = heightImage.width();
    int bitCount = imageWidth * 4 * (imageHeight - 1);
    int widthBits = imageWidth * 4;

    if (m_mode == Scatter) {
        QScatterDataItem *ptrToDataArray = &m_scatterDataArray->first();

        int limitsX = imageWidth / 2;
        int limitsZ = imageHeight / 2;
        qreal height = 0;
        int count = 0;

        for (int i = -limitsZ; i < limitsZ; i++, bitCount -= widthBits) {
            for (int j = -limitsX; j < limitsX; j++) {
                height = qreal(bits[bitCount + ((j + limitsX) * 4)]) - 128.0;
                if (height > -128) {
                    ptrToDataArray->setPosition(QVector3D(qreal(j), height, qreal(i)));
                    ptrToDataArray++;
                    count++;
                }
            }
        }

        QScatterDataArray *dataArray = new QScatterDataArray(m_scatterDataArray->mid(0, count));
        static_cast<QScatterDataProxy *>(m_scatter->activeDataProxy())->resetArray(dataArray);
    } else {
        QBarDataArray *dataArray = m_barDataArray;
        for (int i = 0; i < imageHeight; i++, bitCount -= widthBits) {
            QBarDataRow &newRow = *dataArray->at(i);
            for (int j = 0; j < imageWidth; j++)
                newRow[j] = qreal(bits[bitCount + (j * 4)]);
        }

        static_cast<QBarDataProxy *>(m_bars->activeDataProxy())->resetArray(dataArray);
    }
}

void SurfaceData::changeMode(int mode)
{
    m_mode = VisualizationMode(mode);
    switch (m_mode) {
    case Surface: {
        m_statusArea->append(QStringLiteral("<b>Visualization Type:</b> Surface"));
        break;
    }
    case Scatter: {
        m_statusArea->append(QStringLiteral("<b>Visualization Type:</b> Scatter"));
        break;
    }
    default: {
        m_statusArea->append(QStringLiteral("<b>Visualization Type:</b> Bars"));
        break;
    }
    }
    // Reset resolution after mode change
    setResolution(m_resolutionLevel);
}

ContainerChanger::ContainerChanger(QWidget *surface, QWidget *scatter, QWidget *bars,
                                   QWidget *buttonOne, QWidget *buttonTwo)
    : m_surface(surface),
      m_scatter(scatter),
      m_bars(bars),
      m_button1(buttonOne),
      m_button2(buttonTwo)
{
}

ContainerChanger::~ContainerChanger()
{
}

void ContainerChanger::changeContainer(int container)
{
    switch (container) {
    case 0: {
        m_scatter->setVisible(false);
        m_bars->setVisible(false);
        m_surface->setVisible(true);
        m_button1->setEnabled(true);
        m_button2->setEnabled(true);
        break;
    }
    case 1: {
        m_surface->setVisible(false);
        m_bars->setVisible(false);
        m_scatter->setVisible(true);
        m_button1->setEnabled(false);
        m_button2->setEnabled(false);
        break;
    }
    case 2: {
        m_surface->setVisible(false);
        m_scatter->setVisible(false);
        m_bars->setVisible(true);
        m_button1->setEnabled(false);
        m_button2->setEnabled(false);
        break;
    }
    }
}
