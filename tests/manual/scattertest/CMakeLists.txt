qt_add_executable(scattertest
    GUI
    SOURCES
        main.cpp
        scatterchart.cpp scatterchart.h
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Widgets
        Qt::DataVisualization
)
