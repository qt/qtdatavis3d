# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

set(qml_files
    "designer/Bars3DSpecifics.qml"
    "designer/Scatter3DSpecifics.qml"
    "designer/Surface3DSpecifics.qml"
    "designer/default/Bars3D.qml"
    "designer/default/Scatter3D.qml"
    "designer/default/Surface3D.qml"
)

set(qml_component_resources
    "qml/DatapointSphere.qml"
    "qml/DatapointCube.qml"
)

set(qml_axis_resurces
    "qml/AxisLabel.qml"
    "qml/GridLine.qml"
    "qml/ItemLabel.qml"
)

set(qml_material_resources
    "qml/RangeGradientMaterial.qml"
    "qml/RangeGradientMaterialInstancing.qml"
    "qml/SurfaceShadowNoTex.qml"
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

# .mesh file aliases
set_source_files_properties("meshes/arrowFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "arrowMesh"
)
set_source_files_properties("meshes/arrowSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "arrowMeshSmooth"
)
set_source_files_properties("meshes/background.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "backgroundMesh"
)
set_source_files_properties("meshes/barFilledFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "BevelBarMeshFull"
)
set_source_files_properties("meshes/barFilledSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "BevelBarMeshSmoothFull"
)
set_source_files_properties("meshes/barFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "bevelBarMesh"
)
set_source_files_properties("meshes/barSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "bevelBarMeshSmooth"
)
set_source_files_properties("meshes/cubeFilledFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "barMeshFull"
)
set_source_files_properties("meshes/cubeFilledSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "barMeshSmoothFull"
)
set_source_files_properties("meshes/cubeFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "barMesh"
)
set_source_files_properties("meshes/cubeSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "barMeshSmooth"
)
set_source_files_properties("meshes/coneFilledFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "coneMeshFull"
)
set_source_files_properties("meshes/coneFilledSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "coneMeshSmoothFull"
)
set_source_files_properties("meshes/coneFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "coneMesh"
)
set_source_files_properties("meshes/coneSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "coneMeshSmooth"
)
set_source_files_properties("meshes/cylinderFilledFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderMeshFull"
)
set_source_files_properties("meshes/cylinderFilledSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderMeshSmoothFull"
)
set_source_files_properties("meshes/cylinderFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderMesh"
)
set_source_files_properties("meshes/cylinderSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "cylinderMeshSmooth"
)
set_source_files_properties("meshes/minimalFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "minimalMesh"
)
set_source_files_properties("meshes/minimalSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "minimalMeshSmooth"
)
set_source_files_properties("meshes/plane.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "planeMesh"
)
set_source_files_properties("meshes/pyramidFilledFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidMeshFull"
)
set_source_files_properties("meshes/pyramidFilledSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidMeshSmoothFull"
)
set_source_files_properties("meshes/pyramidFlat.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidMesh"
)
set_source_files_properties("meshes/pyramidSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "pyramidMeshSmooth"
)
set_source_files_properties("meshes/sphere.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "sphereMesh"
)
set_source_files_properties("meshes/sphereSmooth.mesh"
    PROPERTIES QT_RESOURCE_ALIAS "sphereMeshSmooth"
)

set(mesh_resource_files
    # .mesh files
    "meshes/background.mesh"
    "meshes/arrowFlat.mesh"
    "meshes/arrowSmooth.mesh"
    "meshes/barFilledFlat.mesh"
    "meshes/barFilledSmooth.mesh"
    "meshes/barFlat.mesh"
    "meshes/barSmooth.mesh"
    "meshes/coneFilledFlat.mesh"
    "meshes/coneFilledSmooth.mesh"
    "meshes/coneFlat.mesh"
    "meshes/coneSmooth.mesh"
    "meshes/cubeFilledFlat.mesh"
    "meshes/cubeFilledSmooth.mesh"
    "meshes/cubeFlat.mesh"
    "meshes/cubeSmooth.mesh"
    "meshes/cylinderFilledFlat.mesh"
    "meshes/cylinderFilledSmooth.mesh"
    "meshes/cylinderFlat.mesh"
    "meshes/cylinderSmooth.mesh"
    "meshes/minimalFlat.mesh"
    "meshes/minimalSmooth.mesh"
    "meshes/plane.mesh"
    "meshes/pyramidFilledFlat.mesh"
    "meshes/pyramidFilledSmooth.mesh"
    "meshes/pyramidFlat.mesh"
    "meshes/pyramidSmooth.mesh"
    "meshes/sphere.mesh"
    "meshes/sphereSmooth.mesh"
)

set_source_files_properties("qml/DatapointSphere.qml"
    PROPERTIES QT_RESOURCE_ALIAS "DatapointSphere"
)

set_source_files_properties("qml/DatapointCube.qml"
    PROPERTIES QT_RESOURCE_ALIAS "DatapointCube"
)

set_source_files_properties("qml/AxisLabel.qml"
    PROPERTIES QT_RESOURCE_ALIAS "AxisLabel"
)

set_source_files_properties("qml/GridLine.qml"
    PROPERTIES QT_RESOURCE_ALIAS "GridLine"
)

set_source_files_properties("qml/ItemLabel.qml"
    PROPERTIES QT_RESOURCE_ALIAS "ItemLabel"
)

set_source_files_properties("qml/RangeGradientMaterial.qml"
    PROPERTIES QT_RESOURCE_ALIAS "RangeGradientMaterial"
)
set_source_files_properties("qml/RangeGradientMaterialInstancing.qml"
    PROPERTIES QT_RESOURCE_ALIAS "RangeGradientMaterialInstancing"
)

set_source_files_properties(${qml_files} PROPERTIES
    QT_QML_SKIP_QMLDIR_ENTRY TRUE
)

qt_internal_add_qml_module(DataVisualizationQml
    URI "QtDataVisualization"
    VERSION "${PROJECT_VERSION}"
    PAST_MAJOR_VERSIONS 1
    DEPENDENCIES
        QtQuick
    SOURCES
        abstractdeclarative.cpp abstractdeclarative_p.h
        colorgradient.cpp colorgradient_p.h
        declarativebars.cpp declarativebars_p.h
        declarativecolor.cpp declarativecolor_p.h
        declarativerendernode.cpp declarativerendernode_p.h
        declarativescatter.cpp declarativescatter_p.h
        declarativescene.cpp declarativescene_p.h
        declarativeseries.cpp declarativeseries_p.h
        declarativesurface.cpp declarativesurface_p.h
        declarativetheme.cpp declarativetheme_p.h
        enumtostringmap.cpp enumtostringmap_p.h
        foreigntypes_p.h
        glstatestore.cpp glstatestore_p.h
        qquickdatavisitem.cpp qquickdatavisitem_p.h
        qquickdatavissurface.cpp qquickdatavissurface_p.h
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
        Qt::DataVisualizationPrivate
        Qt::Quick3D
        Qt::Quick3DPrivate
        Qt::Quick3DRuntimeRenderPrivate
    PRIVATE_MODULE_INTERFACE
        Qt::Quick3DPrivate
        Qt::Quick3DRuntimeRenderPrivate
)

qt_internal_add_resource(DataVisualizationQml "qml_axis_resurces"
    PREFIX
        "/axis"
    BASE
        "qml"
    FILES
        ${qml_axis_resurces}
)

qt_internal_add_resource(DataVisualizationQml "qml_material_resources"
    PREFIX
        "/materials"
    BASE
        "qml"
    FILES
        ${qml_material_resources}
)

qt_internal_add_resource(DataVisualizationQml "qml_component_resources"
    PREFIX
        "/datapointModels"
    BASE
        "qml"
    FILES
        ${qml_component_resources}
)

qt_internal_add_resource(DataVisualizationQml "meshes"
    PREFIX
        "/defaultMeshes"
    BASE
        "meshes"
    FILES
        ${mesh_resource_files}
)


qt_internal_extend_target(DataVisualizationQml CONDITION MACOS
    PUBLIC_LIBRARIES
        Qt::GuiPrivate
)
