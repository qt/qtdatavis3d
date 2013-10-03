!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp \
    rainfallgraph.cpp \
    variantdataset.cpp \
    variantbardataproxy.cpp \
    variantbardatamapping.cpp \

HEADERS += \
    rainfallgraph.h \
    variantdataset.h \
    variantbardataproxy.h \
    variantbardatamapping.h

INSTALLS += target

RESOURCES += \
    rainfall.qrc

OTHER_FILES += data/raindata.txt \
               doc/src/* \
               doc/images/*

