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

#ifndef Q3DCAMERA_H
#define Q3DCAMERA_H

#include <QtDataVisualization/q3dobject.h>
#include <QMatrix4x4>

class QVector3D;
class QPoint;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DCameraPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DCamera : public Q3DObject
{
    Q_OBJECT
    Q_ENUMS(CameraPreset)
    Q_PROPERTY(float xRotation READ xRotation WRITE setXRotation NOTIFY xRotationChanged)
    Q_PROPERTY(float yRotation READ yRotation WRITE setYRotation NOTIFY yRotationChanged)
    Q_PROPERTY(float minXRotation READ minXRotation NOTIFY minXRotationChanged)
    Q_PROPERTY(float minYRotation READ minYRotation NOTIFY minYRotationChanged)
    Q_PROPERTY(float maxXRotation READ maxXRotation NOTIFY maxXRotationChanged)
    Q_PROPERTY(float maxYRotation READ maxYRotation NOTIFY maxYRotationChanged)
    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QMatrix4x4 viewMatrix READ viewMatrix WRITE setViewMatrix NOTIFY viewMatrixChanged)
    Q_PROPERTY(CameraPreset cameraPreset READ cameraPreset WRITE setCameraPreset NOTIFY cameraPresetChanged)
    Q_PROPERTY(bool viewMatrixAutoUpdateEnabled READ isViewMatrixAutoUpdateEnabled WRITE setViewMatrixAutoUpdateEnabled NOTIFY viewMatrixAutoUpdateChanged)
    Q_PROPERTY(bool wrapXRotation READ wrapXRotation WRITE setWrapXRotation NOTIFY wrapXRotationChanged)
    Q_PROPERTY(bool wrapYRotation READ wrapYRotation WRITE setWrapYRotation NOTIFY wrapYRotationChanged)

public:
    enum CameraPreset {
        CameraPresetNone = -1,
        CameraPresetFrontLow = 0,
        CameraPresetFront,
        CameraPresetFrontHigh,
        CameraPresetLeftLow,
        CameraPresetLeft,
        CameraPresetLeftHigh,
        CameraPresetRightLow,
        CameraPresetRight,
        CameraPresetRightHigh,
        CameraPresetBehindLow,
        CameraPresetBehind,
        CameraPresetBehindHigh,
        CameraPresetIsometricLeft,
        CameraPresetIsometricLeftHigh,
        CameraPresetIsometricRight,
        CameraPresetIsometricRightHigh,
        CameraPresetDirectlyAbove,
        CameraPresetDirectlyAboveCW45,
        CameraPresetDirectlyAboveCCW45,
        CameraPresetFrontBelow,
        CameraPresetLeftBelow,
        CameraPresetRightBelow,
        CameraPresetBehindBelow,
        CameraPresetDirectlyBelow
    };

public:
    Q3DCamera(QObject *parent = 0);
    virtual ~Q3DCamera();

    float xRotation() const;
    void setXRotation(float rotation);
    float yRotation() const;
    void setYRotation(float rotation);

    float minXRotation() const;
    float maxXRotation() const;

    float minYRotation() const;
    float maxYRotation() const;

    bool wrapXRotation() const;
    void setWrapXRotation(bool isEnabled);

    bool wrapYRotation() const;
    void setWrapYRotation(bool isEnabled);

    virtual void copyValuesFrom(const Q3DObject &source);

    QMatrix4x4 viewMatrix() const;
    void setViewMatrix(const QMatrix4x4 &viewMatrix);

    bool isViewMatrixAutoUpdateEnabled() const;
    void setViewMatrixAutoUpdateEnabled(bool isEnabled);

    CameraPreset cameraPreset() const;
    void setCameraPreset(CameraPreset preset);

    int zoomLevel() const;
    void setZoomLevel(int zoomLevel);

    Q_INVOKABLE void setBaseOrientation(const QVector3D &defaultPosition,
                                        const QVector3D &defaultTarget,
                                        const QVector3D &defaultUp);

    QVector3D calculatePositionRelativeToCamera(const QVector3D &relativePosition,
                                                float fixedRotation,
                                                float distanceModifier) const;
    void setCameraPosition(float horizontal, float vertical, float distance = 100.0f);

signals:
    void xRotationChanged(float rotation);
    void yRotationChanged(float rotation);
    void minXRotationChanged(float rotation);
    void minYRotationChanged(float rotation);
    void maxXRotationChanged(float rotation);
    void maxYRotationChanged(float rotation);
    void zoomLevelChanged(int zoomLevel);
    void viewMatrixChanged(QMatrix4x4 viewMatrix);
    void cameraPresetChanged(CameraPreset preset);
    void viewMatrixAutoUpdateChanged(bool enabled);
    void wrapXRotationChanged(bool isEnabled);
    void wrapYRotationChanged(bool isEnabled);

private:
    void setMinXRotation(float rotation);
    void setMinYRotation(float rotation);
    void setMaxXRotation(float rotation);
    void setMaxYRotation(float rotation);

private:
    QScopedPointer<Q3DCameraPrivate> d_ptr;

    Q_DISABLE_COPY(Q3DCamera)

    friend class Q3DCameraPrivate;
    friend class Q3DScenePrivate;
    friend class Abstract3DRenderer;
    friend class Bars3DRenderer;
    friend class Surface3DRenderer;
    friend class Scatter3DRenderer;
    friend class SelectionPointer;
    friend class Q3DInputHandler;
    friend class QTouch3DInputHandlerPrivate;
    friend class QMac3DInputHandler;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
