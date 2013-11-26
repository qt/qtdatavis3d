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
import QtDataVisualization 1.0
import "."

Item {
    id: mainview
    width: 1280
    height: 720
    visible: true

    Data {
        id: surfaceData
    }

    Item {
        id: surfaceView
        width: mainview.width - surfaceGridToggle.width
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
            theme: Theme3D { type: AbstractGraph3D.ThemeStoneMoss }
            shadowQuality: AbstractGraph3D.ShadowQualityMedium
            selectionMode: AbstractGraph3D.SelectionSlice | AbstractGraph3D.SelectionItemAndRow
            smoothSurfaceEnabled: true
            surfaceGridEnabled: false
            font.family: "STCaiyun"
            font.pointSize: 35
            scene.activeCamera.cameraPreset: AbstractGraph3D.CameraPresetIsometricLeft
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
            gradient: surfaceGradient

            Surface3DSeries {
                id: surfaceSeries

                ItemModelSurfaceDataProxy {
                    itemModel: surfaceData.model
                    rowRole: "longitude"
                    columnRole: "latitude"
                    valueRole: "height"
                }
            }

            // Since flat is not supported on all platforms, and changes back to smooth
            // asynchronously on those platforms, handle button text on changed
            // signal handler rather than when we set the value.
            onSmoothSurfaceEnabledChanged: {
                if (enabled === true) {
                    smoothSurfaceToggle.text = "Show Flat"
                } else {
                    smoothSurfaceToggle.text = "Show Smooth"
                }
            }
        }
    }

    // TODO: Kept outside until surface supports multiple added series (QTRD-2579)
    Surface3DSeries {
        id: heightSeries

        HeightMapSurfaceDataProxy {
            heightMapFile: ":/heightmaps/image"
            // We don't want the default data values set by heightmap proxy.
            minZValue: 30
            maxZValue: 60
            minXValue: 67
            maxXValue: 97
        }
    }

    NewButton {
        id: surfaceGridToggle
        anchors.top: parent.top
        anchors.left: parent.left
        width: 200
        text: "Show Surface Grid"
        //! [1]
        onClicked: {
            if (surfaceplot.surfaceGridEnabled === false) {
                surfaceplot.surfaceGridEnabled = true;
                text = "Hide Surface Grid"
            } else {
                surfaceplot.surfaceGridEnabled = false;
                text = "Show Surface Grid"
            }
        }
        //! [1]
    }

    NewButton {
        id: surfaceToggle
        anchors.top: surfaceGridToggle.bottom
        width: surfaceGridToggle.width
        text: "Hide Surface"
        onClicked: {
            if (surfaceplot.seriesList[0].visible === true) {
                surfaceSeries.visible = false;
                heightSeries.visible = false;
                text = "Show Surface"
            } else {
                surfaceSeries.visible = true;
                heightSeries.visible = true;
                text = "Hide Surface"
            }
        }
    }

    NewButton {
        id: smoothSurfaceToggle
        anchors.top: surfaceToggle.bottom
        width: surfaceToggle.width
        text: "Show Flat"
        //! [2]
        onClicked: {
            if (surfaceplot.smoothSurfaceEnabled === true) {
                surfaceplot.smoothSurfaceEnabled = false;
            } else {
                surfaceplot.smoothSurfaceEnabled = true;
            }
        }
        //! [2]
    }

    NewButton {
        id: backgroundToggle
        anchors.top: smoothSurfaceToggle.bottom
        width: smoothSurfaceToggle.width
        text: "Hide Background"
        onClicked: {
            if (surfaceplot.backgroundVisible === true) {
                surfaceplot.backgroundVisible = false;
                text = "Show Background"
            } else {
                surfaceplot.backgroundVisible = true;
                text = "Hide Background"
            }
        }
    }

    NewButton {
        id: gridToggle
        anchors.top: backgroundToggle.bottom
        width: backgroundToggle.width
        text: "Hide Grid"
        onClicked: {
            if (surfaceplot.gridVisible === true) {
                surfaceplot.gridVisible = false;
                text = "Show Grid"
            } else {
                surfaceplot.gridVisible = true;
                text = "Hide Grid"
            }
        }
    }

    NewButton {
        id: seriesToggle
        anchors.top: gridToggle.bottom
        width: gridToggle.width
        text: "Switch to Item Model Series"
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
                middleGradient.position = 0.55
                text = "Switch to Item Model Series"
            }
        }
        //! [3]
    }
}
