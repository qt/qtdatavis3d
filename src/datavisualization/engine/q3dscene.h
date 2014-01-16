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

#include <QtDataVisualization/qdatavisualizationglobal.h>

#include <QObject>
#include <QRect>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DCamera;
class Q3DBox;
class Q3DLight;
class Q3DScenePrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DScene : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRect viewport READ viewport NOTIFY viewportChanged)
    Q_PROPERTY(QRect primarySubViewport READ primarySubViewport WRITE setPrimarySubViewport NOTIFY primarySubViewportChanged)
    Q_PROPERTY(QRect secondarySubViewport READ secondarySubViewport WRITE setSecondarySubViewport NOTIFY secondarySubViewportChanged)
    Q_PROPERTY(QPoint selectionQueryPosition READ selectionQueryPosition WRITE setSelectionQueryPosition NOTIFY selectionQueryPositionChanged)
    Q_PROPERTY(bool secondarySubviewOnTop READ isSecondarySubviewOnTop  WRITE setSecondarySubviewOnTop  NOTIFY secondarySubviewOnTopChanged)
    Q_PROPERTY(bool slicingActive READ isSlicingActive WRITE setSlicingActive NOTIFY slicingActiveChanged)
    Q_PROPERTY(Q3DCamera* activeCamera READ activeCamera WRITE setActiveCamera NOTIFY activeCameraChanged)
    Q_PROPERTY(Q3DLight* activeLight READ activeLight WRITE setActiveLight NOTIFY activeLightChanged)
    Q_PROPERTY(float devicePixelRatio READ devicePixelRatio WRITE setDevicePixelRatio NOTIFY devicePixelRatioChanged)

public:
    Q3DScene(QObject *parent = 0);
    ~Q3DScene();

    QRect viewport() const;

    QRect primarySubViewport() const;
    void setPrimarySubViewport(const QRect &primarySubViewport);
    bool isPointInPrimarySubView(const QPoint &point);

    QRect secondarySubViewport() const;
    void setSecondarySubViewport(const QRect &secondarySubViewport);
    bool isPointInSecondarySubView(const QPoint &point);

    void setSelectionQueryPosition(const QPoint &point);
    QPoint selectionQueryPosition() const;
    static QPoint invalidSelectionPoint();

    void setSlicingActive(bool isSlicing);
    bool isSlicingActive() const;

    void setSecondarySubviewOnTop(bool isSecondaryOnTop);
    bool isSecondarySubviewOnTop() const;

    Q3DCamera *activeCamera() const;
    void setActiveCamera(Q3DCamera *camera);

    Q3DLight *activeLight() const;
    void setActiveLight(Q3DLight *light);

    float devicePixelRatio() const;
    void setDevicePixelRatio(float pixelRatio);

    Q_INVOKABLE void setLightPositionRelativeToCamera(const QVector3D &relativePosition,
                                                      float fixedRotation = 0.0f,
                                                      float distanceModifier = 0.0f);
signals:
    void viewportChanged(QRect viewport);
    void primarySubViewportChanged(QRect subViewport);
    void secondarySubViewportChanged(QRect subViewport);
    void secondarySubviewOnTopChanged(bool isSecondaryOnTop);
    void slicingActiveChanged(bool isSlicingActive);
    void activeCameraChanged(const Q3DCamera *camera);
    void activeLightChanged(const Q3DLight *light);
    void devicePixelRatioChanged(float pixelRatio);
    void selectionQueryPositionChanged(const QPoint position);

private:
    QScopedPointer<Q3DScenePrivate> d_ptr;

    Q_DISABLE_COPY(Q3DScene)

    friend class AbstractDeclarative;
    friend class QAbstract3DGraph;
    friend class Abstract3DController;
    friend class Q3DScenePrivate;
    friend class Abstract3DRenderer;
    friend class Bars3DRenderer;
    friend class Surface3DRenderer;
    friend class Scatter3DRenderer;
    friend class Q3DCameraPrivate;
    friend class Q3DObject;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif // Q3DSCENE_H
