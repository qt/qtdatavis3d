set(CMAKE_INCLUDE_CURRENT_DIR ON)
qt_add_executable(galaxy
    cumulativedistributor.cpp cumulativedistributor.h
    galaxydata.cpp galaxydata.h
    main.cpp
    star.cpp star.h
    )
target_link_libraries(galaxy PUBLIC
    Qt::Gui
    Qt::Widgets
    )
