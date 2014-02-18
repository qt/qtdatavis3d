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
import QtQuick.Controls 1.0
import QtDataVisualization 1.0
import "."

Item {
    id: mainview
    width: 1280
    height: 720

    Item {
        id: surfaceView
        width: mainview.width - buttonLayout.width
        height: mainview.height
        anchors.right: mainview.right;

        //! [0]
        ColorGradient {
            id: layerOneGradient
            ColorGradientStop { position: 0.0; color: "black" }
            ColorGradientStop { position: 0.31; color: "tan" }
            ColorGradientStop { position: 0.32; color: "green" }
            ColorGradientStop { position: 0.40; color: "darkslategray" }
            ColorGradientStop { position: 1.0; color: "white" }
        }

        ColorGradient {
            id: layerTwoGradient
            ColorGradientStop { position: 0.0; color: "red" }
            ColorGradientStop { position: 0.15; color: "black" }
        }

        ColorGradient {
            id: layerThreeGradient
            ColorGradientStop { position: 0.315; color: "blue" }
            ColorGradientStop { position: 0.33; color: "white" }
        }
        //! [0]

        Surface3D {
            id: surfaceLayers
            width: surfaceView.width
            height: surfaceView.height
            theme: Theme3D {
                type: Theme3D.ThemeEbony
                font.pointSize: 35
                colorStyle: Theme3D.ColorStyleRangeGradient
            }
            shadowQuality: AbstractGraph3D.ShadowQualityNone
            selectionMode: AbstractGraph3D.SelectionRow | AbstractGraph3D.SelectionSlice
            scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeft
            axisY.min: 20
            axisY.max: 200
            axisX.segmentCount: 5
            axisX.subSegmentCount: 2
            axisX.labelFormat: "%i"
            axisZ.segmentCount: 5
            axisZ.subSegmentCount: 2
            axisZ.labelFormat: "%i"
            axisY.segmentCount: 5
            axisY.subSegmentCount: 2
            axisY.labelFormat: "%i"

            //! [1]
            //! [2]
            Surface3DSeries {
                id: layerOneSeries
                baseGradient: layerOneGradient
                //! [2]
                HeightMapSurfaceDataProxy {
                    heightMapFile: ":/heightmaps/layer_1.png"
                }
                flatShadingEnabled: false
                drawMode: Surface3DSeries.DrawSurface
                //! [4]
                visible: layerOneToggle.checked // bind to checkbox state
                //! [4]
            }

            Surface3DSeries {
                id: layerTwoSeries
                baseGradient: layerTwoGradient
                HeightMapSurfaceDataProxy {
                    heightMapFile: ":/heightmaps/layer_2.png"
                }
                flatShadingEnabled: false
                drawMode: Surface3DSeries.DrawSurface
                visible: layerTwoToggle.checked // bind to checkbox state
            }

            Surface3DSeries {
                id: layerThreeSeries
                baseGradient: layerThreeGradient
                HeightMapSurfaceDataProxy {
                    heightMapFile: ":/heightmaps/layer_3.png"
                }
                flatShadingEnabled: false
                drawMode: Surface3DSeries.DrawSurface
                visible: layerThreeToggle.checked // bind to checkbox state
            }
            //! [1]
        }
    }

    ColumnLayout {
        id: buttonLayout
        anchors.top: parent.top
        anchors.left: parent.left
        spacing: 0

        //! [3]
        GroupBox {
            title: "Layer Selection"
            Layout.fillWidth: true
            Column {
                CheckBox {
                    id: layerOneToggle
                    text: "Show Ground Layer"
                    checked: true
                }

                CheckBox {
                    id: layerTwoToggle
                    text: "Show Tectonic Layer"
                    checked: true
                }

                CheckBox {
                    id: layerThreeToggle
                    text: "Show Sea Layer"
                    checked: true
                }
            }
        }
        //! [3]

        //! [5]
        GroupBox {
            title: "Layer Style"
            Layout.fillWidth: true
            Column {
                CheckBox {
                    id: layerOneGrid
                    text: "Show Ground as Grid"
                    onCheckedChanged: {
                        if (checked)
                            layerOneSeries.drawMode = Surface3DSeries.DrawWireframe
                        else
                            layerOneSeries.drawMode = Surface3DSeries.DrawSurface
                    }
                }

                CheckBox {
                    id: layerTwoGrid
                    text: "Show Tectonic as Grid"
                    onCheckedChanged: {
                        if (checked)
                            layerTwoSeries.drawMode = Surface3DSeries.DrawWireframe
                        else
                            layerTwoSeries.drawMode = Surface3DSeries.DrawSurface
                    }
                }

                CheckBox {
                    id: layerThreeGrid
                    text: "Show Sea as Grid"
                    onCheckedChanged: {
                        if (checked)
                            layerThreeSeries.drawMode = Surface3DSeries.DrawWireframe
                        else
                            layerThreeSeries.drawMode = Surface3DSeries.DrawSurface
                    }
                }
            }
        }
        //! [5]

        //! [6]
        NewButton {
            id: sliceButton
            text: "Slice All Layers"
            Layout.fillWidth: true
            Layout.minimumHeight: 40
            onClicked: {
                if (surfaceLayers.selectionMode & AbstractGraph3D.SelectionMultiSeries) {
                    surfaceLayers.selectionMode = AbstractGraph3D.SelectionRow
                            | AbstractGraph3D.SelectionSlice
                    text = "Slice All Layers"
                } else {
                    surfaceLayers.selectionMode = AbstractGraph3D.SelectionRow
                            | AbstractGraph3D.SelectionSlice
                            | AbstractGraph3D.SelectionMultiSeries
                    text = "Slice One Layer"
                }
            }
        }
        //! [6]

        NewButton {
            id: shadowButton
            text: "Show Shadows"
            Layout.fillWidth: true
            Layout.minimumHeight: 40
            onClicked: {
                if (surfaceLayers.shadowQuality === AbstractGraph3D.ShadowQualityNone) {
                    surfaceLayers.shadowQuality = AbstractGraph3D.ShadowQualityLow
                    text = "Hide Shadows"
                } else {
                    surfaceLayers.shadowQuality = AbstractGraph3D.ShadowQualityNone
                    text = "Show Shadows"
                }
            }
        }

        NewButton {
            id: aaButton
            text: "Disable Antialiasing"
            Layout.fillWidth: true
            Layout.minimumHeight: 40
            visible: false
            onClicked: {
                if (surfaceLayers.renderingMode === AbstractGraph3D.RenderIndirect_NoAA) {
                    surfaceLayers.renderingMode = AbstractGraph3D.RenderDirectToBackground
                    text = "Disable Antialiasing"
                } else {
                    surfaceLayers.renderingMode = AbstractGraph3D.RenderIndirect_NoAA
                    text = "Enable Antialiasing"
                }
            }
        }

        Component.onCompleted: {
            if (surfaceLayers.antialiasing) {
                aaButton.visible = true
            }
        }
    }
}
