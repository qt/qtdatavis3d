// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick3D

Component {
    Node {
        property string labelText: "Bar"
        property color backgroundColor: "gray"
        property bool backgroundEnabled: false
        property color labelTextColor: "red"
        property bool borderEnabled : false
        property font labelFont
        property real labelWidth: -1
        property real labelHeight: 10

        scale: Qt.vector3d(0.01, 0.01, 0.0)

        Item {
            anchors.centerIn: parent
            width: labelWidth > 0 ? labelWidth : text0.width
            height: text0.height

            Rectangle {
                id: labelBackground
                anchors.fill: parent
                color: backgroundColor
                visible: backgroundEnabled
                border.color: labelTextColor
                border.width: borderEnabled ? 1 : 0
            }

            Text {
                id: text0
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: labelTextColor
                text: labelText
                font: labelFont
            }
        }
    }
}
