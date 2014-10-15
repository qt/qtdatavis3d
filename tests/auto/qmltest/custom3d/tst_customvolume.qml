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

    Custom3DVolume {
        id: initial
    }

    Custom3DVolume {
        id: initialized
        alphaMultiplier: 0.1
        drawSliceFrames: true
        drawSlices: true
        preserveOpacity: false
        sliceFrameColor: "red"
        sliceFrameGaps: Qt.vector3d(2.0, 2.0, 2.0)
        sliceFrameThicknesses: Qt.vector3d(2.0, 2.0, 2.0)
        sliceFrameWidths: Qt.vector3d(2.0, 2.0, 2.0)
        sliceIndexX: 0
        sliceIndexY: 0
        sliceIndexZ: 0
        useHighDefShader: false

        position: Qt.vector3d(1.0, 0.5, 1.0)
        positionAbsolute: true
        rotation: Qt.quaternion(1, 0.5, 0, 0)
        scaling: Qt.vector3d(0.2, 0.2, 0.2)
        scalingAbsolute: false
        shadowCasting: false
        visible: false
    }

    Custom3DVolume {
        id: change
    }

    TestCase {
        name: "Custom3DVolume Initial"

        function test_initial() {
            compare(initial.alphaMultiplier, 1.0)
            compare(initial.drawSliceFrames, false)
            compare(initial.drawSlices, false)
            compare(initial.preserveOpacity, true)
            compare(initial.sliceFrameColor, "#000000")
            compare(initial.sliceFrameGaps, Qt.vector3d(0.01, 0.01, 0.01))
            compare(initial.sliceFrameThicknesses, Qt.vector3d(0.01, 0.01, 0.01))
            compare(initial.sliceFrameWidths, Qt.vector3d(0.01, 0.01, 0.01))
            compare(initial.sliceIndexX, -1)
            compare(initial.sliceIndexY, -1)
            compare(initial.sliceIndexZ, -1)
            compare(initial.useHighDefShader, true)

            compare(initial.meshFile, ":/defaultMeshes/barFull")
            compare(initial.position, Qt.vector3d(0.0, 0.0, 0.0))
            compare(initial.positionAbsolute, false)
            compare(initial.rotation, Qt.quaternion(0, 0, 0, 0))
            compare(initial.scaling, Qt.vector3d(0.1, 0.1, 0.1))
            compare(initial.scalingAbsolute, true)
            compare(initial.shadowCasting, true)
            compare(initial.textureFile, "")
            compare(initial.visible, true)
        }
    }

    TestCase {
        name: "Custom3DVolume Initialized"

        function test_initialized() {
            compare(initialized.alphaMultiplier, 0.1)
            compare(initialized.drawSliceFrames, true)
            compare(initialized.drawSlices, true)
            compare(initialized.preserveOpacity, false)
            compare(initialized.sliceFrameColor, "#ff0000")
            compare(initialized.sliceFrameGaps, Qt.vector3d(2.0, 2.0, 2.0))
            compare(initialized.sliceFrameThicknesses, Qt.vector3d(2.0, 2.0, 2.0))
            compare(initialized.sliceFrameWidths, Qt.vector3d(2.0, 2.0, 2.0))
            compare(initialized.sliceIndexX, 0)
            compare(initialized.sliceIndexY, 0)
            compare(initialized.sliceIndexZ, 0)
            compare(initialized.useHighDefShader, false)

            compare(initialized.position, Qt.vector3d(1.0, 0.5, 1.0))
            compare(initialized.positionAbsolute, true)
            compare(initialized.rotation, Qt.quaternion(1, 0.5, 0, 0))
            compare(initialized.scaling, Qt.vector3d(0.2, 0.2, 0.2))
            compare(initialized.scalingAbsolute, false)
            compare(initialized.shadowCasting, false)
            compare(initialized.visible, false)
        }
    }

    TestCase {
        name: "Custom3DVolume Change"

        function test_change() {
            change.alphaMultiplier = 0.1
            change.drawSliceFrames = true
            change.drawSlices = true
            change.preserveOpacity = false
            change.sliceFrameColor = "red"
            change.sliceFrameGaps = Qt.vector3d(2.0, 2.0, 2.0)
            change.sliceFrameThicknesses = Qt.vector3d(2.0, 2.0, 2.0)
            change.sliceFrameWidths = Qt.vector3d(2.0, 2.0, 2.0)
            change.sliceIndexX = 0
            change.sliceIndexY = 0
            change.sliceIndexZ = 0
            change.useHighDefShader = false

            change.position = Qt.vector3d(1.0, 0.5, 1.0)
            change.positionAbsolute = true
            change.rotation = Qt.quaternion(1, 0.5, 0, 0)
            change.scaling = Qt.vector3d(0.2, 0.2, 0.2)
            change.scalingAbsolute = false
            change.shadowCasting = false
            change.visible = false

            compare(change.alphaMultiplier, 0.1)
            compare(change.drawSliceFrames, true)
            compare(change.drawSlices, true)
            compare(change.preserveOpacity, false)
            compare(change.sliceFrameColor, "#ff0000")
            compare(change.sliceFrameGaps, Qt.vector3d(2.0, 2.0, 2.0))
            compare(change.sliceFrameThicknesses, Qt.vector3d(2.0, 2.0, 2.0))
            compare(change.sliceFrameWidths, Qt.vector3d(2.0, 2.0, 2.0))
            compare(change.sliceIndexX, 0)
            compare(change.sliceIndexY, 0)
            compare(change.sliceIndexZ, 0)
            compare(change.useHighDefShader, false)

            compare(change.position, Qt.vector3d(1.0, 0.5, 1.0))
            compare(change.positionAbsolute, true)
            compare(change.rotation, Qt.quaternion(1, 0.5, 0, 0))
            compare(change.scaling, Qt.vector3d(0.2, 0.2, 0.2))
            compare(change.scalingAbsolute, false)
            compare(change.shadowCasting, false)
            compare(change.visible, false)
        }
    }
}
