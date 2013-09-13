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
#include "q3dcamera.h"
#include <QTimer>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

const qreal maxTapAndHoldJitter = 10;
const int tapAndHoldTime = 250;

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

// Input event listeners
/*!
 * Override this to change handling of mouse double click events.
 * Mouse double click event is given in the \a event.
 */
void QTouch3DInputHandler::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!scene()->isSlicingActive()) {
        setInputState(QDataVis::InputOnScene);
        // update mouse positions to prevent jumping when releasing or repressing a button
        setInputPosition(event->pos());
        // TODO: Get rid of these (QTRD-2307)
        emit selectionAtPoint(inputPosition());
    }
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
        int zoomRate = 1;
        int zoomLevel = scene()->activeCamera()->zoomLevel();
        if (zoomLevel > 100)
            zoomRate = 5;
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
    } else if (!scene()->isSlicingActive() && points.count() == 1) {
        if (event->type() == QEvent::TouchBegin) {
            // Tap-and-hold selection start
            d_ptr->m_startHoldPos = points.at(0).pos();
            d_ptr->m_touchHoldPos = d_ptr->m_startHoldPos;
            d_ptr->m_holdTimer->start();
        } else if (event->type() == QEvent::TouchEnd) {
            d_ptr->m_holdTimer->stop();
        } else if (event->type() == QEvent::TouchUpdate) {
            d_ptr->m_touchHoldPos = points.at(0).pos();
        }
    } else {
        d_ptr->m_holdTimer->stop();
    }
}

/*!
 * Override this to change handling of mouse press events.
 * Mouse press event is given in the \a event and the mouse position in \a mousePos.
 * \warning This method is subject to change or removal.
 */
void QTouch3DInputHandler::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    // TODO: This code needs revisiting with new Qt releases and possibly move to using touch events for these as well.
    if (Qt::LeftButton == event->button()) {
        if (scene()->isSlicingActive()) {
            if (scene()->isPointInPrimarySubView(mousePos)) {
                setInputState(QDataVis::InputOnOverview);
                //qDebug() << "Mouse pressed on overview";
            } else if (scene()->isPointInSecondarySubView(mousePos)) {
                setInputState(QDataVis::InputOnSlice);
                //qDebug() << "Mouse pressed on zoom";
            } else {
                setInputState(QDataVis::InputNone);
            }
        } else {
            setInputState(QDataVis::InputRotating);
            // update mouse positions to prevent jumping when releasing or repressing a button
            setInputPosition(mousePos);
            //qDebug() << "Mouse pressed on scene";
        }
    } else if (Qt::MiddleButton == event->button()) {
        // reset rotations
        setInputPosition(QPoint(0, 0));
    } else if (Qt::RightButton == event->button()) {
        // disable rotating when in slice view
        setInputState(QDataVis::InputOnScene);
        // update mouse positions to prevent jumping when releasing or repressing a button
        setInputPosition(mousePos);
    }
}

QTouch3DInputHandlerPrivate::QTouch3DInputHandlerPrivate(QTouch3DInputHandler *q)
    : q_ptr(q),
      m_holdTimer(0)
{
    m_holdTimer = new QTimer();
    m_holdTimer->setSingleShot(true);
    m_holdTimer->setInterval(tapAndHoldTime);
    QObject::connect(m_holdTimer, &QTimer::timeout, this, &QTouch3DInputHandlerPrivate::tapAndHold);
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
        q_ptr->setInputPosition(QPoint(int(m_touchHoldPos.x()), int(m_touchHoldPos.y())));
        q_ptr->setInputState(QDataVis::InputOnScene);
        // TODO: Get rid of these (QTRD-2307)
        emit q_ptr->selectionAtPoint(q_ptr->inputPosition());
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
