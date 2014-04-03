android|ios {
    error( "This example is not supported for android or ios." )
}

!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES +=  main.cpp \
            data.cpp \
            axesinputhandler.cpp
HEADERS +=  data.h \
            axesinputhandler.h

QT += widgets

OTHER_FILES += doc/src/* \
               doc/images/*
