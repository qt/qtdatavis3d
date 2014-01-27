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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtDataVisualization 1.0
import "."

Item {
    id: mainView
    width: 800
    height: 600

    property int buttonLayoutHeight: 180;

    Data {
        id: graphData
    }

    Theme3D {
        id: firstTheme
        type: Theme3D.ThemeQt
    }

    Theme3D {
        id: secondTheme
        type: Theme3D.ThemeEbony
    }

    Item {
        id: dataView
        anchors.top: buttonLayout.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Bars3D {
            id: barGraph
            anchors.fill: parent
            selectionMode: AbstractGraph3D.SelectionItemAndRow
            scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeftHigh
            theme: firstTheme
            valueAxis.labelFormat: "%d\u00B0C"

            Bar3DSeries {
                id: station1
                name: "Station 1"
                itemLabelFormat: "Temperature at @seriesName for @colLabel, @rowLabel: @valueLabel"

                ItemModelBarDataProxy {
                    itemModel: graphData.model
                    rowRole: "year"
                    columnRole: "month"
                    valueRole: "s1"
                }
            }
            Bar3DSeries {
                id: station2
                name: "Station 2"
                itemLabelFormat: "Temperature at @seriesName for @colLabel, @rowLabel: @valueLabel"

                ItemModelBarDataProxy {
                    itemModel: graphData.model
                    rowRole: "year"
                    columnRole: "month"
                    valueRole: "s2"
                }
            }
            Bar3DSeries {
                id: station3
                name: "Station 3"
                itemLabelFormat: "Temperature at @seriesName for @colLabel, @rowLabel: @valueLabel"

                ItemModelBarDataProxy {
                    itemModel: graphData.model
                    rowRole: "year"
                    columnRole: "month"
                    valueRole: "s2"
                }
            }
        }
    }

    Rectangle {
        property int legendLocation: 3

        id: legendPanel
        width: 200
        height: 100
        border.color: barGraph.theme.labelTextColor
        border.width: 2
        color: "#00000000" // Transparent

        //! [0]
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: parent.border.width
            spacing: 0
            clip: true
            LegendItem {
                Layout.fillWidth: true
                Layout.fillHeight: true
                series: station1
                theme: barGraph.theme
                onColorChanged: legendPanel.relayout()
            }
            LegendItem {
                Layout.fillWidth: true
                Layout.fillHeight: true
                series: station2
                theme: barGraph.theme
                onColorChanged: legendPanel.relayout()
            }
            LegendItem {
                Layout.fillWidth: true
                Layout.fillHeight: true
                series: station3
                theme: barGraph.theme
                onColorChanged: legendPanel.relayout()
            }
        }
        //! [0]

        function relayout() {
            // Workaround for a layout bug that causes transparent colors to use black background
            // instead of what is actually under the items if just the color changes.
            // Forcing a relayout by adjusting layout's available area fixes the background.
            var originalWidth = border.width
            border.width = originalWidth + 1
            border.width = originalWidth
        }

        states: [
            State  {
                name: "topleft"
                when: legendPanel.legendLocation === 1
                AnchorChanges {
                    target: legendPanel
                    anchors.top: dataView.top
                    anchors.bottom: undefined
                    anchors.left: dataView.left
                    anchors.right: undefined
                }
            },
            State  {
                name: "topright"
                when: legendPanel.legendLocation === 2
                AnchorChanges {
                    target: legendPanel
                    anchors.top: dataView.top
                    anchors.bottom: undefined
                    anchors.left: undefined
                    anchors.right: dataView.right
                }
            },
            State  {
                name: "bottomleft"
                when: legendPanel.legendLocation === 3
                AnchorChanges {
                    target: legendPanel
                    anchors.top: undefined
                    anchors.bottom: dataView.bottom
                    anchors.left: dataView.left
                    anchors.right: undefined
                }
            },
            State  {
                name: "bottomright"
                when: legendPanel.legendLocation === 4
                AnchorChanges {
                    target: legendPanel
                    anchors.top: undefined
                    anchors.bottom: dataView.bottom
                    anchors.left: undefined
                    anchors.right: dataView.right
                }
            }
        ]
    }

    RowLayout {
        id: buttonLayout
        Layout.minimumHeight: themeToggle.height
        width: parent.width
        anchors.left: parent.left
        spacing: 0

        NewButton {
            id: themeToggle
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: "Change Theme"
            onClicked: {
                if (barGraph.theme === firstTheme) {
                    barGraph.theme = secondTheme
                } else {
                    barGraph.theme = firstTheme
                }
            }
        }
        NewButton {
            id: repositionLegend
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: "Reposition Legend"
            onClicked: {
                if (legendPanel.legendLocation === 4) {
                    legendPanel.legendLocation = 1
                } else {
                    legendPanel.legendLocation++
                }
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
