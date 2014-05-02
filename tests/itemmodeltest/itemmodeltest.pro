android|ios {
    error( "This test is not supported for android or ios." )
}

!include( ../tests.pri ) {
    error( "Couldn't find the tests.pri file!" )
}

SOURCES += main.cpp

QT += widgets
