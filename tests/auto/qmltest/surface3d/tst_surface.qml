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

import QtQuick 2.0
import QtDataVisualization 1.2
import QtTest 1.0

Item {
    id: top
    height: 150
    width: 150

    Surface3D {
        id: series
        anchors.fill: parent
    }

    TestCase {
        name: "Surface3D Series"

        Surface3DSeries { id: series1 }
        Surface3DSeries { id: series2 }

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

        function test_4_selected_series() {
            series.seriesList = [series1, series2]
            series.seriesList[0].selectedPoint = Qt.point(0, 0)
            compare(series.selectedSeries, series1)
        }
    }
}
