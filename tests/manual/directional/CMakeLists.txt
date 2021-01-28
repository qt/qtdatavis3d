qt_add_executable(directional
    GUI
    SOURCES
        main.cpp
        scatterdatamodifier.cpp scatterdatamodifier.h
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Widgets
        Qt::DataVisualization
)
