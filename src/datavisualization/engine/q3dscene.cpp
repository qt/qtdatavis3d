/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
**
** Licensees holding valid Qt Enterprise licenses may use this file in
** accordance with the Qt Enterprise License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
**
****************************************************************************/

#include <qmath.h>

#include "datavisualizationglobal_p.h"

#include "q3dscene.h"
#include "q3dscene_p.h"
#include "q3dcamera.h"
#include "q3dlight.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Q3DScene::Q3DScene(QObject *parent) :
    QObject(parent),
    d_ptr(new Q3DScenePrivate(this))
{
}

Q3DScene *Q3DScene::clone(QObject *parent)
{
    Q3DScene *cloneScene = new Q3DScene(parent);
    cloneScene->setViewport(viewport());
    cloneScene->setMainViewport(mainViewport());
    cloneScene->setSliceViewport(sliceViewport());

    if (d_ptr->m_camera) {
        Q3DCamera *cloneCamera = new Q3DCamera();
        cloneCamera->copyValuesFrom(*d_ptr->m_camera);
        cloneScene->setCamera(cloneCamera);
    }

    if (d_ptr->m_light) {
        Q3DLight *cloneLight = new Q3DLight();
        cloneLight->copyValuesFrom(*d_ptr->m_light);
        cloneScene->setLight(cloneLight);
    }

    cloneScene->setSlicingActivated(isSlicingActivated());
    cloneScene->setUnderSideCameraEnabled(isUnderSideCameraEnabled());

    return cloneScene;
}

Q3DScene::~Q3DScene()
{
}

QRect Q3DScene::viewport() const
{
    return d_ptr->m_viewport;
}

void Q3DScene::setViewport(const QRect &viewport)
{
    d_ptr->m_viewport = viewport;
    d_ptr->m_viewport.setX(0);
    d_ptr->m_viewport.setY(0);
}

void Q3DScene::setViewportSize(int width, int height)
{
    d_ptr->m_viewport.setWidth(width);
    d_ptr->m_viewport.setHeight(height);
}

QRect Q3DScene::mainViewport() const
{
    return d_ptr->m_mainViewport;
}

void Q3DScene::setMainViewport(const QRect &mainViewPort)
{
    d_ptr->m_mainViewport = mainViewPort;
}

bool Q3DScene::isInputInsideMainView(const QPoint &point)
{
    int x = point.x();
    int y = point.y();
    int areaMinX = d_ptr->m_mainViewport.x();
    int areaMaxX = d_ptr->m_mainViewport.x() + d_ptr->m_mainViewport.width();
    int areaMaxY = d_ptr->m_viewport.height() - d_ptr->m_mainViewport.y();
    int areaMinY = d_ptr->m_viewport.height() - (d_ptr->m_mainViewport.y() + d_ptr->m_mainViewport.height());

    return ( x > areaMinX && x < areaMaxX && y > areaMinY && y < areaMaxY );
}

bool Q3DScene::isInputInsideSliceView(const QPoint &point)
{
    int x = point.x();
    int y = point.y();
    int areaMinX = d_ptr->m_sliceViewport.x();
    int areaMaxX = d_ptr->m_sliceViewport.x() + d_ptr->m_sliceViewport.width();
    int areaMaxY = d_ptr->m_viewport.height() - d_ptr->m_sliceViewport.y();
    int areaMinY = d_ptr->m_viewport.height() - (d_ptr->m_sliceViewport.y() + d_ptr->m_sliceViewport.height());

    return ( x > areaMinX && x < areaMaxX && y > areaMinY && y < areaMaxY );
}

QRect Q3DScene::sliceViewport() const
{
    return d_ptr->m_sliceViewport;
}

void Q3DScene::setSliceViewport(const QRect &sliceViewPort)
{
    d_ptr->m_sliceViewport = sliceViewPort;
}

// TODO: Refactor the current way of building the scene...
//       The scene should have clear ownership of camera, light and other future building blocks of the scene.

Q3DCamera *Q3DScene::camera() const
{
    return d_ptr->m_camera;
}

void Q3DScene::setCamera(Q3DCamera *camera)
{
    if (d_ptr->m_camera)
        d_ptr->m_camera->setParentScene(0);

    d_ptr->m_camera = camera;
    d_ptr->m_camera->setParentScene(this);
}

Q3DLight *Q3DScene::light() const
{
    return d_ptr->m_light;
}

void Q3DScene::setLight(Q3DLight *light)
{
    if (d_ptr->m_light)
        d_ptr->m_light->setParentScene(0);

    d_ptr->m_light = light;
    d_ptr->m_light->setParentScene(this);
}

bool Q3DScene::isUnderSideCameraEnabled() const
{
    return d_ptr->m_isUnderSideCameraEnabled;
}

void Q3DScene::setUnderSideCameraEnabled(bool isEnabled)
{
    d_ptr->m_isUnderSideCameraEnabled = isEnabled;
}

bool Q3DScene::isSlicingActivated() const
{
    return d_ptr->m_isSlicingActivated;
}

void Q3DScene::setSlicingActivated(bool isSlicing)
{
    if (isSlicing != d_ptr->m_isSlicingActivated)
        d_ptr->m_isSlicingActivated = isSlicing;
}

void Q3DScene::setLightPositionRelativeToCamera(const QVector3D &relativePosition, GLfloat fixedRotation, GLfloat distanceModifier)
{
    d_ptr->m_light->setPosition(d_ptr->m_camera->calculatePositionRelativeToCamera(relativePosition, fixedRotation, distanceModifier));
}


Q3DScenePrivate::Q3DScenePrivate(Q3DScene *q) :
    q_ptr(q),
    m_camera(0),
    m_light(0),
    m_isUnderSideCameraEnabled(false),
    m_isSlicingActivated(false)
{
}

Q3DScenePrivate::~Q3DScenePrivate()
{
    delete m_camera;
    delete m_light;
}

QT_DATAVISUALIZATION_END_NAMESPACE
