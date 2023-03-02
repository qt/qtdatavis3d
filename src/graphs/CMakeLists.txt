# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

set(qml_files
    "qml/designer/Bars3DSpecifics.qml"
    "qml/designer/Scatter3DSpecifics.qml"
    "qml/designer/Surface3DSpecifics.qml"
    "qml/designer/default/Bars3D.qml"
    "qml/designer/default/Scatter3D.qml"
    "qml/designer/default/Surface3D.qml"
)

set_source_files_properties(${qml_files} PROPERTIES
    QT_QML_SKIP_QMLDIR_ENTRY TRUE
)

qt_internal_add_qml_module(Graphs
    URI "QtGraphs"
    TARGET_PRODUCT "Qt Graphs (Qt $$QT_VERSION)"
    TARGET_DESCRIPTION "Qt Graphs 3D Visualization component for Qt."
    VERSION "${PROJECT_VERSION}"
    PAST_MAJOR_VERSIONS 1
    DEPENDENCIES
        QtQuick
        QML_FILES
            ${qml_files}
    SOURCES
        axis/qabstract3daxis.cpp axis/qabstract3daxis.h axis/qabstract3daxis_p.h
        axis/qcategory3daxis.cpp axis/qcategory3daxis.h axis/qcategory3daxis_p.h
        axis/qlogvalue3daxisformatter.cpp axis/qlogvalue3daxisformatter.h axis/qlogvalue3daxisformatter_p.h
        axis/qvalue3daxis.cpp axis/qvalue3daxis.h axis/qvalue3daxis_p.h
        axis/qvalue3daxisformatter.cpp axis/qvalue3daxisformatter.h axis/qvalue3daxisformatter_p.h

        data/abstractitemmodelhandler.cpp data/abstractitemmodelhandler_p.h
        data/baritemmodelhandler.cpp data/baritemmodelhandler_p.h
        data/qabstract3dseries.cpp data/qabstract3dseries.h data/qabstract3dseries_p.h
        data/qabstractdataproxy.cpp data/qabstractdataproxy.h data/qabstractdataproxy_p.h
        data/qbar3dseries.cpp data/qbar3dseries.h data/qbar3dseries_p.h
        data/qbardataitem.cpp data/qbardataitem.h data/qbardataitem_p.h
        data/qbardataproxy.cpp data/qbardataproxy.h data/qbardataproxy_p.h
        data/qcustom3ditem.cpp data/qcustom3ditem.h data/qcustom3ditem_p.h
        data/qcustom3dlabel.cpp data/qcustom3dlabel.h data/qcustom3dlabel_p.h
        data/qcustom3dvolume.cpp data/qcustom3dvolume.h data/qcustom3dvolume_p.h
        data/qheightmapsurfacedataproxy.cpp data/qheightmapsurfacedataproxy.h data/qheightmapsurfacedataproxy_p.h
        data/qitemmodelbardataproxy.cpp data/qitemmodelbardataproxy.h data/qitemmodelbardataproxy_p.h
        data/qitemmodelscatterdataproxy.cpp data/qitemmodelscatterdataproxy.h data/qitemmodelscatterdataproxy_p.h
        data/qitemmodelsurfacedataproxy.cpp data/qitemmodelsurfacedataproxy.h data/qitemmodelsurfacedataproxy_p.h
        data/qscatter3dseries.cpp data/qscatter3dseries.h data/qscatter3dseries_p.h
        data/qscatterdataitem.cpp data/qscatterdataitem.h data/qscatterdataitem_p.h
        data/qscatterdataproxy.cpp data/qscatterdataproxy.h data/qscatterdataproxy_p.h
        data/qsurface3dseries.cpp data/qsurface3dseries.h data/qsurface3dseries_p.h
        data/qsurfacedataitem.cpp data/qsurfacedataitem.h data/qsurfacedataitem_p.h
        data/qsurfacedataproxy.cpp data/qsurfacedataproxy.h data/qsurfacedataproxy_p.h
        data/scatteritemmodelhandler.cpp data/scatteritemmodelhandler_p.h
        data/surfaceitemmodelhandler.cpp data/surfaceitemmodelhandler_p.h

        engine/abstract3dcontroller.cpp engine/abstract3dcontroller_p.h
        engine/axishelper.cpp engine/axishelper_p.h
        engine/bars3dcontroller.cpp engine/bars3dcontroller_p.h
        engine/scatterinstancing.cpp engine/scatterinstancing_p.h
        engine/q3dbars.cpp engine/q3dbars.h
        engine/q3dcamera.cpp engine/q3dcamera.h engine/q3dcamera_p.h
        engine/q3dlight.cpp engine/q3dlight.h engine/q3dlight_p.h
        engine/q3dobject.cpp engine/q3dobject.h engine/q3dobject_p.h
        engine/q3dscatter.cpp engine/q3dscatter.h
        engine/q3dscene.cpp engine/q3dscene.h engine/q3dscene_p.h
        engine/q3dsurface.cpp engine/q3dsurface.h
        engine/qabstract3dgraph.cpp engine/qabstract3dgraph.h
        engine/scatter3dcontroller.cpp engine/scatter3dcontroller_p.h
        engine/surface3dcontroller.cpp engine/surface3dcontroller_p.h
        engine/surfaceselectioninstancing.cpp engine/surfaceselectioninstancing_p.h

        global/graphsglobal_p.h
        global/qgraphsglobal.h

        input/q3dinputhandler.cpp input/q3dinputhandler.h input/q3dinputhandler_p.h
        input/qabstract3dinputhandler.cpp input/qabstract3dinputhandler.h input/qabstract3dinputhandler_p.h
        input/qtouch3dinputhandler.cpp input/qtouch3dinputhandler.h input/qtouch3dinputhandler_p.h

        theme/q3dtheme.cpp theme/q3dtheme.h theme/q3dtheme_p.h
        theme/thememanager.cpp theme/thememanager_p.h

        utils/qutils.h
        utils/utils.cpp utils/utils_p.h

        qml/foreigntypes_p.h
        qml/colorgradient.cpp qml/colorgradient_p.h
        qml/declarativecolor.cpp qml/declarativecolor_p.h
        qml/declarativescene.cpp qml/declarativescene_p.h
        qml/declarativeseries.cpp qml/declarativeseries_p.h
        qml/declarativetheme.cpp qml/declarativetheme_p.h
        qml/qquickgraphsitem.cpp qml/qquickgraphsitem_p.h
        qml/qquickgraphsbars.cpp qml/qquickgraphsbars_p.h
        qml/qquickgraphssurface.cpp qml/qquickgraphssurface_p.h
        qml/qquickgraphsscatter.cpp qml/qquickgraphsscatter_p.h
        qml/quickgraphstexturedata.cpp qml/quickgraphstexturedata_p.h
    INCLUDE_DIRECTORIES
        axis
        data
        engine
        global
        input
        theme
        utils
        qml
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::OpenGL
        Qt::Qml
        Qt::Quick
        Qt::QuickWidgets
        Qt::Quick3D
        Qt::Quick3DPrivate
        Qt::Quick3DRuntimeRenderPrivate
    PRIVATE_MODULE_INTERFACE
        Qt::Quick3DPrivate
        Qt::Quick3DRuntimeRenderPrivate
    GENERATE_CPP_EXPORTS
)

set_source_files_properties("qml/resources/DatapointSphere.qml"
    PROPERTIES QT_RESOURCE_ALIAS "DatapointSphere"
)

set_source_files_properties("qml/resources/DatapointCube.qml"
    PROPERTIES QT_RESOURCE_ALIAS "DatapointCube"
)

set_source_files_properties("qml/resources/AxisLabel.qml"
    PROPERTIES QT_RESOURCE_ALIAS "AxisLabel"
)

set_source_files_properties("qml/resources/GridLine.qml"
    PROPERTIES QT_RESOURCE_ALIAS "GridLine"
)

set_source_files_properties("qml/resources/ItemLabel.qml"
    PROPERTIES QT_RESOURCE_ALIAS "ItemLabel"
)

set_source_files_properties("qml/resources/RangeGradientMaterial.qml"
    PROPERTIES QT_RESOURCE_ALIAS "RangeGradientMaterial"
)
set_source_files_properties("qml/resources/RangeGradientMaterialInstancing.qml"
    PROPERTIES QT_RESOURCE_ALIAS "RangeGradientMaterialInstancing"
)

set(qml_files
    "qml/designer/Bars3DSpecifics.qml"
    "qml/designer/Scatter3DSpecifics.qml"
    "qml/designer/Surface3DSpecifics.qml"
    "qml/designer/default/Bars3D.qml"
    "qml/designer/default/Scatter3D.qml"
    "qml/designer/default/Surface3D.qml"
)

set(qml_component_resources
    "qml/resources/DatapointSphere.qml"
    "qml/resources/DatapointCube.qml"
)

set(qml_axis_resurces
    "qml/resources/AxisLabel.qml"
    "qml/resources/GridLine.qml"
    "qml/resources/ItemLabel.qml"
)

set(qml_material_resources
    "qml/resources/RangeGradientMaterial.qml"
    "qml/resources/RangeGradientMaterialInstancing.qml"
    "qml/resources/SurfaceShadowNoTex.qml"
)

set(resources
    "qml/designer/Graphs.metainfo"
    "qml/designer/images/bars3d-icon.png"
    "qml/designer/images/bars3d-icon16.png"
    "qml/designer/images/scatter3d-icon.png"
    "qml/designer/images/scatter3d-icon16.png"
    "qml/designer/images/surface3d-icon.png"
    "qml/designer/images/surface3d-icon16.png"
)

set_source_files_properties("engine/meshes/arrowFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "arrow"
)
set_source_files_properties("engine/meshes/arrowSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "arrowSmooth"
)
set_source_files_properties("engine/meshes/background.obj"
    PROPERTIES QT_RESOURCE_ALIAS "background"
)
set_source_files_properties("engine/meshes/backgroundNoFloor.obj"
    PROPERTIES QT_RESOURCE_ALIAS "backgroundNoFloor"
)
set_source_files_properties("engine/meshes/barFilledFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "bevelbarFull"
)
set_source_files_properties("engine/meshes/barFilledSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "bevelbarSmoothFull"
)
set_source_files_properties("engine/meshes/barFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "bevelbar"
)
set_source_files_properties("engine/meshes/barSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "bevelbarSmooth"
)
set_source_files_properties("engine/meshes/coneFilledFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "coneFull"
)
set_source_files_properties("engine/meshes/coneFilledSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "coneSmoothFull"
)
set_source_files_properties("engine/meshes/coneFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "cone"
)
set_source_files_properties("engine/meshes/coneSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "coneSmooth"
)
set_source_files_properties("engine/meshes/cubeFilledFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "barFull"
)
set_source_files_properties("engine/meshes/cubeFilledSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "barSmoothFull"
)
set_source_files_properties("engine/meshes/cubeFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "bar"
)
set_source_files_properties("engine/meshes/cubeSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "barSmooth"
)
set_source_files_properties("engine/meshes/cylinderFilledFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderFull"
)
set_source_files_properties("engine/meshes/cylinderFilledSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderSmoothFull"
)
set_source_files_properties("engine/meshes/cylinderFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "cylinder"
)
set_source_files_properties("engine/meshes/cylinderSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderSmooth"
)
set_source_files_properties("engine/meshes/minimalFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "minimal"
)
set_source_files_properties("engine/meshes/minimalSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "minimalSmooth"
)
set_source_files_properties("engine/meshes/plane.obj"
    PROPERTIES QT_RESOURCE_ALIAS "plane"
)
set_source_files_properties("engine/meshes/pyramidFilledFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidFull"
)
set_source_files_properties("engine/meshes/pyramidFilledSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidSmoothFull"
)
set_source_files_properties("engine/meshes/pyramidFlat.obj"
    PROPERTIES QT_RESOURCE_ALIAS "pyramid"
)
set_source_files_properties("engine/meshes/pyramidSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidSmooth"
)
set_source_files_properties("engine/meshes/sphere.obj"
    PROPERTIES QT_RESOURCE_ALIAS "sphere"
)
set_source_files_properties("engine/meshes/sphereSmooth.obj"
    PROPERTIES QT_RESOURCE_ALIAS "sphereSmooth"
)

# .mesh file aliases
set_source_files_properties("engine/meshes/arrowFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "arrowMesh"
)
set_source_files_properties("engine/meshes/arrowSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "arrowMeshSmooth"
)
set_source_files_properties("engine/meshes/background.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "backgroundMesh"
)
set_source_files_properties("engine/meshes/barFilledFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "bevelBarMeshFull"
)
set_source_files_properties("engine/meshes/barFilledSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "bevelBarMeshSmoothFull"
)
set_source_files_properties("engine/meshes/barFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "bevelBarMesh"
)
set_source_files_properties("engine/meshes/barSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "bevelBarMeshSmooth"
)
set_source_files_properties("engine/meshes/cubeFilledFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "barMeshFull"
)
set_source_files_properties("engine/meshes/cubeFilledSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "barMeshSmoothFull"
)
set_source_files_properties("engine/meshes/cubeFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "barMesh"
)
set_source_files_properties("engine/meshes/cubeSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "barMeshSmooth"
)
set_source_files_properties("engine/meshes/coneFilledFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "coneMeshFull"
)
set_source_files_properties("engine/meshes/coneFilledSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "coneMeshSmoothFull"
)
set_source_files_properties("engine/meshes/coneFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "coneMesh"
)
set_source_files_properties("engine/meshes/coneSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "coneMeshSmooth"
)
set_source_files_properties("engine/meshes/cylinderFilledFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderMeshFull"
)
set_source_files_properties("engine/meshes/cylinderFilledSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderMeshSmoothFull"
)
set_source_files_properties("engine/meshes/cylinderFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderMesh"
)
set_source_files_properties("engine/meshes/cylinderSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderMeshSmooth"
)
set_source_files_properties("engine/meshes/minimalFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "minimalMesh"
)
set_source_files_properties("engine/meshes/minimalSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "minimalMeshSmooth"
)
set_source_files_properties("engine/meshes/plane.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "planeMesh"
)
set_source_files_properties("engine/meshes/pyramidFilledFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidMeshFull"
)
set_source_files_properties("engine/meshes/pyramidFilledSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidMeshSmoothFull"
)
set_source_files_properties("engine/meshes/pyramidFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidMesh"
)
set_source_files_properties("engine/meshes/pyramidSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidMeshSmooth"
)
set_source_files_properties("engine/meshes/sphere.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "sphereMesh"
)
set_source_files_properties("engine/meshes/sphereSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "sphereMeshSmooth"
)
set_source_files_properties("engine/meshes/backgroundNoFloor.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "backgroundNoFloorMesh"
)
set(mesh_resource_files
    # .mesh files
    "engine/meshes/background.mesh"
    "engine/meshes/arrowFlat.mesh"
    "engine/meshes/arrowSmooth.mesh"
    "engine/meshes/barFilledFlat.mesh"
    "engine/meshes/barFilledSmooth.mesh"
    "engine/meshes/barFlat.mesh"
    "engine/meshes/barSmooth.mesh"
    "engine/meshes/coneFilledFlat.mesh"
    "engine/meshes/coneFilledSmooth.mesh"
    "engine/meshes/coneFlat.mesh"
    "engine/meshes/coneSmooth.mesh"
    "engine/meshes/cubeFilledFlat.mesh"
    "engine/meshes/cubeFilledSmooth.mesh"
    "engine/meshes/cubeFlat.mesh"
    "engine/meshes/cubeSmooth.mesh"
    "engine/meshes/cylinderFilledFlat.mesh"
    "engine/meshes/cylinderFilledSmooth.mesh"
    "engine/meshes/cylinderFlat.mesh"
    "engine/meshes/cylinderSmooth.mesh"
    "engine/meshes/minimalFlat.mesh"
    "engine/meshes/minimalSmooth.mesh"
    "engine/meshes/plane.mesh"
    "engine/meshes/pyramidFilledFlat.mesh"
    "engine/meshes/pyramidFilledSmooth.mesh"
    "engine/meshes/pyramidFlat.mesh"
    "engine/meshes/pyramidSmooth.mesh"
    "engine/meshes/sphere.mesh"
    "engine/meshes/sphereSmooth.mesh"
    "engine/meshes/backgroundNoFloor.mesh"
)

set(obj_resource_files
    "engine/meshes/arrowFlat.obj"
    "engine/meshes/arrowSmooth.obj"
    "engine/meshes/background.obj"
    "engine/meshes/backgroundNoFloor.obj"
    "engine/meshes/barFilledFlat.obj"
    "engine/meshes/barFilledSmooth.obj"
    "engine/meshes/barFlat.obj"
    "engine/meshes/barSmooth.obj"
    "engine/meshes/coneFilledFlat.obj"
    "engine/meshes/coneFilledSmooth.obj"
    "engine/meshes/coneFlat.obj"
    "engine/meshes/coneSmooth.obj"
    "engine/meshes/cubeFilledFlat.obj"
    "engine/meshes/cubeFilledSmooth.obj"
    "engine/meshes/cubeFlat.obj"
    "engine/meshes/cubeSmooth.obj"
    "engine/meshes/cylinderFilledFlat.obj"
    "engine/meshes/cylinderFilledSmooth.obj"
    "engine/meshes/cylinderFlat.obj"
    "engine/meshes/cylinderSmooth.obj"
    "engine/meshes/minimalFlat.obj"
    "engine/meshes/minimalSmooth.obj"
    "engine/meshes/plane.obj"
    "engine/meshes/pyramidFilledFlat.obj"
    "engine/meshes/pyramidFilledSmooth.obj"
    "engine/meshes/pyramidFlat.obj"
    "engine/meshes/pyramidSmooth.obj"
    "engine/meshes/sphere.obj"
    "engine/meshes/sphereSmooth.obj"
)

set_source_files_properties("engine/shaders/3dsliceframes.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragment3DSliceFrames"
)
set_source_files_properties("engine/shaders/colorOnY.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentColorOnY"
)
set_source_files_properties("engine/shaders/colorOnY_ES2.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentColorOnYES2"
)
set_source_files_properties("engine/shaders/default.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragment"
)
set_source_files_properties("engine/shaders/default.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertex"
)
set_source_files_properties("engine/shaders/defaultNoMatrices.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexNoMatrices"
)
set_source_files_properties("engine/shaders/default_ES2.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentES2"
)
set_source_files_properties("engine/shaders/depth.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentDepth"
)
set_source_files_properties("engine/shaders/depth.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexDepth"
)
set_source_files_properties("engine/shaders/label.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentLabel"
)
set_source_files_properties("engine/shaders/label.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexLabel"
)
set_source_files_properties("engine/shaders/plainColor.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentPlainColor"
)
set_source_files_properties("engine/shaders/plainColor.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexPlainColor"
)
set_source_files_properties("engine/shaders/point_ES2.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexPointES2"
)
set_source_files_properties("engine/shaders/point_ES2_UV.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexPointES2_UV"
)
set_source_files_properties("engine/shaders/position.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexPosition"
)
set_source_files_properties("engine/shaders/positionmap.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentPositionMap"
)
set_source_files_properties("engine/shaders/shadow.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentShadow"
)
set_source_files_properties("engine/shaders/shadow.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexShadow"
)
set_source_files_properties("engine/shaders/shadowNoMatrices.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexShadowNoMatrices"
)
set_source_files_properties("engine/shaders/shadowNoTex.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentShadowNoTex"
)
set_source_files_properties("engine/shaders/shadowNoTexColorOnY.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentShadowNoTexColorOnY"
)
set_source_files_properties("engine/shaders/surface.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentSurface"
)
set_source_files_properties("engine/shaders/surfaceFlat.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentSurfaceFlat"
)
set_source_files_properties("engine/shaders/surfaceFlat.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexSurfaceFlat"
)
set_source_files_properties("engine/shaders/surfaceShadowFlat.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentSurfaceShadowFlat"
)
set_source_files_properties("engine/shaders/surfaceShadowFlat.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexSurfaceShadowFlat"
)
set_source_files_properties("engine/shaders/surfaceShadowNoTex.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentSurfaceShadowNoTex"
)
set_source_files_properties("engine/shaders/surfaceTexturedFlat.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentSurfaceTexturedFlat"
)
set_source_files_properties("engine/shaders/surfaceTexturedShadow.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentTexturedSurfaceShadow"
)
set_source_files_properties("engine/shaders/surfaceTexturedShadowFlat.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentTexturedSurfaceShadowFlat"
)
set_source_files_properties("engine/shaders/surface_ES2.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentSurfaceES2"
)
set_source_files_properties("engine/shaders/texture.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentTexture"
)
set_source_files_properties("engine/shaders/texture.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexTexture"
)
set_source_files_properties("engine/shaders/texture3d.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentTexture3D"
)
set_source_files_properties("engine/shaders/texture3d.vert"
    PROPERTIES QT_RESOURCE_ALIAS "vertexTexture3D"
)
set_source_files_properties("engine/shaders/texture3dlowdef.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentTexture3DLowDef"
)
set_source_files_properties("engine/shaders/texture3dslice.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentTexture3DSlice"
)
set_source_files_properties("engine/shaders/texture_ES2.frag"
    PROPERTIES QT_RESOURCE_ALIAS "fragmentTextureES2"
)
set(shader_resource_files
    "engine/shaders/3dsliceframes.frag"
    "engine/shaders/colorOnY.frag"
    "engine/shaders/colorOnY_ES2.frag"
    "engine/shaders/default.frag"
    "engine/shaders/default.vert"
    "engine/shaders/defaultNoMatrices.vert"
    "engine/shaders/default_ES2.frag"
    "engine/shaders/depth.frag"
    "engine/shaders/depth.vert"
    "engine/shaders/label.frag"
    "engine/shaders/label.vert"
    "engine/shaders/plainColor.frag"
    "engine/shaders/plainColor.vert"
    "engine/shaders/point_ES2.vert"
    "engine/shaders/point_ES2_UV.vert"
    "engine/shaders/position.vert"
    "engine/shaders/positionmap.frag"
    "engine/shaders/shadow.frag"
    "engine/shaders/shadow.vert"
    "engine/shaders/shadowNoMatrices.vert"
    "engine/shaders/shadowNoTex.frag"
    "engine/shaders/shadowNoTexColorOnY.frag"
    "engine/shaders/surface.frag"
    "engine/shaders/surfaceFlat.frag"
    "engine/shaders/surfaceFlat.vert"
    "engine/shaders/surfaceShadowFlat.frag"
    "engine/shaders/surfaceShadowFlat.vert"
    "engine/shaders/surfaceShadowNoTex.frag"
    "engine/shaders/surfaceTexturedFlat.frag"
    "engine/shaders/surfaceTexturedShadow.frag"
    "engine/shaders/surfaceTexturedShadowFlat.frag"
    "engine/shaders/surface_ES2.frag"
    "engine/shaders/texture.frag"
    "engine/shaders/texture.vert"
    "engine/shaders/texture3d.frag"
    "engine/shaders/texture3d.vert"
    "engine/shaders/texture3dlowdef.frag"
    "engine/shaders/texture3dslice.frag"
    "engine/shaders/texture_ES2.frag"
)

qt_internal_add_resource(Graphs "graphsshaders"
    PREFIX
        "/shaders"
    BASE
        "engine/shaders"
    FILES
        ${shader_resource_files}
)

qt_internal_add_resource(Graphs "graphsmeshes"
    PREFIX
        "/defaultMeshes"
    BASE
        "engine/meshes"
    FILES
        ${mesh_resource_files}
        ${obj_resource_files}
)

qt_internal_add_resource(Graphs "qml_component_resources"
    PREFIX
        "/datapointModels"
    BASE
        "qml"
    FILES
        ${qml_component_resources}
)

qt_internal_add_resource(Graphs "qml_axis_resurces"
    PREFIX
        "/axis"
    BASE
        "qml"
    FILES
        ${qml_axis_resurces}
)

qt_internal_add_resource(Graphs "qml_material_resources"
    PREFIX
        "/materials"
    BASE
        "qml"
    FILES
        ${qml_material_resources}
)

qt_internal_extend_target(Graphs CONDITION MACOS
    LIBRARIES
        Qt::GuiPrivate
    PUBLIC_LIBRARIES
        Qt::Gui
    PRIVATE_MODULE_INTERFACE
        Qt::GuiPrivate
)
qt_internal_add_docs(Graphs
    doc/qtgraphs.qdocconf
)

