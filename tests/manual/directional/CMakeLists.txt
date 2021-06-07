set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_internal_add_manual_test(directional
    GUI
    SOURCES
    main.cpp
    scatterdatamodifier.cpp scatterdatamodifier.h
    )
target_link_libraries(directional PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::DataVisualization
    )
