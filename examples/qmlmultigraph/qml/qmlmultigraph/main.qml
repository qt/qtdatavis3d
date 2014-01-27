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
import QtQuick.Layouts 1.0
import QtDataVisualization 1.0
import "."

Item {
    id: mainView
    width: 1280
    height: 400

    Data {
        id: data
    }

    //! [0]
    RowLayout {
        id: graphLayout
        spacing: 1
        anchors.top: buttonLayout.bottom
        anchors.bottom: mainView.bottom
        anchors.left: mainView.left
        anchors.right: mainView.right

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            border.color: surfaceGraph.theme.gridLineColor
            border.width: 2
            color: "#00000000" // Transparent

            Surface3D {
                id: surfaceGraph
                anchors.fill: parent
                anchors.margins: parent.border.width
                theme: Theme3D { type: Theme3D.ThemePrimaryColors }
                scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeftHigh

                Surface3DSeries {
                    itemLabelFormat: "Pop density at (@xLabel N, @zLabel E): @yLabel"
                    ItemModelSurfaceDataProxy {
                        itemModel: data.surfaceData
                        // Mapping model roles to surface series rows, columns, and values.
                        rowRole: "longitude"
                        columnRole: "latitude"
                        valueRole: "pop_density"
                    }
                }
            }
        }


        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            border.color: scatterGraph.theme.gridLineColor
            border.width: 2
            color: "#00000000" // Transparent

            Scatter3D {
                id: scatterGraph
                anchors.fill: parent
                anchors.margins: parent.border.width
                theme: Theme3D { type: Theme3D.ThemeDigia }
                scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeftHigh

                Scatter3DSeries {
                    ItemModelScatterDataProxy {
                        itemModel: data.scatterData
                        // Mapping model roles to scatter series item coordinates.
                        xPosRole: "xPos"
                        yPosRole: "yPos"
                        zPosRole: "zPos"
                    }
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            border.color: barGraph.theme.gridLineColor
            border.width: 2
            color: "#00000000" // Transparent

            Bars3D {
                id: barGraph
                anchors.fill: parent
                anchors.margins: parent.border.width
                theme: Theme3D { type: Theme3D.ThemeQt }
                selectionMode: AbstractGraph3D.SelectionItemAndRow | AbstractGraph3D.SelectionSlice
                scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeftHigh

                Bar3DSeries {
                    itemLabelFormat: "@seriesName for @colLabel, @rowLabel: @valueLabel"
                    name: "Yearly expenses"

                    ItemModelBarDataProxy {
                        itemModel: data.barData
                        // Mapping model roles to bar series rows, columns, and values.
                        rowRole: "year"
                        columnRole: "city"
                        valueRole: "expenses"
                    }
                }

                Bar3DSeries {
                    itemLabelFormat: "@seriesName for @colLabel, @rowLabel: @valueLabel"
                    name: "Yearly income"

                    ItemModelBarDataProxy {
                        itemModel: data.barData
                        // Mapping model roles to bar series rows, columns, and values.
                        rowRole: "year"
                        columnRole: "city"
                        valueRole: "income"
                    }
                }
            }
        }
    }
    //! [0]

    RowLayout {
        id: buttonLayout
        Layout.minimumHeight: exitButton.height
        width: parent.width
        anchors.left: parent.left
        anchors.top: parent.top
        spacing: 0

        NewButton {
            id: clearSelectionsButton
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: "Clear Selections"
            onClicked: {
                barGraph.clearSelection()
                scatterGraph.clearSelection()
                surfaceGraph.clearSelection()
            }
        }

        NewButton {
            id: exitButton
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: "Quit"
            onClicked: Qt.quit(0);
        }
    }
}
