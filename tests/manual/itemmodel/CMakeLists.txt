# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(itemmodel
    GUI
    SOURCES
        main.cpp
    )

target_link_libraries(itemmodel PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Graphs
)
