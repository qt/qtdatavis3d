TARGET = datavisualizationqml2
QT += qml quick datavisualization
TARGETPATH = QtDataVisualization
IMPORT_VERSION = $$MODULE_VERSION

# Only build qml plugin static if Qt itself is also built static
!contains(QT_CONFIG, static): CONFIG -= static staticlib

include($$PWD/designer/designer.pri)

INCLUDEPATH += ../../include \
               ../../include/QtDataVisualization \
               ../datavisualization/engine \
               ../datavisualization/global \
               ../datavisualization/data \
               ../datavisualization/theme

SOURCES += \
    datavisualizationqml2_plugin.cpp \
    declarativebars.cpp \
    declarativescatter.cpp \
    declarativesurface.cpp \
    abstractdeclarative.cpp \
    colorgradient.cpp \
    declarativeseries.cpp \
    declarativetheme.cpp \
    declarativecolor.cpp \
    declarativescene.cpp \
    declarativerendernode.cpp \
    glstatestore.cpp \
    enumtostringmap.cpp

HEADERS += \
    datavisualizationqml2_plugin.h \
    declarativebars_p.h \
    declarativescatter_p.h \
    declarativesurface_p.h \
    abstractdeclarative_p.h \
    colorgradient_p.h \
    declarativeseries_p.h \
    declarativetheme_p.h \
    declarativecolor_p.h \
    declarativescene_p.h \
    declarativerendernode_p.h \
    glstatestore_p.h \
    enumtostringmap_p.h

OTHER_FILES = qmldir

CONFIG += no_cxx_module

load(qml_plugin)

# Copy qmldir to DESTDIR so we can use the plugin directly from there in our examples
# without having to do 'make install'.
!android:!ios {
    copy_qmldir.target = $$DESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

QML_FILES += \
    $$PWD/plugins.qmltypes
