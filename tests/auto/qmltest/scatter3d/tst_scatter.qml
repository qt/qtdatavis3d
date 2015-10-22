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

    Scatter3D {
        id: series
        anchors.fill: parent
    }

    TestCase {
        name: "Scatter3D Series"

        Scatter3DSeries { id: series1 }
        Scatter3DSeries { id: series2 }

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
            series.seriesList[0].selectedItem = 0
            compare(series.selectedSeries, series1)
        }
    }
}
