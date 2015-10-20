TEMPLATE = subdirs

qtHaveModule(quick) {
    SUBDIRS += qmlcamera \
               qmldynamicdata \
               qmlmultiwindow \
               qmlmultitest \
               qmlvolume \
               qmlperf
}

!android:!ios:!winrt {
    SUBDIRS += barstest \
               scattertest \
               surfacetest \
               multigraphs \
               directional \
               itemmodeltest \
               volumetrictest

    # For testing code snippets of minimal applications
    SUBDIRS += minimalbars \
               minimalscatter \
               minimalsurface

    # Requires Kinect drivers
    #SUBDIRS += kinectsurface
}

qtHaveModule(multimedia):!android:!static:!winrt: SUBDIRS += spectrum
