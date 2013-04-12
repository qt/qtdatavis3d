/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

Q3DWindow::Q3DWindow(QWindow *parent)
    : QWindow(parent)
    , d_ptr(new Q3DWindowPrivate(this))
{
    setSurfaceType(QWindow::OpenGLSurface);
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(16);
    surfaceFormat.setSamples(4);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    //surfaceFormat.setRenderableType(QSurfaceFormat::OpenGLES); // OpenGL crashes with ANGLE, comment out or define OpenGLES
    setFormat(surfaceFormat);

    create();

    d_ptr->m_context->setFormat(requestedFormat());
    d_ptr->m_context->create();
#if !defined(Q_OS_MAC)
    // These are required here for windows (and linux?), but cause errors on mac
    d_ptr->m_context->makeCurrent(this);
    initializeOpenGLFunctions();
#endif
    initialize();
}

Q3DWindow::~Q3DWindow()
{
}

void Q3DWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}

void Q3DWindow::initialize()
{
    qDebug() << "OpenGL version" << format().majorVersion() << format().minorVersion();
    qDebug() << "OpenGL renderer" << format().renderableType();
    qDebug() << "OpenGL swapBehavior" << format().swapBehavior();
    setAnimating(true);
}

void Q3DWindow::render()
{
}

void Q3DWindow::renderLater()
{
    if (!d_ptr->m_updatePending) {
        d_ptr->m_updatePending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

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

void Q3DWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

//void Q3DWindow::resizeEvent(QResizeEvent *event)
//{
//    Q_UNUSED(event);

//    if (isExposed())
//        renderLater();
//}

void Q3DWindow::renderNow()
{
    if (!isExposed())
        return;

    static bool needsInit = true;

    d_ptr->m_updatePending = false;

    d_ptr->m_context->makeCurrent(this);

    if (needsInit) {
#if defined(Q_OS_MAC)
        initializeOpenGLFunctions();
#endif
        getDevice();
        initialize();
        needsInit = false;
    }

    render();

    d_ptr->m_context->swapBuffers(this);

    if (d_ptr->m_animating)
        renderLater();
}

void Q3DWindow::setAnimating(bool animating)
{
    d_ptr->m_animating = animating;

    if (animating)
        renderLater();
}

QOpenGLPaintDevice *Q3DWindow::getDevice()
{
    if (!d_ptr->m_device)
        d_ptr->m_device = new QOpenGLPaintDevice;
    d_ptr->m_device->setSize(size());
    return d_ptr->m_device;
}

Q3DWindowPrivate::Q3DWindowPrivate(Q3DWindow *q)
    : q_ptr(q)
    , m_updatePending(false)
    , m_animating(false)
    , m_context(new QOpenGLContext(q))
    , m_device(0)
{
}

Q3DWindowPrivate::~Q3DWindowPrivate()
{
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE
