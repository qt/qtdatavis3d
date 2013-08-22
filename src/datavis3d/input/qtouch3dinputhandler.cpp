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
#include "qtouch3dinputhandler.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

QTouch3DInputHandler::QTouch3DInputHandler() :
    Q3DInputHandler()
{
}

// Input event listeners
void QTouch3DInputHandler::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!slicingActivated()) {
        setInputState( QDataVis::InputOnScene );
        // update mouse positions to prevent jumping when releasing or repressing a button
        setInputPosition( event->pos() );
    }
}

void QTouch3DInputHandler::touchEvent(QTouchEvent *event)
{
    QList<QTouchEvent::TouchPoint> points;
    points = event->touchPoints();

    if (!slicingActivated() && points.count() == 2) {
        setInputState( QDataVis::InputOnPinch );

        QPointF distance = points.at(0).pos() - points.at(1).pos();
        int newDistance = distance.manhattanLength();
        int zoomRate = 1;
        int zoomLevel = QAbstract3DInputHandler::zoomLevel();
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
        setZoomLevel(zoomLevel);
        setPrevDistance(newDistance);
    }
}

void QTouch3DInputHandler::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    // TODO: This code needs revisiting with new Qt releases and possibly move to using touch events for these as well.
    QRect mainViewPort = mainViewPortRect();
    if (Qt::LeftButton == event->button()) {
        if (slicingActivated()) {
            if (mousePos.x() <= mainViewPort.width()
                    && mousePos.y() <= mainViewPort.height()) {
                setInputState(QDataVis::InputOnOverview);
                //qDebug() << "Mouse pressed on overview";
            } else {
                setInputState(QDataVis::InputOnSlice);
                //qDebug() << "Mouse pressed on zoom";
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
    } else if (!slicingActivated() && Qt::RightButton == event->button()) {
        // disable rotating when in slice view
        setInputState(QDataVis::InputOnScene);
        // update mouse positions to prevent jumping when releasing or repressing a button
        setInputPosition(mousePos);
    }
    // TODO: Call actual camera class when it's been written.
    //m_cameraHelper->updateMousePos(m_mousePos);
}

QT_DATAVIS3D_END_NAMESPACE
