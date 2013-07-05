!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp

INSTALLS += target

RESOURCES += \
    rainfall.qrc

OTHER_FILES += data/raindata.txt
