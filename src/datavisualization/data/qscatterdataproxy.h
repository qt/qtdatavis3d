/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
******************************************************************************/

#ifndef QSCATTERDATAPROXY_H
#define QSCATTERDATAPROXY_H

#include <QtDataVisualization/qabstractdataproxy.h>
#include <QtDataVisualization/qscatterdataitem.h>

Q_MOC_INCLUDE(<QtDataVisualization/qscatter3dseries.h>)

QT_BEGIN_NAMESPACE

class QScatterDataProxyPrivate;
class QScatter3DSeries;

typedef QList<QScatterDataItem> QScatterDataArray;

class QT_DATAVISUALIZATION_EXPORT QScatterDataProxy : public QAbstractDataProxy
{
    Q_OBJECT

    Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)
    Q_PROPERTY(QScatter3DSeries *series READ series NOTIFY seriesChanged)

public:
    explicit QScatterDataProxy(QObject *parent = nullptr);
    virtual ~QScatterDataProxy();

    QScatter3DSeries *series() const;
    int itemCount() const;
    const QScatterDataArray *array() const;
    const QScatterDataItem *itemAt(int index) const;

    void resetArray(QScatterDataArray *newArray);

    void setItem(int index, const QScatterDataItem &item);
    void setItems(int index, const QScatterDataArray &items);

    int addItem(const QScatterDataItem &item);
    int addItems(const QScatterDataArray &items);

    void insertItem(int index, const QScatterDataItem &item);
    void insertItems(int index, const QScatterDataArray &items);

    void removeItems(int index, int removeCount);

Q_SIGNALS:
    void arrayReset();
    void itemsAdded(int startIndex, int count);
    void itemsChanged(int startIndex, int count);
    void itemsRemoved(int startIndex, int count);
    void itemsInserted(int startIndex, int count);

    void itemCountChanged(int count);
    void seriesChanged(QScatter3DSeries *series);

protected:
    explicit QScatterDataProxy(QScatterDataProxyPrivate *d, QObject *parent = nullptr);
    QScatterDataProxyPrivate *dptr();
    const QScatterDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QScatterDataProxy)

    friend class Scatter3DController;
};

QT_END_NAMESPACE

#endif
