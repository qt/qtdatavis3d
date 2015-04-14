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

#ifndef QBAR3DSERIES_P_H
#define QBAR3DSERIES_P_H

#include "qbar3dseries.h"
#include "qabstract3dseries_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QBar3DSeriesPrivate : public QAbstract3DSeriesPrivate
{
    Q_OBJECT
public:
    QBar3DSeriesPrivate(QBar3DSeries *q);
    virtual ~QBar3DSeriesPrivate();

    virtual void setDataProxy(QAbstractDataProxy *proxy);
    virtual void connectControllerAndProxy(Abstract3DController *newController);
    virtual void createItemLabel();

    void handleMeshRotationChanged(const QQuaternion &rotation);

    void setSelectedBar(const QPoint &position);

    void connectSignals();

private:
    QBar3DSeries *qptr();

    QPoint m_selectedBar;

private:
    friend class QBar3DSeries;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
