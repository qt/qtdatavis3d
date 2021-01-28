qt_add_executable(qmldynamicdata
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::DataVisualization
)

set(qmldynamicdata_resource_files
    "qml/qmldynamicdata/main.qml"
)

qt_add_resource(qmldynamicdata "qmldynamicdata"
    PREFIX
        "/"
    FILES
        ${qmldynamicdata_resource_files}
)
