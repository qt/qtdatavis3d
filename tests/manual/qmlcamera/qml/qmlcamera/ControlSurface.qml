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

import QtQuick 2.0

Rectangle {
    id: controlRect
    color: "#000000"
    property real xValue : 0
    property real yValue : 0
    property real minXValue : 0
    property real minYValue : 0
    property real maxXValue : 0
    property real maxYValue : 0

    property real halfWidth: width / 2.0;
    property real halfHeight: height / 2.0;

    Rectangle {
        id: pointer
        color: "#FFFFFF"
        width: 5
        height: 5
    }

    MouseArea {
        id: inputArea
        anchors.fill: parent
        onPositionChanged: {
            pointer.x = Math.min(Math.max(0, mouse.x), controlRect.width - pointer.width );
            pointer.y = Math.min(Math.max(0, mouse.y), controlRect.height - pointer.height);
            var mixX = (mouse.x / controlRect.width);
            var mixY = (mouse.y / controlRect.width);
            controlRect.xValue = minXValue*(1-mixX) + maxXValue*mixX;
            controlRect.yValue = minYValue*(1-mixY) + maxYValue*mixY;
        }
    }
}
