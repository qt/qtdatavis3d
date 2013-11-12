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

#ifndef Q3DSCENE_P_H
#define Q3DSCENE_P_H

#include "datavisualizationglobal_p.h"
#include <QRect>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DCamera;
class Q3DLight;
class Q3DScene;

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

    Q3DSceneChangeBitField()
        : viewportChanged(true),
          primarySubViewportChanged(true),
          secondarySubViewportChanged(true),
          subViewportOrderChanged(true),
          cameraChanged(true),
          lightChanged(true),
          slicingActivatedChanged(true),
          devicePixelRatioChanged(true),
          selectionQueryPositionChanged(false)
    {
    }
};

class Q3DScenePrivate
{
public:
    Q3DScenePrivate(Q3DScene *q);
    ~Q3DScenePrivate();

    void sync(Q3DScenePrivate &other);

    Q3DScene *q_ptr;
    Q3DSceneChangeBitField m_changeTracker;

    QRect m_viewport;
    QRect m_primarySubViewport;
    QRect m_secondarySubViewport;
    bool m_isSecondarySubviewOnTop;
    qreal m_devicePixelRatio;
    Q3DCamera *m_camera;
    Q3DLight *m_light;
    bool m_isUnderSideCameraEnabled;
    bool m_isSlicingActive;
    QPoint m_selectionQueryPosition;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // Q3DSCENE_P_H
