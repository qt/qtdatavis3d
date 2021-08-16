cmake_minimum_required(VERSION 3.16)
project(customproxy LANGUAGES CXX)

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
find_package(Qt6 COMPONENTS DataVisualization)

qt_add_executable(customproxy
    main.cpp
    rainfallgraph.cpp rainfallgraph.h
    variantbardatamapping.cpp variantbardatamapping.h
    variantbardataproxy.cpp variantbardataproxy.h
    variantdataset.cpp variantdataset.h
)
set_target_properties(customproxy PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(customproxy PUBLIC
    Qt::Core
    Qt::Gui
    Qt::DataVisualization
)

set(customproxy_resource_files
    "data/raindata.txt"
)

qt6_add_resources(customproxy "customproxy"
    PREFIX
        "/"
    FILES
        ${customproxy_resource_files}
)

install(TARGETS customproxy
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
