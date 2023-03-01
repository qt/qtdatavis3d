# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

qt_internal_add_manual_test(MinimalBars
    GUI
    SOURCES
        ../../../src/graphs/doc/snippets/doc_src_q3dbars_construction.cpp
    )
target_link_libraries(MinimalBars PUBLIC
    Qt::Gui
    Qt::Graphs
    )
