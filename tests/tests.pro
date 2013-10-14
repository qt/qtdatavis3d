!contains(QT_CONFIG, opengl) {
    error(Tests requires OpenGL!)
}
contains(QT_CONFIG, opengles1) {
    error(Tests do not support OpenGL ES 1!)
}

TEMPLATE = subdirs

SUBDIRS += barstest \
           scattertest \
           surfacetest \
           qmlcamera

#SUBDIRS += kinectsurface

qtHaveModule(multimedia):!android: SUBDIRS += spectrum
