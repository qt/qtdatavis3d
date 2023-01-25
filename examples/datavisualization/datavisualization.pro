TEMPLATE = subdirs
qtHaveModule(quick) {
    SUBDIRS += qmlbars \
               qmlscatter \
               qmlsurface \
               qml3doscilloscope \
               qmlaxisformatter \
               qmlaxisdrag \
               qmlspectrogram
}

!android:!ios:!winrt {
    SUBDIRS += bars \
               customproxy \
               scatter \
               surface \
               draggableaxes \
               customitems \
               texturesurface \
               volumetric
}

