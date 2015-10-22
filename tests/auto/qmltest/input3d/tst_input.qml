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
