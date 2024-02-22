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

//! [0]
import QtQuick
import QtQuick.Controls
import QtDataVisualization
import "."
//! [0]

//! [1]
Item {
    id: mainView
    //! [1]
    width: 500
    height: 500

    property bool portraitMode: width < height

    //! [4]
    Data {
        id: seriesData
    }
    //! [4]

    //! [13]
    Theme3D {
        id: themeIsabelle
        type: Theme3D.ThemeIsabelle
        font.pointSize: 40
    }
    //! [13]

    Theme3D {
        id: themeArmyBlue
        type: Theme3D.ThemeArmyBlue
    }

    //! [8]
    //! [9]
    Item {
        id: dataView
        anchors.bottom: parent.bottom
        //! [9]
        width: parent.width
        height: parent.height - (portraitMode ? shadowToggle.implicitHeight * 3 + 25
                                              : shadowToggle.implicitHeight + 10)
        //! [8]

        //! [2]
        Scatter3D {
            id: scatterGraph
            width: dataView.width
            height: dataView.height
            //! [2]
            //! [3]
            theme: themeIsabelle
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
                    itemModel: seriesData.model
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
                    itemModel: seriesData.modelTwo
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
                    itemModel: seriesData.modelThree
                    xPosRole: "xPos"
                    yPosRole: "yPos"
                    zPosRole: "zPos"
                }
            }
        }
    }

    //! [7]
    Button {
        id: shadowToggle
        width: portraitMode ? implicitWidth : (mainView.width / 6 - 6)
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 5
        text: scatterGraph.shadowsSupported ? "Hide Shadows" : "Shadows not supported"
        enabled: scatterGraph.shadowsSupported
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

    Button {
        id: smoothToggle
        width: portraitMode ? implicitWidth : (mainView.width / 6 - 6)
        anchors.left: shadowToggle.right
        anchors.top: parent.top
        anchors.margins: 5
        text: "Use Smooth for Series One"
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

    Button {
        id: cameraToggle
        width: portraitMode ? implicitWidth : (mainView.width / 6 - 6)
        anchors.left: portraitMode ? parent.left : smoothToggle.right
        anchors.top: portraitMode ? smoothToggle.bottom : parent.top
        anchors.margins: 5
        text: "Change Camera Placement"
        onClicked: {
            if (scatterGraph.scene.activeCamera.cameraPreset === Camera3D.CameraPresetFront) {
                scatterGraph.scene.activeCamera.cameraPreset =
                        Camera3D.CameraPresetIsometricRightHigh;
            } else {
                scatterGraph.scene.activeCamera.cameraPreset = Camera3D.CameraPresetFront;
            }
        }
    }

    Button {
        id: themeToggle
        width: portraitMode ? implicitWidth : (mainView.width / 6 - 6)
        anchors.left: cameraToggle.right
        anchors.top: portraitMode ? smoothToggle.bottom : parent.top
        anchors.margins: 5
        text: "Change Theme"
        onClicked: {
            if (scatterGraph.theme.type === Theme3D.ThemeArmyBlue) {
                scatterGraph.theme = themeIsabelle
            } else {
                scatterGraph.theme = themeArmyBlue
            }
            if (scatterGraph.theme.backgroundEnabled === true) {
                backgroundToggle.text = "Hide Background";
            } else {
                backgroundToggle.text = "Show Background";
            }
        }
    }

    Button {
        id: backgroundToggle
        width: portraitMode ? implicitWidth : (mainView.width / 6 - 6)
        anchors.left: portraitMode ? parent.left : themeToggle.right
        anchors.top: portraitMode ? themeToggle.bottom : parent.top
        anchors.margins: 5
        text: "Hide Background"
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

    Button {
        id: exitButton
        width: portraitMode ? implicitWidth : (mainView.width / 6 - 6)
        anchors.left: backgroundToggle.right
        anchors.top: portraitMode ? themeToggle.bottom : parent.top
        anchors.margins: 5
        text: "Quit"
        onClicked: Qt.quit();
    }
}
