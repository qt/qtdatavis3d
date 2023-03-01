# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(qmlcustominput
    GUI
    SOURCES
        main.cpp
    )

target_link_libraries(qmlcustominput PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::Graphs
)

set(qmlcustominput_resource_files
    "qml/qmlcustominput/Data.qml"
    "qml/qmlcustominput/main.qml"
)

qt6_add_resources(qmlcustominput "qmlcustominput"
    PREFIX
        "/"
    FILES
        ${qmlcustominput_resource_files}
)
