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

#ifndef SURFACEDATA_H
#define SURFACEDATA_H

#include "QKinectWrapper.h"
#include <QtDataVisualization/q3dsurface.h>
#include <QLabel>

using namespace QtDataVisualization;

class SurfaceData : public QObject
{
    Q_OBJECT

public:
    explicit SurfaceData(Q3DSurface *surface, QLabel *statusLabel);
    ~SurfaceData();

    void start();
    void stop();

    void updateData();
    void updateStatus(QKinect::KinectStatus status);

    void setDistance(int distance);

public slots:
    void setResolution(int selection);

private:
    Q3DSurface *m_surface;
    QLabel *m_statusLabel;
    bool m_resize;
    QSize m_resolution;
    QKinect::QKinectWrapper m_kinect;
};

#endif
