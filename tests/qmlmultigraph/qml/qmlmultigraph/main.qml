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
import QtQuick.Layouts 1.0
import QtDataVisualization 1.0
import "."

Item {
    id: mainView

    Bars3D {
        id: barGraph
        width: parent.width / 3
        anchors.top: emptySpace.bottom
        anchors.bottom: parent.bottom
        theme: Theme3D { type: Theme3D.ThemeIsabelle }
        //clearWindowBeforeRendering: false

        Bar3DSeries {
            itemLabelFormat: "@colLabel, @rowLabel: @valueLabel"

            ItemModelBarDataProxy {
                itemModel: barDataModel
                // Mapping model roles to bar series rows, columns, and values.
                rowRole: "year"
                columnRole: "city"
                valueRole: "expenses"
            }
        }

        onRowAxisChanged: {
            console.log("Bars: Row axis changed.")
        }
        onColumnAxisChanged: {
            console.log("Bars: column axis changed.")
        }
        onValueAxisChanged: {
            console.log("Bars: Value axis changed.")
        }
    }

    ListModel {
        id: barDataModel
        ListElement{ year: "2012"; city: "Oulu";     expenses: "4200"; }
        ListElement{ year: "2012"; city: "Rauma";    expenses: "2100"; }
        ListElement{ year: "2012"; city: "Helsinki"; expenses: "7040"; }
        ListElement{ year: "2012"; city: "Tampere";  expenses: "4330"; }
        ListElement{ year: "2013"; city: "Oulu";     expenses: "3960"; }
        ListElement{ year: "2013"; city: "Rauma";    expenses: "1990"; }
        ListElement{ year: "2013"; city: "Helsinki"; expenses: "7230"; }
        ListElement{ year: "2013"; city: "Tampere";  expenses: "4650"; }
    }

    Scatter3D {
        id: scatterGraph
        width: parent.width / 3
        anchors.left: barGraph.right
        anchors.top: emptySpace.bottom
        anchors.bottom: parent.bottom
        theme: Theme3D { type: Theme3D.ThemeIsabelle }
        //clearWindowBeforeRendering: false

        Scatter3DSeries {
            ItemModelScatterDataProxy {
                itemModel: scatterDataModel
                // Mapping model roles to scatter series item coordinates.
                xPosRole: "xPos"
                yPosRole: "yPos"
                zPosRole: "zPos"
            }
        }

        onAxisXChanged: {
            console.log("Scatter: axis X changed.")
        }
        onAxisYChanged: {
            console.log("Scatter: axis Y changed.")
        }
        onAxisZChanged: {
            console.log("Scatter: axis Z changed.")
        }
    }

    ListModel {
        id: scatterDataModel
        ListElement{ xPos: "2.754"; yPos: "1.455"; zPos: "3.362"; }
        ListElement{ xPos: "3.164"; yPos: "2.022"; zPos: "4.348"; }
        ListElement{ xPos: "4.564"; yPos: "1.865"; zPos: "1.346"; }
        ListElement{ xPos: "1.068"; yPos: "1.224"; zPos: "2.983"; }
        ListElement{ xPos: "2.323"; yPos: "2.502"; zPos: "3.133"; }
    }

    Surface3D {
        id: surfaceGraph
        width: parent.width / 3
        anchors.left: scatterGraph.right
        anchors.top: emptySpace.bottom
        anchors.bottom: parent.bottom
        theme: Theme3D { type: Theme3D.ThemeIsabelle }
        //clearWindowBeforeRendering: false

        Surface3DSeries {
            itemLabelFormat: "Pop density at (@xLabel N, @zLabel E): @yLabel"
            ItemModelSurfaceDataProxy {
                itemModel: surfaceDataModel
                // Mapping model roles to surface series rows, columns, and values.
                rowRole: "longitude"
                columnRole: "latitude"
                valueRole: "pop_density"
            }
        }

        onAxisXChanged: {
            console.log("Surface: axis X changed.")
        }
        onAxisYChanged: {
            console.log("Surface: axis Y changed.")
        }
        onAxisZChanged: {
            console.log("Surface: axis Z changed.")
        }
    }
    ListModel {
        id: surfaceDataModel
        ListElement{ longitude: "20"; latitude: "10"; pop_density: "4.75"; }
        ListElement{ longitude: "21"; latitude: "10"; pop_density: "3.00"; }
        ListElement{ longitude: "22"; latitude: "10"; pop_density: "1.24"; }
        ListElement{ longitude: "23"; latitude: "10"; pop_density: "2.53"; }
        ListElement{ longitude: "20"; latitude: "11"; pop_density: "2.55"; }
        ListElement{ longitude: "21"; latitude: "11"; pop_density: "2.03"; }
        ListElement{ longitude: "22"; latitude: "11"; pop_density: "3.46"; }
        ListElement{ longitude: "23"; latitude: "11"; pop_density: "5.12"; }
        ListElement{ longitude: "20"; latitude: "12"; pop_density: "1.37"; }
        ListElement{ longitude: "21"; latitude: "12"; pop_density: "2.98"; }
        ListElement{ longitude: "22"; latitude: "12"; pop_density: "3.33"; }
        ListElement{ longitude: "23"; latitude: "12"; pop_density: "3.23"; }
        ListElement{ longitude: "20"; latitude: "13"; pop_density: "4.34"; }
        ListElement{ longitude: "21"; latitude: "13"; pop_density: "3.54"; }
        ListElement{ longitude: "22"; latitude: "13"; pop_density: "1.65"; }
        ListElement{ longitude: "23"; latitude: "13"; pop_density: "2.67"; }
    }

    RowLayout {
        id: buttonLayout
        Layout.minimumHeight: exitButton.height
        width: parent.width
        anchors.left: parent.left
        anchors.top: parent.top
        spacing: 0

        NewButton {
            id: exitButton
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: "Quit"
            onClicked: Qt.quit(0);
        }

        NewButton {
            id: resetAxesButton
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: "Reset axes"
            onClicked: {
                barGraph.rowAxis = null
                barGraph.columnAxis = null
                barGraph.valueAxis = null
                scatterGraph.axisX = null
                scatterGraph.axisY = null
                scatterGraph.axisZ = null
                surfaceGraph.axisX = null
                surfaceGraph.axisY = null
                surfaceGraph.axisZ = null
            }
        }
    }

    Rectangle {
        id: emptySpace
        width: parent.width / 2
        height: parent.height / 8
        anchors.top: buttonLayout.bottom
        anchors.left: parent.left
        color: "#55555500"
    }

    Item {
        id: emptySpace2
        width: emptySpace.width
        height: emptySpace.height
        anchors.top: buttonLayout.bottom
        anchors.left: emptySpace.right
    }
}
