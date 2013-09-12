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
#include "q3dcamera_p.h"
#include "q3dlight_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Q3DScene::Q3DScene(QObject *parent) :
    QObject(parent),
    d_ptr(new Q3DScenePrivate(this))
{
    setCamera(new Q3DCamera(0));
    setLight(new Q3DLight(0));
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
    if (d_ptr->m_viewport.width() != viewport.width()
            || d_ptr->m_viewport.height() != viewport.height()) {
        d_ptr->m_viewport = viewport;
        d_ptr->m_viewport.setX(0);
        d_ptr->m_viewport.setY(0);
        d_ptr->m_changeTracker.viewportChanged = true;
    }
}

void Q3DScene::setViewportSize(int width, int height)
{
    if (d_ptr->m_viewport.width() != width || d_ptr->m_viewport.height() != height) {
        d_ptr->m_viewport.setWidth(width);
        d_ptr->m_viewport.setHeight(height);
        d_ptr->m_changeTracker.viewportChanged = true;
    }
}

QRect Q3DScene::mainViewport() const
{
    return d_ptr->m_mainViewport;
}

void Q3DScene::setMainViewport(const QRect &mainViewPort)
{
    if (d_ptr->m_mainViewport != mainViewPort) {
        d_ptr->m_mainViewport = mainViewPort;
        d_ptr->m_changeTracker.mainViewportChanged = true;
    }
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
    if (d_ptr->m_sliceViewport != sliceViewPort) {
        d_ptr->m_sliceViewport = sliceViewPort;
        d_ptr->m_changeTracker.sliceViewportChanged = true;
    }
}

// TODO: Refactor the current way of building the scene...
//       The scene should have clear ownership of camera, light and other future building blocks of the scene.

Q3DCamera *Q3DScene::camera() const
{
    return d_ptr->m_camera;
}

void Q3DScene::setCamera(Q3DCamera *camera)
{
    if (camera != d_ptr->m_camera) {
        // TODO Do we need to be able to swap cameras? May need similar ownership mechanism like axes
        delete d_ptr->m_camera;

        d_ptr->m_camera = camera;
        d_ptr->m_camera->setParentScene(this);

        d_ptr->m_changeTracker.cameraChanged = true;

    }
}

Q3DLight *Q3DScene::light() const
{
    return d_ptr->m_light;
}

void Q3DScene::setLight(Q3DLight *light)
{
    if (light != d_ptr->m_light) {
        // TODO Do we need to be able to swap lights? May need similar ownership mechanism like axes
        delete d_ptr->m_light;

        d_ptr->m_light = light;
        d_ptr->m_light->setParentScene(this);

        d_ptr->m_changeTracker.lightChanged = true;
    }
}

bool Q3DScene::isUnderSideCameraEnabled() const
{
    return d_ptr->m_isUnderSideCameraEnabled;
}

void Q3DScene::setUnderSideCameraEnabled(bool isEnabled)
{
    if (d_ptr->m_isUnderSideCameraEnabled != isEnabled) {
        d_ptr->m_isUnderSideCameraEnabled = isEnabled;
        d_ptr->m_changeTracker.underSideCameraEnabledChanged = true;
    }
}

bool Q3DScene::isSlicingActivated() const
{
    return d_ptr->m_isSlicingActivated;
}

void Q3DScene::setSlicingActivated(bool isSlicing)
{
    if (d_ptr->m_isSlicingActivated != isSlicing) {
        d_ptr->m_isSlicingActivated = isSlicing;
        d_ptr->m_changeTracker.slicingActivatedChanged = true;
    }
}

void Q3DScene::setLightPositionRelativeToCamera(const QVector3D &relativePosition,
                                                qreal fixedRotation, qreal distanceModifier)
{
    d_ptr->m_light->setPosition(
                d_ptr->m_camera->calculatePositionRelativeToCamera(relativePosition,
                                                                   fixedRotation,
                                                                   distanceModifier));
}


Q3DScenePrivate::Q3DScenePrivate(Q3DScene *q) :
    q_ptr(q),
    m_camera(),
    m_light(),
    m_isUnderSideCameraEnabled(false),
    m_isSlicingActivated(false)
{
}

Q3DScenePrivate::~Q3DScenePrivate()
{
    delete m_camera;
    delete m_light;
}

// Copies changed values from this scene to the other scene. If the other scene had same changes,
// those changes are discarded.
void Q3DScenePrivate::sync(Q3DScenePrivate &other)
{
    if (m_changeTracker.viewportChanged) {
        other.q_ptr->setViewport(q_ptr->viewport());
        m_changeTracker.viewportChanged = false;
        other.m_changeTracker.viewportChanged = false;
    }
    if (m_changeTracker.mainViewportChanged) {
        other.q_ptr->setMainViewport(q_ptr->mainViewport());
        m_changeTracker.mainViewportChanged = false;
        other.m_changeTracker.mainViewportChanged = false;
    }
    if (m_changeTracker.sliceViewportChanged) {
        other.q_ptr->setSliceViewport(q_ptr->sliceViewport());
        m_changeTracker.sliceViewportChanged = false;
        other.m_changeTracker.sliceViewportChanged = false;
    }
    if (m_changeTracker.cameraChanged) {
        m_camera->setDirty(true);
        m_changeTracker.cameraChanged = false;
        other.m_changeTracker.cameraChanged = false;
    }
    m_camera->d_ptr->sync(*other.m_camera);

    if (m_changeTracker.lightChanged) {
        m_light->setDirty(true);
        m_changeTracker.lightChanged = false;
        other.m_changeTracker.lightChanged = false;
    }
    m_light->d_ptr->sync(*other.m_light);

    if (m_changeTracker.underSideCameraEnabledChanged) {
        other.q_ptr->setUnderSideCameraEnabled(q_ptr->isUnderSideCameraEnabled());
        m_changeTracker.underSideCameraEnabledChanged = false;
        other.m_changeTracker.underSideCameraEnabledChanged = false;
    }
    if (m_changeTracker.slicingActivatedChanged) {
        other.q_ptr->setSlicingActivated(q_ptr->isSlicingActivated());
        m_changeTracker.slicingActivatedChanged = false;
        other.m_changeTracker.slicingActivatedChanged = false;
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
