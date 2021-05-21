qt_add_executable(qmlvolume
    GUI
    SOURCES
        datasource.cpp datasource.h
        main.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::DataVisualization
)

set(qmlvolume_resource_files
    "qml/qmlvolume/main.qml"
)

qt_internal_add_resource(qmlvolume "qmlvolume"
    PREFIX
        "/"
    FILES
        ${qmlvolume_resource_files}
)
