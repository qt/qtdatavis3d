TEMPLATE = subdirs
SUBDIRS += barchart \
           rainfall \
           widget \
           mapdata \
           qmlbarchart

qtHaveModule(multimedia): SUBDIRS += spectrum
