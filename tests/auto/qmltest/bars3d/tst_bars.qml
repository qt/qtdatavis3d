/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

import QtQuick 2.0
import QtDataVisualization 1.2
import QtTest 1.0

Item {
    id: top
    height: 150
    width: 150

    Bars3D {
        id: series
        anchors.fill: parent
    }

    TestCase {
        name: "Bars3D Series"

        Bar3DSeries { id: series1 }
        Bar3DSeries { id: series2 }

        function test_1_add_series() {
            series.seriesList = [series1, series2]
            compare(series.seriesList.length, 2)
        }

        function test_2_remove_series() {
            series.seriesList = [series1]
            compare(series.seriesList.length, 1)
        }

        function test_3_remove_series() {
            series.seriesList = []
            compare(series.seriesList.length, 0)
        }

        function test_4_primary_series() {
            series.seriesList = [series1, series2]
            compare(series.primarySeries, series1)
            series.primarySeries = series2
            compare(series.primarySeries, series2)
        }

        function test_5_selected_series() {
            series.seriesList[0].selectedBar = Qt.point(0, 0)
            compare(series.selectedSeries, series1)
        }
    }

    // The following tests are not required for scatter or surface, as they are handled identically
    Bars3D {
        id: theme
        anchors.fill: parent
    }

    Bars3D {
        id: input
        anchors.fill: parent
    }

    Custom3DItem { id: item1; meshFile: ":/customitem.obj" }
    Custom3DItem { id: item2; meshFile: ":/customitem.obj" }
    Custom3DItem { id: item3; meshFile: ":/customitem.obj" }
    Custom3DItem { id: item4; meshFile: ":/customitem.obj"; position: Qt.vector3d(0.0, 1.0, 0.0) }

    Bars3D {
        id: custom
        anchors.fill: parent
        customItemList: [item1, item2]
    }

    TestCase {
        name: "Bars3D Theme"
        when: windowShown

        Theme3D { id: newTheme }

        function test_1_add_theme() {
            theme.theme = newTheme
            compare(theme.theme, newTheme)
        }

        function test_2_change_theme() {
            newTheme.type = Theme3D.ThemePrimaryColors
            compare(theme.theme.type, Theme3D.ThemePrimaryColors)
        }
    }

    TestCase {
        name: "Bars3D Input"
        when: windowShown

        function test_1_remove_input() {
            input.inputHandler = null
            compare(input.inputHandler, null)
        }
    }

    TestCase {
        name: "Bars3D Custom"
        when: windowShown

        function test_1_custom_items() {
            compare(custom.customItemList.length, 2)
        }

        function test_2_add_custom_items() {
            custom.addCustomItem(item3)
            compare(custom.customItemList.length, 3)
            custom.addCustomItem(item4)
            compare(custom.customItemList.length, 4)
        }

        function test_3_change_custom_items() {
            item1.position = Qt.vector3d(1.0, 1.0, 1.0)
            compare(custom.customItemList[0].position, Qt.vector3d(1.0, 1.0, 1.0))
        }

        function test_4_remove_custom_items() {
            custom.removeCustomItemAt(Qt.vector3d(0.0, 1.0, 0.0))
            compare(custom.customItemList.length, 3)
            custom.releaseCustomItem(item1)
            compare(custom.customItemList[0], item2)
            custom.releaseCustomItem(item2)
            compare(custom.customItemList.length, 1)
            custom.removeCustomItems()
            compare(custom.customItemList.length, 0)
        }
    }
}
