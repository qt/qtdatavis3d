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

#include "qtouch3dinputhandler_p.h"
#include "q3dcamera_p.h"
#include <QTimer>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

const qreal maxTapAndHoldJitter = 20;
const int tapAndHoldTime = 250;
const float rotationSpeed = 200.0f;
const int zoomRateFar = 9;
const int zoomRateMid = 5;
const int zoomRateNear = 1;

/*!
   \class QTouch3DInputHandler
   \inmodule QtDataVisualization
   \brief Basic touch display based input handler.
   \since 1.0.0

    QTouch3DInputHandler is the basic input handler for touch screen devices.
*/

/*!
 * Constructs the basic touch display input handler. An optional \a parent parameter can be given
 * and is then passed to QObject constructor.
 */
QTouch3DInputHandler::QTouch3DInputHandler(QObject *parent)
    : Q3DInputHandler(parent),
      d_ptr(new QTouch3DInputHandlerPrivate(this))
{
}

/*!
 *  Destroys the input handler.
 */
QTouch3DInputHandler::~QTouch3DInputHandler()
{
}

/*!
 * Override this to change handling of touch events.
 * Touch event is given in the \a event.
 */
void QTouch3DInputHandler::touchEvent(QTouchEvent *event)
{
    QList<QTouchEvent::TouchPoint> points;
    points = event->touchPoints();

    if (!scene()->isSlicingActive() && points.count() == 2) {
        d_ptr->m_holdTimer->stop();

        setInputState(QDataVis::InputOnPinch);

        QPointF distance = points.at(0).pos() - points.at(1).pos();
        int newDistance = distance.manhattanLength();
        int zoomRate = zoomRateFar;
        int zoomLevel = scene()->activeCamera()->zoomLevel();
        if (zoomLevel > 250)
            zoomRate = zoomRateNear;
        else if (zoomLevel > 100)
            zoomRate = zoomRateMid;
        if (newDistance > prevDistance())
            zoomLevel += zoomRate;
        else
            zoomLevel -= zoomRate;
        if (zoomLevel > 500)
            zoomLevel = 500;
        else if (zoomLevel < 10)
            zoomLevel = 10;
        scene()->activeCamera()->setZoomLevel(zoomLevel);
        setPrevDistance(newDistance);
    } else if (points.count() == 1) {
        QPointF pointerPos = points.at(0).pos();
        if (event->type() == QEvent::TouchBegin) {
            if (scene()->isSlicingActive()) {
                if (scene()->isPointInPrimarySubView(pointerPos.toPoint()))
                    setInputState(QDataVis::InputOnOverview);
                else if (scene()->isPointInSecondarySubView(pointerPos.toPoint()))
                    setInputState(QDataVis::InputOnSlice);
                else
                    setInputState(QDataVis::InputNone);
            } else {
                // Possible tap-and-hold selection start
                d_ptr->m_startHoldPos = pointerPos;
                d_ptr->m_touchHoldPos = d_ptr->m_startHoldPos;
                d_ptr->m_holdTimer->start();
                // Set state to rotating
                setInputState(QDataVis::InputRotating);
                // update pointer positions to prevent jumping when releasing or repressing a button
                setInputPosition(pointerPos.toPoint());
            }
        } else if (event->type() == QEvent::TouchEnd) {
            d_ptr->m_holdTimer->stop();
            setInputState(QDataVis::InputNone);
            // update pointer positions to prevent jumping when releasing or repressing a button
            setPreviousInputPos(pointerPos.toPoint());
        } else if (event->type() == QEvent::TouchUpdate) {
            if (!scene()->isSlicingActive()) {
                d_ptr->m_touchHoldPos = pointerPos;
                if (QDataVis::InputRotating == inputState()) {
                    // Calculate mouse movement since last frame
                    QPointF rotations = scene()->activeCamera()->rotations();
                    float xRotation = rotations.x();
                    float yRotation = rotations.y();
                    // TODO: Replace with event's previous position functionality?
                    float mouseMoveX = float(inputPosition().x() - pointerPos.x())
                            / (scene()->viewport().width() / rotationSpeed);
                    float mouseMoveY = float(inputPosition().y() - pointerPos.y())
                            / (scene()->viewport().height() / rotationSpeed);
                    // Apply to rotations
                    xRotation -= mouseMoveX;
                    yRotation -= mouseMoveY;
                    scene()->activeCamera()->setRotations(QPointF(xRotation, yRotation));
                    scene()->activeCamera()->d_ptr->updateViewMatrix(1.0f);

                    setPreviousInputPos(inputPosition());
                    setInputPosition(pointerPos.toPoint());
                }
            }
        }
    } else {
        d_ptr->m_holdTimer->stop();
    }
}

QTouch3DInputHandlerPrivate::QTouch3DInputHandlerPrivate(QTouch3DInputHandler *q)
    : q_ptr(q),
      m_holdTimer(0)
{
    m_holdTimer = new QTimer();
    m_holdTimer->setSingleShot(true);
    m_holdTimer->setInterval(tapAndHoldTime);
    connect(m_holdTimer, &QTimer::timeout, this, &QTouch3DInputHandlerPrivate::tapAndHold);
}

QTouch3DInputHandlerPrivate::~QTouch3DInputHandlerPrivate()
{
    m_holdTimer->stop();
    delete m_holdTimer;
}

void QTouch3DInputHandlerPrivate::tapAndHold()
{
    QPointF distance = m_startHoldPos - m_touchHoldPos;
    if (distance.manhattanLength() < maxTapAndHoldJitter) {
        q_ptr->setInputPosition(m_touchHoldPos.toPoint());
        q_ptr->setInputState(QDataVis::InputOnScene);
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
