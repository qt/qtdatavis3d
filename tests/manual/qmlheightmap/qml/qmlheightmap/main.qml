/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtDataVisualization
import "."

Item {
    id: mainwindow

    width: 1024
    height: 768
    visible: true

    Item {
        id: surfaceview
        width: mainwindow.width
        height: mainwindow.height

        anchors.top: mainwindow.top
        anchors.left: mainwindow.left

        ColorGradient {
            id: surfaceGradient
            ColorGradientStop { position: 0.0; color: "darkslategray" }
            ColorGradientStop { id: middleGradient; position: 0.50; color: "peru" }
            ColorGradientStop { position: 1.0; color: "red" }
        }

        Theme3D {
            id: mainTheme
            type: Q3DTheme.ThemeStoneMoss
            colorStyle: Q3DTheme.ColorStyleRangeGradient
            baseGradients: [surfaceGradient]
        }

        Surface3D {
            id: surfaceGraph
            width: surfaceview.width
            height: surfaceview.height
            theme: mainTheme
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
            axisY.title: "Y"
            axisX.title: "X"
            axisZ.title: "Z"

            Surface3DSeries {
                id: heightSeriesRGB8
                drawMode: Surface3DSeries.DrawSurface
                visible: true
                flatShadingEnabled: false

                HeightMapSurfaceDataProxy {
                    heightMapFile: ":/mapRGB8"
                    minYValue: surfaceGraph.axisY.min
                    maxYValue: surfaceGraph.axisY.max
                }
            }

            Surface3DSeries {
                id: heightSeriesRGB16
                drawMode: Surface3DSeries.DrawSurface
                visible: false
                flatShadingEnabled: false

                HeightMapSurfaceDataProxy {
                    heightMapFile: ":/mapRGB16"
                    minYValue: surfaceGraph.axisY.min
                    maxYValue: surfaceGraph.axisY.max
                }
            }

            Surface3DSeries {
                id: heightSeriesGRAY8
                drawMode: Surface3DSeries.DrawSurface
                visible: false
                flatShadingEnabled: false

                HeightMapSurfaceDataProxy {
                    heightMapFile: ":/mapGRAY8"
                    minYValue: surfaceGraph.axisY.min
                    maxYValue: surfaceGraph.axisY.max
                }
            }

            Surface3DSeries {
                id: heightSeriesGRAY16
                drawMode: Surface3DSeries.DrawSurface
                visible: false
                flatShadingEnabled: false

                HeightMapSurfaceDataProxy {
                    heightMapFile: ":/mapGRAY16"
                    minYValue: surfaceGraph.axisY.min
                    maxYValue: surfaceGraph.axisY.max
                }
            }
        }

        RowLayout {
            id: buttonLayout
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            Button {
                id: toggleHeightSeries
                Layout.fillWidth: true
                Layout.fillHeight: true

                text: qsTr("Use 16-bit rgb map")
                onClicked: {
                    if (heightSeriesRGB8.visible === true) {
                        heightSeriesRGB8.visible = false
                        heightSeriesRGB16.visible = true
                        heightSeriesGRAY8.visible = false
                        heightSeriesGRAY16.visible = false
                        text = "Use 8-bit grayscale map"
                    } else if (heightSeriesRGB16.visible === true){
                        heightSeriesRGB8.visible = false
                        heightSeriesRGB16.visible = false
                        heightSeriesGRAY8.visible = true
                        heightSeriesGRAY16.visible = false
                        text = "Use 16-bit grayscale map"
                    } else if (heightSeriesGRAY8.visible === true){
                        heightSeriesRGB8.visible = false
                        heightSeriesRGB16.visible = false
                        heightSeriesGRAY8.visible = false
                        heightSeriesGRAY16.visible = true
                        text = "Use 8-bit rgb map"
                    } else if (heightSeriesGRAY16.visible === true){
                        heightSeriesRGB8.visible = true
                        heightSeriesRGB16.visible = false
                        heightSeriesGRAY8.visible = false
                        heightSeriesGRAY16.visible = false
                        text = "Use 16-bit rgb map"
                    }
                }
            }

            Button {
                id: toggleAutoScaleY
                Layout.fillWidth: true
                Layout.fillHeight: true

                text: qsTr("Enable autoScaleY")
                onClicked: {
                    if (text === "Enable autoScaleY") {
                        heightSeriesRGB8.dataProxy.autoScaleY = true
                        heightSeriesRGB16.dataProxy.autoScaleY = true
                        heightSeriesGRAY8.dataProxy.autoScaleY = true
                        heightSeriesGRAY16.dataProxy.autoScaleY = true
                        text = "Disable autoScaleY"
                    } else {
                        heightSeriesRGB8.dataProxy.autoScaleY = false
                        heightSeriesRGB16.dataProxy.autoScaleY = false
                        heightSeriesGRAY8.dataProxy.autoScaleY = false
                        heightSeriesGRAY16.dataProxy.autoScaleY = false
                        text = "Enable autoScaleY"
                    }
                }
            }
        }
    }
}
