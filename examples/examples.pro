TEMPLATE = subdirs
SUBDIRS += barchart \
           rainfall \
           widget \
           #mapdata \
           qmlbarchart \
           #qmlmaps \
           qmlscatter \
           surfacechart \
           scatterchart

qtHaveModule(multimedia):!android: SUBDIRS += audiolevels
