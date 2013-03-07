load(qt_parts)

# We need opengl, minimum es2 or desktop
!contains(QT_CONFIG, opengl) {
    error(QtDataVis3D requires OpenGL!)
}
contains(QT_CONFIG, opengles1) {
    error(QtDataVis3D does not support OpenGL ES 1!)
}
