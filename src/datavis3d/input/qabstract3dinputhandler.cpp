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

QAbstract3DInputHandler::QAbstract3DInputHandler(QObject *parent) :
    QObject(parent),
    d_ptr(new QAbstract3DInputHandlerPrivate(this))
{
}

QAbstract3DInputHandler::~QAbstract3DInputHandler()
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

void QAbstract3DInputHandler::setInputState(QDataVis::InputState inputState)
{
    if (inputState != d_ptr->m_inputState) {
        d_ptr->m_inputState = inputState;
        emit inputStateChanged(inputState);
    }
}

QPoint QAbstract3DInputHandler::inputPosition() const
{
    return d_ptr->m_inputPosition;
}

void QAbstract3DInputHandler::setInputPosition(const QPoint &position)
{
    if (position != d_ptr->m_inputPosition) {
        d_ptr->m_inputPosition = position;
        emit positionChanged(position);
    }
}

void QAbstract3DInputHandler::setPrevDistance(int distance)
{
    d_ptr->m_prevDistance = distance;
}

int QAbstract3DInputHandler::prevDistance() const
{
    return d_ptr->m_prevDistance;
}


Q3DScene *QAbstract3DInputHandler::scene() const
{
    return d_ptr->m_scene;
}

void QAbstract3DInputHandler::setScene(Q3DScene *scene)
{
    d_ptr->m_scene = scene;
}

void QAbstract3DInputHandler::setPreviousInputPos(const QPoint &position)
{
    d_ptr->m_previousInputPos = position;
}

QPoint QAbstract3DInputHandler::previousInputPos() const
{
    return d_ptr->m_previousInputPos;
}



QAbstract3DInputHandlerPrivate::QAbstract3DInputHandlerPrivate(QAbstract3DInputHandler *q) :
    q_ptr(q),
    m_prevDistance(0),
    m_inputState(QDataVis::InputNone),
    m_inputPosition(QPoint(0,0)),
    m_previousInputPos(QPoint(0,0)),
    m_scene(0)
{
}

QAbstract3DInputHandlerPrivate::~QAbstract3DInputHandlerPrivate()
{

}

QT_DATAVIS3D_END_NAMESPACE
