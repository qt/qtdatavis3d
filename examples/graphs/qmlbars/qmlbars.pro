!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp

RESOURCES += qmlbars.qrc

OTHER_FILES += doc/src/* \
               doc/images/* \
               qml/qmlbars/*
