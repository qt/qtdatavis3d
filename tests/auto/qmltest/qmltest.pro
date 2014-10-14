TEMPLATE = app
TARGET = tst_qmltest
CONFIG += qmltestcase
CONFIG += console
SOURCES += tst_qmltest.cpp
OTHER_FILES += bars3d\tst_basic.qml \
               bars3d\tst_bars.qml \
               scatter3d\tst_basic.qml \
               scatter3d\tst_scatter.qml \
               surface3d\tst_basic.qml \
               surface3d\tst_surface.qml

RESOURCES += \
    qmltest.qrc
