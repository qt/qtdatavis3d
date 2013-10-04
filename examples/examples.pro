TEMPLATE = subdirs
SUBDIRS += qmlbars \
           qmlscatter \
           qmlsurface
!android: {
    SUBDIRS += bars \
               rainfall \
               widget \
               scatter \
               surface
}

qtHaveModule(multimedia):!android: SUBDIRS += audiolevels
