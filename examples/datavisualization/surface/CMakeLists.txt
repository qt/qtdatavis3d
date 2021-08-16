cmake_minimum_required(VERSION 3.16)
project(surface LANGUAGES CXX)

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

qt_add_executable(surface
    main.cpp
    surfacegraph.cpp surfacegraph.h
)
set_target_properties(surface PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(surface PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::DataVisualization
)

set_source_files_properties("mountain.png"
    PROPERTIES QT_RESOURCE_ALIAS "mountain"
)
set(surface_resource_files
    "mountain.png"
)

qt6_add_resources(surface "surface"
    PREFIX
        "/maps"
    FILES
        ${surface_resource_files}
)

install(TARGETS surface
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
