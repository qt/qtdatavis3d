load(qt_parts)

# We need opengl, minimum es2 or desktop
!contains(QT_CONFIG, opengl) {
    error(QtDataVisualization requires OpenGL!)
}
contains(QT_CONFIG, opengles1) {
    error(QtDataVisualization does not support OpenGL ES 1!)
}
