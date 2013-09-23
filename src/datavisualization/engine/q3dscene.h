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

#ifndef Q3DSCENE_H
#define Q3DSCENE_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QObject>
#include <QRect>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DCamera;
class Q3DBox;
class Q3DLight;
class Q3DScenePrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DScene : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRect viewport READ viewport WRITE setViewport)
    Q_PROPERTY(QRect primarySubViewport READ primarySubViewport WRITE setPrimarySubViewport)
    Q_PROPERTY(QRect secondarySubViewport READ secondarySubViewport WRITE setSecondarySubViewport)
    Q_PROPERTY(bool slicingActive READ isSlicingActive WRITE setSlicingActive)
    Q_PROPERTY(Q3DCamera* activeCamera READ activeCamera WRITE setActiveCamera)
    Q_PROPERTY(Q3DLight* activeLight READ activeLight WRITE setActiveLight)
    Q_PROPERTY(qreal devicePixelRatio READ devicePixelRatio WRITE setDevicePixelRatio)

public:
    Q3DScene(QObject *parent = 0);
    ~Q3DScene();

    QRect viewport() const;
    void setViewport(const QRect &viewport);
    void setViewportSize(int width, int height);

    QRect primarySubViewport() const;
    void setPrimarySubViewport(const QRect &primarySubViewport);
    bool isPointInPrimarySubView(const QPoint &point);

    QRect secondarySubViewport() const;
    void setSecondarySubViewport(const QRect &secondarySubViewport);
    bool isPointInSecondarySubView(const QPoint &point);

    void setSlicingActive(bool isSlicing);
    bool isSlicingActive() const;

    Q3DCamera *activeCamera() const;
    void setActiveCamera(Q3DCamera *camera);

    Q3DLight *activeLight() const;
    void setActiveLight(Q3DLight *light);

    qreal devicePixelRatio() const;
    void setDevicePixelRatio(qreal pixelRatio);

    void setLightPositionRelativeToCamera(const QVector3D &relativePosition,
                                          qreal fixedRotation = 0.0,
                                          qreal distanceModifier = 0.0);

private:
    bool isUnderSideCameraEnabled() const;
    void setUnderSideCameraEnabled(bool isEnabled);

    QScopedPointer<Q3DScenePrivate> d_ptr;

    Q_DISABLE_COPY(Q3DScene)

    friend class Q3DScenePrivate;
    friend class Abstract3DRenderer;
    friend class Bars3DRenderer;
    friend class Surface3DRenderer;
    friend class Scatter3DRenderer;
    friend class Q3DCameraPrivate;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // Q3DSCENE_H
