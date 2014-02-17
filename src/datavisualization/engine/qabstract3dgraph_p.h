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

#ifndef QABSTRACT3DGRAPH_P_H
#define QABSTRACT3DGRAPH_P_H

#include "datavisualizationglobal_p.h"

class QOpenGLContext;
class QOpenGLPaintDevice;

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
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
