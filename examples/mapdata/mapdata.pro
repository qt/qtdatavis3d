!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp mapdata.cpp
HEADERS += mapdata.h

QT += widgets

INSTALLS += target

RESOURCES += \
    mapdata.qrc
