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

#include "q3dwindow.h"
#include "q3dwindow_p.h"
#include "abstract3dcontroller_p.h"
#include <QGuiApplication>

#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>

#include <QDebug>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class Q3DWindow
 * \inmodule QtDataVisualization
 * \brief The Q3DWindow class provides a window and render loop.
 * \since 1.0.0
 *
 * This class creates a QWindow and provides render loop for visualization types inheriting it.
 * \warning This class is not intended to be used directly by developers.
 *
 * \sa Q3DBars, Q3DScatter, Q3DSurface, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs Q3DWindow with \a parent. It creates a QWindow and an OpenGL context. It also sets
 * surface format and initializes OpenGL functions for use.
 */
Q3DWindow::Q3DWindow(QWindow *parent)
    : QWindow(parent),
      d_ptr(new Q3DWindowPrivate(this))
{
    setSurfaceType(QWindow::OpenGLSurface);
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(16);
#if !defined(QT_OPENGL_ES_2)
    surfaceFormat.setSamples(8);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
#else
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGLES);
#endif
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    setFormat(surfaceFormat);

    create();

    d_ptr->m_context->setFormat(requestedFormat());
    d_ptr->m_context->create();
    d_ptr->m_context->makeCurrent(this);

    qDebug() << "initializeOpenGLFunctions()";
    initializeOpenGLFunctions();

    const GLubyte *version = glGetString(GL_VERSION);
    qDebug() << "OpenGL version:" << (const char *)version;
    version = glGetString(GL_SHADING_LANGUAGE_VERSION);
    qDebug() << "GLSL version:" << (const char *)version;
#if !defined(QT_OPENGL_ES_2)
    // If we have real OpenGL, GLSL version must be 1.2 or over. Quit if not.
    QStringList splitversionstr = QString::fromLatin1((const char *)version).split(QChar::fromLatin1(' '));
    if (splitversionstr[0].toFloat() < 1.2)
        qFatal("GLSL version must be 1.20 or higher. Try installing latest display drivers.");
#endif
    renderLater();
}

/*!
 * Destroys Q3DWindow.
 */
Q3DWindow::~Q3DWindow()
{
}

/*!
 * \internal
 */
void Q3DWindow::setVisualController(Abstract3DController *controller)
{
    d_ptr->m_visualController = controller;
}

/*!
 * \internal
 */
void Q3DWindow::handleDevicePixelRatioChange()
{
    if (QWindow::devicePixelRatio() == d_ptr->m_devicePixelRatio || !d_ptr->m_visualController)
        return;

    // Device pixel ratio changed, resize accordingly and inform the scene
    d_ptr->m_devicePixelRatio = QWindow::devicePixelRatio();
    d_ptr->m_visualController->updateDevicePixelRatio(d_ptr->m_devicePixelRatio);

}

/*!
 * \internal
 */
void Q3DWindow::render()
{
    handleDevicePixelRatioChange();
    d_ptr->m_visualController->synchDataToRenderer();
    d_ptr->m_visualController->render();
}

/*!
 * \internal
 */
void Q3DWindow::renderLater()
{
    if (!d_ptr->m_updatePending) {
        d_ptr->m_updatePending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

/*!
 * \internal
 */
bool Q3DWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

/*!
 * \internal
 */
void Q3DWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

/*!
 * \internal
 */
void Q3DWindow::renderNow()
{
    if (!isExposed())
        return;

    d_ptr->m_updatePending = false;

    d_ptr->m_context->makeCurrent(this);

    render();

    d_ptr->m_context->swapBuffers(this);
}

Q3DWindowPrivate::Q3DWindowPrivate(Q3DWindow *q)
    : q_ptr(q),
      m_updatePending(false),
      m_context(new QOpenGLContext(q)),
      m_visualController(0),
      m_devicePixelRatio(1.f)
{
}

Q3DWindowPrivate::~Q3DWindowPrivate()
{
}

QT_DATAVISUALIZATION_END_NAMESPACE
