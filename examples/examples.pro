TEMPLATE = subdirs
SUBDIRS += barchart \
           rainfall \
           widget \
           mapdata \
           qmlbarchart \
           qmlmaps \
           surfacechart \
           scatterchart

qtHaveModule(multimedia):!android: SUBDIRS += spectrum
