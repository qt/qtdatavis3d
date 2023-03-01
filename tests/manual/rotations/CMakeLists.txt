# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(rotations
    GUI
    SOURCES
        main.cpp
        scatterdatamodifier.cpp scatterdatamodifier.h
    )

target_link_libraries(rotations PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Graphs
)

set(rotations_resource_files
    "mesh/largesphere.obj"
    "mesh/narrowarrow.obj"
)

qt6_add_resources(rotations "rotations"
    PREFIX
        "/"
    FILES
        ${rotations_resource_files}
)
