qt_add_executable(MinimalScatter
    GUI
    SOURCES
        ../../../src/datavisualization/doc/snippets/doc_src_q3dscatter_construction.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::DataVisualization
)
