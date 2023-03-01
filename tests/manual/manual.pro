TEMPLATE = subdirs

qtHaveModule(quick) {
    SUBDIRS += qmldynamicdata \
               qmlmultitest \
               qmlvolume \
               qmlperf \
               qmlgradient \
               qmlheightmap \
               qmlbarsrowcolors \
               qmlcustominput \
               qmllegend \
               qmlsurfacelayers
}

!android:!ios:!winrt {
    SUBDIRS += barstest \
               scattertest \
               surfacetest \
               multigraphs \
               directional \
               itemmodeltest \
               volumetrictest \
               rotations \
               custominput \
               itemmodel

    # For testing code snippets of minimal applications
    SUBDIRS += minimalbars \
               minimalscatter \
               minimalsurface
}
