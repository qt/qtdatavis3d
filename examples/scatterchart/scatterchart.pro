!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp scatterchart.cpp
HEADERS += scatterchart.h

QT += widgets

INSTALLS += target
