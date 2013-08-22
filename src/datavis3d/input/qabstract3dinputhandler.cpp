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
#include "qabstract3dinputhandler.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

QAbstract3DInputHandler::QAbstract3DInputHandler() :
    QObject(),
    d_ptr(new QAbstract3DInputHandlerPrivate(this))
{
}

// Input event listeners
void QAbstract3DInputHandler::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void QAbstract3DInputHandler::touchEvent(QTouchEvent *event)
{
    Q_UNUSED(event);
}

void QAbstract3DInputHandler::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    Q_UNUSED(mousePos);
}

void QAbstract3DInputHandler::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    Q_UNUSED(mousePos);
}

void QAbstract3DInputHandler::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    Q_UNUSED(mousePos);
}

void QAbstract3DInputHandler::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
}

// Property get/set
QDataVis::InputState QAbstract3DInputHandler::inputState()
{
    return d_ptr->m_inputState;
}

void QAbstract3DInputHandler::setInputState(const QDataVis::InputState inputState)
{
    if (inputState != d_ptr->m_inputState) {
        d_ptr->m_inputState = inputState;
        emit inputStateChanged(inputState);
    }
}

QPoint QAbstract3DInputHandler::inputPosition()
{
    return d_ptr->m_inputPosition;
}

void QAbstract3DInputHandler::setInputPosition(const QPoint position)
{
    if (position != d_ptr->m_inputPosition) {
        d_ptr->m_inputPosition = position;
        emit positionChanged(position);
    }
}

bool QAbstract3DInputHandler::slicingActivated()
{
    return d_ptr->m_isSlicingActivated;
}

void QAbstract3DInputHandler::setSlicingActivated(const bool isSlicing)
{
    if (isSlicing != d_ptr->m_isSlicingActivated) {
        d_ptr->m_isSlicingActivated = isSlicing;
        emit slicingActiveChanged(isSlicing);
    }
}

int QAbstract3DInputHandler::zoomLevel()
{
    return d_ptr->m_zoomLevel;
}

void QAbstract3DInputHandler::setZoomLevel(const int zoomLevel)
{
    if (zoomLevel != d_ptr->m_zoomLevel) {
        d_ptr->m_zoomLevel = zoomLevel;
        emit zoomLevelChanged(zoomLevel);
    }
}

void QAbstract3DInputHandler::setPrevDistance(int distance)
{
    d_ptr->m_prevDistance = distance;
}

int QAbstract3DInputHandler::prevDistance()
{
    return d_ptr->m_prevDistance;
}


QRect QAbstract3DInputHandler::mainViewPortRect()
{
    return d_ptr->m_mainViewPort;
}

void QAbstract3DInputHandler::setMainViewPortRect(const QRect viewPort)
{
    d_ptr->m_mainViewPort = viewPort;
}

CameraHelper *QAbstract3DInputHandler::camera()
{
    return d_ptr->m_camera;
}

void QAbstract3DInputHandler::setCamera(CameraHelper *camera)
{
    d_ptr->m_camera = camera;
}


QAbstract3DInputHandlerPrivate::QAbstract3DInputHandlerPrivate(QAbstract3DInputHandler *q) :
    q_ptr(q),
    m_prevDistance(0),
    m_inputState(QDataVis::InputNone),
    m_isSlicingActivated(false),
    m_inputPosition(QPoint(0,0)),
    m_zoomLevel(0),
    m_camera(0)
{
}

QAbstract3DInputHandlerPrivate::~QAbstract3DInputHandlerPrivate()
{

}

QT_DATAVIS3D_END_NAMESPACE
