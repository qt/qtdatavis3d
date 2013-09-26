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
#include <QHeightMapSurfaceDataProxy>
#include <Q3DValueAxis>

#include <QDebug>

QT_DATAVISUALIZATION_USE_NAMESPACE

SurfaceData::SurfaceData(Q3DSurface *surface, QLabel *statusLabel) :
    m_surface(surface),
    m_statusLabel(statusLabel),
    m_resize(true),
    m_resolution(QSize(320, 240))
{
    // Initialize surface
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
        m_statusLabel->setText(QStringLiteral("Stopped"));
        break;
    }
    case QKinect::Initializing: {
        m_statusLabel->setText(QStringLiteral("Initializing"));
        break;
    }
    case QKinect::OkRun: {
        m_statusLabel->setText(QStringLiteral("Ok"));
        break;
    }
    default: {
        m_statusLabel->setText(QStringLiteral("Error"));
        break;
    }
    };
}

void SurfaceData::start()
{
    if (m_kinect.isStopped()) {
        m_kinect.start();
#ifdef USE_TIMER_UPDATE
        m_updateTimer.start();
#endif
    }
}

void SurfaceData::stop()
{
    if (m_kinect.isRunning()) {
        m_kinect.stop();
#ifdef USE_TIMER_UPDATE
        m_updateTimer.stop();
#endif
    }
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
}
