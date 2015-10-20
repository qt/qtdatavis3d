/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include "custominputhandler.h"

#include <QtDataVisualization/Q3DCamera>

CustomInputHandler::CustomInputHandler(QObject *parent) :
    QAbstract3DInputHandler(parent)
{
}

//! [0]
void CustomInputHandler::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event)
    setInputPosition(mousePos);
}
//! [0]

//! [1]
void CustomInputHandler::wheelEvent(QWheelEvent *event)
{
    // Adjust zoom level based on what zoom range we're in.
    int zoomLevel = scene()->activeCamera()->zoomLevel();
    if (zoomLevel > 100)
        zoomLevel += event->angleDelta().y() / 12;
    else if (zoomLevel > 50)
        zoomLevel += event->angleDelta().y() / 60;
    else
        zoomLevel += event->angleDelta().y() / 120;
    if (zoomLevel > 500)
        zoomLevel = 500;
    else if (zoomLevel < 10)
        zoomLevel = 10;

    scene()->activeCamera()->setZoomLevel(zoomLevel);
}
//! [1]
