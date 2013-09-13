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
#include "qtouch3dinputhandler.h"
#include "q3dcamera.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

QTouch3DInputHandler::QTouch3DInputHandler(QObject *parent) :
    Q3DInputHandler(parent)
{
}

QTouch3DInputHandler::~QTouch3DInputHandler()
{
}

// Input event listeners
void QTouch3DInputHandler::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!scene()->isSlicingActivated()) {
        setInputState(QDataVis::InputOnScene);
        // update mouse positions to prevent jumping when releasing or repressing a button
        setInputPosition( event->pos() );
    }
}

void QTouch3DInputHandler::touchEvent(QTouchEvent *event)
{
    QList<QTouchEvent::TouchPoint> points;
    points = event->touchPoints();

    if (!scene()->isSlicingActivated() && points.count() == 2) {
        setInputState(QDataVis::InputOnPinch);

        QPointF distance = points.at(0).pos() - points.at(1).pos();
        int newDistance = distance.manhattanLength();
        int zoomRate = 1;
        int zoomLevel = scene()->camera()->zoomLevel();
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
        scene()->camera()->setZoomLevel(zoomLevel);
        setPrevDistance(newDistance);
    }
}

void QTouch3DInputHandler::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    // TODO: This code needs revisiting with new Qt releases and possibly move to using touch events for these as well.
    if (Qt::LeftButton == event->button()) {
        if (scene()->isSlicingActivated()) {
            if (scene()->isInputInsideMainView(mousePos)) {
                setInputState(QDataVis::InputOnOverview);
                //qDebug() << "Mouse pressed on overview";
            } else if (scene()->isInputInsideSliceView(mousePos)) {
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

QT_DATAVISUALIZATION_END_NAMESPACE
