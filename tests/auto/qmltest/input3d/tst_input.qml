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

    InputHandler3D {
        id: initial
    }

    InputHandler3D {
        id: initialized
        rotationEnabled: false
        selectionEnabled: false
        zoomAtTargetEnabled: false
        zoomEnabled: false
    }

    InputHandler3D {
        id: change
    }

    TestCase {
        name: "InputHandler3D Initial"

        function test_initial() {
            compare(initial.rotationEnabled, true)
            compare(initial.selectionEnabled, true)
            compare(initial.zoomAtTargetEnabled, true)
            compare(initial.zoomEnabled, true)
        }
    }

    TestCase {
        name: "InputHandler3D Initialized"

        function test_initialized() {
            compare(initialized.rotationEnabled, false)
            compare(initialized.selectionEnabled, false)
            compare(initialized.zoomAtTargetEnabled, false)
            compare(initialized.zoomEnabled, false)
        }
    }

    TestCase {
        name: "InputHandler3D Change"

        function test_change() {
            change.rotationEnabled = false
            change.selectionEnabled = false
            change.zoomAtTargetEnabled = false
            change.zoomEnabled = false

            compare(change.rotationEnabled, false)
            compare(change.selectionEnabled, false)
            compare(change.zoomAtTargetEnabled, false)
            compare(change.zoomEnabled, false)
        }

        // TODO: QTRD-3380 (mouse events)
    }
}
