# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(qmllegend
    GUI
    SOURCES
        main.cpp
    )

target_link_libraries(qmllegend PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::Graphs
)

set(qmllegend_resource_files
    "qml/qmllegend/Data.qml"
    "qml/qmllegend/LegendItem.qml"
    "qml/qmllegend/main.qml"
)

qt6_add_resources(qmllegend "qmllegend"
    PREFIX
        "/"
    FILES
        ${qmllegend_resource_files}
)
