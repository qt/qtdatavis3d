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
import com.digia.QtDataVisualization 1.0
import "."

Item {
    id: mainview
    width: 1280
    height: 1024
    visible: true

    Data {
        id: chartData
    }

    Axes {
        id: chartAxes
    }

    Item {
        id: dataView
        width: parent.width - tableView.width
        height: parent.height
        anchors.right: parent.right;

        Bars3D {
            id: testchart
            width: dataView.width
            height: dataView.height
            shadowQuality: Bars3D.ShadowMedium
            selectionMode: Bars3D.ModeItem
            font.pointSize: 35
            theme: Bars3D.ThemeBrownSand
            rows: 4
            columns: 12
            dataProxy: chartData.proxy
            barThickness: 0.5
            barSpacing: Qt.size(0.5, 0.5)
            barSpacingRelative: false
            cameraPreset: Bars3D.PresetRight
            rowAxis: chartAxes.row
            columnAxis: chartAxes.column
            valueAxis: chartAxes.expenses
            itemLabelFormat: "@valueTitle for @colLabel, @rowLabel: @valueLabel"
        }
    }

    TableView {
        id: tableView
        x: 0
        y: 0
        width: 298
        height: parent.height - mappingToggle.height - shadowToggle.height
        TableViewColumn{ role: "year"  ; title: "Year" ; width: 80 }
        TableViewColumn{ role: "month" ; title: "Month" ; width: 80 }
        TableViewColumn{ role: "expenses" ; title: "Expenses" ; width: 60 }
        TableViewColumn{ role: "income" ; title: "Income" ; width: 60 }
        model: chartData.model

        onCurrentRowChanged: {
            var rowIndex = chartData.proxy.activeMapping.rowCategoryIndex(chartData.model.get(currentRow).year)
            var colIndex = chartData.proxy.activeMapping.columnCategoryIndex(chartData.model.get(currentRow).month)
            testchart.selectedBarPos = Qt.point(rowIndex, colIndex)
        }
    }

    Button {
        id: mappingToggle
        anchors.bottom: parent.bottom
        width: tableView.width
        text: "Show Income"
        onClicked: {
            if (chartData.mapping.valueRole === "expenses") {
                chartData.mapping.valueRole = "income"
                text = "Show Expenses"
                testchart.valueAxis = chartAxes.income
            } else {
                chartData.mapping.valueRole = "expenses"
                text = "Show Income"
                testchart.valueAxis = chartAxes.expenses
            }
        }
    }

    Button {
        id: shadowToggle
        anchors.bottom: mappingToggle.top
        width: tableView.width
        text: "Hide Shadows"
        onClicked: {
            if (testchart.shadowQuality == Bars3D.ShadowNone) {
                testchart.shadowQuality = Bars3D.ShadowMedium;
                text = "Hide Shadows"
            } else {
                testchart.shadowQuality = Bars3D.ShadowNone;
                text = "Show Shadows"
            }
        }
    }
}
