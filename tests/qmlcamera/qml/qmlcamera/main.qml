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
import QtDataVisualization 1.1
import "."

Rectangle {
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
        width: parent.width - camControlArea.width
        height: parent.height
        anchors.right: parent.right;

        Bars3D {
            id: testChart
            width: dataView.width
            height: dataView.height
            shadowQuality: Bars3D.ShadowQualityMedium
            selectionMode: Bars3D.SelectionItem
            theme: Theme3D {
                type: Theme3D.ThemeDigia
                font.pointSize: 35
                labelBackgroundEnabled: true
            }
            seriesList: [chartData.series]
            barThickness: 0.5
            barSpacing: Qt.size(0.5, 0.5)
            barSpacingRelative: false

            columnAxis: chartAxes.column
            valueAxis: chartAxes.expenses

            // Bind UI controls to the camera
            scene.activeCamera.wrapXRotation: false
            scene.activeCamera.xRotation: camControlArea.xValue
            scene.activeCamera.yRotation: camControlArea.yValue
            scene.activeCamera.zoomLevel: zoomSlider.value
            inputHandler: null

            customItemList: [shuttleItem]
        }

        Custom3DItem {
            id: shuttleItem
            meshFile: ":/items/shuttle.obj"
            textureFile: ":/items/shuttle.png"
            position: Qt.vector3d(5.0,35.0,3.0)
            scaling: Qt.vector3d(0.2,0.2,0.2)
        }

        MouseArea {
            id: inputArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            property bool selectionOn: false

            onPressed: {
                if (mouse.button == Qt.LeftButton) {
                    selectionOn = true;
                    testChart.scene.selectionQueryPosition = Qt.point(mouse.x, mouse.y);
                }
            }

            onReleased: {
                if (mouse.button == Qt.LeftButton)
                    selectionOn = false;
            }

            onPositionChanged: {
                if (selectionOn) {
                    testChart.scene.selectionQueryPosition = Qt.point(mouse.x, mouse.y);
                }
            }
        }
    }

    ControlSurface {
        id: camControlArea
        x: 0
        y: 0
        width:  298
        height: 298
        minXValue: -180
        minYValue: 0
        maxXValue: 180
        maxYValue: 90
    }

    Slider {
        id: zoomSlider
        width: camControlArea.width
        anchors.top: camControlArea.bottom
        value: 100
        minimumValue: 10
        maximumValue: 300
    }

    Button {
        id: mappingToggle
        anchors.bottom: parent.bottom
        width: camControlArea.width
        text: "Show Income"
        onClicked: {
            if (chartData.proxy.valueRole === "expenses") {
                chartData.proxy.valueRole = "income"
                text = "Show Expenses"
                testChart.valueAxis = chartAxes.income
            } else {
                chartData.proxy.valueRole = "expenses"
                text = "Show Income"
                testChart.valueAxis = chartAxes.expenses
            }
        }
    }

    Button {
        id: angleAdjust
        anchors.bottom: mappingToggle.top
        width: camControlArea.width
        text: "Adjust angle"
        property real currentAngle: 0
        onClicked: {
            currentAngle += 5
            chartData.series.meshAngle = currentAngle
        }
    }

    Button {
        id: dataToggle
        anchors.bottom: angleAdjust.top
        width: camControlArea.width
        text: "Show 2010 - 2012"
        onClicked: {
            if (testChart.rowAxis.max !== 6) {
                text = "Show 2010 - 2012"
                chartData.proxy.autoRowCategories = true
            } else {
                text = "Show all years"
                // Explicitly defining row categories, since we do not want to show data for
                // all years in the model, just for the selected ones.
                chartData.proxy.autoRowCategories = false
                chartData.proxy.rowCategories = ["2010", "2011", "2012"]
            }
        }
    }

    Button {
        id: shuttleAdd
        anchors.bottom: dataToggle.top
        width: camControlArea.width
        text: "Remove Shuttle"
        onClicked: {
            testChart.removeCustomItemAt(Qt.vector3d(5.0,35.0,3.0))
            text = "Shuttle has been deleted"
        }
    }
}
