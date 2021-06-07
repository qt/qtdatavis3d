set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_internal_add_manual_test(multigraphs
    GUI
    SOURCES
        data.cpp data.h
        main.cpp
    )
target_link_libraries(multigraphs PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::DataVisualization
    )

set(multigraphs_resource_files
    "australia.png"
    )

qt_internal_add_resource(multigraphs "multigraphs"
    PREFIX
    "/"
    FILES
    ${multigraphs_resource_files}
    )
