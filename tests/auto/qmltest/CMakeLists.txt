qt_internal_add_test(tst_qmltest
    QMLTEST
    SOURCES
        tst_qmltest.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
)

set(qmltest_resource_files
    "customitem.obj"
    "customtexture.jpg"
)

qt_internal_add_resource(tst_qmltest "qmltest"
    PREFIX
        "/"
    FILES
        ${qmltest_resource_files}
)
