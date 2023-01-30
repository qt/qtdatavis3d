TEMPLATE = subdirs
qtHaveModule(quick) {
    SUBDIRS += qmlbars \
               qmlscatter \
               qmlsurface \
               qml3doscilloscope \
               qmlaxishandling \
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

