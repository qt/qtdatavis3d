set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_internal_add_manual_test(scattertest
    GUI
    SOURCES
        main.cpp
        scatterchart.cpp scatterchart.h
    )
target_link_libraries(scattertest PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::DataVisualization
    )
