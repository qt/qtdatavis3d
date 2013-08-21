/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

#include "surface3dcontroller_p.h"
#include "surface3drenderer_p.h"
#include "camerahelper_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

Surface3dController::Surface3dController(QRect rect)
    : Abstract3DController(rect),
      m_renderer(0),
      m_isInitialized(false),
      m_smoothSurface(false),
      m_surfaceGrid(true),
      m_mouseState(MouseNone),
      m_mousePos(QPoint(0, 0))
{
}

Surface3dController::~Surface3dController()
{
}

void Surface3dController::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (m_isInitialized)
        return;

    m_renderer = new Surface3dRenderer(this);
    m_isInitialized = true;
}

void Surface3dController::synchDataToRenderer()
{
    // TODO: Implement
}

void Surface3dController::render(const GLuint defaultFboHandle)
{
    if (!m_isInitialized)
        return;

    m_renderer->render(m_cameraHelper, defaultFboHandle);
}

void Surface3dController::handleAxisAutoAdjustRangeChangedInOrientation(QAbstractAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)

    // TODO: Implement!
}

QMatrix4x4 Surface3dController::calculateViewMatrix(int zoom, int viewPortWidth, int viewPortHeight, bool showUnder)
{
    return m_cameraHelper->calculateViewMatrix(m_mousePos,
                                               zoom,
                                               viewPortWidth,
                                               viewPortHeight,
                                               showUnder);
}

void Surface3dController::setWidth(const int width)
{
    qDebug() << "Surface3dController::setWidth";
    m_renderer->setWidth(width);
}

void Surface3dController::setHeight(const int height)
{
    qDebug() << "Surface3dController::setHeight";
    m_renderer->setHeight(height);
}

void Surface3dController::setSmoothSurface(bool enable)
{
    m_smoothSurface = enable;
    emit smoothStatusChanged(m_smoothSurface);
}

bool Surface3dController::smoothSurface()
{
    return m_smoothSurface;
}

void Surface3dController::setSurfaceGrid(bool enable)
{
    m_surfaceGrid = enable;
    emit surfaceGridChanged(m_surfaceGrid);
}

bool Surface3dController::surfaceGrid()
{
    return m_surfaceGrid;
}


#if defined(Q_OS_ANDROID)
void Surface3dController::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}
void touchEvent(QTouchEvent *event)
{
    Q_UNUSED(event)
}
#endif

void Surface3dController::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    if (Qt::LeftButton == event->button()) {
        m_mousePos = mousePos;
        emit leftMousePressed();
    } else if (Qt::RightButton == event->button()) {
    #if !defined(Q_OS_ANDROID)
        m_mouseState = Abstract3DController::MouseRotating;
    #else
        m_mouseState = Abstract3DController::MouseOnScene;
    #endif
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos; //event->pos();
    }
    m_cameraHelper->updateMousePos(m_mousePos);
}

void Surface3dController::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event)
    if (Abstract3DController::MouseRotating == m_mouseState) {
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos; //event->pos();
        m_cameraHelper->updateMousePos(mousePos); //event->pos());
    }
    m_mouseState = Abstract3DController::MouseNone;
}

void Surface3dController::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event)
    if (Abstract3DController::MouseRotating == m_mouseState)
        m_mousePos = mousePos; //event->pos();
}

void Surface3dController::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
}

QPoint Surface3dController::mousePosition()
{
    return m_mousePos;
}


// TODO: abstract renderer should have accessor for Drawer instead
Drawer *Surface3dController::drawer()
{
    if (m_renderer)
        return m_renderer->drawer();
    else
        return 0;
}

void Surface3dController::setSegmentCount(GLint segmentCount, GLfloat step, GLfloat minimum)
{
    m_segmentCount   = segmentCount;
    m_segmentStep    = step;
    m_segmentMinimum = minimum;

    emit segmentCountChanged(m_segmentCount, m_segmentStep, m_segmentMinimum);
}

void Surface3dController::setGradientColorAt(qreal pos, const QColor &color)
{
    Theme t = theme();
    t.m_surfaceGradient.setColorAt(pos, color);
    emit themeChanged(t);
}

// TODO: Temp
void Surface3dController::setData(QList<qreal> series, int width, int depth)
{
    m_series = series;
    m_dataWidth = width;
    m_dataDepth = depth;

    m_renderer->setXZStuff(width, depth);
    m_renderer->setSeries(series);
}

QT_DATAVIS3D_END_NAMESPACE
