qt_internal_add_manual_test(qmldynamicdata
    GUI
    SOURCES
        main.cpp
    )
target_link_libraries(qmldynamicdata PUBLIC
    Qt::Gui
    Qt::DataVisualization
    )

set(qmldynamicdata_resource_files
    "qml/qmldynamicdata/main.qml"
    )

qt_internal_add_resource(qmldynamicdata "qmldynamicdata"
    PREFIX
    "/"
    FILES
    ${qmldynamicdata_resource_files}
    )
