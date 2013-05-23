SOURCES += main.cpp
QT += datavis3d


android {
    target.path = /libs/$$ANDROID_TARGET_ARCH
} else {
    target.path = $$[QT_INSTALL_EXAMPLES]/datavis3d/rainfall
}
INSTALLS += target
