!include( ../tests.pri ) {
    error( "Couldn't find the tests.pri file!" )
}

SOURCES += main.cpp \
           data.cpp
HEADERS += data.h

QT += widgets

INSTALLS += target

RESOURCES += \
    multigraphs.qrc
