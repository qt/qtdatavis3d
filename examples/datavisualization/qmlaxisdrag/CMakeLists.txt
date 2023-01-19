# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(qmlaxisdrag LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS DataVisualization)

qt_add_executable(qmlaxisdrag
    main.cpp
)
set_target_properties(qmlaxisdrag PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(qmlaxisdrag PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::DataVisualization
)

qt6_add_qml_module(qmlaxisdrag
    URI AxisDrag
    VERSION 1.0
    NO_RESOURCE_TARGET_PATH
    QML_FILES
        qml/qmlaxisdrag/main.qml
    RESOURCES
        qml/qmlaxisdrag/cube.obj
        qml/qmlaxisdrag/cubetexture.png
)

install(TARGETS qmlaxisdrag
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
