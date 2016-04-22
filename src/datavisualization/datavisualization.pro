TARGET = QtDataVisualization

QT += core gui
osx: QT +=  gui-private
DEFINES += QT_DATAVISUALIZATION_LIBRARY

# Fix exports in static builds for applications linking datavisualization module
static: MODULE_DEFINES += QT_DATAVISUALIZATION_STATICLIB

MODULE_INCNAME = QtDataVisualization

QMAKE_DOCS = $$PWD/doc/qtdatavis3d.qdocconf

load(qt_module)

QMAKE_TARGET_PRODUCT = "Qt Data Visualization (Qt $$QT_VERSION)"
QMAKE_TARGET_DESCRIPTION = "3D Data Visualization component for Qt."

include($$PWD/global/global.pri)
include($$PWD/engine/engine.pri)
include($$PWD/utils/utils.pri)
include($$PWD/theme/theme.pri)
include($$PWD/axis/axis.pri)
include($$PWD/data/data.pri)
include($$PWD/input/input.pri)

OTHER_FILES += doc/qtdatavis3d.qdocconf \
               doc/src/* \
               doc/images/* \
               doc/snippets/* \
               global/*.qdoc
