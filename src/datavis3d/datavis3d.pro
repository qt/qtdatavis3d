TARGET = QtDataVis3D
QT = core gui qml

DEFINES += QTCOMMERCIALDATAVIS3D_LIBRARY
#DEFINES += QT_NO_USING_NAMESPACE
#win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x61000000

QMAKE_DOCS = $$PWD/doc/qtdatavis3d.qdocconf

load(qt_module)

include($$PWD/common.pri)
include($$PWD/engine/engine.pri)
include($$PWD/global/global.pri)
include($$PWD/utils/utils.pri)

wince* {
    # The Microsoft MIPS compiler crashes if /Og is specified.
    # -O2/1 expands to /Og plus additional arguments.
    contains(DEFINES, MIPS) {
        QMAKE_CXXFLAGS_RELEASE ~= s/-O2/-Oi -Ot -Oy -Ob2/
        QMAKE_CXXFLAGS_RELEASE ~= s/-O1/-Os -Oy -Ob2/
    }
}
