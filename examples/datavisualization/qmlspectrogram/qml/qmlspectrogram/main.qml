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

import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import QtDataVisualization 1.2
import "."

Window {
    id: mainview
    title: "Qt Quick 2 Spectrogram Example"
    visible: true
    width: 1024
    height: 768
    color: surfaceGraph.theme.windowColor

    Data {
        id: surfaceData
    }

    Item {
        id: surfaceView
        width: mainview.width
        height: mainview.height
        anchors.top: mainview.top
        anchors.left: mainview.left

        //! [0]
        ColorGradient {
            id: surfaceGradient
            ColorGradientStop { position: 0.0; color: "black" }
            ColorGradientStop { position: 0.2; color: "red" }
            ColorGradientStop { position: 0.5; color: "blue" }
            ColorGradientStop { position: 0.8; color: "yellow" }
            ColorGradientStop { position: 1.0; color: "white" }
        }
        //! [0]

        ValueAxis3D {
            id: xAxis
            segmentCount: 8
            labelFormat: "%i"
            title: "Angle"
            titleVisible: true
            titleFixed: false
        }

        ValueAxis3D {
            id: yAxis
            segmentCount: 8
            labelFormat: "%i \%"
            title: "Value"
            titleVisible: true
            labelAutoRotation: 0
            titleFixed: false
        }

        ValueAxis3D {
            id: zAxis
            segmentCount: 5
            labelFormat: "%i"
            title: "Radius"
            titleVisible: true
            titleFixed: false
        }

        Theme3D {
            id: customTheme
            type: Theme3D.ThemeQt
            // Don't show specular spotlight as we don't want it to distort the colors
            lightStrength: 0.0
            ambientLightStrength: 0.9
            backgroundEnabled: false
            font.family: "Lucida Handwriting"
            font.pointSize: 25
        }

        Surface3D {
            id: surfaceGraph
            width: surfaceView.width
            height: surfaceView.height

            shadowQuality: AbstractGraph3D.ShadowQualityNone
            selectionMode: AbstractGraph3D.SelectionSlice | AbstractGraph3D.SelectionItemAndRow
            scene.activeCamera.cameraPreset: Camera3D.CameraPresetDirectlyAbove
            axisX: xAxis
            axisY: yAxis
            axisZ: zAxis

            theme: customTheme

            orthoProjection: true
            flipHorizontalGrid: true
            radialLabelOffset: 0.01  // Add little offset so the labels do not overlap

            Surface3DSeries {
                id: surfaceSeries
                flatShadingEnabled: false
                drawMode: Surface3DSeries.DrawSurface
                baseGradient: surfaceGradient
                colorStyle: Theme3D.ColorStyleRangeGradient

                ItemModelSurfaceDataProxy {
                    itemModel: surfaceData.model
                    rowRole: "radius"
                    columnRole: "angle"
                    yPosRole: "value"
                }
            }
        }
    }

    RowLayout {
        id: buttonLayout
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        opacity: 0.5

        NewButton {
            id: polarToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Switch to polar"
            //! [1]
            onClicked: {
                if (surfaceGraph.polar === false) {
                    surfaceGraph.polar = true
                    text = "Switch to cartesian"
                } else {
                    surfaceGraph.polar = false
                    text = "Switch to polar"
                }
            }
            //! [1]
        }

        NewButton {
            id: orthoToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Switch to perspective"
            onClicked: {
                if (surfaceGraph.orthoProjection === true) {
                    surfaceGraph.orthoProjection = false;
                    xAxis.labelAutoRotation = 30
                    yAxis.labelAutoRotation = 30
                    zAxis.labelAutoRotation = 30
                    text = "Switch to orthographic"
                } else {
                    surfaceGraph.orthoProjection = true;
                    surfaceGraph.scene.activeCamera.cameraPreset = Camera3D.CameraPresetDirectlyAbove
                    xAxis.labelAutoRotation = 0
                    yAxis.labelAutoRotation = 0
                    zAxis.labelAutoRotation = 0
                    text = "Switch to perspective"
                }
            }
        }

        NewButton {
            id: surfaceGridToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Toggle surface grid"
            onClicked: {
                if (surfaceSeries.drawMode & Surface3DSeries.DrawWireframe) {
                    surfaceSeries.drawMode &= ~Surface3DSeries.DrawWireframe;
                } else {
                    surfaceSeries.drawMode |= Surface3DSeries.DrawWireframe;
                }
            }
        }

        NewButton {
            id: flipGridToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Toggle axis grid on top"
            onClicked: {
                onClicked: {
                    if (surfaceGraph.flipHorizontalGrid === true) {
                        surfaceGraph.flipHorizontalGrid = false;
                    } else {
                        surfaceGraph.flipHorizontalGrid = true;
                    }
                }
            }
        }

        NewButton {
            id: labelOffsetToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Toggle radial label position"
            visible: surfaceGraph.polar
            onClicked: {
                if (surfaceGraph.radialLabelOffset > 1.0) {
                    surfaceGraph.radialLabelOffset = 0.01
                } else {
                    surfaceGraph.radialLabelOffset = 1.06
                }
            }
        }
    }

    Rectangle {
        id: legend
        anchors.margins: 20
        anchors.bottom: parent.bottom
        anchors.top: buttonLayout.bottom
        anchors.left: parent.left
        border.color: "black"
        border.width: 1
        width: 50
        rotation: 180
        gradient: Gradient {
            GradientStop { position: 0.0; color: "black" }
            GradientStop { position: 0.2; color: "red" }
            GradientStop { position: 0.5; color: "blue" }
            GradientStop { position: 0.8; color: "yellow" }
            GradientStop { position: 1.0; color: "white" }
        }
    }

    Text {
        anchors.verticalCenter: legend.bottom
        anchors.left: legend.right
        anchors.margins: 2
        text: surfaceGraph.axisY.min  + "%"
    }

    Text {
        anchors.verticalCenter: legend.verticalCenter
        anchors.left: legend.right
        anchors.margins: 2
        text: (surfaceGraph.axisY.max + surfaceGraph.axisY.min) / 2  + "%"
    }

    Text {
        anchors.verticalCenter: legend.top
        anchors.left: legend.right
        anchors.margins: 2
        text: surfaceGraph.axisY.max + "%"
    }
}
