cmake_minimum_required(VERSION 3.16)
project(texturesurface LANGUAGES CXX)

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

qt_add_executable(texturesurface
    custominputhandler.cpp custominputhandler.h
    highlightseries.cpp highlightseries.h
    main.cpp
    surfacegraph.cpp surfacegraph.h
    topographicseries.cpp topographicseries.h
)
set_target_properties(texturesurface PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(texturesurface PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::DataVisualization
)

set_source_files_properties("maptexture.jpg"
    PROPERTIES QT_RESOURCE_ALIAS "maptexture"
)
set_source_files_properties("topography.png"
    PROPERTIES QT_RESOURCE_ALIAS "topography"
)
set(texturedsurface_resource_files
    "maptexture.jpg"
    "topography.png"
)

qt6_add_resources(texturesurface "texturedsurface"
    PREFIX
        "/maps"
    FILES
        ${texturedsurface_resource_files}
)

install(TARGETS texturesurface
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
