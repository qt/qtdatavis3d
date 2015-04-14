/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
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

    LogValueAxis3DFormatter {
        id: invalid
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

    TestCase {
        name: "LogValueAxis3DFormatter Invalid"

        function test_invalid() {
            invalid.base = 1
            compare(invalid.base, 10)
            invalid.base = -1
            compare(invalid.base, 10)
        }
    }
}
