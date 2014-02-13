INCLUDEPATH += ../../include

LIBS += -L$$OUT_PWD/../../lib

TEMPLATE = app

QT += datavisualization

target.path = $$[QT_INSTALL_TESTS]/datavisualization/$$TARGET
INSTALLS += target
