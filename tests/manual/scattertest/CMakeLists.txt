# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(scattertest
    GUI
    SOURCES
        main.cpp
        scatterchart.cpp scatterchart.h
    )
target_link_libraries(scattertest PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::Graphs
    )
