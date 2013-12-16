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

/*!
 * \class Q3DScene
 * \inmodule QtDataVisualization
 * \brief Q3DScene class provides description of the 3D scene being visualized.
 * \since Qt Data Visualization 1.0
 *
 * The 3D scene contains a single active camera and a single active light source.
 * Visualized data is assumed to be at a fixed location.
 *
 * The 3D scene also keeps track of the viewport in which visualization rendering is done,
 * the primary subviewport inside the viewport where the main 3D data visualization view resides
 * and the secondary subviewport where the 2D sliced view of the data resides. The subviewports are
 * by default resized by the \a Q3DScene. To override the resize behavior you need to listen to both
 * \l viewportChanged() and \l slicingActiveChanged() signals and recalculate the subviewports accordingly.
 *
 * Also the scene has flag for tracking if the secondary 2D slicing view is currently active or not.
 * \note Not all visualizations support the secondary 2D slicing view.
 */

/*!
 * \qmltype Scene3D
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates Q3DScene
 * \brief Scene3D type provides description of the 3D scene being visualized.
 *
 * The 3D scene contains a single active camera and a single active light source.
 * Visualized data is assumed to be at a fixed location.
 *
 * The 3D scene also keeps track of the viewport in which visualization rendering is done,
 * the primary subviewport inside the viewport where the main 3D data visualization view resides
 * and the secondary subviewport where the 2D sliced view of the data resides.
 *
 * Also the scene has flag for tracking if the secondary 2D slicing view is currently active or not.
 * \note Not all visualizations support the secondary 2D slicing view.
 */

/*!
 * \qmlproperty rect Scene3D::viewport
 *
 * This property contains the current viewport rectangle where all 3D rendering
 * is targeted.
 */

/*!
 * \qmlproperty rect Scene3D::primarySubViewport
 *
 * This property contains the current subviewport rectangle inside the viewport where the
 * primary view of the data visualization is targeted to.
 */

/*!
 * \qmlproperty rect Scene3D::secondarySubViewport
 *
 * This property contains the secondary viewport rectangle inside the viewport. The secondary
 * viewport is used for drawing the 2D slice view in some visualizations.
 */

/*!
 * \qmlproperty point Scene3D::selectionQueryPosition
 *
 * This property contains the coordinates for the user input that should be processed
 * by the scene as selection. If this is set to value other than \c{(-1, -1)} the
 * graph tries to select a data item at the given point within the primary viewport.
 * After the rendering pass the property is returned to its default state of \c{(-1, -1)}.
 */

/*!
 * \qmlproperty bool Scene3D::slicingActive
 *
 * This property contains whether 2D slicing view is currently active or not.
 * \note Not all visualizations support the 2D slicing view.
 */

/*!
 * \qmlproperty bool Scene3D::secondarySubviewOnTop
 *
 * This property contains whether 2D slicing view is currently drawn on top or if the 3D view is
 * drawn on top.
 */

/*!
 * \qmlproperty Camera3D Scene3D::activeCamera
 *
 * This property contains the currently active camera in the 3D scene.
 * When a Camera3D is set in the property it gets automatically added as child of the scene.
 */

/*!
 * \qmlproperty Light3D Scene3D::activeLight
 *
 * This property contains the currently active light in the 3D scene.
 * When a Light3D is set in the property it gets automatically added as child of the scene.
 */

/*!
 * \qmlproperty float Scene3D::devicePixelRatio
 *
 * This property contains the current device pixel ratio that is used when mapping input
 * coordinates to pixel coordinates.
 */

/*!
 * Constructs a basic scene with one light and one camera in it. An
 * optional \a parent parameter can be given and is then passed to QObject constructor.
 */
Q3DScene::Q3DScene(QObject *parent) :
    QObject(parent),
    d_ptr(new Q3DScenePrivate(this))
{
    setActiveCamera(new Q3DCamera(0));
    setActiveLight(new Q3DLight(0));
}

/*!
 * Destroys the 3D scene and all the objects contained within it.
 */
Q3DScene::~Q3DScene()
{
}

/*!
 * \property Q3DScene::viewport
 *
 * This read only property contains the current viewport rectangle where all 3D rendering
 * is targeted.
 */
QRect Q3DScene::viewport() const
{
    return d_ptr->m_viewport;
}

void Q3DScene::setViewport(const QRect &viewport)
{
    if (d_ptr->m_viewport != viewport) {
        d_ptr->m_viewport = viewport;
        calculateSubViewports();
        emit needRender();
    }
}

/*!
 * Sets the \a width and \a height of the viewport only, without changing its location.
 */
void Q3DScene::setViewportSize(int width, int height)
{
    if (d_ptr->m_viewport.width() != width
            || d_ptr->m_viewport.height() != height) {
        d_ptr->m_viewport.setWidth(width);
        d_ptr->m_viewport.setHeight(height);
        calculateSubViewports();
        emit needRender();
    }
}

/*!
 * \internal
 */
void Q3DScene::calculateSubViewports()
{
    // Calculates the default subviewport layout
    const float smallerViewPortRatio = 0.2;
    if (d_ptr->m_isSlicingActive) {
        setPrimarySubViewport(QRect(0,
                                    0,
                                    d_ptr->m_viewport.width() * smallerViewPortRatio,
                                    d_ptr->m_viewport.height() * smallerViewPortRatio));
        setSecondarySubViewport(QRect(0, 0, d_ptr->m_viewport.width(), d_ptr->m_viewport.height()));
    } else {
        setPrimarySubViewport(QRect(0, 0, d_ptr->m_viewport.width(), d_ptr->m_viewport.height()));
        setSecondarySubViewport(QRect(0, 0, 0, 0));
    }

    updateGLViewport();
}

/*!
 * \internal
 */
void Q3DScene::updateGLViewport()
{
    // Update GL viewport
    d_ptr->m_glViewport.setX(d_ptr->m_viewport.x() * d_ptr->m_devicePixelRatio);
    d_ptr->m_glViewport.setY((d_ptr->m_windowSize.height() - (d_ptr->m_viewport.y() + d_ptr->m_viewport.height())) * d_ptr->m_devicePixelRatio);
    d_ptr->m_glViewport.setWidth(d_ptr->m_viewport.width() * d_ptr->m_devicePixelRatio);
    d_ptr->m_glViewport.setHeight(d_ptr->m_viewport.height() * d_ptr->m_devicePixelRatio);

    d_ptr->m_changeTracker.viewportChanged = true;

    // Do default subviewport changes first, then allow signal listeners to override.
    updateGLSubViewports();
    emit viewportChanged(d_ptr->m_viewport);
}

/*!
 * \internal
 */
void Q3DScene::updateGLSubViewports()
{
    d_ptr->m_glPrimarySubViewport.setX((d_ptr->m_primarySubViewport.x() + d_ptr->m_viewport.x()) * d_ptr->m_devicePixelRatio);
    d_ptr->m_glPrimarySubViewport.setY((d_ptr->m_windowSize.height() - (d_ptr->m_primarySubViewport.y() + d_ptr->m_viewport.y() + d_ptr->m_primarySubViewport.height())) * d_ptr->m_devicePixelRatio);
    d_ptr->m_glPrimarySubViewport.setWidth(d_ptr->m_primarySubViewport.width() * d_ptr->m_devicePixelRatio);
    d_ptr->m_glPrimarySubViewport.setHeight(d_ptr->m_primarySubViewport.height() * d_ptr->m_devicePixelRatio);

    d_ptr->m_glSecondarySubViewport.setX(d_ptr->m_secondarySubViewport.x() * d_ptr->m_devicePixelRatio);
    d_ptr->m_glSecondarySubViewport.setY((d_ptr->m_windowSize.height() - (d_ptr->m_secondarySubViewport.y() + d_ptr->m_viewport.y() + d_ptr->m_secondarySubViewport.height())) * d_ptr->m_devicePixelRatio);
    d_ptr->m_glSecondarySubViewport.setWidth(d_ptr->m_secondarySubViewport.width() * d_ptr->m_devicePixelRatio);
    d_ptr->m_glSecondarySubViewport.setHeight(d_ptr->m_secondarySubViewport.height() * d_ptr->m_devicePixelRatio);
}

/*!
 * \property Q3DScene::primarySubViewport
 *
 * This property contains the current subviewport rectangle inside the viewport where the
 * primary view of the data visualization is targeted to.
 */
QRect Q3DScene::primarySubViewport() const
{
    return d_ptr->m_primarySubViewport;
}

void Q3DScene::setPrimarySubViewport(const QRect &primarySubViewport)
{
    if (d_ptr->m_primarySubViewport != primarySubViewport) {
        QRect clipRect = QRect(0, 0, d_ptr->m_viewport.width(), d_ptr->m_viewport.height());
        d_ptr->m_primarySubViewport = primarySubViewport.intersected(clipRect);
        updateGLSubViewports();
        d_ptr->m_changeTracker.primarySubViewportChanged = true;
        emit primarySubViewportChanged(primarySubViewport);
        emit needRender();
    }
}

/*!
 * Returns whether the given \a point resides inside the primary subview or not.
 * The method takes care of correctly mapping between OpenGL coordinates used in the
 * viewport definitions and the Qt event coordinate system used in the input system.
 * \return \c true if the point is inside the primary subview.
 */
bool Q3DScene::isPointInPrimarySubView(const QPoint &point)
{
    // TODO: Needs fixing. Doesn't respect whether slice or main view is on top or if slicing is even activated currently.
    int x = point.x();
    int y = point.y();
    int areaMinX = d_ptr->m_primarySubViewport.x();
    int areaMaxX = d_ptr->m_viewport.x() + d_ptr->m_primarySubViewport.x() + d_ptr->m_primarySubViewport.width();
    int areaMaxY = d_ptr->m_viewport.y() + d_ptr->m_primarySubViewport.y() + d_ptr->m_primarySubViewport.height();
    int areaMinY = d_ptr->m_viewport.y() + d_ptr->m_primarySubViewport.y();

    return ( x > areaMinX && x < areaMaxX && y > areaMinY && y < areaMaxY );
}

/*!
 * Returns whether the given \a point resides inside the secondary subview or not.
 * The method takes care of correctly mapping between OpenGL coordinates used in the
 * viewport definitions and the Qt event coordinate system used in the input system.
 * \return \c true if the point is inside the secondary subview.
 */
bool Q3DScene::isPointInSecondarySubView(const QPoint &point)
{
    // TODO: Needs fixing. Doesn't respect whether slice or main view is on top or if slicing is even activated currently.
    int x = point.x();
    int y = point.y();
    int areaMinX = d_ptr->m_secondarySubViewport.x();
    int areaMaxX = d_ptr->m_viewport.x() + d_ptr->m_secondarySubViewport.x() + d_ptr->m_secondarySubViewport.width();
    int areaMaxY = d_ptr->m_viewport.y() + d_ptr->m_secondarySubViewport.y() + d_ptr->m_secondarySubViewport.height();
    int areaMinY = d_ptr->m_viewport.y() + d_ptr->m_secondarySubViewport.y();

    return ( x > areaMinX && x < areaMaxX && y > areaMinY && y < areaMaxY );
}

/*!
 * \property Q3DScene::secondarySubViewport
 *
 * This property contains the secondary viewport rectangle inside the viewport. The secondary
 * viewport is used for drawing the 2D slice view in some visualizations.
 */
QRect Q3DScene::secondarySubViewport() const
{
    return d_ptr->m_secondarySubViewport;
}

void Q3DScene::setSecondarySubViewport(const QRect &secondarySubViewport)
{
    if (d_ptr->m_secondarySubViewport != secondarySubViewport) {
        QRect clipRect = QRect(0, 0, d_ptr->m_viewport.width(), d_ptr->m_viewport.height());
        d_ptr->m_secondarySubViewport = secondarySubViewport.intersected(clipRect);
        updateGLSubViewports();
        d_ptr->m_changeTracker.secondarySubViewportChanged = true;
        emit secondarySubViewportChanged(secondarySubViewport);
        emit needRender();
    }
}

/*!
 * \property Q3DScene::selectionQueryPosition
 *
 * This property contains the coordinates for the user input that should be processed
 * by the scene as selection. If this is set to value other than invalidSelectionPoint() the
 * graph tries to select a data item at the given \a point within the primary viewport.
 * After the rendering pass the property is returned to its default state of invalidSelectionPoint().
 */
void Q3DScene::setSelectionQueryPosition(const QPoint &point)
{
    if (point != d_ptr->m_selectionQueryPosition) {
        d_ptr->m_selectionQueryPosition = point;
        d_ptr->m_changeTracker.selectionQueryPositionChanged = true;
        emit selectionQueryPositionChanged(point);
        emit needRender();
    }
}

QPoint Q3DScene::selectionQueryPosition() const
{
    return d_ptr->m_selectionQueryPosition;
}

/*!
 * \return a QPoint signifying an invalid selection position.
 */
const QPoint Q3DScene::invalidSelectionPoint()
{
    static const QPoint invalidSelectionPos(-1, -1);
    return invalidSelectionPos;
}

/*!
 * \property Q3DScene::slicingActive
 *
 * This property contains whether 2D slicing view is currently active or not.
 * \note Not all visualizations support the 2D slicing view.
 */
bool Q3DScene::isSlicingActive() const
{
    return d_ptr->m_isSlicingActive;
}

void Q3DScene::setSlicingActive(bool isSlicing)
{
    if (d_ptr->m_isSlicingActive != isSlicing) {
        d_ptr->m_isSlicingActive = isSlicing;
        d_ptr->m_changeTracker.slicingActivatedChanged = true;
        calculateSubViewports();
        emit slicingActiveChanged(isSlicing);
        emit needRender();
    }
}

/*!
 * \property Q3DScene::secondarySubviewOnTop
 *
 * This property contains whether 2D slicing view is currently drawn on top or if the 3D view is
 * drawn on top.
 */
bool Q3DScene::isSecondarySubviewOnTop() const
{
    return d_ptr->m_isSecondarySubviewOnTop;
}

void Q3DScene::setSecondarySubviewOnTop(bool isSecondaryOnTop)
{
    if (d_ptr->m_isSecondarySubviewOnTop != isSecondaryOnTop) {
        d_ptr->m_isSecondarySubviewOnTop = isSecondaryOnTop;
        d_ptr->m_changeTracker.subViewportOrderChanged = true;
        emit secondarySubviewOnTopChanged(isSecondaryOnTop);
        emit needRender();
    }
}

/*!
 * \property Q3DScene::activeCamera
 *
 * This property contains the currently active camera in the 3D scene.
 * When a new Q3DCamera objects is set in the property it gets automatically added as child of
 * the scene.
 */
Q3DCamera *Q3DScene::activeCamera() const
{
    return d_ptr->m_camera;
}

void Q3DScene::setActiveCamera(Q3DCamera *camera)
{
    Q_ASSERT(camera);

    // Add new camera as child of the scene
    if (camera->parent() != this)
        camera->setParent(this);

    if (camera != d_ptr->m_camera) {
        if (d_ptr->m_camera) {
            disconnect(d_ptr->m_camera, &Q3DCamera::xRotationChanged, this,
                       &Q3DScene::needRender);
            disconnect(d_ptr->m_camera, &Q3DCamera::yRotationChanged, this,
                       &Q3DScene::needRender);
            disconnect(d_ptr->m_camera, &Q3DCamera::zoomLevelChanged, this,
                       &Q3DScene::needRender);
        }

        d_ptr->m_camera = camera;
        d_ptr->m_changeTracker.cameraChanged = true;

        if (camera) {
            connect(camera, &Q3DCamera::xRotationChanged, this,
                    &Q3DScene::needRender);
            connect(camera, &Q3DCamera::yRotationChanged, this,
                    &Q3DScene::needRender);
            connect(camera, &Q3DCamera::zoomLevelChanged, this,
                    &Q3DScene::needRender);
        }

        emit activeCameraChanged(camera);
        emit needRender();
    }
}

/*!
 * \property Q3DScene::activeLight
 *
 * This property contains the currently active light in the 3D scene.
 * When a new Q3DLight objects is set in the property it gets automatically added as child of
 * the scene.
 */
Q3DLight *Q3DScene::activeLight() const
{
    return d_ptr->m_light;
}

void Q3DScene::setActiveLight(Q3DLight *light)
{
    Q_ASSERT(light);

    // Add new light as child of the scene
    if (light->parent() != this)
        light->setParent(this);

    if (light != d_ptr->m_light) {
        d_ptr->m_light = light;
        d_ptr->m_changeTracker.lightChanged = true;
        emit activeLightChanged(light);
    }
}

/*!
 * \property Q3DScene::devicePixelRatio
 *
 * This property contains the current device pixel ratio that is used when mapping input
 * coordinates to pixel coordinates.
 */
float Q3DScene::devicePixelRatio() const
{
    return d_ptr->m_devicePixelRatio;
}

void Q3DScene::setDevicePixelRatio(float pixelRatio)
{
    if (d_ptr->m_devicePixelRatio != pixelRatio) {
        d_ptr->m_devicePixelRatio = pixelRatio;
        d_ptr->m_changeTracker.devicePixelRatioChanged = true;
        emit devicePixelRatioChanged(pixelRatio);
        updateGLViewport();
    }
}

/*!
 * Calculates and sets the light position relative to the currently active camera using the given
 * parameters.
 * The relative 3D offset to the current camera position is defined in \a relativePosition.
 * Optional \a fixedRotation fixes the light rotation around the data visualization area to the
 * given value in degrees.
 * Optional \a distanceModifier modifies the distance of the light from the data visualization.
 */
void Q3DScene::setLightPositionRelativeToCamera(const QVector3D &relativePosition,
                                                float fixedRotation, float distanceModifier)
{
    d_ptr->m_light->setPosition(
                d_ptr->m_camera->calculatePositionRelativeToCamera(relativePosition,
                                                                   fixedRotation,
                                                                   distanceModifier));
}

/*!
 * \internal
 * Sets the size of the window being rendered to. With widget based graphs, this
 * is equal to the size of the QWindow and is same as the bounding rectangle.
 * With declarative graphs this is equal to the size of the QQuickWindow and
 * can be different from the bounding rectangle.
 */
void Q3DScene::setWindowSize(const QSize &size)
{
    if (d_ptr->m_windowSize != size) {
        d_ptr->m_windowSize = size;
        updateGLViewport();
        d_ptr->m_changeTracker.windowSizeChanged = true;
        emit windowSizeChanged(size);
        emit needRender();
    }
}

/*!
 * \internal
 */
QSize Q3DScene::windowSize() const
{
    return d_ptr->m_windowSize;
}

/*!
 * \internal
 */
QRect Q3DScene::glViewport()
{
    return d_ptr->m_glViewport;
}

/*!
 * \internal
 */
QRect Q3DScene::glPrimarySubViewport()
{
    return d_ptr->m_glPrimarySubViewport;
}

/*!
 * \internal
 */
QRect Q3DScene::glSecondarySubViewport()
{
    return d_ptr->m_glSecondarySubViewport;
}

/*!
 * \fn Q3DScene::needRender()
 * \internal
 */
Q3DScenePrivate::Q3DScenePrivate(Q3DScene *q) :
    q_ptr(q),
    m_isSecondarySubviewOnTop(true),
    m_devicePixelRatio(1.f),
    m_camera(),
    m_light(),
    m_isUnderSideCameraEnabled(false),
    m_isSlicingActive(false),
    m_selectionQueryPosition(Q3DScene::invalidSelectionPoint())
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
    if (m_changeTracker.windowSizeChanged) {
        other.q_ptr->setWindowSize(q_ptr->windowSize());
        m_changeTracker.windowSizeChanged = false;
        other.m_changeTracker.windowSizeChanged = false;
    }
    if (m_changeTracker.viewportChanged) {
        other.q_ptr->setViewport(q_ptr->viewport());
        m_changeTracker.viewportChanged = false;
        other.m_changeTracker.viewportChanged = false;
    }
    if (m_changeTracker.subViewportOrderChanged) {
        other.q_ptr->setSecondarySubviewOnTop(q_ptr->isSecondarySubviewOnTop());
        m_changeTracker.subViewportOrderChanged = false;
        other.m_changeTracker.subViewportOrderChanged = false;
    }
    if (m_changeTracker.primarySubViewportChanged) {
        other.q_ptr->setPrimarySubViewport(q_ptr->primarySubViewport());
        m_changeTracker.primarySubViewportChanged = false;
        other.m_changeTracker.primarySubViewportChanged = false;
    }
    if (m_changeTracker.secondarySubViewportChanged) {
        other.q_ptr->setSecondarySubViewport(q_ptr->secondarySubViewport());
        m_changeTracker.secondarySubViewportChanged = false;
        other.m_changeTracker.secondarySubViewportChanged = false;
    }
    if (m_changeTracker.selectionQueryPositionChanged) {
        other.q_ptr->setSelectionQueryPosition(q_ptr->selectionQueryPosition());
        m_changeTracker.selectionQueryPositionChanged = false;
        other.m_changeTracker.selectionQueryPositionChanged = false;
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

    if (m_changeTracker.slicingActivatedChanged) {
        other.q_ptr->setSlicingActive(q_ptr->isSlicingActive());
        m_changeTracker.slicingActivatedChanged = false;
        other.m_changeTracker.slicingActivatedChanged = false;
    }

    if (m_changeTracker.devicePixelRatioChanged) {
        other.q_ptr->setDevicePixelRatio(q_ptr->devicePixelRatio());
        m_changeTracker.devicePixelRatioChanged = false;
        other.m_changeTracker.devicePixelRatioChanged = false;
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
