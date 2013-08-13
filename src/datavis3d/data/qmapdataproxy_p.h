/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QMAPDATAPROXY_P_H
#define QMAPDATAPROXY_P_H

#include "qmapdataproxy.h"
#include "qabstractdataproxy_p.h"
#include "qmapdataitem.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class QMapDataProxyPrivate : public QAbstractDataProxyPrivate
{
    Q_OBJECT
public:
    QMapDataProxyPrivate(QMapDataProxy *q);
    virtual ~QMapDataProxyPrivate();

    bool resetArray(QMapDataArray *newArray);

    QPair<GLfloat, GLfloat> limitValues();

private:
    QMapDataArray m_dataArray;

private:
    friend class QMapDataProxy;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QBARDATAPROXY_P_H
