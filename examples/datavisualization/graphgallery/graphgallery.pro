android|ios|winrt {
    error( "This example is not supported for android, ios, or winrt." )
}

!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += widgets
requires(qtConfig(fontcombobox))
requires(qtConfig(combobox))

SOURCES += main.cpp \
    graphmodifier.cpp \
    rainfalldata.cpp \
    variantdataset.cpp \
    variantbardataproxy.cpp \
    variantbardatamapping.cpp \

HEADERS += \
    graphmodifier.h \
    rainfalldata.h \
    variantdataset.h \
    variantbardataproxy.h \
    variantbardatamapping.h

RESOURCES += graphgallery.qrc

OTHER_FILES += doc/src/* \
               doc/images/* \
               data/raindata.txt
