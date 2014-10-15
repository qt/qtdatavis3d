TEMPLATE = app
TARGET = tst_qmltest
CONFIG += qmltestcase
CONFIG += console
SOURCES += tst_qmltest.cpp
OTHER_FILES += bars3d\tst_basic.qml \
               bars3d\tst_bars.qml \
               bars3d\tst_barseries.qml \
               scatter3d\tst_basic.qml \
               scatter3d\tst_scatter.qml \
               scatter3d\tst_scatterseries.qml \
               surface3d\tst_basic.qml \
               surface3d\tst_surface.qml \
               surface3d\tst_surfaceseries.qml \
               theme3d\tst_theme.qml \
               theme3d\tst_colorgradient.qml \
               theme3d\tst_themecolor.qml

RESOURCES += \
    qmltest.qrc
