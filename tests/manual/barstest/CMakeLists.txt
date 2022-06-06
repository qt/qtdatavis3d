set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_internal_add_manual_test(barstest
    GUI
    SOURCES
        chart.cpp chart.h
        sliderwrapper.cpp sliderwrapper.h
        buttonwrapper.cpp buttonwrapper.h
        custominputhandler.cpp custominputhandler.h
        main.cpp
    )
target_link_libraries(barstest PUBLIC
    Qt::Gui
    Qt::Widgets
    Qt::DataVisualization
    )

set(barstest_resource_files
    "shuttle.obj"
    "shuttle.png"
    )

qt_internal_add_resource(barstest "barstest"
    PREFIX
    "/"
    FILES
    ${barstest_resource_files}
    )
