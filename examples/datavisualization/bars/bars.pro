!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp graphmodifier.cpp
HEADERS += graphmodifier.h

QT += widgets
