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
import QtDataVisualization 1.0
import "."
//! [0]

//! [1]
Item {
    id: mainView
    visible: true
    //! [1]

    //! [4]
    Data {
        id: seriesOneData
    }
    //! [4]

    Data {
        id: seriesTwoData
    }

    Data {
        id: seriesThreeData
    }

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
            theme: Theme3D {
                type: Theme3D.ThemeIsabelle
                font.family: "Lucida Handwriting"
                font.pointSize: 40
            }
            shadowQuality: AbstractGraph3D.ShadowQualitySoftLow
            //! [3]
            //! [6]
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
            //! [5]
            Scatter3DSeries {
                id: scatterSeries
                //! [5]
                //! [10]
                itemLabelFormat: "Series 1: X:@xLabel Y:@yLabel Z:@zLabel"
                //! [10]

                //! [11]
                ItemModelScatterDataProxy {
                    itemModel: seriesOneData.model
                    xPosRole: "xPos"
                    yPosRole: "yPos"
                    zPosRole: "zPos"
                }
                //! [11]
            }

            //! [12]
            Scatter3DSeries {
                id: scatterSeriesTwo
                itemLabelFormat: "Series 2: X:@xLabel Y:@yLabel Z:@zLabel"
                itemSize: 0.1
                mesh: Abstract3DSeries.MeshCube
                //! [12]

                ItemModelScatterDataProxy {
                    itemModel: seriesTwoData.modelTwo
                    xPosRole: "xPos"
                    yPosRole: "yPos"
                    zPosRole: "zPos"
                }
            }
            Scatter3DSeries {
                id: scatterSeriesThree
                itemLabelFormat: "Series 3: X:@xLabel Y:@yLabel Z:@zLabel"
                itemSize: 0.2
                mesh: Abstract3DSeries.MeshMinimal

                ItemModelScatterDataProxy {
                    itemModel: seriesThreeData.modelThree
                    xPosRole: "xPos"
                    yPosRole: "yPos"
                    zPosRole: "zPos"
                }
            }
        }
    }

    //! [7]
    NewButton {
        id: shadowToggle
        width: parent.width / 6 // We're adding 6 buttons and want to divide them equally
        text: "Hide Shadows"
        onClicked: {
            if (scatterGraph.shadowQuality === AbstractGraph3D.ShadowQualityNone) {
                scatterGraph.shadowQuality = AbstractGraph3D.ShadowQualitySoftLow;
                text = "Hide Shadows";
            } else {
                scatterGraph.shadowQuality = AbstractGraph3D.ShadowQualityNone;
                text = "Show Shadows";
            }
        }
    }
    //! [7]

    NewButton {
        id: smoothToggle
        width: parent.width / 6
        text: "Use Smooth for Series One"
        anchors.left: shadowToggle.right
        onClicked: {
            if (scatterSeries.meshSmooth === false) {
                text = "Use Flat for Series One";
                scatterSeries.meshSmooth = true;
            } else {
                text = "Use Smooth for Series One"
                scatterSeries.meshSmooth = false;
            }
        }
    }

    NewButton {
        id: cameraToggle
        width: parent.width / 6
        text: "Change Camera Placement"
        anchors.left: smoothToggle.right
        onClicked: {
            if (scatterGraph.scene.activeCamera.cameraPreset === Camera3D.CameraPresetFront) {
                scatterGraph.scene.activeCamera.cameraPreset = Camera3D.CameraPresetIsometricRightHigh;
            } else {
                scatterGraph.scene.activeCamera.cameraPreset = Camera3D.CameraPresetFront;
            }
        }
    }

    NewButton {
        id: themeToggle
        width: parent.width / 6
        text: "Change Theme"
        anchors.left: cameraToggle.right
        onClicked: {
            if (scatterGraph.theme.type === Theme3D.ThemeArmyBlue) {
                // Ownership of the theme is transferred and old theme is destroyed when setting
                // a new one, so we need to create them dynamically
                scatterGraph.theme = Qt.createQmlObject('import QtDataVisualization 1.0; Theme3D {type: Theme3D.ThemeIsabelle; font.family: "Lucida Handwriting"; font.pointSize: 40}', parent);
            } else {
                scatterGraph.theme = Qt.createQmlObject('import QtDataVisualization 1.0; Theme3D {type: Theme3D.ThemeArmyBlue}', parent);
            }
        }
    }

    NewButton {
        id: backgroundToggle
        width: parent.width / 6
        text: "Hide Background"
        anchors.left: themeToggle.right
        onClicked: {
            if (scatterGraph.theme.backgroundEnabled === true) {
                scatterGraph.theme.backgroundEnabled = false;
                text = "Show Background";
            } else {
                scatterGraph.theme.backgroundEnabled = true;
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
