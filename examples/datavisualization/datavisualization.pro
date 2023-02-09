TEMPLATE = subdirs
qtHaveModule(quick) {
    SUBDIRS += qmlbars \
               qmlscatter \
               qmlaxishandling \
               qmlsurfacegallery
}

!android:!ios:!winrt {
    SUBDIRS += scatter \
               surface \
               draggableaxes \
               customitems \
               texturesurface \
               volumetric
}

