TEMPLATE = subdirs
SUBDIRS += barchart \
           rainfall \
           widget \
           mapdata \
           qmlbarchart \
           qmlmaps

qtHaveModule(multimedia):!android: SUBDIRS += spectrum
