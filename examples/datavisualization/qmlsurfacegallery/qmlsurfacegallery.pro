TEMPLATE = app

DEFINES += QMAKE_BUILD

QT += datavisualization qml quick

SOURCES += main.cpp \
           datasource.cpp
HEADERS += datasource.h

RESOURCES += qmlsurfacegallery.qrc

OTHER_FILES += doc/src/* \
               doc/images/* \
               qml/qmlsurfacegallery/*
