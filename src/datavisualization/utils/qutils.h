/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef QUTILS_H
#define QUTILS_H

#include <QtGui/QSurfaceFormat>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOffscreenSurface>
#include <QtCore/QCoreApplication>

namespace QtDataVisualization {

inline static QSurfaceFormat qDefaultSurfaceFormat(bool antialias = true)
{
    bool isES = false;

    QSurfaceFormat surfaceFormat;

    // Common attributes
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setStencilBufferSize(8);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    surfaceFormat.setRenderableType(QSurfaceFormat::DefaultRenderableType);

    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    QOffscreenSurface *dummySurface = 0;
    if (!ctx) {
        dummySurface = new QOffscreenSurface();
        dummySurface->setFormat(surfaceFormat);
        dummySurface->create();
        ctx = new QOpenGLContext;
        ctx->setFormat(surfaceFormat);
        ctx->create();
        ctx->makeCurrent(dummySurface);
    }

#if defined(QT_OPENGL_ES_2)
    isES = true;
#elif (QT_VERSION < QT_VERSION_CHECK(5, 3, 0))
    isES = false;
#else
    isES = ctx->isOpenGLES();
#endif

    if (dummySurface) {
        ctx->doneCurrent();
        delete ctx;
        delete dummySurface;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    // We support only ES2 emulation with software renderer for now
    if (QCoreApplication::testAttribute(Qt::AA_UseSoftwareOpenGL)) {
        qWarning("Only OpenGL ES2 emulation is available for software rendering.");
        isES = true;
    }
#endif

    if (isES) {
        // For ES2 only attributes
        surfaceFormat.setRedBufferSize(8);
        surfaceFormat.setBlueBufferSize(8);
        surfaceFormat.setGreenBufferSize(8);
    } else {
        // For OpenGL only attributes
        if (antialias)
            surfaceFormat.setSamples(8);
        else
            surfaceFormat.setSamples(0);
    }

    return surfaceFormat;
}

}

#endif
