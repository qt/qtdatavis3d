# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

qt_internal_add_manual_test(itemmodeltest
    GUI
    SOURCES
        main.cpp
    )
target_link_libraries(itemmodeltest PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::Graphs
    )
