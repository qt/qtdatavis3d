set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_internal_add_manual_test(qmlperf
    GUI
    SOURCES
        datagenerator.cpp datagenerator.h
        main.cpp
    )
target_link_libraries(qmlperf PUBLIC
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
