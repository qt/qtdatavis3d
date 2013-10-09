!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp

INSTALLS += target

QT += widgets

OTHER_FILES += doc/src/* \
               doc/images/*
