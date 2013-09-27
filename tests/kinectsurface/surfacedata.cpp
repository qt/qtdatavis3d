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
#if defined(USE_SCATTER)
#include <QtDataVisualization/QScatterDataProxy>
#elif defined(USE_BARS)
#include <QtDataVisualization/QBarDataProxy>
#else
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#endif
#include <QtDataVisualization/Q3DValueAxis>
#include <QScrollBar>
#include <QSize>
#include <QImage>

#include <QDebug>

QT_DATAVISUALIZATION_USE_NAMESPACE

#if defined(USE_SCATTER)
SurfaceData::SurfaceData(Q3DScatter *surface, QTextEdit *statusArea) :
    m_surface(surface),
    m_statusArea(statusArea),
    m_resize(true),
    m_resolution(QSize(80, 60))
{
    // Initialize scatter
    m_surface->setTheme(QDataVis::ThemeStoneMoss);
    m_surface->setObjectType(QDataVis::Dots, false);
    m_surface->setShadowQuality(QDataVis::ShadowSoftLow);
    m_surface->setCameraPosition(0.0, 85.0, 110);
    m_surface->axisY()->setMax(255);
    m_surface->axisX()->setMin(-m_resolution.width() / 2);
    m_surface->axisX()->setMax(m_resolution.width() / 2);
    m_surface->axisZ()->setMin(-m_resolution.height() / 2);
    m_surface->axisZ()->setMax(m_resolution.height() / 2);
#elif defined(USE_BARS)
SurfaceData::SurfaceData(Q3DBars *surface, QTextEdit *statusArea) :
    m_surface(surface),
    m_statusArea(statusArea),
    m_resize(true),
    m_resolution(QSize(80, 60))
{
    // Initialize scatter
    m_surface->setTheme(QDataVis::ThemeQt);
    m_surface->setBarType(QDataVis::Bars, true);
#if 1
    m_surface->setShadowQuality(QDataVis::ShadowLow);
#else
    m_surface->setBarSpacing(QSizeF(0.0, 0.0));
#endif
    m_surface->setCameraPosition(0.0, 75.0);
    m_surface->valueAxis()->setMax(255);
#else
SurfaceData::SurfaceData(Q3DSurface *surface, QTextEdit *statusArea) :
    m_surface(surface),
    m_statusArea(statusArea),
    m_resize(true),
    m_resolution(QSize(320, 240))
{
    // Initialize surface
    m_surface->setTheme(QDataVis::ThemeIsabelle);
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::black);
    gradient.setColorAt(0.33, Qt::blue);
    gradient.setColorAt(0.67, Qt::red);
    gradient.setColorAt(1.0, Qt::yellow);
    m_surface->setGradient(gradient);
    m_surface->axisY()->setMax(255);
    m_surface->setSurfaceGridEnabled(false);
    m_surface->setBackgroundVisible(false);
    m_surface->setSmoothSurfaceEnabled(false);
    m_surface->setActiveDataProxy(new QHeightMapSurfaceDataProxy());
#endif

    // Common initializers
    m_surface->setSelectionMode(QDataVis::ModeNone);
    m_surface->setGridVisible(false);

    // Hide scroll bar
    m_statusArea->verticalScrollBar()->setVisible(false);

    // Connect Kinect signals
    connect(&m_kinect, &QKinect::QKinectWrapper::dataNotification, this, &SurfaceData::updateData);
    connect(&m_kinect, &QKinect::QKinectWrapper::statusNotification, this,
            &SurfaceData::updateStatus);
}

SurfaceData::~SurfaceData()
{
    delete m_surface;
}

void SurfaceData::updateData()
{
    QImage depthMap = m_kinect.getDepth();
    if (m_resize) // Resize for better performance
        depthMap = depthMap.scaled(m_resolution);
#if defined(USE_SCATTER) || defined(USE_BARS)
    setData(depthMap);
#else
    static_cast<QHeightMapSurfaceDataProxy *>(m_surface->activeDataProxy())->setHeightMap(depthMap);
#endif
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
#if defined(USE_SCATTER)
    m_resize = true;
    m_resolution /= 4;
    m_surface->axisX()->setMin(-m_resolution.width() / 2);
    m_surface->axisX()->setMax(m_resolution.width() / 2);
    m_surface->axisZ()->setMin(-m_resolution.height() / 2);
    m_surface->axisZ()->setMax(m_resolution.height() / 2);
#elif defined(USE_BARS)
    m_resize = true;
    m_resolution /= 4;
#endif

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

#if !defined(USE_SCATTER) && !defined(USE_BARS)
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
#else
void SurfaceData::setData(const QImage &image)
{
    QImage heightImage = image;

    uchar *bits = heightImage.bits();

    int imageHeight = heightImage.height();
    int imageWidth = heightImage.width();
    int bitCount = imageWidth * 4 * (imageHeight - 1);
    int widthBits = imageWidth * 4;

#if defined(USE_SCATTER)
    QScatterDataArray *dataArray = new QScatterDataArray;
    dataArray->resize(imageHeight * imageWidth);
    QScatterDataItem *ptrToDataArray = &dataArray->first();

    int limitsX = imageWidth / 2;
    int limitsZ = imageHeight / 2;
    qreal height = 0;

    for (int i = -limitsZ; i < limitsZ; i++, bitCount -= widthBits) {
        for (int j = -limitsX; j < limitsX; j++) {
            height = qreal(bits[bitCount + ((j + limitsX) * 4)]);
            if (height > 0) {
                ptrToDataArray->setPosition(QVector3D(qreal(j), height, qreal(i)));
                ptrToDataArray++;
            }
        }
    }

    static_cast<QScatterDataProxy *>(m_surface->activeDataProxy())->resetArray(dataArray);
#elif defined(USE_BARS)
    QBarDataArray *dataArray = new QBarDataArray;
    dataArray->reserve(imageHeight);
    for (int i = imageHeight; i > 0; i--, bitCount -= widthBits) {
        QBarDataRow *newRow = new QBarDataRow(imageWidth);
        for (int j = 0; j < imageWidth; j++)
            (*newRow)[j] = qreal(bits[bitCount + (j * 4)]);
        *dataArray << newRow;
    }

    static_cast<QBarDataProxy *>(m_surface->activeDataProxy())->resetArray(dataArray);
#endif
}

#endif
