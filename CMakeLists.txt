cmake_minimum_required(VERSION 3.16)

include(.cmake.conf)
project(QtDataVisualization
    VERSION "${QT_REPO_MODULE_VERSION}"
    DESCRIPTION "Qt Data Visualization Libraries"
    HOMEPAGE_URL "https://qt.io/"
    LANGUAGES CXX C
)

find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS BuildInternals)
find_package(Qt6 ${PROJECT_VERSION} CONFIG OPTIONAL_COMPONENTS
    Core OpenGL Quick Qml Gui Widgets QuickTest Test)

macro(assertTargets)
    foreach(qtTarget IN ITEMS ${ARGN})
        if(NOT TARGET Qt::${qtTarget})
           message(NOTICE "Skipping the build as the condition \"TARGET Qt::${qtTarget}\" is not met.")
           return()
        endif()
    endforeach()
endmacro()

assertTargets(Gui Widgets OpenGL Quick Qml QuickTest Test)

qt_build_repo()
