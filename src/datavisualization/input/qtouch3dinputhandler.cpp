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
#include <qmath.h>

namespace QtDataVisualization {

const float maxTapAndHoldJitter = 20.0f;
const int maxPinchJitter = 10;
#if defined (Q_OS_ANDROID)
const int maxSelectionJitter = 10;
#else
const int maxSelectionJitter = 5;
#endif
const int tapAndHoldTime = 250;
const float rotationSpeed = 200.0f;
const int minZoomLevel = 10;
const int maxZoomLevel = 500;

/*!
 * \class QTouch3DInputHandler
 * \inmodule QtDataVisualization
 * \brief Basic touch display based input handler.
 * \since Qt Data Visualization 1.0
 *
 * QTouch3DInputHandler is the basic input handler for touch screen devices.
 *
 * Default touch input handler has the following functionalty:
 * \table
 *   \header
 *     \li Gesture                  \li Action
 *   \row
 *     \li Touch-And-Move           \li Rotate graph within limits set for Q3DCamera
 *   \row
 *     \li Tap                      \li Select the item tapped or remove selection if none.
 *                                      May open the secondary view depending on the
 *                                      selection mode.
 *   \row
 *     \li Tap-And-Hold             \li Same as tap.
 *   \row
 *     \li Pinch                    \li Zoom in/out within default range (10...500%).
 *   \row
 *     \li Tap on the primary view when the secondary view is visible
 *                                  \li Closes the secondary view.
 *                                      \note Secondary view is available only for Q3DBars and Q3DSurface graphs.
 * \endtable
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
        QPointF distance = points.at(0).pos() - points.at(1).pos();
        d_ptr->handlePinchZoom(distance.manhattanLength());
    } else if (points.count() == 1) {
        QPointF pointerPos = points.at(0).pos();
        if (event->type() == QEvent::TouchBegin) {
            if (scene()->isSlicingActive()) {
                if (scene()->isPointInPrimarySubView(pointerPos.toPoint()))
                    setInputState(InputStateOnPrimaryView);
                else if (scene()->isPointInSecondarySubView(pointerPos.toPoint()))
                    setInputState(InputStateOnSecondaryView);
                else
                    setInputState(InputStateNone);
            } else {
                // Handle possible tap-and-hold selection
                d_ptr->m_startHoldPos = pointerPos;
                d_ptr->m_touchHoldPos = d_ptr->m_startHoldPos;
                d_ptr->m_holdTimer->start();
                // Start rotating
                setInputState(InputStateRotating);
                setInputPosition(pointerPos.toPoint());
            }
        } else if (event->type() == QEvent::TouchEnd) {
            d_ptr->m_holdTimer->stop();
            // Handle possible selection
            d_ptr->handleSelection(pointerPos);
        } else if (event->type() == QEvent::TouchUpdate) {
            if (!scene()->isSlicingActive()) {
                d_ptr->m_touchHoldPos = pointerPos;
                // Handle rotation
                d_ptr->handleRotation(pointerPos);
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
    connect(m_holdTimer, &QTimer::timeout, this, &QTouch3DInputHandlerPrivate::handleTapAndHold);
}

QTouch3DInputHandlerPrivate::~QTouch3DInputHandlerPrivate()
{
    m_holdTimer->stop();
    delete m_holdTimer;
}

void QTouch3DInputHandlerPrivate::handlePinchZoom(float distance)
{
    int newDistance = distance;
    int prevDist = q_ptr->prevDistance();
    if (prevDist > 0 && qAbs(prevDist - newDistance) < maxPinchJitter)
        return;
    q_ptr->setInputState(QAbstract3DInputHandler::InputStatePinching);
    Q3DCamera *camera = q_ptr->scene()->activeCamera();
    int zoomLevel = camera->zoomLevel();
    float zoomRate = qSqrt(qSqrt(zoomLevel));
    if (newDistance > prevDist)
        zoomLevel += zoomRate;
    else
        zoomLevel -= zoomRate;
    if (zoomLevel > maxZoomLevel)
        zoomLevel = maxZoomLevel;
    else if (zoomLevel < minZoomLevel)
        zoomLevel = minZoomLevel;
    camera->setZoomLevel(zoomLevel);
    q_ptr->setPrevDistance(newDistance);
}

void QTouch3DInputHandlerPrivate::handleTapAndHold()
{
    QPointF distance = m_startHoldPos - m_touchHoldPos;
    if (distance.manhattanLength() < maxTapAndHoldJitter) {
        q_ptr->setInputPosition(m_touchHoldPos.toPoint());
        q_ptr->setInputState(QAbstract3DInputHandler::InputStateOnScene);
    }
}

void QTouch3DInputHandlerPrivate::handleSelection(const QPointF &position)
{
    QPointF distance = m_startHoldPos - position;
    if (distance.manhattanLength() < maxSelectionJitter)
        q_ptr->setInputState(QAbstract3DInputHandler::InputStateOnScene);
    else
        q_ptr->setInputState(QAbstract3DInputHandler::InputStateNone);
    q_ptr->setPreviousInputPos(position.toPoint());
    q_ptr->scene()->setSelectionQueryPosition(position.toPoint());
}

void QTouch3DInputHandlerPrivate::handleRotation(const QPointF &position)
{
    if (QAbstract3DInputHandler::InputStateRotating == q_ptr->inputState()) {
        Q3DScene *scene = q_ptr->scene();
        Q3DCamera *camera = scene->activeCamera();
        float xRotation = camera->xRotation();
        float yRotation = camera->yRotation();
        QPointF inputPos = q_ptr->inputPosition();
        float mouseMoveX = float(inputPos.x() - position.x())
                / (scene->viewport().width() / rotationSpeed);
        float mouseMoveY = float(inputPos.y() - position.y())
                / (scene->viewport().height() / rotationSpeed);
        xRotation -= mouseMoveX;
        yRotation -= mouseMoveY;
        camera->setXRotation(xRotation);
        camera->setYRotation(yRotation);

        q_ptr->setPreviousInputPos(inputPos.toPoint());
        q_ptr->setInputPosition(position.toPoint());
    }
}

}
