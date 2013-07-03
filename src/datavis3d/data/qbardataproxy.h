/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBARDATAPROXY_H
#define QBARDATAPROXY_H

#include "qabstractdataproxy.h"
#include "qbardataitem.h"
#include <QVector>

QT_DATAVIS3D_BEGIN_NAMESPACE

// TODO: Would QVector or mix of QList and QVector be a better choice?
// TODO: How about not using item pointers but directly allocating items here?
typedef QList<QBarDataItem *> QBarDataRow;
typedef QList<QBarDataRow *> QBarDataArray;

class QBarDataProxyPrivate;

class QT_DATAVIS3D_EXPORT QBarDataProxy : public QAbstractDataProxy
{
    Q_OBJECT

public:
    explicit QBarDataProxy();
    virtual ~QBarDataProxy();

    // The data array is a list of list (rows) of QBarDataItem instances.
    // Each row can contains exactly the column count items, empty items are added if
    // adding too short rows or removed if adding too long rows.

    // All array/item manipulation functions are internally protected by data mutex.

    // TODO: Should we remove internal mutexing and require user to mutex also on data manipulations?
    // TODO: That would lead to signal emissions from inside the mutex, potential for deadlock?

    // TODO Should data manipulation/access methods be protected to enforce subclassing use of proxy?
    // TODO Leaving them public gives user more options.

    // Anyone accessing the data needs to protect the access with QAbstractDataProxy::mutex().
    const QBarDataArray &array() const;
    const QBarDataRow *rowAt(int rowIndex) const;
    QBarDataItem *itemAt(int rowIndex, int columnIndex);

    int rowCount();
    void setRowCount(int count);
    int columnCount();
    void setColumnCount(int count);

    // QBarDataProxy takes ownership of all QBarDataArrays, QBarDataRows, and QBarDataItems passed to it.

    // Clears the array and reserves specified amount of new empty rows.
    void resetArray(int rowCount, int columnCount);
    // TODO void resetArray(QBarDataArray *newArray);
    // TODO void resetRow(int rowIndex); // Clear a row
    // TODO void resetColumn(int columnIndex);
    // TODO void resetItem(int rowIndex, int columnIndex); // deletes the item

    void setRow(int rowIndex, QBarDataRow *row);
    // TODO void setColumn(int columnIndex, QBarDataRow *column);
    // TODO void setItem(int rowIndex, int columnIndex, QBarDataItem *item);

    // TODO int addRow(QBarDataRow *row); // returns the index of added row
    int addRows(QBarDataArray *rows); // returns the index of first added row
    // TODO int addColumns(QBarDataArray *columns); // returns the index of first added column

    // TODO void insertRow(int rowIndex, QBarDataRow *row);
    void insertRows(int rowIndex, QBarDataArray *rows);
    // TODO void insertColumns(int columnIndex, QBarDataArray *columns);

    // TODO void removeRows(int rowIndex, int removeCount);
    // TODO void removeColumns(int columnIndex, int removeCount);


    // TODO void setRowCount(); // Deletes excess if smaller, creates empties if larger
    // TODO void setColumnCount();


signals:
    void arrayReset();
    void rowsAdded(int startIndex, int count);
    void rowsChanged(int startIndex, int count);
    void rowsRemoved(int startIndex, int count); // Index may be over current array size if removed from end
    void rowsInserted(int startIndex, int count);
    // TODO void itemChanged(int rowIndex, int columnIndex);

protected:
    QBarDataProxyPrivate *dptr();
    const QBarDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QBarDataProxy)

    friend class Bars3dController;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QBARDATAPROXY_H
