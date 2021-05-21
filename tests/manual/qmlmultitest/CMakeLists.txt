qt_add_executable(qmlmultitest
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::DataVisualization
)

set(qmlmultitest_resource_files
    "qml/qmlmultitest/Data.qml"
    "qml/qmlmultitest/main.qml"
)

qt_internal_add_resource(qmlmultitest "qmlmultitest"
    PREFIX
        "/"
    FILES
        ${qmlmultitest_resource_files}
)
