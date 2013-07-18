!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp \
    chartmodifier.cpp

INSTALLS += target

HEADERS += \
    chartmodifier.h
