qt_add_executable(qmlperf
    GUI
    SOURCES
        datagenerator.cpp datagenerator.h
        main.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::DataVisualization
)

set(qmlperf_resource_files
    "qml/qmlperf/main.qml"
)

qt_internal_add_resource(qmlperf "qmlperf"
    PREFIX
        "/"
    FILES
        ${qmlperf_resource_files}
)
