TEMPLATE = lib
TARGET = datavisualizationqml2
QT += qml quick datavisualization
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.digia.QtDataVisualization

static {
    DEFINES += QT_DATAVISUALIZATION_STATICLIB
    CONFIG -= static staticlib
}

# Input
INCLUDEPATH += ../datavisualization/engine \
               ../datavisualization/global \
               ../datavisualization/data

SOURCES += \
    datavisualizationqml2_plugin.cpp \
    declarativebars.cpp \
    declarativebarsrenderer.cpp \
    declarativescatter.cpp \
    declarativescatterrenderer.cpp \
    declarativesurface.cpp \
    declarativesurfacerenderer.cpp \
    abstractdeclarative.cpp

HEADERS += \
    datavisualizationqml2_plugin.h \
    declarativebars_p.h \
    declarativebarsrenderer_p.h \
    declarativescatter_p.h \
    declarativescatterrenderer_p.h \
    declarativesurface_p.h \
    declarativesurfacerenderer_p.h \
    abstractdeclarative_p.h

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

