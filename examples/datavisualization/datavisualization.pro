TEMPLATE = subdirs
qtHaveModule(quick) {
    SUBDIRS += qmlbars \
               qmlscatter \
               qmlsurface \
               qmlcustominput \
               qmllegend \
               qmlmultigraph \
               qmloscilloscope \
               qmlsurfacelayers \
               qmlaxisformatter \
               qmlaxisdrag \
               qmlspectrogram
}

!android:!ios {
    SUBDIRS += bars \
               custominput \
               customproxy \
               itemmodel \
               scatter \
               surface \
               rotations \
               draggableaxes \
               customitems \
               texturesurface \
               volumetric

    qtHaveModule(multimedia): SUBDIRS += audiolevels
}

