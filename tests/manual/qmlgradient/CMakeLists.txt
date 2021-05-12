qt_add_executable(qmlgradient
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::DataVisualization
)

set(qmlgradient_resource_files
    "qml/qmlgradient/main.qml"
    "crater.png"
)

set_source_files_properties("crater.png"
    PROPERTIES QT_RESOURCE_ALIAS "map"
)

qt_add_resource(qmlgradient "qmlgradient"
    PREFIX
        "/"
    FILES
        ${qmlgradient_resource_files}
)
