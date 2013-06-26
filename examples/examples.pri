android {
    target.path = /libs/$$ANDROID_TARGET_ARCH
} else {
    target.path = $$[QT_INSTALL_EXAMPLES]/datavis3d/$$TARGET
}

win32 {
    CONFIG(debug, release|debug):DESTDIR = $$OUT_PWD/debug
    CONFIG(release, release|debug):DESTDIR = $$OUT_PWD/release
} else {
    DESTDIR = $$OUT_PWD
}

LIBS += -L$$OUT_PWD/../lib

QT += datavis3d

contains(TARGET, qml.*) {
    uri = com.digia.QtDataVis3D
    lib_name = datavis3dqml2

    uri_replaced = $$replace(uri, \\., $$QMAKE_DIR_SEP)
    make_qmldir_path = $$DESTDIR/$$uri_replaced
    !exists($$make_qmldir_path) {
        make_qmldir_target = \"$$replace(make_qmldir_path, /, $$QMAKE_DIR_SEP)\"
        system($$QMAKE_MKDIR $$make_qmldir_target)
    }
    copy_qmldir_examples.target = $$make_qmldir_path/qmldir
    copy_qmldir_examples.depends = $$_PRO_FILE_PWD_/../../src/$$lib_name/qmldir
    copy_qmldir_examples.commands = $(COPY_FILE) \"$$replace(copy_qmldir_examples.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir_examples.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir_examples
    PRE_TARGETDEPS += $$copy_qmldir_examples.target

    win32 {
        CONFIG(debug, release|debug) {
            src_dir = debug
            src_lib = $${lib_name}d.dll
        }
        CONFIG(release, release|debug){
            src_dir = release
            src_lib = $${lib_name}.dll
        }
    } else {
        src_dir = .
        mac {
            CONFIG(debug, release|debug) {
                src_lib = lib$${lib_name}_debug.dylib
            }
            CONFIG(release, release|debug){
                src_lib = lib$${lib_name}.dylib
            }
        } else {
            src_lib = $${lib_name}.so
        }
    }
    copy_lib.target = $$make_qmldir_path/$$src_lib
    copy_lib.depends = $$OUT_PWD/../../src/$$lib_name/$$src_dir/$$src_lib
    copy_lib.commands = $(COPY_FILE) \"$$replace(copy_lib.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_lib.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_lib
    PRE_TARGETDEPS += $$copy_lib.target
}

#android {
#    contains(TARGET, qml.*) {
#        charts_qmldir.files = $$CHART_BUILD_QML_PLUGIN_DIR/qmldir
#        charts_qmldir.path = /assets/imports/QtCommercial/Chart
#        charts_qmlplugin.files = $$CHART_BUILD_QML_PLUGIN_DIR/libqtcommercialchartqml.so
#        charts_qmlplugin.path = /libs/$$ANDROID_TARGET_ARCH
#        INSTALLS += charts_qmldir charts_qmlplugin
#    }
#}
