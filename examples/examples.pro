TEMPLATE = subdirs
SUBDIRS += qmlbarchart \
           qmlscatter \
           qmlsurface
!android: {
    SUBDIRS += barchart \
               rainfall \
               widget \
               surfacechart \
               scatterchart
}

qtHaveModule(multimedia):!android: SUBDIRS += audiolevels
