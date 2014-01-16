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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef CAMERAPOSITIONER_P_H
#define CAMERAPOSITIONER_P_H

#include "datavisualizationglobal_p.h"
#include "q3dcamera.h"
#include "q3dbars.h"
#include <QObject>

class QMatrix4x4;
class QVector3D;
class QPoint;
class QPointF;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class CameraHelper : public QObject
{
    Q_OBJECT

private:
    QVector3D m_position;
    QVector3D m_target;
    QVector3D m_up;

    QPoint m_previousMousePos;

    GLfloat m_xRotation;
    GLfloat m_yRotation;
    GLfloat m_defaultXRotation;
    GLfloat m_defaultYRotation;

    GLfloat m_rotationSpeed;

public:
    explicit CameraHelper(QObject *parent = 0);
    ~CameraHelper();

    // How fast camera rotates when mouse is dragged. Default is 100.
    void setRotationSpeed(int speed);
    // Set camera rotation in degrees
    void setCameraRotation(const QPointF &rotation);
    // Get camera rotations
    QPointF getCameraRotations();
    // Set default camera orientation. Position's x and y should be 0.
    void setDefaultCameraOrientation(const QVector3D &defaultPosition,
                                            const QVector3D &defaultTarget,
                                            const QVector3D &defaultUp);
    // Calculate view matrix based on rotation and zoom
    QMatrix4x4 calculateViewMatrix(const QPoint &mousePos, int zoom,
                                          int screenWidth, int screenHeight,
                                          bool showUnder = false);
    // Calcluate light position based on rotation. Call after calling calculateViewMatrix to get
    // up-to-date position
    QVector3D calculateLightPosition(const QVector3D &lightPosition,
                                            GLfloat fixedRotation = 0.0f,
                                            GLfloat distanceModifier = 0.0f);
    void updateMousePos(const QPoint &mousePos);
    void setCameraPreset(Q3DCamera::CameraPreset preset);
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
