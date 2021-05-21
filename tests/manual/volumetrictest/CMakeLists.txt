qt_add_executable(volumetrictest
    GUI
    SOURCES
        main.cpp
        volumetrictest.cpp volumetrictest.h
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Widgets
        Qt::DataVisualization
)

set_source_files_properties("cubeFilledFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "mesh"
)
set(volumetrictest_resource_files
    "cubeFilledFlat.obj"
    "logo.png"
    "logo_no_padding.png"
)

qt_internal_add_resource(volumetrictest "volumetrictest"
    PREFIX
        "/"
    FILES
        ${volumetrictest_resource_files}
)
