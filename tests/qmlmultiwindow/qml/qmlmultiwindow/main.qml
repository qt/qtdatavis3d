/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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
import QtQuick.Window 2.1
import QtQuick.Layouts 1.0
import QtDataVisualization 1.0
import "."

Rectangle {
    id: mainView
    width: 800
    height: 600

    Data {
        id: data
    }

    Window {
        id: firstWindow
        x: 100
        y: 100
        width: 500
        height: 500
        visible: true
        Rectangle {
            id: firstRect
            color: "red"
            anchors.fill: parent
        }
    }

    Window {
        id: secondWindow
        x: 100
        y: 700
        width: 500
        height: 500
        visible: true
        Rectangle {
            id: secondRect
            color: "blue"
            anchors.fill: parent
        }
    }

    states: [
        State {
            name: "firstWindow"
            ParentChange { target: surfaceGraph; parent: firstRect; x: 0; y: 0 }
        },
        State {
            name: "secondWindow"
            ParentChange { target: surfaceGraph; parent: secondRect; x: 0; y: 0 }
        }
    ]

    state: "firstWindow"

    //! [0]
    GridLayout {
        id: gridLayout
        columns: 2
        Layout.fillHeight: true
        Layout.fillWidth: true
        anchors.top: mainView.top
        anchors.bottom: mainView.bottom
        anchors.left: mainView.left
        anchors.right: mainView.right

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            border.color: surfaceGraph.theme.gridLineColor
            border.width: 2

            Surface3D {
                id: surfaceGraph
                anchors.fill: parent
                anchors.margins: parent.border.width
                theme: Theme3D {
                    type: Theme3D.ThemePrimaryColors
                    font.pointSize: 60
                }
                scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeftHigh

                Surface3DSeries {
                    itemLabelFormat: "Pop density at (@xLabel N, @zLabel E): @yLabel"
                    ItemModelSurfaceDataProxy {
                        itemModel: data.data
                        // The surface data points are not neatly lined up in rows and columns,
                        // so we define explicit row and column roles.
                        rowRole: "row"
                        columnRole: "col"
                        xPosRole: "latitude"
                        zPosRole: "longitude"
                        yPosRole: "pop_density"
                    }
                }
            }
        }

        // We'll use one grid cell for buttons
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true

            GridLayout {
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                columns: 2

                NewButton {
                    Layout.minimumWidth: parent.width / 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: "Move graph between windows"
                    onClicked: {
                        if (mainView.state === "firstWindow") {
                            mainView.state = "secondWindow"
                        } else {
                            mainView.state = "firstWindow"
                        }
                    }
                }

                NewButton {
                    Layout.minimumWidth: parent.width / 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: "Quit"
                    onClicked: Qt.quit(0);
                }

                NewButton {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: "Reset Cameras"
                    onClicked: resetCameras() // call a helper function to keep button itself simpler
                }

                NewButton {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: "Toggle Mesh Styles"
                    onClicked: toggleMeshStyle() // call a helper function to keep button itself simpler
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            border.color: scatterGraph.theme.gridLineColor
            border.width: 2

            Scatter3D {
                id: scatterGraph
                anchors.fill: parent
                anchors.margins: parent.border.width
                theme: Theme3D {
                    type: Theme3D.ThemeDigia
                    font.pointSize: 60
                }
                scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeftHigh

                Scatter3DSeries {
                    itemLabelFormat: "Pop density at (@xLabel N, @zLabel E): @yLabel"
                    ItemModelScatterDataProxy {
                        itemModel: data.data
                        // Mapping model roles to scatter series item coordinates.
                        xPosRole: "latitude"
                        zPosRole: "longitude"
                        yPosRole: "pop_density"
                    }
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            border.color: barGraph.theme.gridLineColor
            border.width: 2

            Bars3D {
                id: barGraph
                anchors.fill: parent
                anchors.margins: parent.border.width
                theme: Theme3D {
                    type: Theme3D.ThemeQt
                    font.pointSize: 60
                }
                selectionMode: AbstractGraph3D.SelectionItemAndRow | AbstractGraph3D.SelectionSlice
                scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeftHigh

                Bar3DSeries {
                    itemLabelFormat: "@seriesName: @valueLabel"
                    name: "Population density"

                    ItemModelBarDataProxy {
                        itemModel: data.data
                        // Mapping model roles to bar series rows, columns, and values.
                        rowRole: "row"
                        columnRole: "col"
                        valueRole: "pop_density"
                    }
                }
            }
        }
    }
    //! [0]

    function clearSelections() {
        barGraph.clearSelection()
        scatterGraph.clearSelection()
        surfaceGraph.clearSelection()
    }

    function resetCameras() {
        surfaceGraph.scene.activeCamera.cameraPreset = Camera3D.CameraPresetIsometricLeftHigh
        scatterGraph.scene.activeCamera.cameraPreset = Camera3D.CameraPresetIsometricLeftHigh
        barGraph.scene.activeCamera.cameraPreset = Camera3D.CameraPresetIsometricLeftHigh
        surfaceGraph.scene.activeCamera.zoomLevel = 100.0
        scatterGraph.scene.activeCamera.zoomLevel = 100.0
        barGraph.scene.activeCamera.zoomLevel = 100.0
    }

    function toggleMeshStyle() {
        if (barGraph.seriesList[0].meshSmooth === true) {
            barGraph.seriesList[0].meshSmooth = false
            if (surfaceGraph.seriesList[0].flatShadingSupported)
                surfaceGraph.seriesList[0].flatShadingEnabled = true
            scatterGraph.seriesList[0].meshSmooth = false
        } else {
            barGraph.seriesList[0].meshSmooth = true
            surfaceGraph.seriesList[0].flatShadingEnabled = false
            scatterGraph.seriesList[0].meshSmooth = true
        }
    }
}
