!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp chart.cpp
HEADERS += chart.h

QT += datavis3d widgets

INSTALLS += target
