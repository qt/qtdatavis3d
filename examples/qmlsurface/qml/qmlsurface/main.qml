/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
import com.digia.QtDataVis3D 1.0
import "."

Item {
    id: mainview
    width: 1280
    height: 1024
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
            //shadowQuality: Surface3D.ShadowMedium
            font.pointSize: 35
            cameraPreset: Surface3D.PresetIsometricLeft
            itemLabelFormat: "X:@xLabel Y:@yLabel Z:@zLabel"
            dataProxy: surfaceData.proxy
//            axisX.segmentCount: 4
//            axisX.subSegmentCount: 1
//            axisX.labelFormat: "%.2f"
//            axisZ.segmentCount: 1
//            axisZ.subSegmentCount: 1
//            axisZ.labelFormat: "%.2f"
//            axisY.segmentCount: 1
//            axisY.subSegmentCount: 1
//            axisY.labelFormat: "%.2f"
        }
    }

    Button {
        id: surfaceGridToggle
        anchors.left: parent.left
        width: 200
        text: "Hide Surface Grid"
        onClicked: {
            if (surfaceplot.surfaceGrid == false) {
                surfaceplot.surfaceGrid = true;
                text = "Hide Surface Grid"
            } else {
                surfaceplot.surfaceGrid = false;
                text = "Show Surface Grid"
            }
        }
    }

    Button {
        id: smoothSurfaceToggle
        anchors.top: surfaceGridToggle.bottom
        width: surfaceGridToggle.width
        text: "Show Smooth"
        onClicked: {
            if (surfaceplot.smoothSurface == true) {
                surfaceplot.smoothSurface = false;
                text = "Show Smooth"
            } else {
                surfaceplot.smoothSurface = true;
                text = "Show Flat"
            }
        }
    }
}
