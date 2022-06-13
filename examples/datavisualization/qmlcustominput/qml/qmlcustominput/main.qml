// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtDataVisualization 1.2
import "."

Item {
    id: mainView
    width: 1280
    height: 720

    Data {
        id: graphData
    }

    Item {
        id: dataView
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height - buttonLayout.height

        //! [0]
        Scatter3D {
            //! [0]
            id: scatterGraph
            width: dataView.width
            height: dataView.height
            theme: Theme3D { type: Theme3D.ThemeDigia }
            shadowQuality: AbstractGraph3D.ShadowQualityMedium
            scene.activeCamera.yRotation: 30.0
            //! [1]
            inputHandler: null
            //! [1]

            Scatter3DSeries {
                id: scatterSeriesOne
                itemLabelFormat: "One - X:@xLabel Y:@yLabel Z:@zLabel"
                mesh: Abstract3DSeries.MeshCube

                ItemModelScatterDataProxy {
                    itemModel: graphData.modelOne
                    xPosRole: "xPos"
                    yPosRole: "yPos"
                    zPosRole: "zPos"
                }
            }

            Scatter3DSeries {
                id: scatterSeriesTwo
                itemLabelFormat: "Two - X:@xLabel Y:@yLabel Z:@zLabel"
                mesh: Abstract3DSeries.MeshCube

                ItemModelScatterDataProxy {
                    itemModel: graphData.modelTwo
                    xPosRole: "xPos"
                    yPosRole: "yPos"
                    zPosRole: "zPos"
                }
            }

            Scatter3DSeries {
                id: scatterSeriesThree
                itemLabelFormat: "Three - X:@xLabel Y:@yLabel Z:@zLabel"
                mesh: Abstract3DSeries.MeshCube

                ItemModelScatterDataProxy {
                    itemModel: graphData.modelThree
                    xPosRole: "xPos"
                    yPosRole: "yPos"
                    zPosRole: "zPos"
                }
            }
        }

        //! [2]
        MouseArea {
            id: inputArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            property int mouseX: -1
            property int mouseY: -1
            //! [2]

            //! [3]
            onPositionChanged: (mouse)=> {
                mouseX = mouse.x;
                mouseY = mouse.y;
            }
            //! [3]

            //! [5]
            onWheel: (wheel)=> {
                // Adjust zoom level based on what zoom range we're in.
                var zoomLevel = scatterGraph.scene.activeCamera.zoomLevel;
                if (zoomLevel > 100)
                    zoomLevel += wheel.angleDelta.y / 12.0;
                else if (zoomLevel > 50)
                    zoomLevel += wheel.angleDelta.y / 60.0;
                else
                    zoomLevel += wheel.angleDelta.y / 120.0;
                if (zoomLevel > 500)
                    zoomLevel = 500;
                else if (zoomLevel < 10)
                    zoomLevel = 10;

                scatterGraph.scene.activeCamera.zoomLevel = zoomLevel;
            }
            //! [5]
        }

        //! [4]
        Timer {
            id: reselectTimer
            interval: 10
            running: true
            repeat: true
            onTriggered: {
                scatterGraph.scene.selectionQueryPosition = Qt.point(inputArea.mouseX, inputArea.mouseY);
            }
        }
        //! [4]
    }

    //! [6]
    NumberAnimation {
        id: cameraAnimationX
        loops: Animation.Infinite
        running: true
        target: scatterGraph.scene.activeCamera
        property:"xRotation"
        from: 0.0
        to: 360.0
        duration: 20000
    }
    //! [6]


    //! [7]
    SequentialAnimation {
        id: cameraAnimationY
        loops: Animation.Infinite
        running: true

        NumberAnimation {
            target: scatterGraph.scene.activeCamera
            property:"yRotation"
            from: 5.0
            to: 45.0
            duration: 9000
            easing.type: Easing.InOutSine
        }

        NumberAnimation {
            target: scatterGraph.scene.activeCamera
            property:"yRotation"
            from: 45.0
            to: 5.0
            duration: 9000
            easing.type: Easing.InOutSine
        }
    }
    //! [7]

    RowLayout {
        id: buttonLayout
        Layout.minimumHeight: shadowToggle.height
        width: parent.width
        anchors.left: parent.left
        spacing: 0

        Button {
            id: shadowToggle
            Layout.fillHeight: true
            Layout.minimumWidth: parent.width / 3 // 3 buttons divided equally in the layout
            text: scatterGraph.shadowsSupported ? "Hide Shadows" : "Shadows not supported"
            enabled: scatterGraph.shadowsSupported

            onClicked: {
                if (scatterGraph.shadowQuality === AbstractGraph3D.ShadowQualityNone) {
                    scatterGraph.shadowQuality = AbstractGraph3D.ShadowQualityMedium;
                    text = "Hide Shadows";
                } else {
                    scatterGraph.shadowQuality = AbstractGraph3D.ShadowQualityNone;
                    text = "Show Shadows";
                }
            }
        }

        Button {
            id: cameraToggle
            Layout.fillHeight: true
            Layout.minimumWidth: parent.width / 3
            text: "Pause Camera"

            onClicked: {
                cameraAnimationX.paused = !cameraAnimationX.paused;
                cameraAnimationY.paused = cameraAnimationX.paused;
                if (cameraAnimationX.paused) {
                    text = "Animate Camera";
                } else {
                    text = "Pause Camera";
                }
            }
        }

        Button {
            id: exitButton
            Layout.fillHeight: true
            Layout.minimumWidth: parent.width / 3
            text: "Quit"
            onClicked: Qt.quit();
        }
    }
}
