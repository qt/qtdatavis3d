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

#ifndef QMAPDATAPROXY_H
#define QMAPDATAPROXY_H

#include <QtDataVis3D/qabstractdataproxy.h>
#include <QtDataVis3D/qmapdataitem.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

typedef QVector<QMapDataItem> QMapDataArray;

class QMapDataProxyPrivate;

class QT_DATAVIS3D_EXPORT QMapDataProxy : public QAbstractDataProxy
{
    Q_OBJECT

public:
    explicit QMapDataProxy();
    explicit QMapDataProxy(QMapDataProxyPrivate *d);
    virtual ~QMapDataProxy();

    // QMapDataProxy is also optimized to use cases where the only defining characteristics of an individual
    // map item are it's value and position. Modifying other data such as color or mesh of individual bar
    // requires allocating additional data object for the bar.

    // If data is accessed from same thread that sets it, access doesn't need to be protected with mutex.
    // Item pointers are guaranteed to be valid only until next call that modifies data.
    // Array pointer is guaranteed to be valid for lifetime of proxy.
    int itemCount() const;
    const QMapDataArray *array() const;
    const QMapDataItem *itemAt(int index) const; // Index needs to exist or this crashes

    // All array/item manipulation functions are internally protected by data mutex.

    // TODO: Should we remove internal mutexing and require user to mutex also on data manipulations?
    // TODO: Upside would be enabling user to mix data setters and getters within same mutex scope.
    // TODO: Downside would be signal emissions from inside the mutex scope, which has potential for deadlock.

    // TODO Should data manipulation/access methods be protected rather than public to enforce subclassing use of proxy?
    // TODO Leaving them public gives user more options.

    // QMapDataProxy takes ownership of all QMapDataArrays and QMapDataItems passed to it.

    // Clears the existing array and sets it data to new array.
    void resetArray(QMapDataArray *newArray);

    // TODO void setItem(int index, QMapDataItem *item);
    // TODO void setItems(int index, QMapDataArray *items);

    // TODO int addItem(QMapDataItem *item); // returns the index of added item
    // TODO int addItems(QMapDataArray *items); // returns the index of added item

    // TODO void insertItem(int index, QMapDataItem *item);
    // TODO void insertItems(int index, QMapDataArray *items);

    // TODO void removeItems(int index, int removeCount);

signals:
    void arrayReset();
    void itemsAdded(int startIndex, int count);
    void itemsChanged(int startIndex, int count);
    void itemsRemoved(int startIndex, int count); // Index may be over current array size if removed from end
    void itemsInserted(int startIndex, int count);

protected:
    QMapDataProxyPrivate *dptr();
    const QMapDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QMapDataProxy)

    friend class Maps3DController;
};

QT_DATAVIS3D_END_NAMESPACE

#endif
