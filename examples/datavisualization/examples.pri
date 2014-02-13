INCLUDEPATH += ../../../include

LIBS += -L$$OUT_PWD/../../../lib

TEMPLATE = app

QT += datavisualization

target.path = $$[QT_INSTALL_EXAMPLES]/datavisualization/$$TARGET
INSTALLS += target

OTHER_FILES += doc/src/* \
               doc/images/*
