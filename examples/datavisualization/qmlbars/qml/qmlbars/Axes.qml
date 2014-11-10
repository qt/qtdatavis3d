/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

import QtQuick 2.1
import QtDataVisualization 1.1

Item {
    property alias column: columnAxis
    property alias row: rowAxis
    property alias value: valueAxis
    property alias total: totalAxis

    // Custom labels for columns, since the data contains abbreviated month names.
    //! [0]
    CategoryAxis3D {
        id: columnAxis
        labels: ["January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December"]
        labelAutoRotation: 30
    }
    //! [0]
    CategoryAxis3D {
        id: totalAxis
        labels: ["Yearly total"]
        labelAutoRotation: 30
    }
    CategoryAxis3D {
        // For row labels we can use row labels from data proxy, no labels defined for rows.
        id: rowAxis
        labelAutoRotation: 30
    }

    ValueAxis3D {
        id: valueAxis
        min: 0
        max: 35
        labelFormat: "%.2f M\u20AC"
        title: "Monthly income"
        labelAutoRotation: 90
    }
}
