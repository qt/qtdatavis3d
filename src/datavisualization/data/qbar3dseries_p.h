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

#ifndef QBAR3DSERIES_P_H
#define QBAR3DSERIES_P_H

#include "qbar3dseries.h"
#include "qabstract3dseries_p.h"

namespace QtDataVisualization {

class QBar3DSeriesPrivate : public QAbstract3DSeriesPrivate
{
    Q_OBJECT
public:
    QBar3DSeriesPrivate(QBar3DSeries *q);
    virtual ~QBar3DSeriesPrivate();

    virtual void setDataProxy(QAbstractDataProxy *proxy);
    virtual void connectControllerAndProxy(Abstract3DController *newController);

    void setSelectedBar(const QPoint &position);

private:
    QBar3DSeries *qptr();

    QPoint m_selectedBar;

private:
    friend class QBar3DSeries;
};

}

#endif
