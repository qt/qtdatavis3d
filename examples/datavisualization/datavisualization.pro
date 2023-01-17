TEMPLATE = subdirs
qtHaveModule(quick) {
    SUBDIRS += qmlbars \
               qmlscatter \
               qmlsurface \
               qmlmultigraph \
               qml3doscilloscope \
               qmlsurfacelayers \
               qmlaxisformatter \
               qmlaxisdrag \
               qmlspectrogram
}

!android:!ios:!winrt {
    SUBDIRS += bars \
               customproxy \
               itemmodel \
               scatter \
               surface \
               draggableaxes \
               customitems \
               texturesurface \
               volumetric
}

