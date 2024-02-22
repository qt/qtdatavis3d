/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtDataVisualization 1.2
import "."

Item {
    id: mainview
    width: 1280
    height: 720

    property real fontSize: 12
    property bool portraitMode: width < height

    Item {
        id: surfaceView
        width: mainview.width - settings.width
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
            ColorGradientStop { position: 0.315; color: "blue" }
            ColorGradientStop { position: 0.33; color: "white" }
        }

        ColorGradient {
            id: layerThreeGradient
            ColorGradientStop { position: 0.0; color: "red" }
            ColorGradientStop { position: 0.15; color: "black" }
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

    Flickable {
        id: settings
        width: buttonLayout.width
        contentHeight: buttonLayout.height
        height: mainview.height

        ColumnLayout {
            id: buttonLayout
            anchors.top: parent.top
            anchors.left: parent.left
            spacing: 0

            //! [3]
            GroupBox {
                Layout.fillWidth: true
                Column {
                    spacing: 10

                    Label {
                        font.pointSize: fontSize
                        font.bold: true
                        text: portraitMode ? "Layer\nSelection" : "Layer Selection"
                    }

                    CheckBox {
                        id: layerOneToggle
                        checked: true
                        text: portraitMode ? "Show\nGround\nLayer" : "Show Ground Layer"
                    }

                    CheckBox {
                        id: layerTwoToggle
                        checked: true
                        text: portraitMode ? "Show\nSea\nLayer" : "Show Sea Layer"
                    }

                    CheckBox {
                        id: layerThreeToggle
                        checked: true
                        text: portraitMode ? "Show\nTectonic\nLayer" : "Show Tectonic Layer"
                    }
                }
            }
            //! [3]

            //! [5]
            GroupBox {
                Layout.fillWidth: true
                Column {
                    spacing: 10

                    Label {
                        font.pointSize: fontSize
                        font.bold: true
                        text: portraitMode ? "Layer\nStyle" : "Layer Style"
                    }

                    CheckBox {
                        id: layerOneGrid
                        text: portraitMode ? "Show\nGround\nas Grid" : "Show Ground as Grid"
                        onCheckedChanged: {
                            if (checked)
                                layerOneSeries.drawMode = Surface3DSeries.DrawWireframe
                            else
                                layerOneSeries.drawMode = Surface3DSeries.DrawSurface
                        }
                    }

                    CheckBox {
                        id: layerTwoGrid
                        text: portraitMode ? "Show\nSea\nas Grid" : "Show Sea as Grid"

                        onCheckedChanged: {
                            if (checked)
                                layerTwoSeries.drawMode = Surface3DSeries.DrawWireframe
                            else
                                layerTwoSeries.drawMode = Surface3DSeries.DrawSurface
                        }
                    }

                    CheckBox {
                        id: layerThreeGrid
                        text: portraitMode ? "Show\nTectonic\nas Grid" : "Show Tectonic as Grid"
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
            Button {
                id: sliceButton
                text: portraitMode ? "Slice\nAll\nLayers" : "Slice All Layers"
                Layout.fillWidth: true
                Layout.minimumHeight: 40
                onClicked: {
                    if (surfaceLayers.selectionMode & AbstractGraph3D.SelectionMultiSeries) {
                        surfaceLayers.selectionMode = AbstractGraph3D.SelectionRow
                                | AbstractGraph3D.SelectionSlice
                        text = portraitMode ? "Slice\nAll\nLayers" : "Slice All Layers"
                    } else {
                        surfaceLayers.selectionMode = AbstractGraph3D.SelectionRow
                                | AbstractGraph3D.SelectionSlice
                                | AbstractGraph3D.SelectionMultiSeries
                        text = portraitMode ? "Slice\nOne\nLayer" : "Slice One Layer"
                    }
                }
            }
            //! [6]

            Button {
                id: shadowButton
                Layout.fillWidth: true
                Layout.minimumHeight: 40
                text: surfaceLayers.shadowsSupported ? (portraitMode ? "Show\nShadows" : "Show Shadows")
                                                     : (portraitMode ? "Shadows\nnot\nsupported" : "Shadows not supported")
                enabled: surfaceLayers.shadowsSupported
                onClicked: {
                    if (surfaceLayers.shadowQuality === AbstractGraph3D.ShadowQualityNone) {
                        surfaceLayers.shadowQuality = AbstractGraph3D.ShadowQualityLow
                        text = portraitMode ? "Hide\nShadows" : "Hide Shadows"
                    } else {
                        surfaceLayers.shadowQuality = AbstractGraph3D.ShadowQualityNone
                        text = portraitMode ? "Show\nShadows" : "Show Shadows"
                    }
                }
            }

            Button {
                id: renderModeButton
                text: portraitMode ? "Switch\nRender\nMode" : "Switch Render Mode"
                Layout.fillWidth: true
                Layout.minimumHeight: 40
                onClicked: {
                    var modeText = "Indirect, "
                    var aaText
                    if (surfaceLayers.renderingMode === AbstractGraph3D.RenderIndirect &&
                            surfaceLayers.msaaSamples === 0) {
                        surfaceLayers.renderingMode = AbstractGraph3D.RenderDirectToBackground
                        modeText = "BackGround, "
                    } else if (surfaceLayers.renderingMode === AbstractGraph3D.RenderIndirect &&
                               surfaceLayers.msaaSamples === 4) {
                        surfaceLayers.renderingMode = AbstractGraph3D.RenderIndirect
                        surfaceLayers.msaaSamples = 0
                    } else if (surfaceLayers.renderingMode === AbstractGraph3D.RenderIndirect &&
                               surfaceLayers.msaaSamples === 8) {
                        surfaceLayers.renderingMode = AbstractGraph3D.RenderIndirect
                        surfaceLayers.msaaSamples = 4
                    } else {
                        surfaceLayers.renderingMode = AbstractGraph3D.RenderIndirect
                        surfaceLayers.msaaSamples = 8
                    }

                    if (surfaceLayers.msaaSamples <= 0) {
                        aaText = "No AA"
                    } else {
                        aaText = surfaceLayers.msaaSamples + "xMSAA"
                    }

                    renderLabel.text = modeText + aaText
                }
            }

            TextField {
                id: renderLabel
                font.pointSize: fontSize
                Layout.fillWidth: true
                Layout.minimumHeight: 40
                color: "gray"
                enabled: false
                horizontalAlignment: TextInput.AlignHCenter
                wrapMode: TextField.WrapAtWordBoundaryOrAnywhere
                text: "Indirect, " + surfaceLayers.msaaSamples + "xMSAA"
            }
        }
    }
}
