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
