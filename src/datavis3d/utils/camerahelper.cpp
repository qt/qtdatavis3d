/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "camerahelper_p.h"

#include <qmath.h>
#include <QMatrix4x4>
#include <QVector3D>

QT_DATAVIS3D_BEGIN_NAMESPACE

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

void CameraHelper::setCameraPreset(CameraPreset preset)
{
    switch (preset) {
    case PresetFrontLow: {
        qDebug("PresetFrontLow");
        CameraHelper::setCameraRotation(QPointF(0.0f, 0.0f));
        break;
    }
    case PresetFront: {
        qDebug("PresetFront");
        CameraHelper::setCameraRotation(QPointF(0.0f, 22.5f));
        break;
    }
    case PresetFrontHigh: {
        qDebug("PresetFrontHigh");
        CameraHelper::setCameraRotation(QPointF(0.0f, 45.0f));
        break;
    }
    case PresetLeftLow: {
        qDebug("PresetLeftLow");
        CameraHelper::setCameraRotation(QPointF(90.0f, 0.0f));
        break;
    }
    case PresetLeft: {
        qDebug("PresetLeft");
        CameraHelper::setCameraRotation(QPointF(90.0f, 22.5f));
        break;
    }
    case PresetLeftHigh: {
        qDebug("PresetLeftHigh");
        CameraHelper::setCameraRotation(QPointF(90.0f, 45.0f));
        break;
    }
    case PresetRightLow: {
        qDebug("PresetRightLow");
        CameraHelper::setCameraRotation(QPointF(-90.0f, 0.0f));
        break;
    }
    case PresetRight: {
        qDebug("PresetRight");
        CameraHelper::setCameraRotation(QPointF(-90.0f, 22.5f));
        break;
    }
    case PresetRightHigh: {
        qDebug("PresetRightHigh");
        CameraHelper::setCameraRotation(QPointF(-90.0f, 45.0f));
        break;
    }
    case PresetBehindLow: {
        qDebug("PresetBehindLow");
        CameraHelper::setCameraRotation(QPointF(180.0f, 0.0f));
        break;
    }
    case PresetBehind: {
        qDebug("PresetBehind");
        CameraHelper::setCameraRotation(QPointF(180.0f, 22.5f));
        break;
    }
    case PresetBehindHigh: {
        qDebug("PresetBehindHigh");
        CameraHelper::setCameraRotation(QPointF(180.0f, 45.0f));
        break;
    }
    case PresetIsometricLeft: {
        qDebug("PresetIsometricLeft");
        CameraHelper::setCameraRotation(QPointF(45.0f, 22.5f));
        break;
    }
    case PresetIsometricLeftHigh: {
        qDebug("PresetIsometricLeftHigh");
        CameraHelper::setCameraRotation(QPointF(45.0f, 45.0f));
        break;
    }
    case PresetIsometricRight: {
        qDebug("PresetIsometricRight");
        CameraHelper::setCameraRotation(QPointF(-45.0f, 22.5f));
        break;
    }
    case PresetIsometricRightHigh: {
        qDebug("PresetIsometricRightHigh");
        CameraHelper::setCameraRotation(QPointF(-45.0f, 45.0f));
        break;
    }
    case PresetDirectlyAbove: {
        qDebug("PresetDirectlyAbove");
        CameraHelper::setCameraRotation(QPointF(0.0f, 90.0f));
        break;
    }
    case PresetDirectlyAboveCW45: {
        qDebug("PresetDirectlyAboveCW45");
        CameraHelper::setCameraRotation(QPointF(-45.0f, 90.0f));
        break;
    }
    case PresetDirectlyAboveCCW45: {
        qDebug("PresetDirectlyAboveCCW45");
        CameraHelper::setCameraRotation(QPointF(45.0f, 90.0f));
        break;
    }
    case PresetFrontBelow: {
        qDebug("PresetFrontBelow");
        CameraHelper::setCameraRotation(QPointF(0.0f, -45.0f));
        break;
    }
    case PresetLeftBelow: {
        qDebug("PresetLeftBelow");
        CameraHelper::setCameraRotation(QPointF(90.0f, -45.0f));
        break;
    }
    case PresetRightBelow: {
        qDebug("PresetRightBelow");
        CameraHelper::setCameraRotation(QPointF(-90.0f, -45.0f));
        break;
    }
    case PresetBehindBelow: {
        qDebug("PresetBehindBelow");
        CameraHelper::setCameraRotation(QPointF(180.0f, -45.0f));
        break;
    }
    case PresetDirectlyBelow: {
        qDebug("PresetDirectlyBelow");
        CameraHelper::setCameraRotation(QPointF(0.0f, -90.0f));
        break;
    }
    default:
        break;
    }
}

QT_DATAVIS3D_END_NAMESPACE
