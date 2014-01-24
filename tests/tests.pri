android {
    target.path = /libs/$$ANDROID_TARGET_ARCH
} else {
    target.path = $$[QT_INSTALL_TESTS]/datavisualization/$$TARGET
}

win32 {
    CONFIG(debug, release|debug):DESTDIR = $$OUT_PWD/debug
    CONFIG(release, release|debug):DESTDIR = $$OUT_PWD/release
} else {
    DESTDIR = $$OUT_PWD
}

INCLUDEPATH += ../../include

LIBS += -L$$OUT_PWD/../../lib

TEMPLATE = app

QT += datavisualization

!static:android {
    # Add Qt library to be loaded in shared android build
    vis_lib_name = DataVisualization
    vis_src_lib = lib$${vis_lib_name}.so
    vis_lib_dir = $$OUT_PWD/../../lib/$$vis_src_lib
    vis_lib_formatted = $$replace(vis_lib_dir, /, $$QMAKE_DIR_SEP)
    ANDROID_EXTRA_LIBS = $$vis_lib_formatted
}

contains(TARGET, qml.*) {
    uri = QtDataVisualization
    lib_name = datavisualizationqml2

    uri_replaced = $$replace(uri, \\., $$QMAKE_DIR_SEP)
    make_qmldir_path = $$DESTDIR/$$uri_replaced
    !exists($$make_qmldir_path) {
        make_qmldir_target = \"$$replace(make_qmldir_path, /, $$QMAKE_DIR_SEP)\"
        system($$QMAKE_MKDIR $$make_qmldir_target)
    }
    copy_qmldir.target = $$make_qmldir_path/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/../../src/$$lib_name/qmldir
    copy_qmldir_formatted = \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    copy_qmldir.commands = $(COPY_FILE) $$copy_qmldir_formatted
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target

    static:contains(QT_CONFIG, static) {
        # Make import scan find our copied qmldir & statically built qml lib
        !win32 {
            LIB_EXTENSION = a
        } else {
            LIB_EXTENSION = lib
        }
        QMLPATHS += $$DESTDIR
        # We need to copy the qmldir & lib already at qmake run stage as import scan is done then
        exists($$copy_qmldir.depends): system($$QMAKE_COPY $$copy_qmldir_formatted)
    } else {
        win32 {
            LIB_EXTENSION = dll
        } else {
            mac|ios {
                LIB_EXTENSION = dylib
            } else {
                LIB_EXTENSION = so
            }
        }
    }

    win32 {
        CONFIG(debug, release|debug) {
            src_dir = /debug
            src_lib = $${lib_name}d.$$LIB_EXTENSION
        }
        CONFIG(release, release|debug){
            src_dir = /release
            src_lib = $${lib_name}.$$LIB_EXTENSION
        }
    } else {
        src_dir =
            mac|ios {
            CONFIG(debug, release|debug) {
                src_lib = lib$${lib_name}_debug.$$LIB_EXTENSION
            }
            CONFIG(release, release|debug){
                src_lib = lib$${lib_name}.$$LIB_EXTENSION
            }
        } else {
            # linux, android
            src_lib = lib$${lib_name}.$$LIB_EXTENSION
        }
    }
    copy_lib.target = $$make_qmldir_path/$$src_lib
    copy_lib.depends = $$OUT_PWD/../../src/$$lib_name$$src_dir/$$src_lib
    copy_lib_formatted = \"$$replace(copy_lib.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_lib.target, /, $$QMAKE_DIR_SEP)\"
    copy_lib.commands = $(COPY_FILE) $$copy_lib_formatted
    QMAKE_EXTRA_TARGETS += copy_lib
    PRE_TARGETDEPS += $$copy_lib.target

    android {
        system($$QMAKE_COPY $$copy_qmldir_formatted)
        android_qmldir.files = $$copy_qmldir.target
        android_qmldir.path = /assets/qml/$$uri_replaced
        INSTALLS += android_qmldir
    }
    ios {
        system($$QMAKE_COPY $$copy_qmldir_formatted)
        ios_qmldir.files = $$copy_qmldir.target
        ios_qmldir.path = /TODO/$$uri_replaced
        INSTALLS += ios_qmldir
    }
    android|ios {
        system($$QMAKE_COPY $$copy_lib_formatted)
        android_qmlplugin.files = $$copy_lib.target
        android_qmlplugin.path = $$target.path
        INSTALLS += android_qmlplugin
    }
}
