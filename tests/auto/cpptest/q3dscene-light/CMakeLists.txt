# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

qt_internal_add_test(q3dscene-light
    SOURCES
        tst_light.cpp
    LIBRARIES
        Qt::Gui
        Qt::DataVisualization
)
