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

#ifndef QUTILS_H
#define QUTILS_H

#include <QtGui/QSurfaceFormat>

namespace QtDataVisualization {

inline static QSurfaceFormat qDefaultSurfaceFormat(bool antialias = true)
{
    QSurfaceFormat surfaceFormat;

    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setStencilBufferSize(8);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
#if !defined(QT_OPENGL_ES_2)
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
#else
    // Antialias not supported for ES
    antialias = false;
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGLES);
    surfaceFormat.setRedBufferSize(8);
    surfaceFormat.setBlueBufferSize(8);
    surfaceFormat.setGreenBufferSize(8);
#endif

    if (antialias)
        surfaceFormat.setSamples(8);
    else
        surfaceFormat.setSamples(0);

    return surfaceFormat;
}

}

#endif
