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
import QtQuick.Controls 1.0
import com.digia.QtDataVisualization 1.0
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

        Surface3D {
            id: surfaceplot
            width: surfaceView.width
            height: surfaceView.height
            shadowQuality: Surface3D.ShadowMedium
            smoothSurfaceEnabled: true
            surfaceGridEnabled: false
            font.family: "STCaiyun"
            font.pointSize: 35
            cameraPreset: Surface3D.PresetIsometricLeft
            dataProxy: surfaceData.proxy
            axisX.min: 0.0
            axisX.max: 99.0
            axisZ.min: 0.0
            axisZ.max: 99.0
            axisY.min: 0.0
            axisY.max: 50.0
            axisX.segmentCount: 10
            axisX.subSegmentCount: 5
            axisX.labelFormat: "%i"
            axisZ.segmentCount: 10
            axisZ.subSegmentCount: 5
            axisZ.labelFormat: "%i"
            axisY.segmentCount: 5
            axisY.subSegmentCount: 5
            axisY.labelFormat: "%i"
            Component.onCompleted: {
                setGradientColorAt(0, "black");
                setGradientColorAt(1, "white");
            }
        }
    }

    Button {
        id: surfaceGridToggle
        anchors.left: parent.left
        width: 200
        text: "Show Surface Grid"
        onClicked: {
            if (surfaceplot.surfaceGridEnabled == false) {
                surfaceplot.surfaceGridEnabled = true;
                text = "Hide Surface Grid"
            } else {
                surfaceplot.surfaceGridEnabled = false;
                text = "Show Surface Grid"
            }
        }
    }

    Button {
        id: smoothSurfaceToggle
        anchors.top: surfaceGridToggle.bottom
        width: surfaceGridToggle.width
        text: "Show Flat"
        onClicked: {
            if (surfaceplot.smoothSurfaceEnabled == true) {
                surfaceplot.smoothSurfaceEnabled = false;
                text = "Show Smooth"
            } else {
                surfaceplot.smoothSurfaceEnabled = true;
                text = "Show Flat"
            }
        }
    }

    Button {
        id: backgroundToggle
        anchors.top: smoothSurfaceToggle.bottom
        width: smoothSurfaceToggle.width
        text: "Hide Background"
        onClicked: {
            if (surfaceplot.backgroundVisible == true) {
                surfaceplot.backgroundVisible = false;
                text = "Show Background"
            } else {
                surfaceplot.backgroundVisible = true;
                text = "Hide Background"
            }
        }
    }

    Button {
        id: gridToggle
        anchors.top: backgroundToggle.bottom
        width: backgroundToggle.width
        text: "Hide Grid"
        onClicked: {
            if (surfaceplot.gridVisible == true) {
                surfaceplot.gridVisible = false;
                text = "Show Grid"
            } else {
                surfaceplot.gridVisible = true;
                text = "Hide Grid"
            }
        }
    }
}
