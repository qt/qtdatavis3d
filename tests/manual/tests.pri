INCLUDEPATH += ../../../include

LIBS += -L$$OUT_PWD/../../lib

TEMPLATE = app

QT += graphs

contains(TARGET, qml.*) {
    QT += qml quick
}

target.path = $$[QT_INSTALL_TESTS]/graphs/$$TARGET
INSTALLS += target
