!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

SOURCES += main.cpp \
    rainfallchart.cpp \
    variantdataset.cpp \
    variantbardataproxy.cpp \
    variantbardatamapping.cpp \

HEADERS += \
    rainfallchart.h \
    variantdataset.h \
    variantbardataproxy.h \
    variantbardatamapping.h

INSTALLS += target

RESOURCES += \
    rainfall.qrc

OTHER_FILES += data/raindata.txt

