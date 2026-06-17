android|ios|winrt {
    error( "This example is not supported for android, ios, or winrt." )
}

TEMPLATE = app

QT += datavisualization widgets

SOURCES += main.cpp volumetric.cpp
HEADERS += volumetric.h

OTHER_FILES += doc/src/* \
               doc/images/*

RESOURCES += volumetric.qrc
