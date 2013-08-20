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

import QtQuick 2.1
import QtQuick.Controls 1.0
import com.digia.QtDataVis3D 1.0

Item {
    id: mainview
    width: 800
    height: 700
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
            ListElement{ xPos: -10.0; yPos: 4.9; zPos: -5.0 }
            ListElement{ xPos: 10.0; yPos: 4.9; zPos: -5.0 }
            ListElement{ xPos: -10.0; yPos: 4.9; zPos: 5.0 }
            ListElement{ xPos: 10.0; yPos: 4.9; zPos: 5.0 }
            ListElement{ xPos: -10.0; yPos: -4.9; zPos: -5.0 }
            ListElement{ xPos: 10.0; yPos: -4.9; zPos: -5.0 }
            ListElement{ xPos: -10.0; yPos: -4.9; zPos: 5.0 }
            ListElement{ xPos: 10.0; yPos: -4.9; zPos: 5.0 }

            ListElement{ xPos: -1.0; yPos: 0.3; zPos: -0.5 }
            ListElement{ xPos: 1.0; yPos: 2.105; zPos: 0.5 }
            ListElement{ xPos: 0.5; yPos: -0.65; zPos: -0.5 }
            ListElement{ xPos: -0.5; yPos: 1.225; zPos: 0.5 }
            ListElement{ xPos: 0.0; yPos: 0.0; zPos: 0.0 }
            ListElement{ xPos: 0.0; yPos: 2.0; zPos: 0.0 }
            ListElement{ xPos: 0.0; yPos: -0.5; zPos: 0.0 }

            ListElement{ xPos: 6.0; yPos: 0.0; zPos: 4.0 }
            ListElement{ xPos: 5.8; yPos: 0.2; zPos: 5.0 }
            ListElement{ xPos: 5.6; yPos: 0.4; zPos: 4.5 }
            ListElement{ xPos: 5.4; yPos: 0.6; zPos: 3.8 }
            ListElement{ xPos: 5.2; yPos: 0.8; zPos: 4.8 }
            ListElement{ xPos: 5.0; yPos: 0.3; zPos: 4.1 }
            ListElement{ xPos: 4.9; yPos: -0.3; zPos: 4.9 }
            ListElement{ xPos: 4.7; yPos: -0.5; zPos: 3.5 }
            ListElement{ xPos: 4.5; yPos: -0.7; zPos: 3.3 }
            ListElement{ xPos: 4.3; yPos: -0.4; zPos: 3.7 }
        }

        Scatter3D {
            id: testscatter
            width: dataView.width
            height: dataView.height
            fontSize: 30.0
            mapping: scatterMapping
            shadowQuality: Scatter3D.ShadowNone
            selectionMode: Scatter3D.ModeItem
            labelTransparency: Scatter3D.TransparencyNoBackground
            itemLabelFormat: "X:@xLabel Y:@yLabel Z:@zLabel"

            Component.onCompleted: {
                console.log("testscatter complete");
                data = dataModel
            }
        }
    }

    Rectangle {
        id: shadowToggle
        width: 120
        height: 30

        TextArea {
            id: shadowText
            text: "Toggle Shadows On"
            anchors.fill: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (testscatter.shadowQuality === Scatter3D.ShadowNone) {
                    testscatter.shadowQuality = Scatter3D.ShadowMedium;
                    shadowText.text = "Toggle Shadows Off";
                } else {
                    testscatter.shadowQuality = Scatter3D.ShadowNone;
                    shadowText.text = "Toggle Shadows On";
                }
            }
        }
    }
    Rectangle {
        id: smoothToggle
        width: 140
        height: 30
        anchors.left: shadowToggle.right

        TextArea {
            id: smoothText
            text: "Toggle Smooth Objects On"
            anchors.fill: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (testscatter.objectSmooth === false) {
                    smoothText.text = "Toggle Smooth Objects Off"
                    testscatter.objectSmooth = true;
                } else {
                    smoothText.text = "Toggle Smooth Objects On"
                    testscatter.objectSmooth = false;
                }
            }
        }
    }
    Rectangle {
        id: cameraToggle
        width: 130
        height: 30
        anchors.left: smoothToggle.right

        TextArea {
            text: "Toggle Camera Preset"
            anchors.fill: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (testscatter.cameraPreset === Scatter3D.PresetFront) {
                    testscatter.cameraPreset = Scatter3D.PresetIsometricRightHigh;
                } else {
                    testscatter.cameraPreset = Scatter3D.PresetFront;
                }
            }
        }
    }
    Rectangle {
        id: themeToggle
        width: 120
        height: 30
        anchors.left: cameraToggle.right

        TextArea {
            text: "Toggle Theme"
            anchors.fill: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (testscatter.theme === Scatter3D.ThemeBlueCerulean) {
                    testscatter.theme = Scatter3D.ThemeSystem;
                } else {
                    testscatter.theme = Scatter3D.ThemeBlueCerulean;
                }
            }
        }
    }
    Rectangle {
        id: backgroundToggle
        width: 130
        height: 30
        anchors.left: themeToggle.right

        TextArea {
            id: backgroundText
            text: "Toggle Background Off"
            anchors.fill: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (testscatter.backgroundVisible === true) {
                    testscatter.backgroundVisible = false;
                    backgroundText.text = "Toggle Background On";
                } else {
                    testscatter.backgroundVisible = true;
                    backgroundText.text = "Toggle Background Off";
                }
            }
        }
    }

}
