set(qml_files
    "designer/Bars3DSpecifics.qml"
    "designer/Scatter3DSpecifics.qml"
    "designer/Surface3DSpecifics.qml"
    "designer/default/Bars3D.qml"
    "designer/default/Scatter3D.qml"
    "designer/default/Surface3D.qml"
)

set(resources
    "designer/qtdatavisualization.metainfo"
    "designer/images/bars3d-icon.png"
    "designer/images/bars3d-icon16.png"
    "designer/images/scatter3d-icon.png"
    "designer/images/scatter3d-icon16.png"
    "designer/images/surface3d-icon.png"
    "designer/images/surface3d-icon16.png"
)

set_source_files_properties(
    ${qml_files}
    PROPERTIES
        QT_QML_SKIP_QMLDIR_ENTRY TRUE
)

qt_internal_add_qml_module(DataVisualizationQml2
    URI "QtDataVisualization"
    VERSION "${PROJECT_VERSION}"
    PAST_MAJOR_VERSIONS 1
    CLASS_NAME QtDataVisualizationQml2Plugin
    PLUGIN_TARGET DataVisualizationQml2
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    NO_GENERATE_QMLTYPES
    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
    SOURCES
        abstractdeclarative.cpp abstractdeclarative_p.h
        colorgradient.cpp colorgradient_p.h
        datavisualizationqml2_plugin.cpp datavisualizationqml2_plugin.h
        declarativebars.cpp declarativebars_p.h
        declarativecolor.cpp declarativecolor_p.h
        declarativerendernode.cpp declarativerendernode_p.h
        declarativescatter.cpp declarativescatter_p.h
        declarativescene.cpp declarativescene_p.h
        declarativeseries.cpp declarativeseries_p.h
        declarativesurface.cpp declarativesurface_p.h
        declarativetheme.cpp declarativetheme_p.h
        enumtostringmap.cpp enumtostringmap_p.h
        glstatestore.cpp glstatestore_p.h
    INCLUDE_DIRECTORIES
        ../../include
        ../../include/QtDataVisualization
        ../datavisualization/axis
        ../datavisualization/data
        ../datavisualization/engine
        ../datavisualization/global
        ../datavisualization/input
        ../datavisualization/theme
    QML_FILES
        ${qml_files}
    RESOURCES
        ${resources}
    LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::OpenGL
        Qt::Qml
        Qt::Quick
        DataVisualization
)

qt_internal_extend_target(DataVisualizationQml2 CONDITION MACOS
    PUBLIC_LIBRARIES
        Qt::GuiPrivate
)

