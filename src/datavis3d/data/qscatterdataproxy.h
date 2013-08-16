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

public:
    explicit QScatterDataProxy();
    explicit QScatterDataProxy(QScatterDataProxyPrivate *d);
    virtual ~QScatterDataProxy();

    // QScatterDataProxy is optimized to use cases where the only defining characteristics of an
    // individual scatter item are it's position and size. Modifying other data that might be
    // added in the future such as color requires allocating additional data object for the bar.

    // Item pointers are guaranteed to be valid only until next call that modifies data.
    // Array pointer is guaranteed to be valid for lifetime of proxy.
    int itemCount() const;
    const QScatterDataArray *array() const;
    const QScatterDataItem *itemAt(int index) const;

    // Clears the existing array and takes ownership of the new array.
    // Passing null array clears all data.
    void resetArray(QScatterDataArray *newArray);

    // Change existing items
    void setItem(int index, const QScatterDataItem &item);
    void setItems(int index, const QScatterDataArray &items);

    int addItem(const QScatterDataItem &item); // returns the index of added item
    int addItems(const QScatterDataArray &items); // returns the index of first added item

    // If index is equal to data array size, item(s) are added to the array.
    void insertItem(int index, const QScatterDataItem &item);
    void insertItems(int index, const QScatterDataArray &items);

    // Attempting to remove items past the end of the array does nothing.
    void removeItems(int index, int removeCount);

signals:
    void arrayReset();
    void itemsAdded(int startIndex, int count);
    void itemsChanged(int startIndex, int count);
    void itemsRemoved(int startIndex, int count); // Index may be over current array size if removed from end
    void itemsInserted(int startIndex, int count);

protected:
    QScatterDataProxyPrivate *dptr();
    const QScatterDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QScatterDataProxy)

    friend class Scatter3DController;
};

QT_DATAVIS3D_END_NAMESPACE

#endif
