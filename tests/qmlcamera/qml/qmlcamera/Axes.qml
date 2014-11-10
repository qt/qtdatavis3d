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
import QtDataVisualization 1.0

Item {
    property alias column: columnAxis
    property alias expenses: expensesAxis
    property alias income: incomeAxis

    // For row labels we can use row labels from data proxy, so default axis
    // suffices for rows.

    // Custom labels for columns, since the data contains abbreviated month names.
    CategoryAxis3D {
        id: columnAxis
        labels: ["January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December"]
    }
    ValueAxis3D {
        id: incomeAxis
        labelFormat: "%.2f M\u20AC"
        title: "Monthly income"
    }
    ValueAxis3D {
        id: expensesAxis
        labelFormat: "-%.2f M\u20AC"
        title: "Monthly expenses"
    }
}
