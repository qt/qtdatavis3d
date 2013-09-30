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
    visible: true

    Data {
        id: chartData
    }

    Item {
        id: dataView
        width: parent.width
        height: parent.height - shadowToggle.height
        anchors.bottom: parent.bottom

        Scatter3D {
            id: testscatter
            width: dataView.width
            height: dataView.height
            font.family: "Lucida Handwriting"
            font.pointSize: 40
            dataProxy: chartData.proxy
            theme: Scatter3D.ThemeIsabelle
            shadowQuality: Scatter3D.ShadowSoftLow
            selectionMode: Scatter3D.ModeItem
            itemLabelFormat: "X:@xLabel Y:@yLabel Z:@zLabel"
            axisX.segmentCount: 3
            axisX.subSegmentCount: 2
            axisX.labelFormat: "%.2f"
            axisZ.segmentCount: 2
            axisZ.subSegmentCount: 2
            axisZ.labelFormat: "%.2f"
            axisY.segmentCount: 2
            axisY.subSegmentCount: 2
            axisY.labelFormat: "%.2f"
        }
    }

    Button {
        id: shadowToggle
        width: parent.width / 6
        text: "Hide Shadows"
        onClicked: {
            if (testscatter.shadowQuality === Scatter3D.ShadowNone) {
                testscatter.shadowQuality = Scatter3D.ShadowSoftLow;
                text = "Hide Shadows";
            } else {
                testscatter.shadowQuality = Scatter3D.ShadowNone;
                text = "Show Shadows";
            }
        }
    }

    Button {
        id: smoothToggle
        width: parent.width / 6
        text: "Use Smooth Dots"
        anchors.left: shadowToggle.right
        onClicked: {
            if (testscatter.objectSmoothingEnabled === false) {
                text = "Use Flat Dots";
                testscatter.objectSmoothingEnabled = true;
            } else {
                text = "Use Smooth Dots"
                testscatter.objectSmoothingEnabled = false;
            }
        }
    }

    Button {
        id: cameraToggle
        width: parent.width / 6
        text: "Change Camera Placement"
        anchors.left: smoothToggle.right
        onClicked: {
            if (testscatter.cameraPreset === Scatter3D.PresetFront) {
                testscatter.cameraPreset = Scatter3D.PresetIsometricRightHigh;
            } else {
                testscatter.cameraPreset = Scatter3D.PresetFront;
            }
        }
    }

    Button {
        id: themeToggle
        width: parent.width / 6
        text: "Change Theme"
        anchors.left: cameraToggle.right
        onClicked: {
            if (testscatter.theme === Scatter3D.ThemeArmyBlue) {
                testscatter.theme = Scatter3D.ThemeIsabelle;
            } else {
                testscatter.theme = Scatter3D.ThemeArmyBlue;
            }
        }
    }

    Button {
        id: backgroundToggle
        width: parent.width / 6
        text: "Hide Background"
        anchors.left: themeToggle.right
        onClicked: {
            if (testscatter.backgroundVisible === true) {
                testscatter.backgroundVisible = false;
                text = "Show Background";
            } else {
                testscatter.backgroundVisible = true;
                text = "Hide Background";
            }
        }
    }
    Button {
        id: exitButton
        width: parent.width / 6
        text: "Quit"
        anchors.left: backgroundToggle.right
        onClicked: Qt.quit(0);
    }
}
