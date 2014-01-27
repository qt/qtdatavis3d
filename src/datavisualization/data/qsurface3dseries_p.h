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

#ifndef QSURFACE3DSERIES_P_H
#define QSURFACE3DSERIES_P_H

#include "qsurface3dseries.h"
#include "qabstract3dseries_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QSurface3DSeriesPrivate : public QAbstract3DSeriesPrivate
{
    Q_OBJECT
public:
    QSurface3DSeriesPrivate(QSurface3DSeries *q);
    virtual ~QSurface3DSeriesPrivate();

    virtual void setDataProxy(QAbstractDataProxy *proxy);
    virtual void connectControllerAndProxy(Abstract3DController *newController);

    void setSelectedPoint(const QPoint &position);
    void setFlatShadingEnabled(bool enabled);
    void setDrawMode(QSurface3DSeries::DrawFlags mode);

private:
    QSurface3DSeries *qptr();

    QPoint m_selectedPoint;
    bool m_flatShadingEnabled;
    QSurface3DSeries::DrawFlags m_drawMode;

private:
    friend class QSurface3DSeries;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
