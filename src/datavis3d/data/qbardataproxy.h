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

#ifndef QBARDATAPROXY_H
#define QBARDATAPROXY_H

#include <QtDataVis3D/qabstractdataproxy.h>
#include <QtDataVis3D/qbardataitem.h>
#include <QVector>

QT_DATAVIS3D_BEGIN_NAMESPACE

typedef QVector<QBarDataItem> QBarDataRow;
typedef QList<QBarDataRow *> QBarDataArray;

class QBarDataProxyPrivate;

class QT_DATAVIS3D_EXPORT QBarDataProxy : public QAbstractDataProxy
{
    Q_OBJECT

public:
    explicit QBarDataProxy();
    explicit QBarDataProxy(QBarDataProxyPrivate *d);
    virtual ~QBarDataProxy();

    // BarDataProxy is optimized for adding, inserting, and removing rows of data.
    // Adding a column essentially means modifying every row, which is comparatively very inefficient.
    // Proxy is also optimized to use cases where the only defining characteristic of an individual
    // bar is its value. Modifying other data such as color or label format of individual bar
    // requires allocating additional data object for the bar.

    // If data is accessed from same thread that sets it, access doesn't need to be protected with mutex.
    // Row and item pointers are guaranteed to be valid only until next call that modifies data.
    // Array pointer is guaranteed to be valid for lifetime of proxy.
    int rowCount() const;
    const QBarDataArray *array() const;
    const QBarDataRow *rowAt(int rowIndex) const;
    const QBarDataItem *itemAt(int rowIndex, int columnIndex) const; // Row and column in said row need to exist or this crashes

    // The data array is a list of list (rows) of QBarDataItem instances.
    // Each row can contain different amount of items or even be null.

    // All array/item manipulation functions are internally protected by data mutex.

    // TODO: Should we remove internal mutexing and require user to mutex also on data manipulations?
    // TODO: Upside would be enabling user to mix data setters and getters within same mutex scope.
    // TODO: Downside would be signal emissions from inside the mutex scope, which has potential for deadlock.

    // TODO Should data manipulation/access methods be protected rather than public to enforce subclassing use of proxy?
    // TODO Leaving them public gives user more options.

    // QBarDataProxy takes ownership of all QBarDataArrays, QBarDataRows, and QBarDataItems
    // passed to it. The pointers passed to it are not guaranteed to be valid after the calls
    // and should not be used to modify data further.

    // Clears the existing array and sets it data to new array.
    void resetArray(QBarDataArray *newArray);

    // Change existing rows
    void setRow(int rowIndex, QBarDataRow *row);
    void setRows(int rowIndex, QBarDataArray *rows);

    // Setting a column is comparatively inefficient as it changes all rows.
    // Can resize rows that are shorter than columnIndex.
    // TODO void setColumn(int columnIndex, QBarDataRow *column);
    // TODO void setColumns(int columnIndex, QBarDataArray *columns);

    // Change single item
    void setItem(int rowIndex, int columnIndex, QBarDataItem *item);
    // Change block of items
    // TODO setItems(int rowIndex, int columnIndex, QBarDataArray *items);

    int addRow(QBarDataRow *row); // returns the index of added row
    int addRows(QBarDataArray *rows); // returns the index of first added row
    // TODO int addColumn(QBarDataRow *column); // returns the index of the added column
    // TODO int addColumns(QBarDataArray *columns); // returns the index of the first added column

    // If rowIndex is equal to array size, rows are added to end of the array.
    void insertRow(int rowIndex, QBarDataRow *row);
    void insertRows(int rowIndex, QBarDataArray *rows);
    // TODO void insertColumn(int columnIndex, QBarDataRow *column);
    // TODO void insertColumns(int columnIndex, QBarDataArray *columns);

    // Attempting to remove rows past the end of the array does nothing.
    void removeRows(int rowIndex, int removeCount);
    // TODO void removeColumns(int columnIndex, int removeCount);

signals:
    void arrayReset();
    void rowsAdded(int startIndex, int count);
    void rowsChanged(int startIndex, int count);
    // Index is the current array size if rows were removed from the end of the array
    void rowsRemoved(int startIndex, int count);
    void rowsInserted(int startIndex, int count);
    // TODO void columnsChanged(int startIndex, int count);
    // TODO void columnsAdded(int startIndex, int count);
    // TODO void columnsRemoved(int startIndex, int count);
    // TODO void columnsInserted(int startIndex, int count);
    void itemChanged(int rowIndex, int columnIndex);
    // TODO void itemsChanged(int rowIndex, int columnIndex, int rowCount, int columnCount);

protected:
    QBarDataProxyPrivate *dptr();
    const QBarDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QBarDataProxy)

    friend class Bars3dController;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QBARDATAPROXY_H
