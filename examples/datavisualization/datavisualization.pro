TEMPLATE = subdirs
qtHaveModule(quick) {
    SUBDIRS += qmlbars \
               qmlscatter \
               qmlsurface \
               qmlcustominput \
               qmllegend \
               qmlmultigraph \
               qml3doscilloscope \
               qmlsurfacelayers \
               qmlaxisformatter \
               qmlaxisdrag \
               qmlspectrogram
}

!android:!ios:!winrt {
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
}

