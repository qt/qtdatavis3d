TEMPLATE = subdirs
SUBDIRS += qmlbars \
           qmlscatter \
           qmlsurface \
           qmlcustominput \
           qmllegend \
           qmlmultigraph \
           qmloscilloscope \
           qmlsurfacelayers \
           qmlaxisformatter \
           qmlaxisdrag

!android:!ios {
    SUBDIRS += bars \
               custominput \
               customproxy \
               itemmodel \
               scatter \
               surface \
               rotations \
               draggableaxes \
               customitems
}

qtHaveModule(multimedia):!android:!ios: SUBDIRS += audiolevels
