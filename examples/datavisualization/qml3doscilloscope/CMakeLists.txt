cmake_minimum_required(VERSION 3.16)
project(qml3doscilloscope LANGUAGES CXX)

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

qt_add_executable(qml3doscilloscope
    datasource.cpp datasource.h
    main.cpp
)
set_target_properties(qml3doscilloscope PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(qml3doscilloscope PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::DataVisualization
)

set(qml3doscilloscope_resource_files
    "qml/qml3doscilloscope/main.qml"
)

qt6_add_resources(qml3doscilloscope "qml3doscilloscope"
    PREFIX
        "/"
    FILES
        ${qml3doscilloscope_resource_files}
)

install(TARGETS qml3doscilloscope
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
