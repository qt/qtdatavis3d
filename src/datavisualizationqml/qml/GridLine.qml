// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick3D

Component {
    Model {
        property color lineColor: "green"
        source: "#Rectangle"
        materials: [ PrincipledMaterial {
                baseColor: lineColor
                lighting: PrincipledMaterial.NoLighting
            }
        ]
        scale: Qt.vector3d(0.0001, 0.0001, 0.0001)
    }
}
