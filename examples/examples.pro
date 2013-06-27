TEMPLATE = subdirs
SUBDIRS += barchart \
           rainfall \
           widget \
           mapdata \
           qmlbarchart \
           qmlmaps \
           surfacechart

qtHaveModule(multimedia):!android: SUBDIRS += spectrum
