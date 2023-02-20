# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

if(NOT ANDROID)
    add_subdirectory(cpptest)
endif()
if(TARGET Qt::Quick AND NOT boot2qt)
#    add_subdirectory(qmltest) # TODO: commented out for now; see QTBUG-110000 & QTBUG-110001
endif()
