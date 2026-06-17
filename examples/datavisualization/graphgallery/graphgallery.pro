android|ios|winrt {
    error( "This example is not supported for android, ios, or winrt." )
}

TEMPLATE = app

QT += datavisualization widgets
requires(qtConfig(fontcombobox))
requires(qtConfig(combobox))

SOURCES += main.cpp \
    bargraph.cpp \
    graphmodifier.cpp \
    rainfalldata.cpp \
    variantdataset.cpp \
    variantbardataproxy.cpp \
    variantbardatamapping.cpp \
    scattergraph.cpp \
    scatterdatamodifier.cpp \
    axesinputhandler.cpp \
    surfacegraph.cpp \
    surfacegraphmodifier.cpp \
    custominputhandler.cpp \
    highlightseries.cpp \
    topographicseries.cpp

HEADERS += \
    bargraph.h \
    graphmodifier.h \
    rainfalldata.h \
    variantdataset.h \
    variantbardataproxy.h \
    variantbardatamapping.h \
    scattergraph.h \
    scatterdatamodifier.h \
    axesinputhandler.h \
    surfacegraph.h \
    surfacegraphmodifier.h \
    custominputhandler.h \
    highlightseries.h \
    topographicseries.h

RESOURCES += graphgallery.qrc

OTHER_FILES += doc/src/* \
               doc/images/* \
               data/*
