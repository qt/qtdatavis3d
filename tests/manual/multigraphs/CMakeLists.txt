# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(multigraphs
    GUI
    SOURCES
        data.cpp data.h
        main.cpp
    )
target_link_libraries(multigraphs PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::Graphs
    )

set(multigraphs_resource_files
    "australia.png"
    )

qt_internal_add_resource(multigraphs "multigraphs"
    PREFIX
    "/"
    FILES
    ${multigraphs_resource_files}
    )
