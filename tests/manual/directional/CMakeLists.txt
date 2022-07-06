# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(directional
    GUI
    SOURCES
    main.cpp
    scatterdatamodifier.cpp scatterdatamodifier.h
    )
target_link_libraries(directional PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::DataVisualization
    )
