/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QSCATTERDATAPROXY_P_H
#define QSCATTERDATAPROXY_P_H

#include "qscatterdataproxy.h"
#include "qabstractdataproxy_p.h"
#include "qscatterdataitem.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QScatterDataProxyPrivate : public QAbstractDataProxyPrivate
{
    Q_OBJECT
public:
    QScatterDataProxyPrivate(QScatterDataProxy *q);
    virtual ~QScatterDataProxyPrivate();

    void resetArray(QScatterDataArray *newArray);
    void setItem(int index, const QScatterDataItem &item);
    void setItems(int index, const QScatterDataArray &items);
    int addItem(const QScatterDataItem &item);
    int addItems(const QScatterDataArray &items);
    void insertItem(int index, const QScatterDataItem &item);
    void insertItems(int index, const QScatterDataArray &items);
    void removeItems(int index, int removeCount);

    void limitValues(QVector3D &minValues, QVector3D &maxValues) const;

    virtual void setSeries(QAbstract3DSeries *series);
private:
    QScatterDataProxy *qptr();
    QScatterDataArray *m_dataArray;

    friend class QScatterDataProxy;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
