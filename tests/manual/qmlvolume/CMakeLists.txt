set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_internal_add_manual_test(qmlvolume
    GUI
    SOURCES
        datasource.cpp datasource.h
        main.cpp
    )
target_link_libraries(qmlvolume PUBLIC
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
