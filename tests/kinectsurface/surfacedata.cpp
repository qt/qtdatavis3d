/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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
#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DCamera>
#include <QtDataVisualization/QBar3DSeries>
#include <QtDataVisualization/QScatter3DSeries>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/Q3DTheme>
#include <QScrollBar>
#include <QSize>
#include <QImage>

using namespace QtDataVisualization;

#define LOW_END_DEVICE // Uncomment for devices with limited processing/grpahics power

#ifdef LOW_END_DEVICE
const QSize lowRes = QSize(160, 120);
const QSize medRes = QSize(192, 144);
const QSize hiRes = QSize(256, 192);
const QSize maxRes = QSize(320, 240);
#else
const QSize lowRes = QSize(320, 240);
const QSize medRes = QSize(384, 288);
const QSize hiRes = QSize(512, 384);
const QSize maxRes = QSize(640, 480);
#endif

SurfaceData::SurfaceData(Q3DSurface *surface, Q3DScatter *scatter, Q3DBars *bars,
                         QTextEdit *statusArea) :
    m_surface(surface),
    m_scatter(scatter),
    m_bars(bars),
    m_statusArea(statusArea),
    m_resize(true),
    m_resolution(lowRes),
    m_resolutionLevel(0),
    m_mode(Surface)
{
    // Initialize surface
    m_surface->activeTheme()->setType(Q3DTheme::ThemeIsabelle);
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::black);
    gradient.setColorAt(0.33, Qt::blue);
    gradient.setColorAt(0.67, Qt::red);
    gradient.setColorAt(1.0, Qt::yellow);
    m_surface->setSelectionMode(QAbstract3DGraph::SelectionNone);
    m_surface->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    m_surface->activeTheme()->setGridEnabled(false);
    m_surface->activeTheme()->setBackgroundEnabled(false);
    m_surface->scene()->activeCamera()->setCameraPosition(0.0, 90.0, 150);
    m_surface->axisY()->setMax(255);
    QSurface3DSeries *series1 = new QSurface3DSeries(new QHeightMapSurfaceDataProxy());
    series1->setFlatShadingEnabled(true);
    series1->setDrawMode(QSurface3DSeries::DrawSurface);
    series1->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    series1->setBaseGradient(gradient);
    m_surface->addSeries(series1);

    // Initialize scatter
    m_scatter->activeTheme()->setType(Q3DTheme::ThemeStoneMoss);
    m_scatter->setSelectionMode(QAbstract3DGraph::SelectionNone);
    m_scatter->activeTheme()->setGridEnabled(false);
    m_scatter->setShadowQuality(QAbstract3DGraph::ShadowQualityLow);
    m_scatter->scene()->activeCamera()->setCameraPosition(0.0, 85.0, 150);
    QScatter3DSeries *series2 = new QScatter3DSeries;
#ifdef LOW_END_DEVICE
    series2->setMesh(QAbstract3DSeries::MeshPoint);
#else
    m_scatter->activeTheme()->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    series2->setMesh(QAbstract3DSeries::MeshMinimal);
#endif
    m_scatter->addSeries(series2);
    m_scatter->axisY()->setMin(-128);
    m_scatter->axisY()->setMax(128);
    m_scatter->axisX()->setMin(-m_resolution.width() / 2);
    m_scatter->axisX()->setMax(m_resolution.width() / 2);
    m_scatter->axisZ()->setMin(-m_resolution.height() / 2);
    m_scatter->axisZ()->setMax(m_resolution.height() / 2);

    // Initialize bars
    m_bars->activeTheme()->setType(Q3DTheme::ThemeArmyBlue);
    QLinearGradient bargradient;
    bargradient.setColorAt(0.48, Qt::white);
    bargradient.setColorAt(0.481, Qt::red);
    bargradient.setColorAt(0.52, Qt::red);
    bargradient.setColorAt(0.521, Qt::black);
    QList<QLinearGradient> bargradients;
    bargradients.append(bargradient);
    m_bars->activeTheme()->setBaseGradients(bargradients);
    m_bars->activeTheme()->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    m_bars->setSelectionMode(QAbstract3DGraph::SelectionNone);
    m_bars->activeTheme()->setGridEnabled(false);
    m_bars->activeTheme()->setBackgroundEnabled(false);
#ifdef LOW_END_DEVICE
    m_bars->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
#else
    m_bars->setShadowQuality(QAbstract3DGraph::ShadowQualityLow);
#endif
    m_bars->setBarSpacing(QSizeF(0.0, 0.0));
    m_bars->scene()->activeCamera()->setCameraPosition(0.0, 65.0, 130);
    QBar3DSeries *series3 = new QBar3DSeries;
    series3->setMesh(QAbstract3DSeries::MeshBar);
    m_bars->addSeries(series3);
    m_bars->valueAxis()->setMax(255);

    // Hide scroll bar
    m_statusArea->verticalScrollBar()->setVisible(false);

    // Connect Kinect signals
    connect(&m_kinect, &QKinect::QKinectWrapper::dataNotification, this, &SurfaceData::updateData);
    connect(&m_kinect, &QKinect::QKinectWrapper::statusNotification, this,
            &SurfaceData::updateStatus);
}

SurfaceData::~SurfaceData()
{
    delete m_bars;
    delete m_surface;
    delete m_scatter;
}

void SurfaceData::updateData()
{
    QImage depthMap = m_kinect.getDepth();
    if (m_resize) // Resize for better performance
        depthMap = depthMap.scaled(m_resolution);
    if (m_mode != Surface) {
        setData(depthMap);
    } else {
        static_cast<QHeightMapSurfaceDataProxy *>(m_surface->seriesList().at(0)->dataProxy())->setHeightMap(
                depthMap);
    }
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
    m_kinect.setMaxDist((unsigned int)(distance * 100));
}

void SurfaceData::setResolution(int selection)
{
    m_resolutionLevel = selection;
    switch (selection) {
    case 0: {
        m_resize = true;
        m_resolution = lowRes;
        break;
    }
    case 1: {
        m_resize = true;
        m_resolution = medRes;
        break;
    }
    case 2: {
        m_resize = true;
        m_resolution = hiRes;
        break;
    }
    case 3: {
        m_resize = false;
        m_resolution = maxRes;
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
    } else if (m_mode == MeshBars) {
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
    m_surface->activeTheme()->setType(Q3DTheme::ThemeIsabelle);
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::black);
    gradient.setColorAt(0.33, Qt::blue);
    gradient.setColorAt(0.67, Qt::red);
    gradient.setColorAt(1.0, Qt::yellow);
    m_surface->seriesList().at(0)->setBaseGradient(gradient);
    m_surface->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    m_statusArea->append(QStringLiteral("<b>Colors:</b> Thermal image imitation"));
}

void SurfaceData::useGradientTwo()
{
    m_surface->activeTheme()->setType(Q3DTheme::ThemeQt);
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(0.8, Qt::red);
    gradient.setColorAt(1.0, Qt::green);
    m_surface->seriesList().at(0)->setBaseGradient(gradient);
    m_surface->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
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
        m_scatter->seriesList().at(0)->dataProxy()->resetArray(dataArray);
    } else {
        QBarDataArray *dataArray = m_barDataArray;
        for (int i = 0; i < imageHeight; i++, bitCount -= widthBits) {
            QBarDataRow &newRow = *dataArray->at(i);
            for (int j = 0; j < imageWidth; j++)
                newRow[j] = qreal(bits[bitCount + (j * 4)]);
        }

        m_bars->seriesList().at(0)->dataProxy()->resetArray(dataArray);
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
