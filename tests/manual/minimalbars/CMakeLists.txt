qt_add_executable(MinimalBars
    GUI
    SOURCES
        ../../../src/datavisualization/doc/snippets/doc_src_q3dbars_construction.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        datavisualization
)
