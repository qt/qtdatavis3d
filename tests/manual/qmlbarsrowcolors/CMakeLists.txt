# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_internal_add_manual_test(qmlbarsrowcolors
    GUI
    SOURCES
        main.cpp
    )

set_target_properties(qmlbarsrowcolors PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_compile_definitions(qmlbarsrowcolors PUBLIC
    QT_DISABLE_DEPRECATED_UP_TO=0x050F00
)

target_link_libraries(qmlbarsrowcolors PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::DataVisualization
)

set(qmlbarsrowcolors_resource_files
    "qml/qmlbarsrowcolors/Axes.qml"
    "qml/qmlbarsrowcolors/Data.qml"
    "qml/qmlbarsrowcolors/main.qml"
)

qt6_add_resources(qmlbarsrowcolors "qmlbarsrowcolors"
    PREFIX
        "/"
    FILES
        ${qmlbarsrowcolors_resource_files}
)

install(TARGETS qmlbarsrowcolors
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
