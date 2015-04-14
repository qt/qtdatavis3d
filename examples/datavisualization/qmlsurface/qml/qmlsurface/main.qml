/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtDataVisualization 1.0
import "."

Rectangle {
    id: mainview
    width: 1024
    height: 768
    color: surfacePlot.theme.windowColor

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
                onDrawModeChanged: checkState()
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

                onDrawModeChanged: checkState()
            }
            //! [4]
        }
    }

    RowLayout {
        id: buttonLayout
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        opacity: 0.5

        NewButton {
            id: surfaceGridToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Show Surface Grid"
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

        NewButton {
            id: surfaceToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Hide Surface"
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

        NewButton {
            id: flatShadingToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: surfaceSeries.flatShadingSupported ? "Show Flat" : "Flat not supported"
            enabled: surfaceSeries.flatShadingSupported
            //! [2]
            onClicked: {
                if (surfaceSeries.flatShadingEnabled === true) {
                    surfaceSeries.flatShadingEnabled = false;
                    heightSeries.flatShadingEnabled = false;
                    text = "Show Flat"
                } else {
                    surfaceSeries.flatShadingEnabled = true;
                    heightSeries.flatShadingEnabled = true;
                    text = "Show Smooth"
                }
            }
            //! [2]
        }

        NewButton {
            id: backgroundToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Hide Background"
            onClicked: {
                if (surfacePlot.theme.backgroundEnabled === true) {
                    surfacePlot.theme.backgroundEnabled = false;
                    text = "Show Background"
                } else {
                    surfacePlot.theme.backgroundEnabled = true;
                    text = "Hide Background"
                }
            }
        }

        NewButton {
            id: gridToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Hide Grid"
            onClicked: {
                if (surfacePlot.theme.gridEnabled === true) {
                    surfacePlot.theme.gridEnabled = false;
                    text = "Show Grid"
                } else {
                    surfacePlot.theme.gridEnabled = true;
                    text = "Hide Grid"
                }
            }
        }

        NewButton {
            id: seriesToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
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
            surfaceToggle.text = "Hide Surface"
        else
            surfaceToggle.text = "Show Surface"

        if (surfaceSeries.drawMode & Surface3DSeries.DrawWireframe)
            surfaceGridToggle.text = "Hide Surface Grid"
        else
            surfaceGridToggle.text = "Show Surface Grid"
    }
}
