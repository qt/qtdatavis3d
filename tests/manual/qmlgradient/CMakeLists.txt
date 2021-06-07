qt_internal_add_manual_test(qmlgradient
    GUI
    SOURCES
        main.cpp
    )
target_link_libraries(qmlgradient PUBLIC
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

qt_internal_add_resource(qmlgradient "qmlgradient"
    PREFIX
    "/"
    FILES
    ${qmlgradient_resource_files}
    )
