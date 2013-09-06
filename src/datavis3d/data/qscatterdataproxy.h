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

#ifndef QSCATTERDATAPROXY_H
#define QSCATTERDATAPROXY_H

#include <QtDataVis3D/qabstractdataproxy.h>
#include <QtDataVis3D/qscatterdataitem.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

typedef QVector<QScatterDataItem> QScatterDataArray;

class QScatterDataProxyPrivate;

class QT_DATAVIS3D_EXPORT QScatterDataProxy : public QAbstractDataProxy
{
    Q_OBJECT

    Q_PROPERTY(int itemCount READ itemCount)
public:
    explicit QScatterDataProxy(QObject *parent = 0);
    virtual ~QScatterDataProxy();

    // TODO: Replace first part of class description in docs with this once all TODOs are done:
    /*
    * QScatterDataProxy handles adding, inserting, changing and removing data items.
    * QScatterDataProxy is optimized to use cases where the only defining characteristics of an
    * individual scatter item are it's position and size. Modifying other data that might be
    * added in the future such as color requires allocating additional data object for the bar.
    */

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

signals:
    void arrayReset();
    void itemsAdded(int startIndex, int count);
    void itemsChanged(int startIndex, int count);
    void itemsRemoved(int startIndex, int count);
    void itemsInserted(int startIndex, int count);

protected:
    explicit QScatterDataProxy(QScatterDataProxyPrivate *d, QObject *parent = 0);
    QScatterDataProxyPrivate *dptr();
    const QScatterDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QScatterDataProxy)

    friend class Scatter3DController;
};

QT_DATAVIS3D_END_NAMESPACE

#endif
