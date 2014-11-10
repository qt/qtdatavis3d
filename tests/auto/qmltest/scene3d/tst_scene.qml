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

import QtQuick 2.0
import QtDataVisualization 1.2
import QtTest 1.0

Item {
    id: top
    height: 150
    width: 150

    // Scene3D is uncreatable, so it needs to be accessed via a graph
    Bars3D {
        id: initial
    }

    Bars3D {
        id: initialized
        scene.activeCamera: Camera3D { zoomLevel: 200 }
        scene.devicePixelRatio: 2.0
        //scene.graphPositionQuery: Qt.point(0, 0) // TODO: Unusable until QTBUG-40043 is fixed
        scene.primarySubViewport: Qt.rect(0, 0, 50, 50)
        scene.secondarySubViewport: Qt.rect(50, 50, 100, 100)
        scene.secondarySubviewOnTop: false
        scene.selectionQueryPosition: Qt.point(0, 0)
        scene.slicingActive: true
    }

    Bars3D {
        id: change
    }

    Bars3D {
        id: invalid
    }

    TestCase {
        name: "Scene3D Initial"

        function test_initial() {
            verify(initial.scene.activeCamera)
            verify(initial.scene.activeLight)
            compare(initial.scene.devicePixelRatio, 1.0)
            compare(initial.scene.graphPositionQuery, Qt.point(-1, -1))
            compare(initial.scene.invalidSelectionPoint, Qt.point(-1, -1))
            compare(initial.scene.primarySubViewport, Qt.rect(0, 0, 0, 0))
            compare(initial.scene.secondarySubViewport, Qt.rect(0, 0, 0, 0))
            compare(initial.scene.secondarySubviewOnTop, true)
            compare(initial.scene.selectionQueryPosition, Qt.point(-1, -1))
            compare(initial.scene.slicingActive, false)
            compare(initial.scene.viewport, Qt.rect(0, 0, 0, 0))
        }
    }

    TestCase {
        name: "Scene3D Initialized"

        function test_initialized() {
            compare(initialized.scene.activeCamera.zoomLevel, 200)
            compare(initialized.scene.devicePixelRatio, 2.0)
            //compare(initialized.scene.graphPositionQuery, Qt.point(0, 0)) // TODO: Unusable until QTBUG-40043 is fixed
            compare(initialized.scene.primarySubViewport, Qt.rect(0, 0, 50, 50))
            compare(initialized.scene.secondarySubViewport, Qt.rect(50, 50, 100, 100))
            compare(initialized.scene.secondarySubviewOnTop, false)
            compare(initialized.scene.selectionQueryPosition, Qt.point(0, 0))
            compare(initialized.scene.slicingActive, true)
            compare(initialized.scene.viewport, Qt.rect(0, 0, 100, 100))
        }
    }

    TestCase {
        name: "Scene3D Change"

        Camera3D {
            id: camera1
            zoomLevel: 200
        }

        function test_change() {
            change.scene.activeCamera = camera1
            change.scene.devicePixelRatio = 2.0
            change.scene.graphPositionQuery = Qt.point(0, 0)
            change.scene.primarySubViewport = Qt.rect(0, 0, 50, 50)
            change.scene.secondarySubViewport = Qt.rect(50, 50, 100, 100)
            change.scene.secondarySubviewOnTop = false
            change.scene.selectionQueryPosition = Qt.point(0, 0) // TODO: When doing signal checks, add tests to check that queries return something (asynchronously)
            change.scene.slicingActive = true

            compare(change.scene.activeCamera.zoomLevel, 200)
            compare(change.scene.devicePixelRatio, 2.0)
            compare(change.scene.graphPositionQuery, Qt.point(0, 0))
            compare(change.scene.primarySubViewport, Qt.rect(0, 0, 50, 50))
            compare(change.scene.secondarySubViewport, Qt.rect(50, 50, 100, 100))
            compare(change.scene.secondarySubviewOnTop, false)
            compare(change.scene.selectionQueryPosition, Qt.point(0, 0))
            compare(change.scene.slicingActive, true)
            compare(change.scene.viewport, Qt.rect(0, 0, 100, 100))
        }
    }

    TestCase {
        name: "Scene3D Invalid"

        function test_invalid() {
            invalid.scene.primarySubViewport = Qt.rect(0, 0, -50, -50)
            compare(invalid.scene.primarySubViewport, Qt.rect(0, 0, 0, 0))
        }
    }
}
