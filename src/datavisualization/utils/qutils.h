/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QUTILS_H
#define QUTILS_H

#include <QtGui/QSurfaceFormat>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOffscreenSurface>
#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

#ifdef Q_OS_MACOS
#pragma clang diagnostic ignored "-Wunused-function"
#endif
static inline QSurfaceFormat qDefaultSurfaceFormat(bool antialias)
{
    bool isES = false;

    QSurfaceFormat surfaceFormat;

    // Common attributes
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setStencilBufferSize(8);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    surfaceFormat.setRenderableType(QSurfaceFormat::DefaultRenderableType);

    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    QOffscreenSurface *dummySurface = nullptr;
    if (!ctx) {
        dummySurface = new QOffscreenSurface();
        dummySurface->setFormat(surfaceFormat);
        dummySurface->create();
        ctx = new QOpenGLContext;
        ctx->setFormat(surfaceFormat);
        ctx->create();
        ctx->makeCurrent(dummySurface);
    }

#if QT_CONFIG(opengles2)
    isES = true;
#elif (QT_VERSION < QT_VERSION_CHECK(5, 3, 0))
    isES = false;
#else
    isES = ctx->isOpenGLES();
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    // We support only ES2 emulation with software renderer for now
    QString versionStr;
#ifdef Q_OS_WIN
    const GLubyte *openGLVersion = ctx->functions()->glGetString(GL_VERSION);
    versionStr = QString::fromLatin1(reinterpret_cast<const char *>(openGLVersion)).toLower();
#endif
    if (versionStr.contains(QStringLiteral("mesa"))
            || QCoreApplication::testAttribute(Qt::AA_UseSoftwareOpenGL)) {
        qWarning("Only OpenGL ES2 emulation is available for software rendering.");
        isES = true;
    }
#endif

    if (dummySurface) {
        ctx->doneCurrent();
        delete ctx;
        delete dummySurface;
    }

    if (isES) {
        // For ES2 only attributes
        surfaceFormat.setRedBufferSize(8);
        surfaceFormat.setBlueBufferSize(8);
        surfaceFormat.setGreenBufferSize(8);
    } else {
        surfaceFormat.setVersion(2, 1);
        surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
        // For OpenGL only attributes
        if (antialias)
            surfaceFormat.setSamples(8);
        else
            surfaceFormat.setSamples(0);
    }

    return surfaceFormat;
}

QT_END_NAMESPACE

#endif
