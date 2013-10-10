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

//! [0]
import QtQuick 2.1
import com.digia.QtDataVisualization 1.0
import "."
//! [0]

//! [1]
Item {
    id: mainView
    visible: true
    //! [1]

    //! [4]
    Data {
        id: graphData
    }
    //! [4]

    //! [8]
    //! [9]
    Item {
        id: dataView
        anchors.bottom: parent.bottom
        //! [9]
        width: parent.width
        height: parent.height - shadowToggle.height
        //! [8]

        //! [2]
        Scatter3D {
            id: scatterGraph
            width: dataView.width
            height: dataView.height
            //! [2]
            //! [3]
            font.family: "Lucida Handwriting"
            font.pointSize: 40
            theme: Scatter3D.ThemeIsabelle
            shadowQuality: Scatter3D.ShadowQualitySoftLow
            //! [3]
            //! [5]
            dataProxy: graphData.proxy
            //! [5]
            //! [6]
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
            //! [6]
        }
    }

    //! [7]
    NewButton {
        id: shadowToggle
        width: parent.width / 6 // We're adding 6 buttons and want to divide them equally
        text: "Hide Shadows"
        onClicked: {
            if (scatterGraph.shadowQuality === Scatter3D.ShadowQualityNone) {
                scatterGraph.shadowQuality = Scatter3D.ShadowQualitySoftLow;
                text = "Hide Shadows";
            } else {
                scatterGraph.shadowQuality = Scatter3D.ShadowQualityNone;
                text = "Show Shadows";
            }
        }
    }
    //! [7]

    NewButton {
        id: smoothToggle
        width: parent.width / 6
        text: "Use Smooth Dots"
        anchors.left: shadowToggle.right
        onClicked: {
            if (scatterGraph.objectSmoothingEnabled === false) {
                text = "Use Flat Dots";
                scatterGraph.objectSmoothingEnabled = true;
            } else {
                text = "Use Smooth Dots"
                scatterGraph.objectSmoothingEnabled = false;
            }
        }
    }

    NewButton {
        id: cameraToggle
        width: parent.width / 6
        text: "Change Camera Placement"
        anchors.left: smoothToggle.right
        onClicked: {
            if (scatterGraph.scene.activeCamera.cameraPreset === Scatter3D.CameraPresetFront) {
                scatterGraph.scene.activeCamera.cameraPreset = Scatter3D.CameraPresetIsometricRightHigh;
            } else {
                scatterGraph.scene.activeCamera.cameraPreset = Scatter3D.CameraPresetFront;
            }
        }
    }

    NewButton {
        id: themeToggle
        width: parent.width / 6
        text: "Change Theme"
        anchors.left: cameraToggle.right
        onClicked: {
            if (scatterGraph.theme === Scatter3D.ThemeArmyBlue) {
                scatterGraph.theme = Scatter3D.ThemeIsabelle;
            } else {
                scatterGraph.theme = Scatter3D.ThemeArmyBlue;
            }
        }
    }

    NewButton {
        id: backgroundToggle
        width: parent.width / 6
        text: "Hide Background"
        anchors.left: themeToggle.right
        onClicked: {
            if (scatterGraph.backgroundVisible === true) {
                scatterGraph.backgroundVisible = false;
                text = "Show Background";
            } else {
                scatterGraph.backgroundVisible = true;
                text = "Hide Background";
            }
        }
    }

    NewButton {
        id: exitButton
        width: parent.width / 6
        text: "Quit"
        anchors.left: backgroundToggle.right
        onClicked: Qt.quit(0);
    }
}
