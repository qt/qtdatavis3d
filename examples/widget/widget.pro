SOURCES += main.cpp chart.cpp
HEADERS += chart.h

QT += datavis3d widgets

android {
    target.path = /libs/$$ANDROID_TARGET_ARCH
} else {
    target.path = $$[QT_INSTALL_EXAMPLES]/datavis3d/widget
}
INSTALLS += target
