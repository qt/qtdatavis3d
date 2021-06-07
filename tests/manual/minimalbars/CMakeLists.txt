qt_internal_add_manual_test(MinimalBars
    GUI
    SOURCES
        ../../../src/datavisualization/doc/snippets/doc_src_q3dbars_construction.cpp
    )
target_link_libraries(MinimalBars PUBLIC
    Qt::Gui
    Qt::DataVisualization
    )
