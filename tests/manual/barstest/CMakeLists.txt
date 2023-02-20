# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(barstest
    GUI
    SOURCES
        chart.cpp chart.h
        sliderwrapper.cpp sliderwrapper.h
        buttonwrapper.cpp buttonwrapper.h
        custominputhandler.cpp custominputhandler.h
        main.cpp
    )
target_link_libraries(barstest PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::Graphs
    )

set(barstest_resource_files
    "shuttle.obj"
    "shuttle.png"
    )

qt_internal_add_resource(barstest "barstest"
    PREFIX
    "/"
    FILES
    ${barstest_resource_files}
    )
