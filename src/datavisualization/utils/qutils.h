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

    // We support only ES2 emulation with software renderer for now
    if (QCoreApplication::testAttribute(Qt::AA_UseSoftwareOpenGL)) {
        qWarning("Only OpenGL ES2 emulation is available for software rendering.");
        isES = true;
    }

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
