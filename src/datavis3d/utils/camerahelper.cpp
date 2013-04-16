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

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

// Initial camera position
QVector3D m_position = QVector3D(0, 0.25, 3);
QVector3D m_target = QVector3D(0, 0, 0);
QVector3D m_up = QVector3D(0, 1, 0);

QPoint m_previousMousePos(0, 0);

float m_xRotation = 0;
float m_yRotation = 0;
float m_defaultXRotation = 0;
float m_defaultYRotation = 0;

const float m_pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679f;
float m_rotationSpeed = 100;

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
                                             int screenWidth, int screenHeight)
{
    QMatrix4x4 viewMatrix;

    // Calculate mouse movement since last frame
    float mouseMoveX = float(m_previousMousePos.x() - mousePos.x())
            / (screenWidth / m_rotationSpeed);
    float mouseMoveY = float(m_previousMousePos.y() - mousePos.y())
            / (screenHeight / m_rotationSpeed);
    // Apply to rotations
    m_xRotation -= mouseMoveX;
    m_yRotation -= mouseMoveY;
    // Reset at 360 in x and limit to 0...90 in y
    if (qFabs(m_xRotation) >= 360)
        m_xRotation = 0;
    if (m_yRotation >= 90)
        m_yRotation = 90;
    else if (m_yRotation <= 0)
        m_yRotation = 0;

    // Apply to view matrix
    viewMatrix.lookAt(m_position, m_target, m_up);
    // Compensate for translation (if m_target is off origin)
    viewMatrix.translate(m_target.x(), m_target.y(), m_target.z());
    // Apply rotations
    // Handle x and z rotation when y -angle is other than 0
    viewMatrix.rotate(m_xRotation, 0, cos(m_yRotation*m_pi/180), sin(m_yRotation*m_pi/180));
    // y rotation is always "clean"
    viewMatrix.rotate(m_yRotation, 1.0f, 0, 0);
    // handle zoom by scaling
    viewMatrix.scale((float)zoom / 100.0f);
    // Compensate for translation (if m_target is off origin)
    viewMatrix.translate(-m_target.x(), -m_target.y(), -m_target.z());
    //qDebug() << m_xRotation << m_yRotation;

    //qDebug() << "sin(m_yRotation)" << sin(m_yRotation*m_pi/180);
    //qDebug() << "asin(m_yRotation)" << asin(m_yRotation*m_pi/180);
    //qDebug() << "cos(m_yRotation)" << cos(m_yRotation*m_pi/180);
    //qDebug() << "tan(m_yRotation)" << tan(m_yRotation*m_pi/180);

    m_previousMousePos = mousePos;
    return viewMatrix;
}

QVector3D CameraHelper::calculateLightPosition(const QVector3D &lightPosition, float fixedRotation)
{
    // Move light with camera
    QVector3D newLightPosition;
    float xAngle;
    float yAngle;
    if (!fixedRotation) {
        xAngle = m_xRotation * m_pi / 180;
        yAngle = m_yRotation * m_pi / 180;
    } else {
        xAngle = fixedRotation * m_pi / 180;
        yAngle = 0;
    }
    float radius = lightPosition.y() * 2.0f; // set radius to match the highest height of the light
    float zPos = radius * cos(xAngle) * cos(yAngle);
    float xPos = radius * sin(xAngle) * cos(yAngle);
    float yPos = lightPosition.y() * sin(yAngle);
    // Keep light in the set position in relation to camera
    // TODO: Does not work perfectly yet; Light seems wrong when viewing scene from sides (or isometrically)
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

void CameraHelper::setCameraPreset(Q3DBars::CameraPreset preset)
{
    switch (preset) {
    case Q3DBars::PresetFrontLow: {
        qDebug("PresetFrontLow");
        CameraHelper::setCameraRotation(QPointF(0.0f, 0.0f));
        break;
    }
    case Q3DBars::PresetFront: {
        qDebug("PresetFront");
        CameraHelper::setCameraRotation(QPointF(0.0f, 22.5f));
        break;
    }
    case Q3DBars::PresetFrontHigh: {
        qDebug("PresetFrontHigh");
        CameraHelper::setCameraRotation(QPointF(0.0f, 45.0f));
        break;
    }
    case Q3DBars::PresetLeftLow: {
        qDebug("PresetLeftLow");
        CameraHelper::setCameraRotation(QPointF(90.0f, 0.0f));
        break;
    }
    case Q3DBars::PresetLeft: {
        qDebug("PresetLeft");
        CameraHelper::setCameraRotation(QPointF(90.0f, 22.5f));
        break;
    }
    case Q3DBars::PresetLeftHigh: {
        qDebug("PresetLeftHigh");
        CameraHelper::setCameraRotation(QPointF(90.0f, 45.0f));
        break;
    }
    case Q3DBars::PresetRightLow: {
        qDebug("PresetRightLow");
        CameraHelper::setCameraRotation(QPointF(-90.0f, 0.0f));
        break;
    }
    case Q3DBars::PresetRight: {
        qDebug("PresetRight");
        CameraHelper::setCameraRotation(QPointF(-90.0f, 22.5f));
        break;
    }
    case Q3DBars::PresetRightHigh: {
        qDebug("PresetRightHigh");
        CameraHelper::setCameraRotation(QPointF(-90.0f, 45.0f));
        break;
    }
    case Q3DBars::PresetBehindLow: {
        qDebug("PresetBehindLow");
        CameraHelper::setCameraRotation(QPointF(180.0f, 0.0f));
        break;
    }
    case Q3DBars::PresetBehind: {
        qDebug("PresetBehind");
        CameraHelper::setCameraRotation(QPointF(180.0f, 22.5f));
        break;
    }
    case Q3DBars::PresetBehindHigh: {
        qDebug("PresetBehindHigh");
        CameraHelper::setCameraRotation(QPointF(180.0f, 45.0f));
        break;
    }
    case Q3DBars::PresetIsometricLeft: {
        qDebug("PresetIsometricLeft");
        CameraHelper::setCameraRotation(QPointF(45.0f, 22.5f));
        break;
    }
    case Q3DBars::PresetIsometricLeftHigh: {
        qDebug("PresetIsometricLeftHigh");
        CameraHelper::setCameraRotation(QPointF(45.0f, 45.0f));
        break;
    }
    case Q3DBars::PresetIsometricRight: {
        qDebug("PresetIsometricRight");
        CameraHelper::setCameraRotation(QPointF(-45.0f, 22.5f));
        break;
    }
    case Q3DBars::PresetIsometricRightHigh: {
        qDebug("PresetIsometricRightHigh");
        CameraHelper::setCameraRotation(QPointF(-45.0f, 45.0f));
        break;
    }
    case Q3DBars::PresetDirectlyAbove: {
        qDebug("PresetDirectlyAbove");
        CameraHelper::setCameraRotation(QPointF(0.0f, 90.0f));
        break;
    }
    case Q3DBars::PresetDirectlyAboveCW45: {
        qDebug("PresetDirectlyAboveCW45");
        CameraHelper::setCameraRotation(QPointF(-45.0f, 90.0f));
        break;
    }
    case Q3DBars::PresetDirectlyAboveCCW45: {
        qDebug("PresetDirectlyAboveCCW45");
        CameraHelper::setCameraRotation(QPointF(45.0f, 90.0f));
        break;
    }
    default:
        break;
    }
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE
