TEMPLATE = lib
TARGET = datavis3dqml2
QT += qml quick datavis3d
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.digia.QtDataVis3D

static {
    DEFINES += QT_DATAVIS3D_STATICLIB
    CONFIG -= static staticlib
}

# Input
INCLUDEPATH += ../datavis3d/engine \
               ../datavis3d/global \
               ../datavis3d/data

SOURCES += \
    datavis3dqml2_plugin.cpp \
    declarativebars.cpp \
    declarativebarsrenderer.cpp \
    declarativescatter.cpp \
    declarativescatterrenderer.cpp \
    declarativemaps.cpp \
    declarativemapsrenderer.cpp

HEADERS += \
    datavis3dqml2_plugin.h \
    declarativebars_p.h \
    declarativebarsrenderer_p.h \
    declarativescatter_p.h \
    declarativescatterrenderer_p.h \
    declarativemaps_p.h \
    declarativemapsrenderer_p.h

OTHER_FILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

