TEMPLATE = subdirs
SUBDIRS += barchart \
           rainfall \
           widget \
           mapdata

qtHaveModule(multimedia): SUBDIRS += spectrum
