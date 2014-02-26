TEMPLATE = subdirs
SUBDIRS += qmlbars \
           qmlscatter \
           qmlsurface \
           qmlcustominput \
           qmllegend \
           qmlmultigraph \
           qmloscilloscope \
           qmlsurfacelayers

!android:!ios {
    SUBDIRS += bars \
               custominput \
               customproxy \
               itemmodel \
               scatter \
               surface \
               rotations
}

qtHaveModule(multimedia):!android:!ios: SUBDIRS += audiolevels
