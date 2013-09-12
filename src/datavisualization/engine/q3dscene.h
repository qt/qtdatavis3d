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

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DCamera;
class Q3DBox;
class Q3DLight;
class Q3DScenePrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DScene : public QObject
{
    Q_OBJECT

public:
    Q3DScene(QObject *parent = 0);
    ~Q3DScene();

    QRect viewport() const;
    void setViewport(const QRect &viewport);
    void setViewportSize(int width, int height);

    QRect mainViewport() const;
    void setMainViewport(const QRect &mainViewport);
    bool isInputInsideMainView(const QPoint &point);

    QRect sliceViewport() const;
    void setSliceViewport(const QRect &sliceViewport);
    bool isInputInsideSliceView(const QPoint &point);

    Q3DCamera *camera() const;
    void setCamera(Q3DCamera *camera);

    Q3DLight *light() const;
    void setLight(Q3DLight *light);

    bool isUnderSideCameraEnabled() const;
    void setUnderSideCameraEnabled(bool isEnabled);

    void setSlicingActivated(bool isSlicing);
    bool isSlicingActivated() const;

    // Calcluate light position based on rotation.
    // Call after calling calculateViewMatrix to get up-to-date position
    void setLightPositionRelativeToCamera(const QVector3D &relativePosition,
                                          qreal fixedRotation = 0.0,
                                          qreal distanceModifier = 0.0);

private:
    QScopedPointer<Q3DScenePrivate> d_ptr;

    Q_DISABLE_COPY(Q3DScene)

    friend class Abstract3DRenderer;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // Q3DSCENE_H
