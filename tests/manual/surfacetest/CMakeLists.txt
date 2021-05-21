qt_add_executable(surfacetest
    GUI
    SOURCES
        graphmodifier.cpp graphmodifier.h
        main.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Widgets
        Qt::DataVisualization
)

set_source_files_properties("Heightmap.png"
    PROPERTIES QT_RESOURCE_ALIAS "map"
)
set_source_files_properties("mapimage.png"
    PROPERTIES QT_RESOURCE_ALIAS "mapimage"
)
set(surfacetest_resource_files
    "Heightmap.png"
    "mapimage.png"
)

qt_internal_add_resource(surfacetest "surfacetest"
    PREFIX
        "/maps"
    FILES
        ${surfacetest_resource_files}
)
