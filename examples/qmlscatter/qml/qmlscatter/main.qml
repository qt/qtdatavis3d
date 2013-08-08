/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE$
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.1
import QtQuick.Controls 1.0
import com.digia.QtDataVis3D 1.0

Item {
    id: mainview
    width: 800
    height: 500
    visible: true

    Item {
        id: dataView
        width: parent.width
        height: parent.height - shadowToggle.height
        anchors.bottom: parent.bottom

        ScatterDataMapping {
            id: scatterMapping
            xPosRole: "xPos"
            yPosRole: "yPos"
            zPosRole: "zPos"
        }

        ListModel {
            id: dataModel
//            ListElement{ xPos: -100.0; yPos: 490.0; zPos: -50.0 }
//            ListElement{ xPos: 100.0; yPos: 105.0; zPos: 50.0 }
//            ListElement{ xPos: 50.0; yPos: 465.0; zPos: -50.0 }
//            ListElement{ xPos: -50.0; yPos: 225.0; zPos: 50.0 }
            ListElement{ xPos: -1.0; yPos: 4.3; zPos: -0.5 }
            ListElement{ xPos: 1.0; yPos: 0.105; zPos: 0.5 }
            ListElement{ xPos: 0.5; yPos: -4.65; zPos: -0.5 }
            ListElement{ xPos: -0.5; yPos: 0.225; zPos: 0.5 }
            ListElement{ xPos: 0.0; yPos: 0.0; zPos: 0.0 }
            ListElement{ xPos: 0.0; yPos: 2.0; zPos: 0.0 }
            ListElement{ xPos: 0.0; yPos: -2.0; zPos: 0.0 }
            ListElement{ xPos: -10.0; yPos: 4.9; zPos: -5.0 }
            ListElement{ xPos: 10.0; yPos: 4.9; zPos: -5.0 }
            ListElement{ xPos: -10.0; yPos: 4.9; zPos: 5.0 }
            ListElement{ xPos: 10.0; yPos: 4.9; zPos: 5.0 }
            ListElement{ xPos: -10.0; yPos: -4.9; zPos: -5.0 }
            ListElement{ xPos: 10.0; yPos: -4.9; zPos: -5.0 }
            ListElement{ xPos: -10.0; yPos: -4.9; zPos: 5.0 }
            ListElement{ xPos: 10.0; yPos: -4.9; zPos: 5.0 }
        }

        Scatter3D {
            id: testscatter
            width: dataView.width
            height: dataView.height
            fontSize: 30.0
            mapping: scatterMapping

            Component.onCompleted: {
                console.log("testscatter complete");
                shadowQuality = Scatter3D.ShadowNone
                selectionMode = Scatter3D.ModeBar
                labelTransparency = Scatter3D.TransparencyNoBackground
                data = dataModel
            }
        }
    }

    Component.onCompleted: {
        console.log("mainview complete");
    }

    Rectangle {
        id: shadowToggle
        color: "#FFFFFF"
        x: 0
        y: 0
        width: parent.width
        height: 60

        TextArea {
            id: buttonText
            text: "Toggle Shadows"
            anchors.fill: parent
            textColor: "#000000"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (testscatter.shadowQuality === Scatter3D.ShadowNone) {
                    testscatter.shadowQuality = Scatter3D.ShadowLow;
                    buttonText.textColor = "#999999";
                } else {
                    testscatter.shadowQuality = Scatter3D.ShadowNone;
                    buttonText.textColor = "#000000";
                }
            }
        }
    }
}
