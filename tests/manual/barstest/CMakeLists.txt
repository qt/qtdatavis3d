qt_add_executable(barstest
    GUI
    SOURCES
        chart.cpp chart.h
        custominputhandler.cpp custominputhandler.h
        main.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Widgets
)

set(barstest_resource_files
    "shuttle.obj"
    "shuttle.png"
)

qt_add_resource(barstest "barstest"
    PREFIX
        "/"
    FILES
        ${barstest_resource_files}
)
