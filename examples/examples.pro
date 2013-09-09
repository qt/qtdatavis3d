TEMPLATE = subdirs
SUBDIRS += barchart \
           rainfall \
           widget \
           qmlbarchart \
           qmlscatter \
           qmlsurface \
           surfacechart \
           scatterchart

qtHaveModule(multimedia):!android: SUBDIRS += audiolevels
