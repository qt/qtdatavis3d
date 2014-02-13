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

RESOURCES += customproxy.qrc

OTHER_FILES += data/raindata.txt

