TEMPLATE = lib
TARGET = datavis3dqml2
QT += qml quick datavis3d
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.digia.QtDataVis3D

staticlib:DEFINES+=QTENTERPRISE_DATAVIS3D_STATICLIB

# Input
INCLUDEPATH += ../datavis3d/engine

SOURCES += \
    datavis3dqml2_plugin.cpp \
    declarativebars.cpp \
    declarativemaps.cpp  #\
    #declarativedataitem.cpp \
    #declarativedatarow.cpp \
    #declarativedataset.cpp

HEADERS += \
    datavis3dqml2_plugin.h \
    declarativebars.h \
    declarativemaps.h \  #\
    declarativebars_p.h
    #declarativedataitem.h \
    #declarativedatarow.h \
    #declarativedataset.h

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

