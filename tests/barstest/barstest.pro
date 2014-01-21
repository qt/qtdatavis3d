!include( ../tests.pri ) {
    error( "Couldn't find the tests.pri file!" )
}

SOURCES += main.cpp chart.cpp custominputhandler.cpp
HEADERS += chart.h custominputhandler.h

QT += widgets

INSTALLS += target
