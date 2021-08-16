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

set(qmlaxisdrag_resource_files
    "qml/qmlaxisdrag/main.qml"
)

qt6_add_resources(qmlaxisdrag "qmlaxisdrag"
    PREFIX
        "/"
    FILES
        ${qmlaxisdrag_resource_files}
)
set_source_files_properties("qml/qmlaxisdrag/cube.obj"
    PROPERTIES QT_RESOURCE_ALIAS "cube"
)
set(qmlaxisdrag1_resource_files
    "qml/qmlaxisdrag/cube.obj"
)

qt6_add_resources(qmlaxisdrag "qmlaxisdrag1"
    PREFIX
        "/mesh"
    FILES
        ${qmlaxisdrag1_resource_files}
)
set_source_files_properties("qml/qmlaxisdrag/cubetexture.png"
    PROPERTIES QT_RESOURCE_ALIAS "texture"
)
set(qmlaxisdrag2_resource_files
    "qml/qmlaxisdrag/cubetexture.png"
)

qt6_add_resources(qmlaxisdrag "qmlaxisdrag2"
    PREFIX
        "/texture"
    FILES
        ${qmlaxisdrag2_resource_files}
)

install(TARGETS qmlaxisdrag
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
