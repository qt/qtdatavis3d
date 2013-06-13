/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "q3dwindow.h"
#include "q3dwindow_p.h"

#include <QGuiApplication>

#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class Q3DWindow
 * \inmodule QtDataVis3D
 * \brief The Q3DWindow class provides a window and render loop.
 * \since 1.0.0
 *
 * This class creates a QWindow and provides render loop for visualization types inheriting it.
 * \warning This class is not intended to be used directly by developers.
 *
 * \sa Q3DBars, Q3DMaps, {Qt Data Visualization 3D C++ Classes}
 */

/*!
 * \a parent A QWindow parent.
 *
 * Constructs Q3DWindow. It creates a QWindow and an OpenGL context. It also sets surface
 * format and initializes OpenGL functions for use.
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

    initialize();
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
void Q3DWindow::initialize()
{
    const GLubyte *version = glGetString(GL_VERSION);
    qDebug() << "OpenGL version:" << (const char *)version;
    version = glGetString(GL_SHADING_LANGUAGE_VERSION);
    qDebug() << "GLSL version:" << (const char *)version;
#if !defined(QT_OPENGL_ES_2)
    // If we have real OpenGL, GLSL version must be 1.2 or over. Quit if not.
    QStringList splitversionstr = QString((const char *)version).split(" ");
    if (splitversionstr[0].toFloat() < 1.2)
        qFatal("GLSL version must be 1.20 or higher. Try installing latest display drivers.");
#endif
    setAnimating(true);
}

/*!
 * \internal
 */
void Q3DWindow::render()
{
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

    static bool needsInit = true;

    d_ptr->m_updatePending = false;

    d_ptr->m_context->makeCurrent(this);

    if (needsInit) {
        initialize();
        needsInit = false;
    }

    render();

    d_ptr->m_context->swapBuffers(this);

    if (d_ptr->m_animating)
        renderLater();
}

/*!
 * \internal
 */
void Q3DWindow::setAnimating(bool animating)
{
    d_ptr->m_animating = animating;

    if (animating)
        renderLater();
}

Q3DWindowPrivate::Q3DWindowPrivate(Q3DWindow *q)
    : q_ptr(q),
      m_updatePending(false),
      m_animating(false),
      m_context(new QOpenGLContext(q))
{
}

Q3DWindowPrivate::~Q3DWindowPrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE
