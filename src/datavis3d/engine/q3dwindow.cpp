/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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
    surfaceFormat.setSamples(4);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    //surfaceFormat.setRenderableType(QSurfaceFormat::OpenGLES); // OpenGL crashes with ANGLE, comment out or define OpenGLES
    setFormat(surfaceFormat);

    create();

    d_ptr->m_context->setFormat(requestedFormat());
    d_ptr->m_context->create();
    //d_ptr->m_context->makeCurrent(this); // doesn't work with Mac

    //initializeOpenGLFunctions(); // doesn't work with Mac
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

void Q3DWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void Q3DWindow::renderNow()
{
    if (!isExposed())
        return;

    static bool needsInit = true;

    d_ptr->m_updatePending = false;

    d_ptr->m_context->makeCurrent(this); // needed here for Mac

    if (needsInit) {
        initializeOpenGLFunctions(); // needed here for Mac
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
