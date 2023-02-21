// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick3D
import QtQuick

CustomMaterial {
    property color uColor: "purple"
    property real gradientPos: 0.0
    property TextureInput custex: TextureInput {}

    shadingMode: CustomMaterial.Unshaded
    fragmentShader: "qrc:/shaders/fragmentrangegradient"
}
