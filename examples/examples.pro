TEMPLATE = subdirs
SUBDIRS += qmlbars \
           qmlscatter \
           qmlsurface
!android: {
    SUBDIRS += bars \
               customproxy \
               itemmodel \
               scatter \
               surface
}

qtHaveModule(multimedia):!android: SUBDIRS += audiolevels
