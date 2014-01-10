TEMPLATE = lib
TARGET = datavisualizationqml2
QT += qml quick datavisualization
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = QtDataVisualization

static {
    DEFINES += QT_DATAVISUALIZATION_STATICLIB
    # Only build qml plugin static if Qt itself is also built static
    !contains(QT_CONFIG, static): CONFIG -= static staticlib

    # Insert the plugin URI into its meta data to enable static plugin usage
    QMAKE_MOC_OPTIONS += -Muri=$$uri
}

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
    declarativescene.cpp

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
    declarativescene_p.h

OTHER_FILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir

installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
qmldir.path = $$installPath
target.path = $$installPath
INSTALLS += target qmldir

