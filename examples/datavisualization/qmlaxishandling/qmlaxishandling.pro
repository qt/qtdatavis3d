TEMPLATE = app

QT += datavisualization qml quick

DEFINES += QMAKE_BUILD

SOURCES += main.cpp \
    customformatter.cpp

HEADERS += customformatter.h

RESOURCES += qmlaxishandling.qrc

OTHER_FILES += doc/src/* \
               doc/images/* \
               qml/qmlaxishandling/*
