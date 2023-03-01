# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(qmlvolume
    GUI
    SOURCES
        datasource.cpp datasource.h
        main.cpp
    )
target_link_libraries(qmlvolume PUBLIC
    Qt::Gui
    Qt::Graphs
    )

set(qmlvolume_resource_files
    "qml/qmlvolume/main.qml"
    )

qt_internal_add_resource(qmlvolume "qmlvolume"
    PREFIX
    "/"
    FILES
    ${qmlvolume_resource_files}
    )
