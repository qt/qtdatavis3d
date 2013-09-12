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

#ifndef Q3DCAMERA_H
#define Q3DCAMERA_H

#include <QtDataVisualization/q3dobject.h>
#include <QMatrix4x4>

class QVector3D;
class QPoint;
class QPointF;

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DCameraPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DCamera : public Q3DObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF rotations READ rotations WRITE setRotations)
    Q_PROPERTY(QMatrix4x4 viewMatrix READ viewMatrix WRITE setViewMatrix)
    Q_PROPERTY(QtDataVisualization::QDataVis::CameraPreset cameraPreset READ cameraPreset WRITE setCameraPreset)
    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel)


public:
    Q3DCamera(QObject *parent = 0);
    virtual ~Q3DCamera();

    void copyValuesFrom(const Q3DCamera &source);

    // Set camera rotation in degrees
    virtual void setRotations(const QPointF &rotation);
    // Get camera rotations
    virtual QPointF rotations() const;

    virtual void setViewMatrix(const QMatrix4x4 &viewMatrix);
    virtual QMatrix4x4 viewMatrix() const;

    // Set default camera orientation. Position's x and y should be 0.
    virtual void setDefaultOrientation(const QVector3D &defaultPosition,
                                       const QVector3D &defaultTarget,
                                       const QVector3D &defaultUp);

    // Calculate view matrix based on zoomadjustment, current rotation and current zoom level
    virtual void updateViewMatrix(qreal zoomAdjustment);

    virtual void setCameraPreset(QDataVis::CameraPreset preset);
    virtual QDataVis::CameraPreset cameraPreset();

    virtual void setZoomLevel(int zoomLevel);
    virtual int zoomLevel();

    virtual QVector3D calculatePositionRelativeToCamera(const QVector3D &relativePosition,
                                                        qreal fixedRotation,
                                                        qreal distanceModifier) const;

private:
    QScopedPointer<Q3DCameraPrivate> d_ptr;

    Q_DISABLE_COPY(Q3DCamera)

    friend class Q3DCameraPrivate;
    friend class Q3DScenePrivate;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // Q3DCAMERA_H
