# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(volumetrictest
    GUI
    SOURCES
        main.cpp
        volumetrictest.cpp volumetrictest.h
    )

target_link_libraries(volumetrictest PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::Graphs
    )

set_source_files_properties("cubeFilledFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "mesh"
    )
set(volumetrictest_resource_files
    "cubeFilledFlat.obj"
    "logo.png"
    "logo_no_padding.png"
    )

qt_internal_add_resource(volumetrictest "volumetrictest"
    PREFIX
    "/"
    FILES
    ${volumetrictest_resource_files}
    )
