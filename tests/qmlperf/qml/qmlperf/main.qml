/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtDataVisualization 1.1
import "."

Rectangle {
    id: mainview
    width: 1280
    height: 1024

    property var itemCount: 1000.0
    property var addItems: 500.0

    Button {
        id: changeButton
        width: parent.width / 7
        height: 50
        anchors.left: parent.left
        enabled: true
        text: "Change"
        onClicked: {
            console.log("changeButton clicked");
            if (graphView.state == "meshsphere") {
                graphView.state = "meshcube"
            } else if (graphView.state == "meshcube") {
                graphView.state = "meshpyramid"
            } else if (graphView.state == "meshpyramid") {
                graphView.state = "meshpoint"
            } else if (graphView.state == "meshpoint") {
                graphView.state = "meshsphere"
            }
        }
    }

    Text {
        id: fpsText
        text: "Reading"
        width: (parent.width / 7) * 3
        height: 50
        anchors.left: changeButton.right
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Button {
        id: optimization
        width: parent.width / 7
        height: 50
        anchors.left: fpsText.right
        enabled: true
        text: scatterPlot.optimizationHints === AbstractGraph3D.OptimizationDefault ? "To Static" : "To Default"
        onClicked: {
            console.log("Optimization");
            if (scatterPlot.optimizationHints === AbstractGraph3D.OptimizationDefault) {
                scatterPlot.optimizationHints = AbstractGraph3D.OptimizationStatic;
                optimization.text = "To Default";
            } else {
                scatterPlot.optimizationHints = AbstractGraph3D.OptimizationDefault;
                optimization.text = "To Static";
            }
        }
    }

    Button {
        id: itemAdd
        width: parent.width / 7
        height: 50
        anchors.left: optimization.right
        enabled: true
        text: "Add"
        onClicked: {
            itemCount = itemCount + addItems;
            dataGenerator.add(scatterSeries, addItems);
        }
    }

    Button {
        id: writeLine
        width: parent.width / 7
        height: 50
        anchors.left: itemAdd.right
        enabled: true
        text: "Write"
        onClicked: {
            dataGenerator.writeLine(itemCount, scatterPlot.currentFps.toFixed(1));
        }
    }

    Item {
        id: graphView
        width: mainview.width
        height: mainview.height
        anchors.top: changeButton.bottom
        anchors.left: mainview.left
        state: "meshsphere"

        Scatter3D {
            id: scatterPlot
            width: graphView.width
            height: graphView.height
            shadowQuality: AbstractGraph3D.ShadowQualityNone
            optimizationHints: AbstractGraph3D.OptimizationDefault
            scene.activeCamera.yRotation: 45.0
            measureFps: true
            onCurrentFpsChanged: {
                fpsText.text = itemCount + " : " + scatterPlot.currentFps.toFixed(1);
            }

//            theme: Theme3D {
//                type: Theme3D.ThemeRetro
//                colorStyle: Theme3D.ColorStyleRangeGradient
//                baseGradients: customGradient

//                ColorGradient {
//                    id: customGradient
//                    ColorGradientStop { position: 1.0; color: "red" }
//                    ColorGradientStop { position: 0.0; color: "blue" }
//                }
//            }

            Scatter3DSeries {
                id: scatterSeries
                mesh: Abstract3DSeries.MeshSphere
            }

            Component.onCompleted: dataGenerator.generateData(scatterSeries, itemCount);
        }

        states: [
            State {
                name: "meshsphere"
                StateChangeScript {
                    name: "doSphere"
                    script: {
                        console.log("Do the sphere");
                        scatterSeries.mesh = Abstract3DSeries.MeshSphere;
                    }
                }
            },
            State {
                name: "meshcube"
                StateChangeScript {
                    name: "doCube"
                    script: {
                        console.log("Do the cube");
                        scatterSeries.mesh = Abstract3DSeries.MeshCube;
                    }
                }
            },
            State {
                name: "meshpyramid"
                StateChangeScript {
                    name: "doPyramid"
                    script: {
                        console.log("Do the pyramid");
                        scatterSeries.mesh = Abstract3DSeries.MeshPyramid;
                    }
                }
            },
            State {
                name: "meshpoint"
                StateChangeScript {
                    name: "doPoint"
                    script: {
                        console.log("Do the point");
                        scatterSeries.mesh = Abstract3DSeries.MeshPoint;
                    }
                }
            }
        ]
    }
}
