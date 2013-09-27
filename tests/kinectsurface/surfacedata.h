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

//#define USE_SCATTER
//#define USE_BARS

#include "QKinectWrapper.h"
#if defined(USE_SCATTER)
#include <QtDataVisualization/Q3DScatter>
#elif defined(USE_BARS)
#include <QtDataVisualization/Q3DBars>
#else
#include <QtDataVisualization/Q3DSurface>
#endif
#include <QTextEdit>

using namespace QtDataVisualization;

class SurfaceData : public QObject
{
    Q_OBJECT

public:
#if defined(USE_SCATTER)
    explicit SurfaceData(Q3DScatter *surface, QTextEdit *statusLabel);
#elif defined(USE_BARS)
    explicit SurfaceData(Q3DBars *surface, QTextEdit *statusLabel);
#else
    explicit SurfaceData(Q3DSurface *surface, QTextEdit *statusLabel);
#endif
    ~SurfaceData();

    void start();
    void stop();

    void updateData();
    void updateStatus(QKinect::KinectStatus status);

    void setDistance(int distance);
    void scrollDown();
#if defined(USE_SCATTER) || defined(USE_BARS)
    void setData(const QImage &image);
#else
    void useGradientOne();
    void useGradientTwo();
#endif

public slots:
    void setResolution(int selection);

private:
#if defined(USE_SCATTER)
    Q3DScatter *m_surface;
#elif defined(USE_BARS)
    Q3DBars *m_surface;
#else
    Q3DSurface *m_surface;
#endif
    QTextEdit *m_statusArea;
    bool m_resize;
    QSize m_resolution;
    QKinect::QKinectWrapper m_kinect;
};

#endif
