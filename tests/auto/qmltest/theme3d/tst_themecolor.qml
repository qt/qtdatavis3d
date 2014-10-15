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
    width: 150
    height: 150

    ThemeColor {
        id: initial
    }

    ThemeColor {
        id: initialized
        color: "red"
    }

    ThemeColor {
        id: change
    }

    TestCase {
        name: "ThemeColor Initial"

        function test_initial() {
            compare(initial.color, "#000000")
        }
    }

    TestCase {
        name: "ThemeColor Initialized"

        function test_initialized() {
            compare(initialized.color, "#ff0000")
        }
    }

    TestCase {
        name: "ThemeColor Change"

        function test_1_change() {
            change.color = "blue"

            compare(change.color, "#0000ff")
        }
    }
}
