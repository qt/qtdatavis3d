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

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DCameraPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DCamera : public Q3DObject
{
    Q_OBJECT
    Q_PROPERTY(qreal xRotation READ xRotation WRITE setXRotation NOTIFY xRotationChanged)
    Q_PROPERTY(qreal yRotation READ yRotation WRITE setYRotation NOTIFY yRotationChanged)
    Q_PROPERTY(qreal minXRotation READ minXRotation NOTIFY minXRotationChanged)
    Q_PROPERTY(qreal minYRotation READ minYRotation NOTIFY minYRotationChanged)
    Q_PROPERTY(qreal maxXRotation READ maxXRotation NOTIFY maxXRotationChanged)
    Q_PROPERTY(qreal maxYRotation READ maxYRotation NOTIFY maxYRotationChanged)
    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QMatrix4x4 viewMatrix READ viewMatrix WRITE setViewMatrix NOTIFY viewMatrixChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::CameraPreset cameraPreset READ cameraPreset WRITE setCameraPreset NOTIFY cameraPresetChanged)
    Q_PROPERTY(bool viewMatrixAutoUpdateEnabled READ isViewMatrixAutoUpdateEnabled WRITE setViewMatrixAutoUpdateEnabled NOTIFY viewMatrixAutoUpdateChanged)
    Q_PROPERTY(bool wrapXRotation READ wrapXRotation WRITE setWrapXRotation NOTIFY wrapXRotationChanged )
    Q_PROPERTY(bool wrapYRotation READ wrapYRotation WRITE setWrapYRotation NOTIFY wrapYRotationChanged )
    Q_ENUMS(QtDataVisualization::QDataVis::CameraPreset)

public:
    Q3DCamera(QObject *parent = 0);
    virtual ~Q3DCamera();

    qreal xRotation() const;
    void setXRotation(qreal rotation);
    qreal yRotation() const;
    void setYRotation(qreal rotation);

    qreal minXRotation() const;
    qreal maxXRotation() const;

    qreal minYRotation() const;
    qreal maxYRotation() const;

    bool wrapXRotation() const;
    void setWrapXRotation(bool isEnabled);

    bool wrapYRotation() const;
    void setWrapYRotation(bool isEnabled);

    void copyValuesFrom(const Q3DCamera &source);

    QMatrix4x4 viewMatrix() const;
    void setViewMatrix(const QMatrix4x4 &viewMatrix);

    bool isViewMatrixAutoUpdateEnabled();
    void setViewMatrixAutoUpdateEnabled(bool isEnabled);

    QDataVis::CameraPreset cameraPreset();
    void setCameraPreset(QDataVis::CameraPreset preset);

    int zoomLevel();
    void setZoomLevel(int zoomLevel);

    void setBaseOrientation(const QVector3D &defaultPosition,
                            const QVector3D &defaultTarget,
                            const QVector3D &defaultUp);

    QVector3D calculatePositionRelativeToCamera(const QVector3D &relativePosition,
                                                qreal fixedRotation,
                                                qreal distanceModifier) const;
    void setCameraPosition(qreal horizontal, qreal vertical, qreal distance = 100.0);

signals:
    void xRotationChanged(qreal rotation);
    void yRotationChanged(qreal rotation);
    void minXRotationChanged(qreal rotation);
    void minYRotationChanged(qreal rotation);
    void maxXRotationChanged(qreal rotation);
    void maxYRotationChanged(qreal rotation);
    void zoomLevelChanged(int zoomLevel);
    void viewMatrixChanged(QMatrix4x4 viewMatrix);
    void cameraPresetChanged(QDataVis::CameraPreset preset);
    void viewMatrixAutoUpdateChanged(bool enabled);
    void wrapXRotationChanged(bool isEnabled);
    void wrapYRotationChanged(bool isEnabled);

protected:
    void setMinXRotation(qreal rotation);
    void setMinYRotation(qreal rotation);
    void setMaxXRotation(qreal rotation);
    void setMaxYRotation(qreal rotation);

private:
    QScopedPointer<Q3DCameraPrivate> d_ptr;

    Q_DISABLE_COPY(Q3DCamera)

    friend class Q3DCameraPrivate;
    friend class Q3DScenePrivate;
    friend class Bars3DRenderer;
    friend class Surface3DRenderer;
    friend class Scatter3DRenderer;
    friend class SelectionPointer;
    friend class Q3DInputHandler;
    friend class QTouch3DInputHandlerPrivate;
    friend class QMac3DInputHandler;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // Q3DCAMERA_H
