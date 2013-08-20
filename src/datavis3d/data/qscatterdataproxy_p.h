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

#ifndef QSCATTERDATAPROXY_P_H
#define QSCATTERDATAPROXY_P_H

#include "qscatterdataproxy.h"
#include "qabstractdataproxy_p.h"
#include "qscatterdataitem.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class QScatterDataProxyPrivate : public QAbstractDataProxyPrivate
{
    Q_OBJECT
public:
    QScatterDataProxyPrivate(QScatterDataProxy *q);
    virtual ~QScatterDataProxyPrivate();

    bool resetArray(QScatterDataArray *newArray);
    void setItem(int index, const QScatterDataItem &item);
    void setItems(int index, const QScatterDataArray &items);
    int addItem(const QScatterDataItem &item);
    int addItems(const QScatterDataArray &items);
    void insertItem(int index, const QScatterDataItem &item);
    void insertItems(int index, const QScatterDataArray &items);
    void removeItems(int index, int removeCount);

    QVector3D limitValues();

private:
    QScatterDataArray *m_dataArray;

    friend class QScatterDataProxy;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QBARDATAPROXY_P_H
