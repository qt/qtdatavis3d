cmake_minimum_required(VERSION 3.16)
project(rotations LANGUAGES CXX)

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
find_package(Qt6 COMPONENTS Widgets)
find_package(Qt6 COMPONENTS DataVisualization)

qt_add_executable(rotations
    main.cpp
    scatterdatamodifier.cpp scatterdatamodifier.h
)
set_target_properties(rotations PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(rotations PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::DataVisualization
)

set(rotations_resource_files
    "mesh/largesphere.obj"
    "mesh/narrowarrow.obj"
)

qt6_add_resources(rotations "rotations"
    PREFIX
        "/"
    FILES
        ${rotations_resource_files}
)

install(TARGETS rotations
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
