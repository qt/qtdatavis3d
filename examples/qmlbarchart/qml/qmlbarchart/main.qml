/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
import com.digia.QtDataVis3D 1.0
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

        property point storedSelection: Qt.point(-1, -1)

        Bars3D {
            id: testchart
            width: dataView.width
            height: dataView.height
            shadowQuality: Bars3D.ShadowNone
            selectionMode: Bars3D.ModeItem
            labelTransparency: Bars3D.TransparencyNone
            theme: Bars3D.ThemeBrownSand
            rows: 4
            columns: 12
            mapping: chartData.mapping
            barThickness: 0.5
            barSpacing: Qt.size(0.5, 0.5)
            barSpacingRelative: false
            rowAxis: chartAxes.row
            columnAxis: chartAxes.column
            valueAxis: chartAxes.expenses
            itemLabelFormat: "@valueTitle for @colLabel, @rowLabel: @valueLabel"

            onDataResolved: {
                // Can't select a bar until data has been resolved from model to proxy
                selectedBarPos = dataView.storedSelection
            }
        }
    }

    Component.onCompleted: {
        testchart.data = chartData.model
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

        onClicked: {
            var rowIndex = testchart.mapping.rowCategoryIndex(chartData.model.get(row).year)
            var colIndex = testchart.mapping.columnCategoryIndex(chartData.model.get(row).month)
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
            dataView.storedSelection = testchart.selectedBarPos
        }
    }

    Button {
        id: shadowToggle
        anchors.bottom: mappingToggle.top
        width: tableView.width
        text: "Show Shadows"
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
