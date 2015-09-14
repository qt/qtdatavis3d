# Target can't start with 'Qt' as it gets major version number inserted into it in that case,
# which we don't want. Exception is OS X bundles, where the target name must match the module name
mac:CONFIG(shared, static|shared):contains(QT_CONFIG, qt_framework) {
    TARGET = QtDataVisualization
} else {
    TARGET = DataVisualization
}
message($$QT_CONFIG)

QT += core gui
osx: QT +=  gui-private
DEFINES += QT_DATAVISUALIZATION_LIBRARY

# Fix exports in static builds for applications linking datavisualization module
static: MODULE_DEFINES += QT_DATAVISUALIZATION_STATICLIB

MODULE_INCNAME = QtDataVisualization

QMAKE_DOCS = $$PWD/doc/qtdatavisualization.qdocconf

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

OTHER_FILES += doc/qtdatavisualization.qdocconf \
               doc/src/* \
               doc/images/* \
               doc/snippets/* \
               global/*.qdoc

