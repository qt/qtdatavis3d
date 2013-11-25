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

#include "q3dcamera.h"
#include "q3dcamera_p.h"
#include "q3dscene.h"
#include "q3dbox.h"
#include "q3dobject.h"
#include "utils_p.h"

#include <qmath.h>
#include <QVector3D>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
   \class Q3DCamera
   \inmodule QtDataVisualization
   \brief Representation of a camera in 3D space.
   \since Qt Data Visualization 1.0

    Q3DCamera represents a basic orbit around centerpoint 3D camera that is used when rendering the data visualization.
    The class offers simple methods for setting the orbit point in rotations, but allows also setting the 4x4 viewmatrix
    directly in case a more customized camera behavior is needed.
*/

/*!
 * Constructs a new 3D camera with position set to origo, up direction facing towards the Y-axis and looking at origo by default. An
 * optional \a parent parameter can be given and is then passed to QObject constructor.
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
void Q3DCamera::copyValuesFrom(const Q3DCamera &source)
{
    Q3DObject::copyValuesFrom(source);

    d_ptr->m_target.setX(source.d_ptr->m_target.x());
    d_ptr->m_target.setY(source.d_ptr->m_target.y());
    d_ptr->m_target.setZ(source.d_ptr->m_target.z());

    d_ptr->m_up.setX(source.d_ptr->m_up.x());
    d_ptr->m_up.setY(source.d_ptr->m_up.y());
    d_ptr->m_up.setZ(source.d_ptr->m_up.z());

    float *values = new float[16];
    source.d_ptr->m_viewMatrix.copyDataTo(values);
    d_ptr->m_viewMatrix = QMatrix4x4(values);
    delete[] values;

    d_ptr->m_xRotation = source.d_ptr->m_xRotation;
    d_ptr->m_yRotation = source.d_ptr->m_yRotation;

    d_ptr->m_minXRotation = source.d_ptr->m_minXRotation;
    d_ptr->m_minYRotation = source.d_ptr->m_minYRotation;
    d_ptr->m_maxXRotation = source.d_ptr->m_maxXRotation;
    d_ptr->m_maxYRotation = source.d_ptr->m_maxYRotation;

    d_ptr->m_wrapXRotation = source.d_ptr->m_wrapXRotation;
    d_ptr->m_wrapYRotation = source.d_ptr->m_wrapYRotation;

    d_ptr->m_zoomLevel = source.d_ptr->m_zoomLevel;
    d_ptr->m_activePreset = source.d_ptr->m_activePreset;
}

/*!
 * \property Q3DCamera::xRotation
 *
 * This property contains the X-rotation angle of the camera around the target point in degrees starting from
 * the current base position set by the setBaseOrientation() methods.
 */
float Q3DCamera::xRotation() const {
    return d_ptr->m_xRotation;
}

void Q3DCamera::setXRotation(float rotation)
{
    if (d_ptr->m_wrapXRotation)
        rotation = Utils::wrapValue(rotation, d_ptr->m_minXRotation, d_ptr->m_maxXRotation);
    else
        rotation = qBound(float(d_ptr->m_minXRotation), float(rotation), float(d_ptr->m_maxXRotation));

    if (d_ptr->m_xRotation != rotation) {
        d_ptr->setXRotation(rotation);
        if (d_ptr->m_activePreset != QDataVis::CameraPresetNone) {
            d_ptr->m_activePreset = QDataVis::CameraPresetNone;
            setDirty(true);
        }

        emit xRotationChanged(d_ptr->m_xRotation);
    }
}

/*!
 * \property Q3DCamera::yRotation
 *
 * This property contains the Y-rotation angle of the camera around the target point in degrees starting from
 * the current base position set by the setBaseOrientation() methods.
 */
float Q3DCamera::yRotation() const {
    return d_ptr->m_yRotation;
}

void Q3DCamera::setYRotation(float rotation)
{
    if (d_ptr->m_wrapYRotation)
        rotation = Utils::wrapValue(rotation, d_ptr->m_minYRotation, d_ptr->m_maxYRotation);
    else
        rotation = qBound(float(d_ptr->m_minYRotation), float(rotation), float(d_ptr->m_maxYRotation));

    if (d_ptr->m_yRotation != rotation) {
        d_ptr->setYRotation(rotation);
        if (d_ptr->m_activePreset != QDataVis::CameraPresetNone) {
            d_ptr->m_activePreset = QDataVis::CameraPresetNone;
            setDirty(true);
        }

        emit yRotationChanged(d_ptr->m_yRotation);
    }
}

/*!
 * \property Q3DCamera::minXRotation
 *
 * This property contains the current minimum X-rotation for the camera.
 * The full circle range is [-180, 180] and the minimum value is limited to -180.
 * Also the value can't be higher than the maximum, and is adjusted if necessary.
 *
 * \sa wrapXRotation, maxXRotation
 */
float Q3DCamera::minXRotation() const
{
    return d_ptr->m_minXRotation;
}

/*!
 * \internal
 */
void Q3DCamera::setMinXRotation(float minRotation)
{
    minRotation = qBound(-180.0f, minRotation, 180.0f);
    if (minRotation > d_ptr->m_maxXRotation)
        minRotation = d_ptr->m_maxXRotation;

    if (d_ptr->m_minXRotation != minRotation) {
        d_ptr->m_minXRotation = minRotation;
        emit minXRotationChanged(minRotation);

        if (d_ptr->m_xRotation < d_ptr->m_minXRotation)
            setXRotation(d_ptr->m_xRotation);
    }
}

/*!
 * \property Q3DCamera::minYRotation
 *
 * This property contains the current minimum Y-rotation for the camera.
 * The full Y angle range is [-90,90] and the minimum value is limited to -90.
 * Also the value can't be higher than the maximum, and is adjusted if necessary.
 *
 * \sa wrapYRotation, maxYRotation
 */
float Q3DCamera::minYRotation() const
{
    return d_ptr->m_minYRotation;
}

/*!
 * \internal
 */
void Q3DCamera::setMinYRotation(float minRotation)
{
    minRotation = qBound(-90.0f, minRotation, 90.0f);
    if (minRotation > d_ptr->m_maxYRotation)
        minRotation = d_ptr->m_maxYRotation;

    if (d_ptr->m_minYRotation != minRotation) {
        d_ptr->m_minYRotation = minRotation;
        emit minYRotationChanged(minRotation);

        if (d_ptr->m_yRotation < d_ptr->m_minYRotation)
            setYRotation(d_ptr->m_yRotation);
    }
}

/*!
 * \property Q3DCamera::maxXRotation
 *
 * This property contains the current maximum X-rotation for the camera.
 * The full circle range is [-180,180] and the maximum value is limited to 180.
 * Also the value can't be lower than the minimum, and is adjusted if necessary.
 *
 * \sa wrapXRotation, minXRotation
 */
float Q3DCamera::maxXRotation() const
{
    return d_ptr->m_maxXRotation;
}

/*!
 * \internal
 */
void Q3DCamera::setMaxXRotation(float maxRotation)
{
    maxRotation = qBound(-180.0f, maxRotation, 180.0f);

    if (maxRotation < d_ptr->m_minXRotation)
        maxRotation = d_ptr->m_minXRotation;

    if (d_ptr->m_maxXRotation != maxRotation) {
        d_ptr->m_maxXRotation = maxRotation;
        emit maxXRotationChanged(maxRotation);

        if (d_ptr->m_xRotation > d_ptr->m_maxXRotation)
            setXRotation(d_ptr->m_xRotation);
    }
}

/*!
 * \property Q3DCamera::maxYRotation
 *
 * This property contains the current maximum Y-rotation for the camera.
 * The full Y angle range is [-90,90] and the maximum value is limited to 90.
 * Also the value can't be lower than the minimum, and is adjusted if necessary.
 *
 * \sa wrapYRotation, minYRotation
 */
float Q3DCamera::maxYRotation() const
{
    return d_ptr->m_maxYRotation;
}

/*!
 * \internal
 */
void Q3DCamera::setMaxYRotation(float maxRotation)
{
    maxRotation = qBound(-90.0f, maxRotation, 90.0f);

    if (maxRotation < d_ptr->m_minYRotation)
        maxRotation = d_ptr->m_minYRotation;

    if (d_ptr->m_maxYRotation != maxRotation) {
        d_ptr->m_maxYRotation = maxRotation;
        emit maxYRotationChanged(maxRotation);

        if (d_ptr->m_yRotation > d_ptr->m_maxYRotation)
            setYRotation(d_ptr->m_yRotation);
    }
}

/*!
 * Sets the base values for the camera that are used when calculating the camera position using the rotation values.
 * The base position of the camera is defined by \a basePosition, expectation is that the x and y values are 0.
 * Look at target point is defined by \a target and the camera rotates around it. Up direction for the camera is
 * defined by \a baseUp, normally this is a vector with only y values set to 1.
 */
void Q3DCamera::setBaseOrientation(const QVector3D &basePosition,
                                   const QVector3D &target,
                                   const QVector3D &baseUp)
{
    if (position() != basePosition
            || d_ptr->m_target != target
            || d_ptr->m_up != baseUp) {
        setPosition(basePosition);
        d_ptr->m_target   = target;
        d_ptr->m_up       = baseUp;
        setDirty(true);
    }
}

/*!
 * \property Q3DCamera::viewMatrix
 *
 * This property contains the view matrix used in the 3D calculations. When the default orbiting camera behavior is sufficient,
 * there is no need to touch this property. But if the default behavior is insufficient, the view matrix can be set directly.
 * When setting the view matrix directly remember to set Q3DCamera::viewMatrixAutoUpdateEnabled to false.
 */
QMatrix4x4 Q3DCamera::viewMatrix() const
{
    return d_ptr->m_viewMatrix;
}

void Q3DCamera::setViewMatrix(const QMatrix4x4 &viewMatrix)
{
    if (d_ptr->m_viewMatrix != viewMatrix) {
        d_ptr->m_viewMatrix = viewMatrix;
        setDirty(true);
        emit viewMatrixChanged(d_ptr->m_viewMatrix);
    }
}

/*!
 * \property Q3DCamera::viewMatrixAutoUpdateEnabled
 *
 * This property determines if view matrix is automatically updated each render cycle using the current base orientation and
 * rotations. If set to false, no automatic recalculation is done and the view matrix can be set using the
 * Q3DMatrix::viewMatrix property.
 */
bool Q3DCamera::isViewMatrixAutoUpdateEnabled() const
{
    return d_ptr->m_isViewMatrixUpdateActive;
}

void Q3DCamera::setViewMatrixAutoUpdateEnabled(bool isEnabled)
{
    d_ptr->m_isViewMatrixUpdateActive = isEnabled;
    emit viewMatrixAutoUpdateChanged(isEnabled);
}

/*!
 * \property Q3DCamera::cameraPreset
 *
 * This property contains the currently active camera preset,
 * if no preset is active the value is QDataVis::CameraPresetNone.
 * \note The base camera orientation set by setBaseOrientation() will affect
 * the presets as all calculations are based on those values.
 */
QDataVis::CameraPreset Q3DCamera::cameraPreset() const
{
    return d_ptr->m_activePreset;
}

void Q3DCamera::setCameraPreset(QDataVis::CameraPreset preset)
{
    switch (preset) {
    case QDataVis::CameraPresetFrontLow: {
        setXRotation(0.0f);
        setYRotation(0.0f);
        break;
    }
    case QDataVis::CameraPresetFront: {
        setXRotation(0.0f);
        setYRotation(22.5f);
        break;
    }
    case QDataVis::CameraPresetFrontHigh: {
        setXRotation(0.0f);
        setYRotation(45.0f);
        break;
    }
    case QDataVis::CameraPresetLeftLow: {
        setXRotation(90.0f);
        setYRotation(0.0f);
        break;
    }
    case QDataVis::CameraPresetLeft: {
        setXRotation(90.0f);
        setYRotation(22.5f);
        break;
    }
    case QDataVis::CameraPresetLeftHigh: {
        setXRotation(90.0f);
        setYRotation(45.0f);
        break;
    }
    case QDataVis::CameraPresetRightLow: {
        setXRotation(-90.0f);
        setYRotation(0.0f);
        break;
    }
    case QDataVis::CameraPresetRight: {
        setXRotation(-90.0f);
        setYRotation(22.5f);
        break;
    }
    case QDataVis::CameraPresetRightHigh: {
        setXRotation(-90.0f);
        setYRotation(45.0f);
        break;
    }
    case QDataVis::CameraPresetBehindLow: {
        setXRotation(180.0f);
        setYRotation(0.0f);
        break;
    }
    case QDataVis::CameraPresetBehind: {
        setXRotation(180.0f);
        setYRotation(22.5f);
        break;
    }
    case QDataVis::CameraPresetBehindHigh: {
        setXRotation(180.0f);
        setYRotation(45.0f);
        break;
    }
    case QDataVis::CameraPresetIsometricLeft: {
        setXRotation(45.0f);
        setYRotation(22.5f);
        break;
    }
    case QDataVis::CameraPresetIsometricLeftHigh: {
        setXRotation(45.0f);
        setYRotation(45.0f);
        break;
    }
    case QDataVis::CameraPresetIsometricRight: {
        setXRotation(-45.0f);
        setYRotation(22.5f);
        break;
    }
    case QDataVis::CameraPresetIsometricRightHigh: {
        setXRotation(-45.0f);
        setYRotation(45.0f);
        break;
    }
    case QDataVis::CameraPresetDirectlyAbove: {
        setXRotation(0.0f);
        setYRotation(90.0f);
        break;
    }
    case QDataVis::CameraPresetDirectlyAboveCW45: {
        setXRotation(-45.0f);
        setYRotation(90.0f);
        break;
    }
    case QDataVis::CameraPresetDirectlyAboveCCW45: {
        setXRotation(45.0f);
        setYRotation(90.0f);
        break;
    }
    case QDataVis::CameraPresetFrontBelow: {
        setXRotation(0.0f);
        setYRotation(-45.0f);
        break;
    }
    case QDataVis::CameraPresetLeftBelow: {
        setXRotation(90.0f);
        setYRotation(-45.0f);
        break;
    }
    case QDataVis::CameraPresetRightBelow: {
        setXRotation(-90.0f);
        setYRotation(-45.0f);
        break;
    }
    case QDataVis::CameraPresetBehindBelow: {
        setXRotation(180.0f);
        setYRotation(-45.0f);
        break;
    }
    case QDataVis::CameraPresetDirectlyBelow: {
        setXRotation(0.0f);
        setYRotation(-90.0f);
        break;
    }
    default:
        preset = QDataVis::CameraPresetNone;
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
 * This property contains the the camera zoom level in percentages.
 * 100% means there is no zoom in or out set in the camera.
 */
int Q3DCamera::zoomLevel() const
{
    return d_ptr->m_zoomLevel;
}

void Q3DCamera::setZoomLevel(int zoomLevel)
{
    if (d_ptr->m_zoomLevel != zoomLevel) {
        d_ptr->m_zoomLevel = zoomLevel;
        setDirty(true);
        emit zoomLevelChanged(zoomLevel);
    }
}

/*!
 * Calculates and returns a position relative to the camera using the given parameters
 * and the current camera viewMatrix property.
 * \a relativePosition defines the relative 3D offset to the current camera position.
 * \a fixedRotation is optional, if given fixes rotation of the calculated point around the data visualization area to the given value in degrees.
 * \a distanceModifier is also optional, if given modifies the distance of the calculated point from the data visualization.
 * \return Calculated position relative to this camera's position.
 */
QVector3D Q3DCamera::calculatePositionRelativeToCamera(const QVector3D &relativePosition,
                                                       float fixedRotation,
                                                       float distanceModifier) const
{
    // Move the position with camera
    GLfloat radiusFactor = cameraDistance * (1.5f + distanceModifier);
    GLfloat xAngle;
    GLfloat yAngle;
    if (!fixedRotation) {
        xAngle = qDegreesToRadians(d_ptr->m_xRotation);
        yAngle = qDegreesToRadians(d_ptr->m_yRotation);
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

/*!
 * \property Q3DCamera::wrapXRotation
 *
 * This property determines the behavior of the minimum and maximum limits in the X-rotation.
 * By default the X-rotation wraps from minimum value to maximum and from maximum to minimum.
 *
 * If set to true the X-rotation of the camera is wrapped from minimum to maximum and from maximum to minimum.
 * If set to false the X-rotation of the camera is limited to the sector determined by minimum and maximum values.
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
 * If true the Y-rotation of the camera is wrapped from minimum to maximum and from maximum to minimum.
 * If false the Y-rotation of the camera is limited to the sector determined by minimum and maximum values.
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
 * Utility function that sets the camera rotations and distance.\a horizontal and \a vertical define the camera rotations to be used.
 * Optional \a zoom parameter can be given to set the zoom of the camera in range of 10-500%.
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
    m_wrapXRotation(true),
    m_wrapYRotation(false),
    m_zoomLevel(100),
    m_activePreset(QDataVis::CameraPresetNone)
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

void Q3DCameraPrivate::setMinXRotation(const float rotation)
{
    if (m_minXRotation != rotation) {
        m_minXRotation = rotation;
        q_ptr->setDirty(true);
    }
}

void Q3DCameraPrivate::setMinYRotation(const float rotation)
{
    if (m_minYRotation != rotation) {
        m_minYRotation = rotation;
        q_ptr->setDirty(true);
    }
}

void Q3DCameraPrivate::setMaxXRotation(const float rotation)
{
    if (m_maxXRotation != rotation) {
        m_maxXRotation = rotation;
        q_ptr->setDirty(true);
    }
}

void Q3DCameraPrivate::setMaxYRotation(const float rotation)
{
    if (m_maxYRotation != rotation) {
        m_maxYRotation = rotation;
        q_ptr->setDirty(true);
    }
}

// Recalculates the view matrix based on the currently set base orientation, rotation and zoom level values.
//  zoomAdjustment is adjustment to ensure that the 3D visualization stays inside the view area in the 100% zoom.
void Q3DCameraPrivate::updateViewMatrix(float zoomAdjustment)
{
    if (!m_isViewMatrixUpdateActive)
        return;

    int zoom = m_zoomLevel * zoomAdjustment;
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
    viewMatrix.scale((GLfloat)zoom / 100.0f);
    // Compensate for translation (if d_ptr->m_target is off origin)
    viewMatrix.translate(-m_target.x(), -m_target.y(), -m_target.z());

    q_ptr->setViewMatrix(viewMatrix);
}


QT_DATAVISUALIZATION_END_NAMESPACE
