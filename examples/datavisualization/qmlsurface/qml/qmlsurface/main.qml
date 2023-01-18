// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Controls
import QtDataVisualization

Rectangle {
    id: mainview
    width: 1024
    height: 768
    color: surfacePlot.theme.windowColor

    property bool portraitMode: width < height

    Data {
        id: surfaceData
    }

    Item {
        id: surfaceView
        anchors.top: buttons.bottom
        anchors.bottom: mainview.bottom
        anchors.left: mainview.left
        anchors.right: mainview.right

        //! [0]
        ColorGradient {
            id: surfaceGradient
            ColorGradientStop { position: 0.0; color: "darkslategray" }
            ColorGradientStop { id: middleGradient; position: 0.25; color: "peru" }
            ColorGradientStop { position: 1.0; color: "red" }
        }
        //! [0]

        Surface3D {
            id: surfacePlot
            width: surfaceView.width
            height: surfaceView.height
            //! [7]
            theme: Theme3D {
                type: Theme3D.ThemeStoneMoss
                font.family: "STCaiyun"
                font.pointSize: 35
                colorStyle: Theme3D.ColorStyleRangeGradient
                baseGradients: [surfaceGradient]
            }
            //! [7]
            shadowQuality: AbstractGraph3D.ShadowQualityMedium
            selectionMode: AbstractGraph3D.SelectionSlice | AbstractGraph3D.SelectionItemAndRow
            scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeft
            scene.activeCamera.zoomLevel: 75
            axisY.min: 0.0
            axisY.max: 500.0
            axisX.segmentCount: 10
            axisX.subSegmentCount: 2
            axisX.labelFormat: "%i"
            axisZ.segmentCount: 10
            axisZ.subSegmentCount: 2
            axisZ.labelFormat: "%i"
            axisY.segmentCount: 5
            axisY.subSegmentCount: 2
            axisY.labelFormat: "%i"
            axisY.title: "Height"
            axisX.title: "Latitude"
            axisZ.title: "Longitude"

            //! [5]
            Surface3DSeries {
                id: surfaceSeries
                flatShadingEnabled: false
                drawMode: Surface3DSeries.DrawSurface

                ItemModelSurfaceDataProxy {
                    //! [5]
                    //! [6]
                    itemModel: surfaceData.model
                    rowRole: "longitude"
                    columnRole: "latitude"
                    yPosRole: "height"
                }
                //! [6]
                onDrawModeChanged: mainview.checkState()
            }
            //! [4]
            Surface3DSeries {
                id: heightSeries
                flatShadingEnabled: false
                drawMode: Surface3DSeries.DrawSurface
                visible: false

                HeightMapSurfaceDataProxy {
                    heightMapFile: ":/heightmaps/image"
                    // We don't want the default data values set by heightmap proxy.
                    minZValue: 30
                    maxZValue: 60
                    minXValue: 67
                    maxXValue: 97
                }

                onDrawModeChanged: mainview.checkState()
            }
            //! [4]
        }
    }

    Item {
        id: buttons
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        height: mainview.portraitMode ? surfaceGridToggle.implicitHeight * 4 + 15
                                      : surfaceGridToggle.implicitHeight * 3 + 10
        opacity: 0.5

        Button {
            id: surfaceGridToggle
            anchors.margins: 5
            anchors.left: parent.left
            anchors.top: parent.top
            width: mainview.portraitMode ? (mainview.width - 35) / 2 : (mainview.width - 40) / 3
            text: "Show Surface\nGrid"
            //! [1]
            onClicked: {
                if (surfaceSeries.drawMode & Surface3DSeries.DrawWireframe) {
                    surfaceSeries.drawMode &= ~Surface3DSeries.DrawWireframe;
                    heightSeries.drawMode &= ~Surface3DSeries.DrawWireframe;
                } else {
                    surfaceSeries.drawMode |= Surface3DSeries.DrawWireframe;
                    heightSeries.drawMode |= Surface3DSeries.DrawWireframe;
                }
            }
            //! [1]
        }

        Button {
            id: surfaceGridColor
            anchors.margins: 5
            anchors.left: surfaceGridToggle.right
            anchors.top: parent.top
            width: mainview.portraitMode ? (mainview.width - 35) / 2 : (mainview.width - 40) / 3
            text: "Red surface\ngrid color"
            onClicked: {
                if (Qt.colorEqual(surfaceSeries.wireframeColor, "#000000")
                        && Qt.colorEqual(heightSeries.wireframeColor, "#000000")) {
                    surfaceSeries.wireframeColor = "red"
                    heightSeries.wireframeColor = "red"
                    text = "Black surface\ngrid color"
                }
                else {
                    surfaceSeries.wireframeColor = "black"
                    heightSeries.wireframeColor = "black"
                    text = "Red surface\ngrid color"
                }
            }
        }

        Button {
            id: surfaceToggle
            anchors.margins: 5
            anchors.left: mainview.portraitMode ? parent.left : surfaceGridColor.right
            anchors.top: mainview.portraitMode ? surfaceGridColor.bottom : parent.top
            width: mainview.portraitMode ? (mainview.width - 35) / 2 : (mainview.width - 40) / 3
            text: "Hide\nSurface"
            //! [8]
            onClicked: {
                if (surfaceSeries.drawMode & Surface3DSeries.DrawSurface) {
                    surfaceSeries.drawMode &= ~Surface3DSeries.DrawSurface;
                    heightSeries.drawMode &= ~Surface3DSeries.DrawSurface;
                } else {
                    surfaceSeries.drawMode |= Surface3DSeries.DrawSurface;
                    heightSeries.drawMode |= Surface3DSeries.DrawSurface;
                }
            }
            //! [8]
        }

        Button {
            id: flatShadingToggle
            anchors.margins: 5
            anchors.left: mainview.portraitMode ? surfaceToggle.right : parent.left
            anchors.top: mainview.portraitMode ? surfaceGridColor.bottom : surfaceToggle.bottom
            width: mainview.portraitMode ? (mainview.width - 35) / 2 : (mainview.width - 40) / 3
            text: surfaceSeries.flatShadingSupported ? "Show\nFlat" : "Flat not\nsupported"
            enabled: surfaceSeries.flatShadingSupported
            //! [2]
            onClicked: {
                if (surfaceSeries.flatShadingEnabled === true) {
                    surfaceSeries.flatShadingEnabled = false;
                    heightSeries.flatShadingEnabled = false;
                    text = "Show\nFlat"
                } else {
                    surfaceSeries.flatShadingEnabled = true;
                    heightSeries.flatShadingEnabled = true;
                    text = "Show\nSmooth"
                }
            }
            //! [2]
        }

        Button {
            id: backgroundToggle
            anchors.margins: 5
            anchors.left: mainview.portraitMode ? parent.left : flatShadingToggle.right
            anchors.top: mainview.portraitMode ? flatShadingToggle.bottom : surfaceToggle.bottom
            width: mainview.portraitMode ? (mainview.width - 35) / 2 : (mainview.width - 40) / 3
            text: "Hide\nBackground"
            onClicked: {
                if (surfacePlot.theme.backgroundEnabled === true) {
                    surfacePlot.theme.backgroundEnabled = false;
                    text = "Show\nBackground"
                } else {
                    surfacePlot.theme.backgroundEnabled = true;
                    text = "Hide\nBackground"
                }
            }
        }

        Button {
            id: gridToggle
            anchors.margins: 5
            anchors.left: backgroundToggle.right
            anchors.top: mainview.portraitMode ? flatShadingToggle.bottom : surfaceToggle.bottom
            width: mainview.portraitMode ? (mainview.width - 35) / 2 : (mainview.width - 40) / 3
            text: "Hide\nGrid"
            onClicked: {
                if (surfacePlot.theme.gridEnabled === true) {
                    surfacePlot.theme.gridEnabled = false;
                    text = "Show\nGrid"
                } else {
                    surfacePlot.theme.gridEnabled = true;
                    text = "Hide\nGrid"
                }
            }
        }

        Button {
            id: seriesToggle
            anchors.margins: 5
            anchors.left: parent.left
            anchors.top: gridToggle.bottom
            width: mainview.width - 30
            text: "Switch to Height Map Series"
            //! [3]
            onClicked: {
                if (surfaceSeries.visible === false) {
                    surfacePlot.axisY.max = 500.0
                    surfaceSeries.visible = true
                    heightSeries.visible = false
                    middleGradient.position = 0.25
                    text = "Switch to Height Map Series"
                } else {
                    surfacePlot.axisY.max = 250.0
                    surfaceSeries.visible = false
                    heightSeries.visible = true
                    middleGradient.position = 0.50
                    text = "Switch to Item Model Series"
                }
            }
            //! [3]
        }
    }

    function checkState() {
        if (surfaceSeries.drawMode & Surface3DSeries.DrawSurface)
            surfaceToggle.text = "Hide\nSurface"
        else
            surfaceToggle.text = "Show\nSurface"

        if (surfaceSeries.drawMode & Surface3DSeries.DrawWireframe)
            surfaceGridToggle.text = "Hide Surface\nGrid"
        else
            surfaceGridToggle.text = "Show Surface\nGrid"
    }
}
