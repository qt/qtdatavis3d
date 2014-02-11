!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp scatterdatamodifier.cpp
HEADERS += scatterdatamodifier.h

QT += widgets

RESOURCES += rotations.qrc

INSTALLS += target

OTHER_FILES += doc/src \
               doc/images
