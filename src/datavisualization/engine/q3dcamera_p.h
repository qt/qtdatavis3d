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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q3DCAMERA_P_H
#define Q3DCAMERA_P_H

#include "datavisualizationglobal_p.h"
#include "q3dcamera.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DCamera;

class Q3DCameraPrivate : public QObject
{
    Q_OBJECT
public:
    Q3DCameraPrivate(Q3DCamera *q);
    ~Q3DCameraPrivate();

    void sync(Q3DCamera &other);

    void setXRotation(float rotation);
    void setYRotation(float rotation);
    void setMinXRotation(float rotation);
    float minXRotation() const;
    void setMinYRotation(float rotation);
    float minYRotation() const;
    void setMaxXRotation(float rotation);
    float maxXRotation() const;
    void setMaxYRotation(float rotation);
    float maxYRotation() const;

    void updateViewMatrix(float zoomAdjustment);

    QMatrix4x4 viewMatrix() const;
    void setViewMatrix(const QMatrix4x4 &viewMatrix);

    bool isViewMatrixAutoUpdateEnabled() const;
    void setViewMatrixAutoUpdateEnabled(bool isEnabled);

    void setBaseOrientation(const QVector3D &defaultPosition,
                            const QVector3D &defaultTarget,
                            const QVector3D &defaultUp);

    QVector3D calculatePositionRelativeToCamera(const QVector3D &relativePosition,
                                                float fixedRotation,
                                                float distanceModifier) const;

signals:
    void minXRotationChanged(float rotation);
    void minYRotationChanged(float rotation);
    void maxXRotationChanged(float rotation);
    void maxYRotationChanged(float rotation);
    void viewMatrixChanged(QMatrix4x4 viewMatrix);
    void viewMatrixAutoUpdateChanged(bool enabled);

public:
    Q3DCamera *q_ptr;

    QVector3D m_target;
    QVector3D m_up;

    QMatrix4x4 m_viewMatrix;
    bool m_isViewMatrixUpdateActive;

    GLfloat m_xRotation;
    GLfloat m_yRotation;
    GLfloat m_minXRotation;
    GLfloat m_minYRotation;
    GLfloat m_maxXRotation;
    GLfloat m_maxYRotation;
    bool m_wrapXRotation;
    bool m_wrapYRotation;
    int m_zoomLevel;
    Q3DCamera::CameraPreset m_activePreset;

    friend class Bars3DRenderer;
    friend class Surface3DRenderer;
    friend class Scatter3DRenderer;
    friend class SelectionPointer;
    friend class Q3DInputHandler;
    friend class QTouch3DInputHandler;
    friend class QMac3DInputHandler;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
