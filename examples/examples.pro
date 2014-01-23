TEMPLATE = subdirs
SUBDIRS += qmlbars \
           qmlscatter \
           qmlsurface \
           qmlcustominput \
           qmllegend

!android: {
    SUBDIRS += bars \
               custominput \
               customproxy \
               itemmodel \
               scatter \
               surface
}

qtHaveModule(multimedia):!android: SUBDIRS += audiolevels
