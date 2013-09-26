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
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/Q3DValueAxis>
#include <QScrollBar>
#include <QSize>

#include <QDebug>

QT_DATAVISUALIZATION_USE_NAMESPACE

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
    m_surface->setGridVisible(false);
    m_surface->setSmoothSurfaceEnabled(false);
    m_surface->setActiveDataProxy(new QHeightMapSurfaceDataProxy());

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
    static_cast<QHeightMapSurfaceDataProxy *>(m_surface->activeDataProxy())->setHeightMap(depthMap);
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
