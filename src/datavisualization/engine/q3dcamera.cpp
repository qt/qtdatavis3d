/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#include "q3dcamera_p.h"
#include "utils_p.h"

#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class Q3DCamera
 * \inmodule QtDataVisualization
 * \brief Representation of a camera in 3D space.
 * \since QtDataVisualization 1.0
 *
 * Q3DCamera represents a basic orbit around centerpoint 3D camera that is used when rendering the
 * data visualization. The class offers simple methods for rotating the camera around the origin
 * and setting zoom level.
 */

/*!
 * \enum Q3DCamera::CameraPreset
 *
 * Predefined positions for camera.
 *
 * \value CameraPresetNone
 *        Used to indicate a preset has not been set, or the scene has been rotated freely.
 * \value CameraPresetFrontLow
 * \value CameraPresetFront
 * \value CameraPresetFrontHigh
 * \value CameraPresetLeftLow
 * \value CameraPresetLeft
 * \value CameraPresetLeftHigh
 * \value CameraPresetRightLow
 * \value CameraPresetRight
 * \value CameraPresetRightHigh
 * \value CameraPresetBehindLow
 * \value CameraPresetBehind
 * \value CameraPresetBehindHigh
 * \value CameraPresetIsometricLeft
 * \value CameraPresetIsometricLeftHigh
 * \value CameraPresetIsometricRight
 * \value CameraPresetIsometricRightHigh
 * \value CameraPresetDirectlyAbove
 * \value CameraPresetDirectlyAboveCW45
 * \value CameraPresetDirectlyAboveCCW45
 * \value CameraPresetFrontBelow
 *        In Q3DBars from CameraPresetFrontBelow onward these only work for graphs including negative
 *        values. They act as Preset...Low for positive-only values.
 * \value CameraPresetLeftBelow
 * \value CameraPresetRightBelow
 * \value CameraPresetBehindBelow
 * \value CameraPresetDirectlyBelow
 *        Acts as CameraPresetFrontLow for positive-only bars.
 */

/*!
 * \qmltype Camera3D
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates Q3DCamera
 * \brief Representation of a camera in 3D space.
 *
 * Camera3D represents a basic orbit around centerpoint 3D camera that is used when rendering the
 * data visualization. The type offers simple methods for rotating the camera around the origin
 * and setting zoom level.
 *
 * For Camera3D enums, see \l{Q3DCamera::CameraPreset}.
 */

/*!
 * \qmlproperty float Camera3D::xRotation
 *
 * This property contains the X-rotation angle of the camera around the target point in degrees
 * starting from the current base position.
 */

/*!
 * \qmlproperty float Camera3D::yRotation
 *
 * This property contains the Y-rotation angle of the camera around the target point in degrees
 * starting from the current base position.
 */

/*!
 * \qmlproperty Camera3D.CameraPreset Camera3D::cameraPreset
 *
 * This property contains the currently active camera preset, which is one of
 * \l{Q3DCamera::CameraPreset}{Camera3D.CameraPreset}. If no preset is active the value
 * is \l{Q3DCamera::CameraPresetNone}{Camera3D.CameraPresetNone}.
 */

/*!
 * \qmlproperty float Camera3D::zoomLevel
 *
 * This property contains the the camera zoom level in percentage. 100.0 means there is no zoom
 * in or out set in the camera.
 */

/*!
 * \qmlproperty bool Camera3D::wrapXRotation
 *
 * This property determines the behavior of the minimum and maximum limits in the X-rotation.
 * By default the X-rotation wraps from minimum value to maximum and from maximum to minimum.
 *
 * If set to \c true the X-rotation of the camera is wrapped from minimum to maximum and from maximum
 * to minimum. If set to \c false the X-rotation of the camera is limited to the sector determined by
 * minimum and maximum values.
 */

/*!
 * \qmlproperty bool Camera3D::wrapYRotation
 *
 * This property determines the behavior of the minimum and maximum limits in the Y-rotation.
 * By default the Y-rotation is limited between the minimum and maximum values without any wrapping.
 *
 * If \c true the Y-rotation of the camera is wrapped from minimum to maximum and from maximum
 * to minimum. If \c false the Y-rotation of the camera is limited to the sector determined by minimum
 * and maximum values.
 */

/*!
 * Constructs a new 3D camera with position set to origin, up direction facing towards the Y-axis
 * and looking at origin by default. An optional \a parent parameter can be given and is then passed
 * to QObject constructor.
 */
Q3DCamera::Q3DCamera(QObject *parent) :
    Q3DObject(parent),
    d_ptr(new Q3DCameraPrivate(this))
{
}

/*!
 *  Destroys the camera object.
 */
Q3DCamera::~Q3DCamera()
{
}

/*!
 * Copies the 3D camera's properties from the given source camera.
 * Values are copied from the \a source to this object.
 */
void Q3DCamera::copyValuesFrom(const Q3DObject &source)
{
    Q3DObject::copyValuesFrom(source);

    const Q3DCamera &sourceCamera = static_cast<const Q3DCamera &>(source);

    d_ptr->m_target.setX(sourceCamera.d_ptr->m_target.x());
    d_ptr->m_target.setY(sourceCamera.d_ptr->m_target.y());
    d_ptr->m_target.setZ(sourceCamera.d_ptr->m_target.z());

    d_ptr->m_up.setX(sourceCamera.d_ptr->m_up.x());
    d_ptr->m_up.setY(sourceCamera.d_ptr->m_up.y());
    d_ptr->m_up.setZ(sourceCamera.d_ptr->m_up.z());

    float *values = new float[16];
    sourceCamera.d_ptr->m_viewMatrix.copyDataTo(values);
    d_ptr->m_viewMatrix = QMatrix4x4(values);
    delete[] values;

    d_ptr->m_xRotation = sourceCamera.d_ptr->m_xRotation;
    d_ptr->m_yRotation = sourceCamera.d_ptr->m_yRotation;

    d_ptr->m_minXRotation = sourceCamera.d_ptr->m_minXRotation;
    d_ptr->m_minYRotation = sourceCamera.d_ptr->m_minYRotation;
    d_ptr->m_maxXRotation = sourceCamera.d_ptr->m_maxXRotation;
    d_ptr->m_maxYRotation = sourceCamera.d_ptr->m_maxYRotation;

    d_ptr->m_wrapXRotation = sourceCamera.d_ptr->m_wrapXRotation;
    d_ptr->m_wrapYRotation = sourceCamera.d_ptr->m_wrapYRotation;

    d_ptr->m_zoomLevel = sourceCamera.d_ptr->m_zoomLevel;
    d_ptr->m_activePreset = sourceCamera.d_ptr->m_activePreset;
}

/*!
 * \property Q3DCamera::xRotation
 *
 * This property contains the X-rotation angle of the camera around the target point in degrees.
 */
float Q3DCamera::xRotation() const {
    return d_ptr->m_xRotation;
}

void Q3DCamera::setXRotation(float rotation)
{
    if (d_ptr->m_wrapXRotation) {
        rotation = Utils::wrapValue(rotation, d_ptr->m_minXRotation, d_ptr->m_maxXRotation);
    } else {
        rotation = qBound(float(d_ptr->m_minXRotation), float(rotation),
                          float(d_ptr->m_maxXRotation));
    }

    if (d_ptr->m_xRotation != rotation) {
        d_ptr->setXRotation(rotation);
        if (d_ptr->m_activePreset != CameraPresetNone) {
            d_ptr->m_activePreset = CameraPresetNone;
            setDirty(true);
        }

        emit xRotationChanged(d_ptr->m_xRotation);
    }
}

/*!
 * \property Q3DCamera::yRotation
 *
 * This property contains the Y-rotation angle of the camera around the target point in degrees.
 */
float Q3DCamera::yRotation() const {
    return d_ptr->m_yRotation;
}

void Q3DCamera::setYRotation(float rotation)
{
    if (d_ptr->m_wrapYRotation) {
        rotation = Utils::wrapValue(rotation, d_ptr->m_minYRotation, d_ptr->m_maxYRotation);
    } else {
        rotation = qBound(float(d_ptr->m_minYRotation), float(rotation),
                          float(d_ptr->m_maxYRotation));
    }

    if (d_ptr->m_yRotation != rotation) {
        d_ptr->setYRotation(rotation);
        if (d_ptr->m_activePreset != CameraPresetNone) {
            d_ptr->m_activePreset = CameraPresetNone;
            setDirty(true);
        }

        emit yRotationChanged(d_ptr->m_yRotation);
    }
}

/*!
 * \property Q3DCamera::cameraPreset
 *
 * This property contains the currently active camera preset, if no preset is active the value
 * is CameraPresetNone.
 */
Q3DCamera::CameraPreset Q3DCamera::cameraPreset() const
{
    return d_ptr->m_activePreset;
}

void Q3DCamera::setCameraPreset(CameraPreset preset)
{
    switch (preset) {
    case CameraPresetFrontLow: {
        setXRotation(0.0f);
        setYRotation(0.0f);
        break;
    }
    case CameraPresetFront: {
        setXRotation(0.0f);
        setYRotation(22.5f);
        break;
    }
    case CameraPresetFrontHigh: {
        setXRotation(0.0f);
        setYRotation(45.0f);
        break;
    }
    case CameraPresetLeftLow: {
        setXRotation(90.0f);
        setYRotation(0.0f);
        break;
    }
    case CameraPresetLeft: {
        setXRotation(90.0f);
        setYRotation(22.5f);
        break;
    }
    case CameraPresetLeftHigh: {
        setXRotation(90.0f);
        setYRotation(45.0f);
        break;
    }
    case CameraPresetRightLow: {
        setXRotation(-90.0f);
        setYRotation(0.0f);
        break;
    }
    case CameraPresetRight: {
        setXRotation(-90.0f);
        setYRotation(22.5f);
        break;
    }
    case CameraPresetRightHigh: {
        setXRotation(-90.0f);
        setYRotation(45.0f);
        break;
    }
    case CameraPresetBehindLow: {
        setXRotation(180.0f);
        setYRotation(0.0f);
        break;
    }
    case CameraPresetBehind: {
        setXRotation(180.0f);
        setYRotation(22.5f);
        break;
    }
    case CameraPresetBehindHigh: {
        setXRotation(180.0f);
        setYRotation(45.0f);
        break;
    }
    case CameraPresetIsometricLeft: {
        setXRotation(45.0f);
        setYRotation(22.5f);
        break;
    }
    case CameraPresetIsometricLeftHigh: {
        setXRotation(45.0f);
        setYRotation(45.0f);
        break;
    }
    case CameraPresetIsometricRight: {
        setXRotation(-45.0f);
        setYRotation(22.5f);
        break;
    }
    case CameraPresetIsometricRightHigh: {
        setXRotation(-45.0f);
        setYRotation(45.0f);
        break;
    }
    case CameraPresetDirectlyAbove: {
        setXRotation(0.0f);
        setYRotation(90.0f);
        break;
    }
    case CameraPresetDirectlyAboveCW45: {
        setXRotation(-45.0f);
        setYRotation(90.0f);
        break;
    }
    case CameraPresetDirectlyAboveCCW45: {
        setXRotation(45.0f);
        setYRotation(90.0f);
        break;
    }
    case CameraPresetFrontBelow: {
        setXRotation(0.0f);
        setYRotation(-45.0f);
        break;
    }
    case CameraPresetLeftBelow: {
        setXRotation(90.0f);
        setYRotation(-45.0f);
        break;
    }
    case CameraPresetRightBelow: {
        setXRotation(-90.0f);
        setYRotation(-45.0f);
        break;
    }
    case CameraPresetBehindBelow: {
        setXRotation(180.0f);
        setYRotation(-45.0f);
        break;
    }
    case CameraPresetDirectlyBelow: {
        setXRotation(0.0f);
        setYRotation(-90.0f);
        break;
    }
    default:
        preset = CameraPresetNone;
        break;
    }

    if (d_ptr->m_activePreset != preset) {
        d_ptr->m_activePreset = preset;
        setDirty(true);
        emit cameraPresetChanged(preset);
    }
}

/*!
 * \property Q3DCamera::zoomLevel
 *
 * This property contains the the camera zoom level in percentage. \c 100.0f means there is no zoom
 * in or out set in the camera.
 */
float Q3DCamera::zoomLevel() const
{
    return d_ptr->m_zoomLevel;
}

void Q3DCamera::setZoomLevel(float zoomLevel)
{
    if (d_ptr->m_zoomLevel != zoomLevel) {
        d_ptr->m_zoomLevel = zoomLevel;
        setDirty(true);
        emit zoomLevelChanged(zoomLevel);
    }
}

/*!
 * \property Q3DCamera::wrapXRotation
 *
 * This property determines the behavior of the minimum and maximum limits in the X-rotation.
 * By default the X-rotation wraps from minimum value to maximum and from maximum to minimum.
 *
 * If set to \c true the X-rotation of the camera is wrapped from minimum to maximum and from
 * maximum to minimum. If set to \c false the X-rotation of the camera is limited to the sector
 * determined by minimum and maximum values.
 */
bool Q3DCamera::wrapXRotation() const
{
    return d_ptr->m_wrapXRotation;
}

void Q3DCamera::setWrapXRotation(bool isEnabled)
{
    d_ptr->m_wrapXRotation = isEnabled;
}

/*!
 * \property Q3DCamera::wrapYRotation
 *
 * This property determines the behavior of the minimum and maximum limits in the Y-rotation.
 * By default the Y-rotation is limited between the minimum and maximum values without any wrapping.
 *
 * If \c true the Y-rotation of the camera is wrapped from minimum to maximum and from maximum to
 * minimum. If \c false the Y-rotation of the camera is limited to the sector determined by minimum
 * and maximum values.
 */
bool Q3DCamera::wrapYRotation() const
{
    return d_ptr->m_wrapYRotation;
}

void Q3DCamera::setWrapYRotation(bool isEnabled)
{
    d_ptr->m_wrapYRotation = isEnabled;
}

/*!
 * Utility function that sets the camera rotations and distance.\a horizontal and \a vertical
 * define the camera rotations to be used.
 * Optional \a zoom parameter can be given to set the zoom percentage of the camera in range of
 * \c{10.0f - 500.0f}.
 */
void Q3DCamera::setCameraPosition(float horizontal, float vertical, float zoom)
{
    setZoomLevel(qBound(10.0f, zoom, 500.0f));
    setXRotation(horizontal);
    setYRotation(vertical);
}

Q3DCameraPrivate::Q3DCameraPrivate(Q3DCamera *q) :
    q_ptr(q),
    m_isViewMatrixUpdateActive(true),
    m_xRotation(0.0f),
    m_yRotation(0.0f),
    m_minXRotation(-180.0f),
    m_minYRotation(0.0f),
    m_maxXRotation(180.0f),
    m_maxYRotation(90.0f),
    m_zoomLevel(100.0f),
    m_wrapXRotation(true),
    m_wrapYRotation(false),
    m_activePreset(Q3DCamera::CameraPresetNone)
{
}

Q3DCameraPrivate::~Q3DCameraPrivate()
{
}

// Copies changed values from this camera to the other camera. If the other camera had same changes,
// those changes are discarded.
void Q3DCameraPrivate::sync(Q3DCamera &other)
{
    if (q_ptr->isDirty()) {
        other.copyValuesFrom(*q_ptr);
        q_ptr->setDirty(false);
        other.setDirty(false);
    }
}

void Q3DCameraPrivate::setXRotation(const float rotation)
{
    if (m_xRotation != rotation) {
        m_xRotation = rotation;
        q_ptr->setDirty(true);
    }
}

void Q3DCameraPrivate::setYRotation(const float rotation)
{
    if (m_yRotation != rotation) {
        m_yRotation = rotation;
        q_ptr->setDirty(true);
    }
}

/*!
 * \internal
 * This property contains the current minimum X-rotation for the camera.
 * The full circle range is \c{[-180, 180]} and the minimum value is limited to \c -180.
 * Also the value can't be higher than the maximum, and is adjusted if necessary.
 *
 * \sa wrapXRotation, maxXRotation
 */
float Q3DCameraPrivate::minXRotation() const
{
    return m_minXRotation;
}

void Q3DCameraPrivate::setMinXRotation(float minRotation)
{
    minRotation = qBound(-180.0f, minRotation, 180.0f);
    if (minRotation > m_maxXRotation)
        minRotation = m_maxXRotation;

    if (m_minXRotation != minRotation) {
        m_minXRotation = minRotation;
        emit minXRotationChanged(minRotation);

        if (m_xRotation < m_minXRotation)
            setXRotation(m_xRotation);
        q_ptr->setDirty(true);
    }
}

/*!
 * \internal
 * This property contains the current minimum Y-rotation for the camera.
 * The full Y angle range is \c{[-90, 90]} and the minimum value is limited to \c -90.
 * Also the value can't be higher than the maximum, and is adjusted if necessary.
 *
 * \sa wrapYRotation, maxYRotation
 */
float Q3DCameraPrivate::minYRotation() const
{
    return m_minYRotation;
}

void Q3DCameraPrivate::setMinYRotation(float minRotation)
{
    minRotation = qBound(-90.0f, minRotation, 90.0f);
    if (minRotation > m_maxYRotation)
        minRotation = m_maxYRotation;

    if (m_minYRotation != minRotation) {
        m_minYRotation = minRotation;
        emit minYRotationChanged(minRotation);

        if (m_yRotation < m_minYRotation)
            setYRotation(m_yRotation);
        q_ptr->setDirty(true);
    }
}

/*!
 * \internal
 * This property contains the current maximum X-rotation for the camera.
 * The full circle range is \c{[-180, 180]} and the maximum value is limited to \c 180.
 * Also the value can't be lower than the minimum, and is adjusted if necessary.
 *
 * \sa wrapXRotation, minXRotation
 */
float Q3DCameraPrivate::maxXRotation() const
{
    return m_maxXRotation;
}

void Q3DCameraPrivate::setMaxXRotation(float maxRotation)
{
    maxRotation = qBound(-180.0f, maxRotation, 180.0f);

    if (maxRotation < m_minXRotation)
        maxRotation = m_minXRotation;

    if (m_maxXRotation != maxRotation) {
        m_maxXRotation = maxRotation;
        emit maxXRotationChanged(maxRotation);

        if (m_xRotation > m_maxXRotation)
            setXRotation(m_xRotation);
        q_ptr->setDirty(true);
    }
}

/*!
 * \internal
 * This property contains the current maximum Y-rotation for the camera.
 * The full Y angle range is \c{[-90, 90]} and the maximum value is limited to \c 90.
 * Also the value can't be lower than the minimum, and is adjusted if necessary.
 *
 * \sa wrapYRotation, minYRotation
 */
float Q3DCameraPrivate::maxYRotation() const
{
    return m_maxYRotation;
}

void Q3DCameraPrivate::setMaxYRotation(float maxRotation)
{
    maxRotation = qBound(-90.0f, maxRotation, 90.0f);

    if (maxRotation < m_minYRotation)
        maxRotation = m_minYRotation;

    if (m_maxYRotation != maxRotation) {
        m_maxYRotation = maxRotation;
        emit maxYRotationChanged(maxRotation);

        if (m_yRotation > m_maxYRotation)
            setYRotation(m_yRotation);
        q_ptr->setDirty(true);
    }
}

// Recalculates the view matrix based on the currently set base orientation, rotation and zoom level values.
//  zoomAdjustment is adjustment to ensure that the 3D visualization stays inside the view area in the 100% zoom.
void Q3DCameraPrivate::updateViewMatrix(float zoomAdjustment)
{
    if (!m_isViewMatrixUpdateActive)
        return;

    GLfloat zoom = m_zoomLevel * zoomAdjustment;
    QMatrix4x4 viewMatrix;

    // Apply to view matrix
    viewMatrix.lookAt(q_ptr->position(), m_target, m_up);
    // Compensate for translation (if d_ptr->m_target is off origin)
    viewMatrix.translate(m_target.x(), m_target.y(), m_target.z());
    // Apply rotations
    // Handle x and z rotation when y -angle is other than 0
    viewMatrix.rotate(m_xRotation, 0, qCos(qDegreesToRadians(m_yRotation)),
                      qSin(qDegreesToRadians(m_yRotation)));
    // y rotation is always "clean"
    viewMatrix.rotate(m_yRotation, 1.0f, 0.0f, 0.0f);
    // handle zoom by scaling
    viewMatrix.scale(zoom / 100.0f);
    // Compensate for translation (if d_ptr->m_target is off origin)
    viewMatrix.translate(-m_target.x(), -m_target.y(), -m_target.z());

    setViewMatrix(viewMatrix);
}

/*!
 * \internal
 * This property contains the view matrix used in the 3D calculations. When the default orbiting
 * camera behavior is sufficient, there is no need to touch this property. If the default
 * behavior is insufficient, the view matrix can be set directly.
 * \note When setting the view matrix directly remember to set viewMatrixAutoUpdateEnabled to
 * \c false.
 */
QMatrix4x4 Q3DCameraPrivate::viewMatrix() const
{
    return m_viewMatrix;
}

void Q3DCameraPrivate::setViewMatrix(const QMatrix4x4 &viewMatrix)
{
    if (m_viewMatrix != viewMatrix) {
        m_viewMatrix = viewMatrix;
        q_ptr->setDirty(true);
        emit viewMatrixChanged(m_viewMatrix);
    }
}

/*!
 * \internal
 * This property determines if view matrix is automatically updated each render cycle using the
 * current base orientation and rotations. If set to \c false, no automatic recalculation is done
 * and the view matrix can be set using the viewMatrix property.
 */
bool Q3DCameraPrivate::isViewMatrixAutoUpdateEnabled() const
{
    return m_isViewMatrixUpdateActive;
}

void Q3DCameraPrivate::setViewMatrixAutoUpdateEnabled(bool isEnabled)
{
    m_isViewMatrixUpdateActive = isEnabled;
    emit viewMatrixAutoUpdateChanged(isEnabled);
}

/*!
 * \internal
 * Sets the base values for the camera that are used when calculating the camera position using the
 * rotation values. The base position of the camera is defined by \a basePosition, expectation is
 * that the x and y values are 0. Look at target point is defined by \a target and the camera
 * rotates around it. Up direction for the camera is defined by \a baseUp, normally this is a
 * vector with only y value set to 1.
 */
void Q3DCameraPrivate::setBaseOrientation(const QVector3D &basePosition,
                                          const QVector3D &target,
                                          const QVector3D &baseUp)
{
    if (q_ptr->position() != basePosition || m_target != target || m_up != baseUp) {
        q_ptr->setPosition(basePosition);
        m_target = target;
        m_up = baseUp;
        q_ptr->setDirty(true);
    }
}

/*!
 * \internal
 * Calculates and returns a position relative to the camera using the given parameters
 * and the current camera viewMatrix property.
 * The relative 3D offset to the current camera position is defined in \a relativePosition.
 * An optional fixed rotation of the calculated point around the data visualization area can be
 * given in \a fixedRotation. The rotation is given in degrees.
 * An optional \a distanceModifier modifies the distance of the calculated point from the data
 * visualization.
 * \return calculated position relative to this camera's position.
 */
QVector3D Q3DCameraPrivate::calculatePositionRelativeToCamera(const QVector3D &relativePosition,
                                                              float fixedRotation,
                                                              float distanceModifier) const
{
    // Move the position with camera
    GLfloat radiusFactor = cameraDistance * (1.5f + distanceModifier);
    GLfloat xAngle;
    GLfloat yAngle;
    if (!fixedRotation) {
        xAngle = qDegreesToRadians(m_xRotation);
        yAngle = qDegreesToRadians(m_yRotation);
    } else {
        xAngle = qDegreesToRadians(fixedRotation);
        yAngle = 0;
    }
    GLfloat radius = (radiusFactor + relativePosition.y()); // set radius to match the highest height of the position
    GLfloat zPos = radius * qCos(xAngle) * qCos(yAngle);
    GLfloat xPos = radius * qSin(xAngle) * qCos(yAngle);
    GLfloat yPos = (radiusFactor + relativePosition.y()) * qSin(yAngle);

    // Keep in the set position in relation to camera
    return QVector3D(-xPos + relativePosition.x(),
                     yPos + relativePosition.y(),
                     zPos + relativePosition.z());
}

QT_END_NAMESPACE_DATAVISUALIZATION
