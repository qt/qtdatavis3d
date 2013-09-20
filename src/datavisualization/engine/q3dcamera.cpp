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

#include <qmath.h>
#include <QVector3D>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
   \class Q3DCamera
   \inmodule QtDataVisualization
   \brief Representation of a camera in 3D space.
   \since 1.0.0

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

    d_ptr->m_zoomLevel = source.d_ptr->m_zoomLevel;
    d_ptr->m_activePreset = source.d_ptr->m_activePreset;
}

/*!
 * \property Q3DCamera::rotations
 *
 * This property contains the rotation angles of the camera around the target point in degrees starting from
 * the current base position set by the setBaseOrientation() methods.
 */
QPointF Q3DCamera::rotations() const
{
    QPointF rotations(d_ptr->m_xRotation, d_ptr->m_yRotation);
    return rotations;
}

void Q3DCamera::setRotations(const QPointF &rotation)
{
    d_ptr->setRotations(rotation);
    if (d_ptr->m_activePreset != QDataVis::NoPreset) {
        d_ptr->m_activePreset = QDataVis::NoPreset;
        setDirty(true);
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
 * This property contains the view matrix used in the 3D calculations. When the default orbiting camera behavior is sufficient
 * there is no need to touch this property. But if the default behavior is insufficient the view matrix can be set directly.
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
    }
}

/*!
 * \property Q3DCamera::viewMatrixAutoUpdateEnabled
 *
 * This property determines if view matrix is automatically updated each render cycle using the current base orientation and
 * rotations. If set to false, no automatic recalculation is done and the view matrix can be set using the
 * Q3DMatrix::viewMatrix property.
 */
bool Q3DCamera::isViewMatrixAutoUpdateEnabled()
{
    return d_ptr->m_isViewMatrixUpdateActive;
}

void Q3DCamera::setViewMatrixAutoUpdateEnabled(bool isEnabled)
{
    d_ptr->m_isViewMatrixUpdateActive = isEnabled;
}

/*!
 * \property Q3DCamera::cameraPreset
 *
 * This property contains the currently active camera preset,
 * if no preset is active the value is QDataVis::NoPreset.
 * \note The base camera orientation set by setBaseOrientation() will affect
 * the presets as all calculations are based on those values.
 */
QDataVis::CameraPreset Q3DCamera::cameraPreset()
{
    return d_ptr->m_activePreset;
}

void Q3DCamera::setCameraPreset(QDataVis::CameraPreset preset)
{
    switch (preset) {
    case QDataVis::PresetFrontLow: {
        d_ptr->setRotations(QPointF(0.0f, 0.0f));
        break;
    }
    case QDataVis::PresetFront: {
        d_ptr->setRotations(QPointF(0.0f, 22.5f));
        break;
    }
    case QDataVis::PresetFrontHigh: {
        d_ptr->setRotations(QPointF(0.0f, 45.0f));
        break;
    }
    case QDataVis::PresetLeftLow: {
        d_ptr->setRotations(QPointF(90.0f, 0.0f));
        break;
    }
    case QDataVis::PresetLeft: {
        d_ptr->setRotations(QPointF(90.0f, 22.5f));
        break;
    }
    case QDataVis::PresetLeftHigh: {
        d_ptr->setRotations(QPointF(90.0f, 45.0f));
        break;
    }
    case QDataVis::PresetRightLow: {
        d_ptr->setRotations(QPointF(-90.0f, 0.0f));
        break;
    }
    case QDataVis::PresetRight: {
        d_ptr->setRotations(QPointF(-90.0f, 22.5f));
        break;
    }
    case QDataVis::PresetRightHigh: {
        d_ptr->setRotations(QPointF(-90.0f, 45.0f));
        break;
    }
    case QDataVis::PresetBehindLow: {
        d_ptr->setRotations(QPointF(180.0f, 0.0f));
        break;
    }
    case QDataVis::PresetBehind: {
        d_ptr->setRotations(QPointF(180.0f, 22.5f));
        break;
    }
    case QDataVis::PresetBehindHigh: {
        d_ptr->setRotations(QPointF(180.0f, 45.0f));
        break;
    }
    case QDataVis::PresetIsometricLeft: {
        d_ptr->setRotations(QPointF(45.0f, 22.5f));
        break;
    }
    case QDataVis::PresetIsometricLeftHigh: {
        d_ptr->setRotations(QPointF(45.0f, 45.0f));
        break;
    }
    case QDataVis::PresetIsometricRight: {
        d_ptr->setRotations(QPointF(-45.0f, 22.5f));
        break;
    }
    case QDataVis::PresetIsometricRightHigh: {
        d_ptr->setRotations(QPointF(-45.0f, 45.0f));
        break;
    }
    case QDataVis::PresetDirectlyAbove: {
        d_ptr->setRotations(QPointF(0.0f, 90.0f));
        break;
    }
    case QDataVis::PresetDirectlyAboveCW45: {
        d_ptr->setRotations(QPointF(-45.0f, 90.0f));
        break;
    }
    case QDataVis::PresetDirectlyAboveCCW45: {
        d_ptr->setRotations(QPointF(45.0f, 90.0f));
        break;
    }
    case QDataVis::PresetFrontBelow: {
        d_ptr->setRotations(QPointF(0.0f, -45.0f));
        break;
    }
    case QDataVis::PresetLeftBelow: {
        d_ptr->setRotations(QPointF(90.0f, -45.0f));
        break;
    }
    case QDataVis::PresetRightBelow: {
        d_ptr->setRotations(QPointF(-90.0f, -45.0f));
        break;
    }
    case QDataVis::PresetBehindBelow: {
        d_ptr->setRotations(QPointF(180.0f, -45.0f));
        break;
    }
    case QDataVis::PresetDirectlyBelow: {
        d_ptr->setRotations(QPointF(0.0f, -90.0f));
        break;
    }
    default:
        preset = QDataVis::NoPreset;
        break;
    }

    if (d_ptr->m_activePreset != preset) {
        d_ptr->m_activePreset = preset;
        setDirty(true);
    }
}

/*!
 * \property Q3DCamera::zoomLevel
 *
 * This property contains the the camera zoom level in percentages.
 * 100% means there is no zoom in or out set in the camera.
 */
int Q3DCamera::zoomLevel()
{
    return d_ptr->m_zoomLevel;
}

void Q3DCamera::setZoomLevel(int zoomLevel)
{
    if (d_ptr->m_zoomLevel != zoomLevel) {
        d_ptr->m_zoomLevel = zoomLevel;
        setDirty(true);
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
                                                       qreal fixedRotation,
                                                       qreal distanceModifier) const
{
    // Move the position with camera
    GLfloat radiusFactor = relativePosition.z() * (1.5f + distanceModifier);
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


Q3DCameraPrivate::Q3DCameraPrivate(Q3DCamera *q) :
    q_ptr(q),
    m_isViewMatrixUpdateActive(true),
    m_xRotation(0.0f),
    m_yRotation(0.0f),
    m_zoomLevel(100),
    m_activePreset(QDataVis::NoPreset)
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

void Q3DCameraPrivate::setRotations(const QPointF &rotation)
{
    if (m_xRotation != rotation.x() || m_yRotation != rotation.y()) {
        m_xRotation = rotation.x();
        m_yRotation = rotation.y();
        q_ptr->setDirty(true);
    }
}

// Recalculates the view matrix based on the currently set base orientation, rotation and zoom level values.
//  zoomAdjustment is adjustment to ensure that the 3D visualization stays inside the view area in the 100% zoom.
void Q3DCameraPrivate::updateViewMatrix(qreal zoomAdjustment)
{
    if (!m_isViewMatrixUpdateActive)
        return;

    bool showUnder = q_ptr->parentScene()->isUnderSideCameraEnabled();
    int zoom = m_zoomLevel * zoomAdjustment;
    QMatrix4x4 viewMatrix;
    GLfloat lowerLimit = 0.0f;
    if (showUnder)
        lowerLimit = -90.0f;

    // Reset at 360 in x and limit to 0...90 in y
    if (qAbs(m_xRotation) >= 360.0f)
        m_xRotation = 0.0f;
    if (m_yRotation >= 90.0f)
        m_yRotation = 90.0f;
    else if (m_yRotation <= lowerLimit)
        m_yRotation = lowerLimit;

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
