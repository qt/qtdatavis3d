qt_add_executable(minimalSurface
    GUI
    SOURCES
        ../../../src/datavisualization/doc/snippets/doc_src_q3dsurface_construction.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        datavisualization
)
