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

import QtQuick 2.1

Item {
    id: button

    property alias text: buttonText.text
    property alias color: buttonRectangle.color
    property alias radius: buttonRectangle.radius

    property color defaultColor: "wheat"
    property color pressedColor: "tan"
    property color borderColor: "sienna"

    signal clicked

    height: 90

    Rectangle {
        id: buttonRectangle
        width: parent.width
        height: parent.height
        color: defaultColor
        radius: 5
        border.color: borderColor

        Text {
            id: buttonText
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
        }

        MouseArea {
            anchors.fill: parent
            onPressed: buttonRectangle.color = pressedColor
            onReleased: buttonRectangle.color = defaultColor
            onClicked: button.clicked()
        }
    }
}
