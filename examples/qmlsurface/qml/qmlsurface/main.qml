/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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
import QtDataVisualization 1.0
import "."

Item {
    id: mainview
    width: 1280
    height: 720

    property int buttonMaximumWidth: surfaceGridToggle.width
    property int buttonMinimumHeight: seriesToggle.height

    Data {
        id: surfaceData
    }

    Item {
        id: surfaceView
        width: mainview.width - buttonLayout.width
        height: mainview.height
        anchors.right: mainview.right;

        //! [0]
        ColorGradient {
            id: surfaceGradient
            ColorGradientStop { position: 0.0; color: "darkslategray" }
            ColorGradientStop { id: middleGradient; position: 0.25; color: "peru" }
            ColorGradientStop { position: 1.0; color: "red" }
        }
        //! [0]

        Surface3D {
            id: surfaceplot
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
                    valueRole: "height"
                }
                //! [6]

                onFlatShadingSupportedChanged: {
                    flatShadingToggle.text = "Flat not supported"
                }
            }
        }
    }

    // TODO: Kept outside until surface supports multiple added series (QTRD-2579)
    //! [4]
    Surface3DSeries {
        id: heightSeries
        flatShadingEnabled: false
        drawMode: Surface3DSeries.DrawSurface

        HeightMapSurfaceDataProxy {
            heightMapFile: ":/heightmaps/image"
            // We don't want the default data values set by heightmap proxy.
            minZValue: 30
            maxZValue: 60
            minXValue: 67
            maxXValue: 97
        }
    }
    //! [4]
    ColumnLayout {
        id: buttonLayout
        anchors.top: parent.top
        anchors.left: parent.left
        spacing: 0

        NewButton {
            id: surfaceGridToggle
            Layout.maximumWidth: buttonMaximumWidth
            Layout.fillWidth: true
            Layout.minimumHeight: buttonMinimumHeight
            text: "Show Surface Grid"
            //! [1]
            onClicked: {
                if (surfaceSeries.drawMode & Surface3DSeries.DrawWireframe) {
                    surfaceSeries.drawMode &= ~Surface3DSeries.DrawWireframe;
                    heightSeries.drawMode &= ~Surface3DSeries.DrawWireframe;
                    text = "Show Surface Grid"
                } else {
                    surfaceSeries.drawMode |= Surface3DSeries.DrawWireframe;
                    heightSeries.drawMode |= Surface3DSeries.DrawWireframe;
                    text = "Hide Surface Grid"
                }
            }
            //! [1]
        }

        NewButton {
            id: surfaceToggle
            Layout.maximumWidth: buttonMaximumWidth
            Layout.fillWidth: true
            Layout.minimumHeight: buttonMinimumHeight
            text: "Hide Surface"
            //! [8]
            onClicked: {
                if (surfaceSeries.drawMode & Surface3DSeries.DrawSurface) {
                    surfaceSeries.drawMode &= ~Surface3DSeries.DrawSurface;
                    heightSeries.drawMode &= ~Surface3DSeries.DrawSurface;
                    text = "Show Surface"
                } else {
                    surfaceSeries.drawMode |= Surface3DSeries.DrawSurface;
                    heightSeries.drawMode |= Surface3DSeries.DrawSurface;
                    text = "Hide Surface"
                }
            }
            //! [8]
        }

        NewButton {
            id: flatShadingToggle
            Layout.maximumWidth: buttonMaximumWidth
            Layout.fillWidth: true
            Layout.minimumHeight: buttonMinimumHeight

            text: "Show Flat"
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
            Layout.maximumWidth: buttonMaximumWidth
            Layout.fillWidth: true
            Layout.minimumHeight: buttonMinimumHeight
            text: "Hide Background"
            onClicked: {
                if (surfaceplot.theme.backgroundEnabled === true) {
                    surfaceplot.theme.backgroundEnabled = false;
                    text = "Show Background"
                } else {
                    surfaceplot.theme.backgroundEnabled = true;
                    text = "Hide Background"
                }
            }
        }

        NewButton {
            id: gridToggle
            Layout.maximumWidth: buttonMaximumWidth
            Layout.fillWidth: true
            Layout.minimumHeight: buttonMinimumHeight
            text: "Hide Grid"
            onClicked: {
                if (surfaceplot.theme.gridEnabled === true) {
                    surfaceplot.theme.gridEnabled = false;
                    text = "Show Grid"
                } else {
                    surfaceplot.theme.gridEnabled = true;
                    text = "Hide Grid"
                }
            }
        }

        NewButton {
            id: seriesToggle
            Layout.maximumWidth: buttonMaximumWidth
            Layout.fillWidth: true
            Layout.minimumHeight: buttonMinimumHeight
            text: "Switch to Height Map Series"
            //! [3]
            onClicked: {
                if (surfaceplot.seriesList[0] === heightSeries) {
                    surfaceplot.axisY.max = 500.0
                    surfaceplot.seriesList = [surfaceSeries]
                    middleGradient.position = 0.25
                    text = "Switch to Height Map Series"
                } else {
                    surfaceplot.axisY.max = 250.0
                    surfaceplot.seriesList = [heightSeries]
                    middleGradient.position = 0.50
                    text = "Switch to Item Model Series"
                }
            }
            //! [3]
        }
    }
}
