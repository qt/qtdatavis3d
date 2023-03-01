# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(qmlbarsrowcolors
    GUI
    SOURCES
        main.cpp
    )

target_link_libraries(qmlbarsrowcolors PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::Graphs
)

set(qmlbarsrowcolors_resource_files
    "qml/qmlbarsrowcolors/Axes.qml"
    "qml/qmlbarsrowcolors/Data.qml"
    "qml/qmlbarsrowcolors/main.qml"
)

qt6_add_resources(qmlbarsrowcolors "qmlbarsrowcolors"
    PREFIX
        "/"
    FILES
        ${qmlbarsrowcolors_resource_files}
)
