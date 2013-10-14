TARGET = QtDataVisualization
QT = core gui

DEFINES += QT_DATAVISUALIZATION_LIBRARY

QMAKE_DOCS = $$PWD/doc/qtdatavisualization.qdocconf

load(qt_module)

include($$PWD/common.pri)
include($$PWD/engine/engine.pri)
include($$PWD/global/global.pri)
include($$PWD/utils/utils.pri)
include($$PWD/axis/axis.pri)
include($$PWD/data/data.pri)
include($$PWD/input/input.pri)

android {
    CONFIG += static
}

OTHER_FILES += doc/qtdatavisualization.qdocconf \
               doc/src/* \
               doc/images/* \
               doc/snippets/* \
               global/*.qdoc

