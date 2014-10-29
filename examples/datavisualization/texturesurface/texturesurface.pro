android|ios {
    error( "This example is not supported for android or ios." )
}

!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp \
           surfacegraph.cpp \
           topographicseries.cpp \
           highlightseries.cpp \
           custominputhandler.cpp

HEADERS += surfacegraph.h \
           topographicseries.h \
           highlightseries.h \
           custominputhandler.h

QT += widgets

RESOURCES += texturedsurface.qrc

OTHER_FILES += doc/src/* \
               doc/images/*
