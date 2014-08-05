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

#ifndef Q3DSCENE_P_H
#define Q3DSCENE_P_H

#include "datavisualizationglobal_p.h"
#include "q3dscene.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DCamera;
class Q3DLight;

struct Q3DSceneChangeBitField {
    bool viewportChanged               : 1;
    bool primarySubViewportChanged     : 1;
    bool secondarySubViewportChanged   : 1;
    bool subViewportOrderChanged       : 1;
    bool cameraChanged                 : 1;
    bool lightChanged                  : 1;
    bool slicingActivatedChanged       : 1;
    bool devicePixelRatioChanged       : 1;
    bool selectionQueryPositionChanged : 1;
    bool windowSizeChanged             : 1;

    Q3DSceneChangeBitField()
        : viewportChanged(true),
          primarySubViewportChanged(true),
          secondarySubViewportChanged(true),
          subViewportOrderChanged(true),
          cameraChanged(true),
          lightChanged(true),
          slicingActivatedChanged(true),
          devicePixelRatioChanged(true),
          selectionQueryPositionChanged(false),
          windowSizeChanged(true)
    {
    }
};

class QT_DATAVISUALIZATION_EXPORT Q3DScenePrivate : public QObject
{
    Q_OBJECT
public:
    Q3DScenePrivate(Q3DScene *q);
    ~Q3DScenePrivate();

    void sync(Q3DScenePrivate &other);

    void setViewport(const QRect &viewport);
    void setViewportSize(int width, int height);
    void setWindowSize(const QSize &size);
    QSize windowSize() const;
    void calculateSubViewports();
    void updateGLViewport();
    void updateGLSubViewports();

    QRect glViewport();
    QRect glPrimarySubViewport();
    QRect glSecondarySubViewport();

    void setLightPositionRelativeToCamera(const QVector3D &relativePosition,
                                          float fixedRotation = 0.0f,
                                          float distanceModifier = 0.0f);

    void markDirty();

signals:
    void needRender();

public:
    Q3DScene *q_ptr;
    Q3DSceneChangeBitField m_changeTracker;

    QRect m_viewport;
    QRect m_primarySubViewport;
    QRect m_secondarySubViewport;
    bool m_isSecondarySubviewOnTop;
    float m_devicePixelRatio;
    Q3DCamera *m_camera;
    Q3DLight *m_light;
    bool m_isUnderSideCameraEnabled;
    bool m_isSlicingActive;
    QPoint m_selectionQueryPosition;
    QSize m_windowSize;
    QRect m_glViewport;
    QRect m_glPrimarySubViewport;
    QRect m_glSecondarySubViewport;
    bool m_sceneDirty;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
