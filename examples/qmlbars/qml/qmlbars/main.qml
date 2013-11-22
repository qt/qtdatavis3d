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
import QtDataVisualization 1.0
import "."

Item {
    id: mainview
    width: 1280
    height: 1024
    visible: true

    Data {
        id: graphData
    }

    Axes {
        id: graphAxes
    }

    property Bar3DSeries selectedSeries
    selectedSeries: barSeries

    function handleSelectionChange(series, position) {
        if (position != series.invalidSelectionPosition()) {
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
        width: parent.width - tableView.width
        height: parent.height
        anchors.right: parent.right;

        Bars3D {
            id: testGraph
            width: dataView.width
            height: dataView.height
            shadowQuality: AbstractGraph3D.ShadowQualityMedium
            selectionMode: AbstractGraph3D.SelectionItem
            font.pointSize: 35
            theme: Theme3D { type: AbstractGraph3D.ThemeRetro }
            labelStyle: AbstractGraph3D.LabelStyleFromTheme
            barThickness: 0.7
            barSpacing: Qt.size(0.5, 0.5)
            barSpacingRelative: false
            scene.activeCamera.cameraPreset: AbstractGraph3D.CameraPresetIsometricLeftHigh
            columnAxis: graphAxes.column
            valueAxis: graphAxes.income

            Bar3DSeries {
                id: barSeries
                itemLabelFormat: "Income for @colLabel, @rowLabel: @valueLabel"

                ItemModelBarDataProxy {
                    id: modelProxy
                    activeMapping: graphData.mapping
                    itemModel: graphData.model
                }

                onSelectedBarChanged: handleSelectionChange(barSeries, position)
            }

            Bar3DSeries {
                id: secondarySeries
                visible: false
                itemLabelFormat: "Expenses for @colLabel, @rowLabel: @valueLabel"

                ItemModelBarDataProxy {
                    id: secondaryProxy
                    activeMapping: graphData.secondaryMapping
                    itemModel: graphData.model
                }

                onSelectedBarChanged: handleSelectionChange(secondarySeries, position)
            }
        }
    }

    Button {
        id: seriesToggle
        anchors.bottom: parent.bottom
        width: tableView.width
        height: 60
        text: "Show Expenses"
        //! [0]
        onClicked: {
            if (!secondarySeries.visible) {
                text = "Show Both"
                testGraph.valueAxis = graphAxes.expenses
                barSeries.visible = false
                secondarySeries.visible = true
            } else if (!barSeries.visible){
                barSeries.visible = true
                text = "Show Income"
                testGraph.valueAxis = graphAxes.income
            } else {
                secondarySeries.visible = false
                text = "Show Expenses"
                testGraph.valueAxis = graphAxes.income
            }
        }
        //! [0]
    }

    Button {
        id: shadowToggle
        anchors.bottom: seriesToggle.top
        width: tableView.width
        height: 60
        text: "Hide Shadows"
        onClicked: {
            if (testGraph.shadowQuality == AbstractGraph3D.ShadowQualityNone) {
                testGraph.shadowQuality = AbstractGraph3D.ShadowQualityMedium;
                text = "Hide Shadows"
            } else {
                testGraph.shadowQuality = AbstractGraph3D.ShadowQualityNone;
                text = "Show Shadows"
            }
        }
    }

    Button {
        id: dataToggle
        anchors.bottom: shadowToggle.top
        width: tableView.width
        height: 60
        text: "Show 2010 - 2012"
        //! [1]
        onClicked: {
            if (testGraph.rowAxis.max !== 6) {
                text = "Show 2010 - 2012"
                graphData.mapping.autoRowCategories = true
                graphData.secondaryMapping.autoRowCategories = true
            } else {
                text = "Show all years"
                // Explicitly defining row categories, since we do not want to show data for
                // all years in the model, just for the selected ones.
                graphData.mapping.autoRowCategories = false
                graphData.secondaryMapping.autoRowCategories = false
                graphData.mapping.rowCategories = ["2010", "2011", "2012"]
                graphData.secondaryMapping.rowCategories = ["2010", "2011", "2012"]
            }
        }
        //! [1]
    }

    TableView {
        id: tableView
        x: 0
        y: 0
        width: 298
        height: parent.height - seriesToggle.height - shadowToggle.height - dataToggle.height
        TableViewColumn{ role: "year"  ; title: "Year" ; width: 80 }
        TableViewColumn{ role: "month" ; title: "Month" ; width: 80 }
        TableViewColumn{ role: "expenses" ; title: "Expenses" ; width: 60 }
        TableViewColumn{ role: "income" ; title: "Income" ; width: 60 }
        model: graphData.model

        //! [2]
        onCurrentRowChanged: {
            var rowIndex = modelProxy.activeMapping.rowCategoryIndex(graphData.model.get(currentRow).year)
            var colIndex = modelProxy.activeMapping.columnCategoryIndex(graphData.model.get(currentRow).month)
            if (selectedSeries.visible)
                mainview.selectedSeries.selectedBar = Qt.point(rowIndex, colIndex)
            else if (barSeries.visible)
                barSeries.selectedBar = Qt.point(rowIndex, colIndex)
            else
                secondarySeries.selectedBar = Qt.point(rowIndex, colIndex)
        }
        //! [2]
    }
}
