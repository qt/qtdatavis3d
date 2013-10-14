!include( ../tests.pri ) {
    error( "Couldn't find the tests.pri file!" )
}

SOURCES += main.cpp \
    graphmodifier.cpp

QT += widgets

INSTALLS += target

HEADERS += \
    graphmodifier.h

RESOURCES += \
    surfacetest.qrc
