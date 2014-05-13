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
import QtDataVisualization 1.1
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
        var colRole
        if (barGraph.columnAxis === graphAxes.total)
            colRole = "01";
        else
            colRole = series.dataProxy.columnLabels[position.y];
        var checkTimestamp = rowRole + "-" + colRole
        var currentRow = tableView.currentRow
        if (currentRow === -1 || checkTimestamp !== graphData.model.get(currentRow).timestamp) {
            var totalRows = tableView.rowCount;
            for (var i = 0; i < totalRows; i++) {
                var modelTimestamp = graphData.model.get(i).timestamp
                if (modelTimestamp === checkTimestamp) {
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
            valueAxis: graphAxes.value

            //! [4]
            Bar3DSeries {
                id: secondarySeries
                visible: false
                itemLabelFormat: "Expenses, @colLabel, @rowLabel: -@valueLabel"
                baseGradient: secondaryGradient

                ItemModelBarDataProxy {
                    id: secondaryProxy
                    itemModel: graphData.model
                    rowRole: "timestamp"
                    columnRole: "timestamp"
                    valueRole: "expenses"
                    rowRolePattern: /^(\d\d\d\d).*$/
                    columnRolePattern: /^.*-(\d\d)$/
                    valueRolePattern: /-/
                    rowRoleReplace: "\\1"
                    columnRoleReplace: "\\1"
                    multiMatchBehavior: ItemModelBarDataProxy.MMBCumulative
                }
                //! [4]

                ColorGradient {
                    id: secondaryGradient
                    ColorGradientStop { position: 1.0; color: "#FF0000" }
                    ColorGradientStop { position: 0.0; color: "#600000" }
                }

                onSelectedBarChanged: handleSelectionChange(secondarySeries, position)
            }

            //! [3]
            Bar3DSeries {
                id: barSeries
                itemLabelFormat: "Income, @colLabel, @rowLabel: @valueLabel"
                baseGradient: barGradient

                ItemModelBarDataProxy {
                    id: modelProxy
                    itemModel: graphData.model
                    rowRole: "timestamp"
                    columnRole: "timestamp"
                    valueRole: "income"
                    rowRolePattern: /^(\d\d\d\d).*$/
                    columnRolePattern: /^.*-(\d\d)$/
                    rowRoleReplace: "\\1"
                    columnRoleReplace: "\\1"
                    multiMatchBehavior: ItemModelBarDataProxy.MMBCumulative
                }
                //! [3]

                ColorGradient {
                    id: barGradient
                    ColorGradientStop { position: 1.0; color: "#00FF00" }
                    ColorGradientStop { position: 0.0; color: "#006000" }
                }

                onSelectedBarChanged: handleSelectionChange(barSeries, position)
            }
        }
    }

    TableView {
        id: tableView
        anchors.top: parent.top
        anchors.left: parent.left
        TableViewColumn{ role: "timestamp" ; title: "Month" ; width: tableView.width / 2 }
        TableViewColumn{ role: "expenses" ; title: "Expenses" ; width: tableView.width / 4 }
        TableViewColumn{ role: "income" ; title: "Income" ; width: tableView.width / 4 }
        itemDelegate: Item {
            Text {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                anchors.leftMargin: 4
                anchors.left: parent.left
                anchors.right: parent.right
                color: styleData.textColor
                elide: styleData.elideMode
                text: styleData.value
                horizontalAlignment: styleData.textAlignment

                Component.onCompleted: {
                    if (styleData.column === 0) {
                        var pattern = /(\d\d\d\d)-(\d\d)/
                        var matches = pattern.exec(styleData.value)
                        var colIndex = parseInt(matches[2], 10) - 1
                        text = matches[1] + " - " + graphAxes.column.labels[colIndex]

                    }
                }
            }
        }

        model: graphData.model

        //! [2]
        onCurrentRowChanged: {
            var timestamp = graphData.model.get(currentRow).timestamp
            var pattern = /(\d\d\d\d)-(\d\d)/
            var matches = pattern.exec(timestamp)
            var rowIndex = modelProxy.rowCategoryIndex(matches[1])
            var colIndex
            if (barGraph.columnAxis === graphAxes.total)
                colIndex = 0 // Just one column when showing yearly totals
            else
                colIndex = modelProxy.columnCategoryIndex(matches[2])
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
            id: changeDataButton
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Show 2010 - 2012"
            clip: true
            //! [1]
            onClicked: {
                if (text === "Show yearly totals") {
                    modelProxy.autoRowCategories = true
                    secondaryProxy.autoRowCategories = true
                    modelProxy.columnRolePattern = /^.*$/
                    secondaryProxy.columnRolePattern = /^.*$/
                    graphAxes.value.autoAdjustRange = true
                    barGraph.columnAxis = graphAxes.total
                    text = "Show all years"
                } else if (text === "Show all years") {
                    modelProxy.autoRowCategories = true
                    secondaryProxy.autoRowCategories = true
                    modelProxy.columnRolePattern = /^.*-(\d\d)$/
                    secondaryProxy.columnRolePattern = /^.*-(\d\d)$/
                    graphAxes.value.min = 0
                    graphAxes.value.max = 35
                    barGraph.columnAxis = graphAxes.column
                    text = "Show 2010 - 2012"
                } else { // text === "Show 2010 - 2012"
                    // Explicitly defining row categories, since we do not want to show data for
                    // all years in the model, just for the selected ones.
                    modelProxy.autoRowCategories = false
                    secondaryProxy.autoRowCategories = false
                    modelProxy.rowCategories = ["2010", "2011", "2012"]
                    secondaryProxy.rowCategories = ["2010", "2011", "2012"]
                    text = "Show yearly totals"
                }
            }
            //! [1]
        }

        Button {
            id: shadowToggle
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: barGraph.shadowsSupported ? "Hide Shadows" : "Shadows not supported"
            clip: true
            enabled: barGraph.shadowsSupported
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
                if (text === "Show Expenses") {
                    barSeries.visible = false
                    secondarySeries.visible = true
                    barGraph.valueAxis.labelFormat = "-%.2f M\u20AC"
                    secondarySeries.itemLabelFormat = "Expenses, @colLabel, @rowLabel: @valueLabel"
                    text = "Show Both"
                } else if (text === "Show Both") {
                    barSeries.visible = true
                    barGraph.valueAxis.labelFormat = "%.2f M\u20AC"
                    secondarySeries.itemLabelFormat = "Expenses, @colLabel, @rowLabel: -@valueLabel"
                    text = "Show Income"
                } else { // text === "Show Income"
                    secondarySeries.visible = false
                    text = "Show Expenses"
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
