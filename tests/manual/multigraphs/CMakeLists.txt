qt_add_executable(multigraphs
    GUI
    SOURCES
        data.cpp data.h
        main.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Widgets
)

set(multigraphs_resource_files
    "australia.png"
)

qt_add_resource(multigraphs "multigraphs"
    PREFIX
        "/"
    FILES
        ${multigraphs_resource_files}
)
