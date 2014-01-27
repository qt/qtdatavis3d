/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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
