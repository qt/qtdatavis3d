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

wince* {
    # The Microsoft MIPS compiler crashes if /Og is specified.
    # -O2/1 expands to /Og plus additional arguments.
    contains(DEFINES, MIPS) {
        QMAKE_CXXFLAGS_RELEASE ~= s/-O2/-Oi -Ot -Oy -Ob2/
        QMAKE_CXXFLAGS_RELEASE ~= s/-O1/-Os -Oy -Ob2/
    }
}

OTHER_FILES += doc/qtdatavisualization.qdocconf \
               doc/src/* \
               doc/images/* \
               doc/snippets/* \
               global/*.qdoc

