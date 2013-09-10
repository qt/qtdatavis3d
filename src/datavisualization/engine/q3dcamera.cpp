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

Q3DCamera::Q3DCamera(QObject *parent) :
    Q3DObject(parent),
    d_ptr(new Q3DCameraPrivate(this))
{
}

Q3DCamera::~Q3DCamera()
{
}

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
    delete values;

    d_ptr->m_xRotation = source.d_ptr->m_xRotation;
    d_ptr->m_yRotation = source.d_ptr->m_yRotation;

    d_ptr->m_zoomLevel = source.d_ptr->m_zoomLevel;
    d_ptr->m_activePreset = source.d_ptr->m_activePreset;
}

void Q3DCamera::setRotations(const QPointF &rotation)
{
    setRotationsPrivate(rotation);
    d_ptr->m_activePreset = QDataVis::NoPreset;
}

void Q3DCamera::setDefaultOrientation(const QVector3D &defaultPosition,
                                      const QVector3D &defaultTarget,
                                      const QVector3D &defaultUp)
{
    Q3DObject::setPosition(defaultPosition);
    d_ptr->m_target   = defaultTarget;
    d_ptr->m_up       = defaultUp;
}

QPointF Q3DCamera::rotations() const
{
    QPointF rotations(d_ptr->m_xRotation, d_ptr->m_yRotation);
    return rotations;
}

void Q3DCamera::setViewMatrix(const QMatrix4x4 &viewMatrix)
{
    d_ptr->m_viewMatrix = viewMatrix;
}

QMatrix4x4 Q3DCamera::viewMatrix() const
{
    return d_ptr->m_viewMatrix;
}

void Q3DCamera::updateViewMatrix(GLfloat zoomAdjustment)
{
    bool showUnder = parentScene()->isUnderSideCameraEnabled();
    int zoom = zoomLevel() * zoomAdjustment;
    QMatrix4x4 viewMatrix;
    GLfloat lowerLimit = 0.0f;
    if (showUnder)
        lowerLimit = -90.0f;

    // Reset at 360 in x and limit to 0...90 in y
    if (qAbs(d_ptr->m_xRotation) >= 360.0f)
        d_ptr->m_xRotation = 0.0f;
    if (d_ptr->m_yRotation >= 90.0f)
        d_ptr->m_yRotation = 90.0f;
    else if (d_ptr->m_yRotation <= lowerLimit)
        d_ptr->m_yRotation = lowerLimit;

    // Apply to view matrix
    viewMatrix.lookAt(position(), d_ptr->m_target, d_ptr->m_up);
    // Compensate for translation (if d_ptr->m_target is off origin)
    viewMatrix.translate(d_ptr->m_target.x(), d_ptr->m_target.y(), d_ptr->m_target.z());
    // Apply rotations
    // Handle x and z rotation when y -angle is other than 0
    viewMatrix.rotate(d_ptr->m_xRotation, 0, qCos(qDegreesToRadians(d_ptr->m_yRotation)),
                      qSin(qDegreesToRadians(d_ptr->m_yRotation)));
    // y rotation is always "clean"
    viewMatrix.rotate(d_ptr->m_yRotation, 1.0f, 0.0f, 0.0f);
    // handle zoom by scaling
    viewMatrix.scale((GLfloat)zoom / 100.0f);
    // Compensate for translation (if d_ptr->m_target is off origin)
    viewMatrix.translate(-d_ptr->m_target.x(), -d_ptr->m_target.y(), -d_ptr->m_target.z());

    setViewMatrix(viewMatrix);
}

QDataVis::CameraPreset Q3DCamera::cameraPreset()
{
    return d_ptr->m_activePreset;
}

void Q3DCamera::setCameraPreset(QDataVis::CameraPreset preset)
{
    switch (preset) {
    case QDataVis::PresetFrontLow: {
        Q3DCamera::setRotationsPrivate(QPointF(0.0f, 0.0f));
        break;
    }
    case QDataVis::PresetFront: {
        Q3DCamera::setRotationsPrivate(QPointF(0.0f, 22.5f));
        break;
    }
    case QDataVis::PresetFrontHigh: {
        Q3DCamera::setRotationsPrivate(QPointF(0.0f, 45.0f));
        break;
    }
    case QDataVis::PresetLeftLow: {
        Q3DCamera::setRotationsPrivate(QPointF(90.0f, 0.0f));
        break;
    }
    case QDataVis::PresetLeft: {
        Q3DCamera::setRotationsPrivate(QPointF(90.0f, 22.5f));
        break;
    }
    case QDataVis::PresetLeftHigh: {
        Q3DCamera::setRotationsPrivate(QPointF(90.0f, 45.0f));
        break;
    }
    case QDataVis::PresetRightLow: {
        Q3DCamera::setRotationsPrivate(QPointF(-90.0f, 0.0f));
        break;
    }
    case QDataVis::PresetRight: {
        Q3DCamera::setRotationsPrivate(QPointF(-90.0f, 22.5f));
        break;
    }
    case QDataVis::PresetRightHigh: {
        Q3DCamera::setRotationsPrivate(QPointF(-90.0f, 45.0f));
        break;
    }
    case QDataVis::PresetBehindLow: {
        Q3DCamera::setRotationsPrivate(QPointF(180.0f, 0.0f));
        break;
    }
    case QDataVis::PresetBehind: {
        Q3DCamera::setRotationsPrivate(QPointF(180.0f, 22.5f));
        break;
    }
    case QDataVis::PresetBehindHigh: {
        Q3DCamera::setRotationsPrivate(QPointF(180.0f, 45.0f));
        break;
    }
    case QDataVis::PresetIsometricLeft: {
        Q3DCamera::setRotationsPrivate(QPointF(45.0f, 22.5f));
        break;
    }
    case QDataVis::PresetIsometricLeftHigh: {
        Q3DCamera::setRotationsPrivate(QPointF(45.0f, 45.0f));
        break;
    }
    case QDataVis::PresetIsometricRight: {
        Q3DCamera::setRotationsPrivate(QPointF(-45.0f, 22.5f));
        break;
    }
    case QDataVis::PresetIsometricRightHigh: {
        Q3DCamera::setRotationsPrivate(QPointF(-45.0f, 45.0f));
        break;
    }
    case QDataVis::PresetDirectlyAbove: {
        Q3DCamera::setRotationsPrivate(QPointF(0.0f, 90.0f));
        break;
    }
    case QDataVis::PresetDirectlyAboveCW45: {
        Q3DCamera::setRotationsPrivate(QPointF(-45.0f, 90.0f));
        break;
    }
    case QDataVis::PresetDirectlyAboveCCW45: {
        Q3DCamera::setRotationsPrivate(QPointF(45.0f, 90.0f));
        break;
    }
    case QDataVis::PresetFrontBelow: {
        Q3DCamera::setRotationsPrivate(QPointF(0.0f, -45.0f));
        break;
    }
    case QDataVis::PresetLeftBelow: {
        Q3DCamera::setRotationsPrivate(QPointF(90.0f, -45.0f));
        break;
    }
    case QDataVis::PresetRightBelow: {
        Q3DCamera::setRotationsPrivate(QPointF(-90.0f, -45.0f));
        break;
    }
    case QDataVis::PresetBehindBelow: {
        Q3DCamera::setRotationsPrivate(QPointF(180.0f, -45.0f));
        break;
    }
    case QDataVis::PresetDirectlyBelow: {
        Q3DCamera::setRotationsPrivate(QPointF(0.0f, -90.0f));
        break;
    }
    default:
        preset = QDataVis::NoPreset;
        break;
    }

    d_ptr->m_activePreset = preset;
}

void Q3DCamera::setRotationsPrivate(const QPointF &rotation)
{
    d_ptr->m_xRotation = rotation.x();
    d_ptr->m_yRotation = rotation.y();
}

void Q3DCamera::setZoomLevel(int zoomLevel)
{
    d_ptr->m_zoomLevel = zoomLevel;
}

int Q3DCamera::zoomLevel()
{
    return d_ptr->m_zoomLevel;
}

QVector3D Q3DCamera::calculatePositionRelativeToCamera(const QVector3D &relativePosition,
                                                       GLfloat fixedRotation,
                                                       GLfloat distanceModifier) const
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
    m_zoomLevel(100)
{
}


Q3DCameraPrivate::~Q3DCameraPrivate()
{
}

QT_DATAVISUALIZATION_END_NAMESPACE
