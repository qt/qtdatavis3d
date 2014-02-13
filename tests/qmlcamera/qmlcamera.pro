!include( ../tests.pri ) {
    error( "Couldn't find the tests.pri file!" )
}

SOURCES += main.cpp

RESOURCES += qmlcamera.qrc

OTHER_FILES += qml/qmlcamera/*

