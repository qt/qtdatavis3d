TEMPLATE = subdirs
SUBDIRS += barchart \
           rainfall \
           widget \
           qmlbarchart \
           qmlscatter \
           surfacechart \
           scatterchart

qtHaveModule(multimedia):!android: SUBDIRS += audiolevels
