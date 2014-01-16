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

#ifndef Q3DBARS_P_H
#define Q3DBARS_P_H

#include "bars3dcontroller_p.h"
#include "qabstract3dgraph_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DBars;

class Q3DBarsPrivate : public QAbstract3DGraphPrivate
{
    Q_OBJECT
public:
    Q3DBarsPrivate(Q3DBars *q);
    ~Q3DBarsPrivate();

    void handleAxisXChanged(QAbstract3DAxis *axis);
    void handleAxisYChanged(QAbstract3DAxis *axis);
    void handleAxisZChanged(QAbstract3DAxis *axis);

    Q3DBars *qptr();

    Bars3DController *m_shared;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
