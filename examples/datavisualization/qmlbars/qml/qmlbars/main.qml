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
import QtQuick.Window 2.0
import "."

Rectangle {
    id: mainview
    width: 1280
    height: 1024

    property int buttonLayoutHeight: 180;
    state: Screen.width < Screen.height ? "portrait" : "landscape"

    Data {
        id: graphData
    }

    Axes {
        id: graphAxes
    }

    property Bar3DSeries selectedSeries
    selectedSeries: barSeries

    function handleSelectionChange(series, position) {
        if (position != series.invalidSelectionPosition) {
            selectedSeries = series
        }

        // Set tableView current row to selected bar
        var rowRole = series.dataProxy.rowLabels[position.x];
        var colRole = series.dataProxy.columnLabels[position.y];
        var currentRow = tableView.currentRow
        if (currentRow === -1 || rowRole !== graphData.model.get(currentRow).year
                || colRole !== graphData.model.get(currentRow).month) {
            var totalRows = tableView.rowCount;
            for (var i = 0; i < totalRows; i++) {
                var currentRowRole = graphData.model.get(i).year
                var currentColRole = graphData.model.get(i).month
                if (currentRowRole === rowRole && currentColRole === colRole) {
                    tableView.currentRow = i
                    // Workaround to 5.2 row selection issue
                    if (typeof tableView.selection != "undefined") {
                        tableView.selection.clear()
                        tableView.selection.select(i)
                    }
                    break
                }
            }
        }
    }

    Item {
        id: dataView
        anchors.right: mainview.right;
        anchors.bottom: mainview.bottom

        Bars3D {
            id: barGraph
            width: dataView.width
            height: dataView.height
            shadowQuality: AbstractGraph3D.ShadowQualityMedium
            selectionMode: AbstractGraph3D.SelectionItem
            theme: Theme3D {
                type: Theme3D.ThemeRetro
                labelBorderEnabled: true
                font.pointSize: 35
                labelBackgroundEnabled: true
                colorStyle: Theme3D.ColorStyleRangeGradient
                singleHighlightGradient: customGradient

                ColorGradient {
                    id: customGradient
                    ColorGradientStop { position: 1.0; color: "#FFFF00" }
                    ColorGradientStop { position: 0.0; color: "#808000" }
                }
            }
            barThickness: 0.7
            barSpacing: Qt.size(0.5, 0.5)
            barSpacingRelative: false
            scene.activeCamera.cameraPreset: Camera3D.CameraPresetIsometricLeftHigh
            columnAxis: graphAxes.column
            valueAxis: graphAxes.income

            //! [3]
            Bar3DSeries {
                id: barSeries
                itemLabelFormat: "Income for @colLabel, @rowLabel: @valueLabel"
                baseGradient: barGradient

                ItemModelBarDataProxy {
                    id: modelProxy
                    itemModel: graphData.model
                    rowRole: "year"
                    columnRole: "month"
                    valueRole: "income"
                }
                //! [3]

                ColorGradient {
                    id: barGradient
                    ColorGradientStop { position: 1.0; color: "#00FF00" }
                    ColorGradientStop { position: 0.0; color: "#006000" }
                }

                onSelectedBarChanged: handleSelectionChange(barSeries, position)
            }

            //! [4]
            Bar3DSeries {
                id: secondarySeries
                visible: false
                itemLabelFormat: "Expenses for @colLabel, @rowLabel: @valueLabel"
                baseGradient: secondaryGradient

                ItemModelBarDataProxy {
                    id: secondaryProxy
                    itemModel: graphData.model
                    rowRole: "year"
                    columnRole: "month"
                    valueRole: "expenses"
                }
                //! [4]

                ColorGradient {
                    id: secondaryGradient
                    ColorGradientStop { position: 1.0; color: "#FF0000" }
                    ColorGradientStop { position: 0.0; color: "#600000" }
                }

                onSelectedBarChanged: handleSelectionChange(secondarySeries, position)
            }
        }
    }

    TableView {
        id: tableView
        anchors.top: parent.top
        anchors.left: parent.left
        TableViewColumn{ role: "year"  ; title: "Year" ; width: tableView.width / 4 }
        TableViewColumn{ role: "month" ; title: "Month" ; width: tableView.width / 4 }
        TableViewColumn{ role: "expenses" ; title: "Expenses" ; width: tableView.width / 4 }
        TableViewColumn{ role: "income" ; title: "Income" ; width: tableView.width / 4 }
        model: graphData.model

        //! [2]
        onCurrentRowChanged: {
            var rowIndex = modelProxy.rowCategoryIndex(graphData.model.get(currentRow).year)
            var colIndex = modelProxy.columnCategoryIndex(graphData.model.get(currentRow).month)
            if (selectedSeries.visible)
                mainview.selectedSeries.selectedBar = Qt.point(rowIndex, colIndex)
            else if (barSeries.visible)
                barSeries.selectedBar = Qt.point(rowIndex, colIndex)
            else
                secondarySeries.selectedBar = Qt.point(rowIndex, colIndex)
        }
        //! [2]
    }

    ColumnLayout {
        id: controlLayout
        spacing: 0

        Button {
            id: dataToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Show 2010 - 2012"
            clip: true
            //! [1]
            onClicked: {
                if (barGraph.rowAxis.max !== 6) {
                    text = "Show 2010 - 2012"
                    modelProxy.autoRowCategories = true
                    secondaryProxy.autoRowCategories = true
                } else {
                    text = "Show all years"
                    // Explicitly defining row categories, since we do not want to show data for
                    // all years in the model, just for the selected ones.
                    modelProxy.autoRowCategories = false
                    secondaryProxy.autoRowCategories = false
                    modelProxy.rowCategories = ["2010", "2011", "2012"]
                    secondaryProxy.rowCategories = ["2010", "2011", "2012"]
                }
            }
            //! [1]
        }

        Button {
            id: shadowToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Hide Shadows"
            clip: true
            onClicked: {
                if (barGraph.shadowQuality == AbstractGraph3D.ShadowQualityNone) {
                    barGraph.shadowQuality = AbstractGraph3D.ShadowQualityMedium;
                    text = "Hide Shadows"
                } else {
                    barGraph.shadowQuality = AbstractGraph3D.ShadowQualityNone;
                    text = "Show Shadows"
                }
            }
        }

        Button {
            id: seriesToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Show Expenses"
            clip: true
            //! [0]
            onClicked: {
                if (!secondarySeries.visible) {
                    text = "Show Both"
                    barGraph.valueAxis = graphAxes.expenses
                    barSeries.visible = false
                    secondarySeries.visible = true
                } else if (!barSeries.visible){
                    barSeries.visible = true
                    text = "Show Income"
                    barGraph.valueAxis = graphAxes.income
                } else {
                    secondarySeries.visible = false
                    text = "Show Expenses"
                    barGraph.valueAxis = graphAxes.income
                }
            }
            //! [0]
        }
    }

    states: [
        State  {
            name: "landscape"
            PropertyChanges {
                target: dataView
                width: mainview.width / 4 * 3
                height: mainview.height
            }
            PropertyChanges  {
                target: tableView
                height: mainview.height - buttonLayoutHeight
                anchors.right: dataView.left
                anchors.left: mainview.left
                anchors.bottom: undefined
            }
            PropertyChanges  {
                target: controlLayout
                width: mainview.width / 4
                height: buttonLayoutHeight
                anchors.top: tableView.bottom
                anchors.bottom: mainview.bottom
                anchors.left: mainview.left
                anchors.right: dataView.left
            }
        },
        State  {
            name: "portrait"
            PropertyChanges {
                target: dataView
                width: mainview.height / 4 * 3
                height: mainview.width
            }
            PropertyChanges  {
                target: tableView
                height: mainview.width
                anchors.right: controlLayout.left
                anchors.left: mainview.left
                anchors.bottom: dataView.top
            }
            PropertyChanges  {
                target: controlLayout
                width: mainview.height / 4
                height: mainview.width / 4
                anchors.top: mainview.top
                anchors.bottom: dataView.top
                anchors.left: undefined
                anchors.right: mainview.right
            }
        }
    ]
}
