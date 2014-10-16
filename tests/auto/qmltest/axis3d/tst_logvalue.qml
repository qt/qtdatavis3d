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

    LogValueAxis3DFormatter {
        id: initial
    }

    LogValueAxis3DFormatter {
        id: initialized
        autoSubGrid: false
        base: 0.1
        showEdgeLabels: false
    }

    LogValueAxis3DFormatter {
        id: change
    }

    TestCase {
        name: "LogValueAxis3DFormatter Initial"

        function test_initial() {
            compare(initial.autoSubGrid, true)
            compare(initial.base, 10)
            compare(initial.showEdgeLabels, true)
        }
    }

    TestCase {
        name: "LogValueAxis3DFormatter Initialized"

        function test_initialized() {
            compare(initialized.autoSubGrid, false)
            compare(initialized.base, 0.1)
            compare(initialized.showEdgeLabels, false)
        }
    }

    TestCase {
        name: "LogValueAxis3DFormatter Change"

        function test_change() {
            change.autoSubGrid = false
            change.base = 0.1
            change.showEdgeLabels = false

            compare(change.autoSubGrid, false)
            compare(change.base, 0.1)
            compare(change.showEdgeLabels, false)
        }
    }
}
