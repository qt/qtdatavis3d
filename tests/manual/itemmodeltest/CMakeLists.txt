qt_internal_add_manual_test(itemmodeltest
    GUI
    SOURCES
        main.cpp
    )
target_link_libraries(itemmodeltest PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::DataVisualization
    )
