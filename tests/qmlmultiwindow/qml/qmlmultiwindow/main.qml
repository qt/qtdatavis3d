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

    property QtObject surfaceWindowObject;
    property string surfaceWindowStr:
    "\n
        import QtQuick 2.1\n
        import QtQuick.Window 2.1\n
        import QtQuick.Layouts 1.0\n
        import QtDataVisualization 1.0\n
        import \".\"\n
        Window {\n
               Data {\n
                   id: data\n
               }\n
        id: firstWindow\n
        x: 100\n
        y: 100\n
        width: 500\n
        height: 500\n
        visible: true\n
        Rectangle {\n
            id: firstRect\n
            color: \"red\"\n
            anchors.fill: parent\n
            Surface3D {\n
                id: surfaceGraph\n
                anchors.fill: parent\n
                anchors.margins: parent.border.width\n
                theme: Theme3D {\n
                    type: Theme3D.ThemePrimaryColors\n
                    font.pointSize: 60\n
                }\n
                scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeftHigh\n
                Surface3DSeries {\n
                    itemLabelFormat: \"Pop density at (@xLabel N, @zLabel E): @yLabel\"\n
                    ItemModelSurfaceDataProxy {\n
                        itemModel: data.myData\n
                        rowRole: \"row\"\n
                        columnRole: \"col\"\n
                        xPosRole: \"latitude\"\n
                        zPosRole: \"longitude\"\n
                        yPosRole: \"pop_density\"\n
                    }\n
                }\n
            }\n
        }\n
    }"

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

    function destroyWindow() {
        if (surfaceWindowObject != null)
            surfaceWindowObject.destroy()
    }

    Component.onDestruction: destroyWindow()

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
            border.width: 2
        }

        Timer {
            id: windowToggleTimer
            interval: 1000
            running: false
            repeat: false
            onTriggered: {
                destroyWindow()
                surfaceWindowObject = Qt.createQmlObject(surfaceWindowStr, mainView)
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
                    text: "(re)construct surface window in a loop"
                    onClicked: {
                        windowToggleTimer.running = true
                        windowToggleTimer.repeat = true
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
                        itemModel: data.myData
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
                        itemModel: data.myData
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
    }

    function resetCameras() {
        scatterGraph.scene.activeCamera.cameraPreset = Camera3D.CameraPresetIsometricLeftHigh
        barGraph.scene.activeCamera.cameraPreset = Camera3D.CameraPresetIsometricLeftHigh
        scatterGraph.scene.activeCamera.zoomLevel = 100.0
        barGraph.scene.activeCamera.zoomLevel = 100.0
    }

    function toggleMeshStyle() {
        if (barGraph.seriesList[0].meshSmooth === true) {
            barGraph.seriesList[0].meshSmooth = false
            scatterGraph.seriesList[0].meshSmooth = false
        } else {
            barGraph.seriesList[0].meshSmooth = true
            scatterGraph.seriesList[0].meshSmooth = true
        }
    }
}
