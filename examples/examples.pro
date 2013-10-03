TEMPLATE = subdirs
SUBDIRS += qmlbars \
           qmlscatter \
           qmlsurface
!android: {
    SUBDIRS += bars \
               rainfall \
               widget \
               scatter
}

qtHaveModule(multimedia):!android: SUBDIRS += audiolevels
