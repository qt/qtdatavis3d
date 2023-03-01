# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(qmlsurfacelayers
    GUI
    SOURCES
        main.cpp
    )

target_link_libraries(qmlsurfacelayers PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::Graphs
)

set(qmlsurfacelayers_resource_files
    "qml/qmlsurfacelayers/main.qml"
)

qt6_add_resources(qmlsurfacelayers "qmlsurfacelayers"
    PREFIX
        "/"
    FILES
        ${qmlsurfacelayers_resource_files}
)
set(qmlsurfacelayers1_resource_files
    "layer_1.png"
    "layer_2.png"
    "layer_3.png"
)

qt6_add_resources(qmlsurfacelayers "qmlsurfacelayers1"
    PREFIX
        "/heightmaps"
    FILES
        ${qmlsurfacelayers1_resource_files}
)
