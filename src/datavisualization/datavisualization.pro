# Target can't start with 'Qt' as it gets major version number inserted into it in that case,
# which we don't want.
TARGET = DataVisualization

QT = core gui
DEFINES += QT_DATAVISUALIZATION_LIBRARY
MODULE_INCNAME = QtDataVisualization

QMAKE_DOCS = $$PWD/doc/qtdatavisualization.qdocconf

load(qt_module)

include($$PWD/common.pri)
include($$PWD/engine/engine.pri)
include($$PWD/global/global.pri)
include($$PWD/utils/utils.pri)
include($$PWD/theme/theme.pri)
include($$PWD/axis/axis.pri)
include($$PWD/data/data.pri)
include($$PWD/input/input.pri)

OTHER_FILES += doc/qtdatavisualization.qdocconf \
               doc/src/* \
               doc/images/* \
               doc/snippets/* \
               global/*.qdoc

