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

#include "camerahelper_p.h"

#include <qmath.h>
#include <QMatrix4x4>
#include <QVector3D>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

CameraHelper::CameraHelper(QObject *parent) :
    QObject(parent),
    m_position(0, 0.25, 3),
    m_target(0, 0, 0),
    m_up(0, 1, 0),
    m_previousMousePos(0,0),
    m_xRotation(0),
    m_yRotation(0),
    m_defaultXRotation(0),
    m_defaultYRotation(0),
    m_rotationSpeed(100)
{
}

CameraHelper::~CameraHelper()
{
}


// FUNCTIONS
void CameraHelper::setRotationSpeed(int speed)
{
    // increase for faster rotation
    m_rotationSpeed = speed;
}

void CameraHelper::setCameraRotation(const QPointF &rotation)
{
    m_xRotation = rotation.x();
    m_defaultXRotation = m_xRotation;
    m_yRotation = rotation.y();
    m_defaultYRotation = m_yRotation;
}

void CameraHelper::setDefaultCameraOrientation(const QVector3D &defaultPosition,
                                               const QVector3D &defaultTarget,
                                               const QVector3D &defaultUp)
{
    m_position = defaultPosition;
    m_target = defaultTarget;
    m_up = defaultUp;
}

QMatrix4x4 CameraHelper::calculateViewMatrix(const QPoint &mousePos, int zoom,
                                             int screenWidth, int screenHeight, bool showUnder)
{
    QMatrix4x4 viewMatrix;
    GLfloat lowerLimit = 0.0f;

    if (showUnder)
        lowerLimit = -90.0f;

    // Calculate mouse movement since last frame
    GLfloat mouseMoveX = GLfloat(m_previousMousePos.x() - mousePos.x())
            / (screenWidth / m_rotationSpeed);
    GLfloat mouseMoveY = GLfloat(m_previousMousePos.y() - mousePos.y())
            / (screenHeight / m_rotationSpeed);
    // Apply to rotations
    m_xRotation -= mouseMoveX;
    m_yRotation -= mouseMoveY;
    // Reset at 360 in x and limit to 0...90 in y
    if (qAbs(m_xRotation) >= 360.0f)
        m_xRotation = 0.0f;
    if (m_yRotation >= 90.0f)
        m_yRotation = 90.0f;
    else if (m_yRotation <= lowerLimit)
        m_yRotation = lowerLimit;

    // Apply to view matrix
    viewMatrix.lookAt(m_position, m_target, m_up);
    // Compensate for translation (if m_target is off origin)
    viewMatrix.translate(m_target.x(), m_target.y(), m_target.z());
    // Apply rotations
    // Handle x and z rotation when y -angle is other than 0
    viewMatrix.rotate(m_xRotation, 0, qCos(qDegreesToRadians(m_yRotation)),
                      qSin(qDegreesToRadians(m_yRotation)));
    // y rotation is always "clean"
    viewMatrix.rotate(m_yRotation, 1.0f, 0.0f, 0.0f);
    // handle zoom by scaling
    viewMatrix.scale((GLfloat)zoom / 100.0f);
    // Compensate for translation (if m_target is off origin)
    viewMatrix.translate(-m_target.x(), -m_target.y(), -m_target.z());
    //qDebug() << m_xRotation << m_yRotation;

    m_previousMousePos = mousePos;
    return viewMatrix;
}

QVector3D CameraHelper::calculateLightPosition(const QVector3D &lightPosition,
                                               GLfloat fixedRotation, GLfloat distanceModifier)
{
    // Move light with camera
    QVector3D newLightPosition;
    GLfloat radiusFactor = lightPosition.z() * (1.5f + distanceModifier); // for making sure light is outside the scene at its lowest point
    GLfloat xAngle;
    GLfloat yAngle;
    if (!fixedRotation) {
        xAngle = qDegreesToRadians(m_xRotation);
        yAngle = qDegreesToRadians(m_yRotation);
    } else {
        xAngle = qDegreesToRadians(fixedRotation);
        yAngle = 0;
    }
    GLfloat radius = (radiusFactor + lightPosition.y()); // set radius to match the highest height of the light
    GLfloat zPos = radius * qCos(xAngle) * qCos(yAngle);
    GLfloat xPos = radius * qSin(xAngle) * qCos(yAngle);
    GLfloat yPos = (radiusFactor + lightPosition.y()) * qSin(yAngle);
    // Keep light in the set position in relation to camera
    newLightPosition = QVector3D(-xPos + lightPosition.x(),
                                 yPos + lightPosition.y(),
                                 zPos + lightPosition.z());
    //qDebug() << newLightPosition << xAngle << yAngle << fixedRotation;
    return newLightPosition;
}

void CameraHelper::updateMousePos(const QPoint &mousePos)
{
    m_previousMousePos = mousePos;
    // if mouse position is set to (0, 0), reset rotations
    if (QPoint(0, 0) == mousePos) {
        m_xRotation = m_defaultXRotation;
        m_yRotation = m_defaultYRotation;
    }
}

QPointF CameraHelper::getCameraRotations()
{
    QPointF rotations(m_xRotation, m_yRotation);
    return rotations;
}

void CameraHelper::setCameraPreset(QDataVis::CameraPreset preset)
{
    switch (preset) {
    case QDataVis::PresetFrontLow: {
        qDebug("PresetFrontLow");
        CameraHelper::setCameraRotation(QPointF(0.0f, 0.0f));
        break;
    }
    case QDataVis::PresetFront: {
        qDebug("PresetFront");
        CameraHelper::setCameraRotation(QPointF(0.0f, 22.5f));
        break;
    }
    case QDataVis::PresetFrontHigh: {
        qDebug("PresetFrontHigh");
        CameraHelper::setCameraRotation(QPointF(0.0f, 45.0f));
        break;
    }
    case QDataVis::PresetLeftLow: {
        qDebug("PresetLeftLow");
        CameraHelper::setCameraRotation(QPointF(90.0f, 0.0f));
        break;
    }
    case QDataVis::PresetLeft: {
        qDebug("PresetLeft");
        CameraHelper::setCameraRotation(QPointF(90.0f, 22.5f));
        break;
    }
    case QDataVis::PresetLeftHigh: {
        qDebug("PresetLeftHigh");
        CameraHelper::setCameraRotation(QPointF(90.0f, 45.0f));
        break;
    }
    case QDataVis::PresetRightLow: {
        qDebug("PresetRightLow");
        CameraHelper::setCameraRotation(QPointF(-90.0f, 0.0f));
        break;
    }
    case QDataVis::PresetRight: {
        qDebug("PresetRight");
        CameraHelper::setCameraRotation(QPointF(-90.0f, 22.5f));
        break;
    }
    case QDataVis::PresetRightHigh: {
        qDebug("PresetRightHigh");
        CameraHelper::setCameraRotation(QPointF(-90.0f, 45.0f));
        break;
    }
    case QDataVis::PresetBehindLow: {
        qDebug("PresetBehindLow");
        CameraHelper::setCameraRotation(QPointF(180.0f, 0.0f));
        break;
    }
    case QDataVis::PresetBehind: {
        qDebug("PresetBehind");
        CameraHelper::setCameraRotation(QPointF(180.0f, 22.5f));
        break;
    }
    case QDataVis::PresetBehindHigh: {
        qDebug("PresetBehindHigh");
        CameraHelper::setCameraRotation(QPointF(180.0f, 45.0f));
        break;
    }
    case QDataVis::PresetIsometricLeft: {
        qDebug("PresetIsometricLeft");
        CameraHelper::setCameraRotation(QPointF(45.0f, 22.5f));
        break;
    }
    case QDataVis::PresetIsometricLeftHigh: {
        qDebug("PresetIsometricLeftHigh");
        CameraHelper::setCameraRotation(QPointF(45.0f, 45.0f));
        break;
    }
    case QDataVis::PresetIsometricRight: {
        qDebug("PresetIsometricRight");
        CameraHelper::setCameraRotation(QPointF(-45.0f, 22.5f));
        break;
    }
    case QDataVis::PresetIsometricRightHigh: {
        qDebug("PresetIsometricRightHigh");
        CameraHelper::setCameraRotation(QPointF(-45.0f, 45.0f));
        break;
    }
    case QDataVis::PresetDirectlyAbove: {
        qDebug("PresetDirectlyAbove");
        CameraHelper::setCameraRotation(QPointF(0.0f, 90.0f));
        break;
    }
    case QDataVis::PresetDirectlyAboveCW45: {
        qDebug("PresetDirectlyAboveCW45");
        CameraHelper::setCameraRotation(QPointF(-45.0f, 90.0f));
        break;
    }
    case QDataVis::PresetDirectlyAboveCCW45: {
        qDebug("PresetDirectlyAboveCCW45");
        CameraHelper::setCameraRotation(QPointF(45.0f, 90.0f));
        break;
    }
    case QDataVis::PresetFrontBelow: {
        qDebug("PresetFrontBelow");
        CameraHelper::setCameraRotation(QPointF(0.0f, -45.0f));
        break;
    }
    case QDataVis::PresetLeftBelow: {
        qDebug("PresetLeftBelow");
        CameraHelper::setCameraRotation(QPointF(90.0f, -45.0f));
        break;
    }
    case QDataVis::PresetRightBelow: {
        qDebug("PresetRightBelow");
        CameraHelper::setCameraRotation(QPointF(-90.0f, -45.0f));
        break;
    }
    case QDataVis::PresetBehindBelow: {
        qDebug("PresetBehindBelow");
        CameraHelper::setCameraRotation(QPointF(180.0f, -45.0f));
        break;
    }
    case QDataVis::PresetDirectlyBelow: {
        qDebug("PresetDirectlyBelow");
        CameraHelper::setCameraRotation(QPointF(0.0f, -90.0f));
        break;
    }
    default:
        break;
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
