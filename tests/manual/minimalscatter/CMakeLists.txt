# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

qt_internal_add_manual_test(MinimalScatter
    GUI
    SOURCES
        ../../../src/graphs/doc/snippets/doc_src_q3dscatter_construction.cpp
    )
target_link_libraries(MinimalScatter PUBLIC
    Qt::Gui
    Qt::Graphs
    )
