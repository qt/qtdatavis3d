cmake_minimum_required(VERSION 3.16)
project(qmlcustominput LANGUAGES CXX)

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

qt_add_executable(qmlcustominput
    main.cpp
)
set_target_properties(qmlcustominput PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(qmlcustominput PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::DataVisualization
)

set(qmlcustominput_resource_files
    "qml/qmlcustominput/Data.qml"
    "qml/qmlcustominput/main.qml"
)

qt6_add_resources(qmlcustominput "qmlcustominput"
    PREFIX
        "/"
    FILES
        ${qmlcustominput_resource_files}
)

install(TARGETS qmlcustominput
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
