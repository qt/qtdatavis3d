qt_add_executable(galaxy
    GUI
    EXCEPTIONS
    SOURCES
        cumulativedistributor.cpp cumulativedistributor.h
        galaxydata.cpp galaxydata.h
        main.cpp
        star.cpp star.h
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Widgets
)
