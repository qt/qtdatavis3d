/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
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

#ifndef QABSTRACT3DGRAPH_P_H
#define QABSTRACT3DGRAPH_P_H

#include "datavisualizationglobal_p.h"

class QOpenGLContext;
class QOffscreenSurface;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QAbstract3DGraph;
class Abstract3DController;
class QAbstract3DAxis;

class QAbstract3DGraphPrivate : public QObject
{
    Q_OBJECT
public:
    QAbstract3DGraphPrivate(QAbstract3DGraph *q);
    ~QAbstract3DGraphPrivate();

    void setVisualController(Abstract3DController *controller);
    void handleDevicePixelRatioChange();

    void render();

    QImage renderToImage(int msaaSamples, const QSize &imageSize);

public slots:
    void renderLater();
    void renderNow();

    virtual void handleAxisXChanged(QAbstract3DAxis *axis) = 0;
    virtual void handleAxisYChanged(QAbstract3DAxis *axis) = 0;
    virtual void handleAxisZChanged(QAbstract3DAxis *axis) = 0;

public:
    QAbstract3DGraph *q_ptr;

    bool m_updatePending;
    QOpenGLContext *m_context;
    Abstract3DController *m_visualController;
    float m_devicePixelRatio;
    QOffscreenSurface *m_offscreenSurface;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
