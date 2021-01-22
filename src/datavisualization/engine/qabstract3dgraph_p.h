/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
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

QT_BEGIN_NAMESPACE
class QOpenGLContext;
class QOffscreenSurface;
QT_END_NAMESPACE

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

public Q_SLOTS:
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
    bool m_initialized;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
