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

#include "qbardataproxy.h"
#include "qbardataproxy_p.h"
#include "qabstractdataitem_p.h"
#include <QMutexLocker>

QT_DATAVIS3D_BEGIN_NAMESPACE

QBarDataProxy::QBarDataProxy() :
    QAbstractDataProxy(new QBarDataProxyPrivate(this))
{
}

QBarDataProxy::~QBarDataProxy()
{
}

void QBarDataProxy::resetArray(int rowCount, int columnCount)
{
    resetArray(0, rowCount, columnCount);
}

void QBarDataProxy::resetArray(QBarDataArray *newArray, int rowCount, int columnCount)
{
    dptr()->resetArray(newArray, rowCount, columnCount);
    emit arrayReset();
}

void QBarDataProxy::resetArray(QBarDataArray *newArray)
{
    int rowCount(newArray->size());
    int columnCount(0);

    if (rowCount)
        columnCount = newArray->at(0)->size();

    resetArray(newArray, rowCount, columnCount);
}

void QBarDataProxy::setRow(int rowIndex, QBarDataRow *row)
{
    dptr()->setRow(rowIndex, row);
    emit rowsChanged(rowIndex, 1);
}

int QBarDataProxy::addRow(QBarDataRow *row)
{
    int addIndex = dptr()->addRow(row);
    emit rowsAdded(addIndex, 1);
    return addIndex;
}

int QBarDataProxy::addRows(QBarDataArray *rows)
{
    int addCount = rows->size();
    int addIndex = dptr()->addRows(rows); // deletes original rows array
    emit rowsAdded(addIndex, addCount);
    return addIndex;
}

void QBarDataProxy::insertRow(int rowIndex, QBarDataRow *row)
{
    dptr()->insertRow(rowIndex, row);
    emit rowsInserted(rowIndex, 1);
}

void QBarDataProxy::insertRows(int rowIndex, QBarDataArray *rows)
{
    int insertCount = rows->size();
    dptr()->insertRows(rowIndex, rows); // deletes original rows array
    emit rowsInserted(rowIndex, insertCount);
}

void QBarDataProxy::setRowCount(int count)
{
    QMutexLocker locker(mutex());
    int oldCount = dptr()->m_rowCount;

    dptr()->setRowCount(count);

    locker.unlock();

    if (oldCount < count)
        emit rowsAdded(oldCount, count - oldCount);
    else if (oldCount > count)
        emit rowsRemoved(count, oldCount - count);
}

void QBarDataProxy::setColumnCount(int count)
{
    QMutexLocker locker(mutex());
    dptr()->setColumnCount(count);
    // TODO emit columnsAdded/columnsRemoved
}

// Mutexing data accessors should be done by user
int QBarDataProxy::rowCount()
{
    return dptr()->m_rowCount;
}

int QBarDataProxy::columnCount()
{
    return dptr()->m_columnCount;
}

const QBarDataArray &QBarDataProxy::array() const
{
    return dptrc()->m_dataArray;
}

const QBarDataRow *QBarDataProxy::rowAt(int rowIndex) const
{
    return dptrc()->m_dataArray[rowIndex];
}

QBarDataItem *QBarDataProxy::itemAt(int rowIndex, int columnIndex)
{
    return dptr()->m_dataArray[rowIndex]->at(columnIndex);
}

QBarDataProxyPrivate *QBarDataProxy::dptr()
{
    return static_cast<QBarDataProxyPrivate *>(d_ptr.data());
}

const QBarDataProxyPrivate *QBarDataProxy::dptrc() const
{
    return static_cast<const QBarDataProxyPrivate *>(d_ptr.data());
}

// QBarDataProxyPrivate

QBarDataProxyPrivate::QBarDataProxyPrivate(QBarDataProxy *q)
    : QAbstractDataProxyPrivate(q, QAbstractDataProxy::DataTypeBar),
      m_rowCount(0),
      m_columnCount(0)
{
}

QBarDataProxyPrivate::~QBarDataProxyPrivate()
{
    clearArray();
}

void QBarDataProxyPrivate::resetArray(QBarDataArray *newArray, int rowCount, int columnCount)
{
    QMutexLocker locker(&m_mutex);
    clearArray();

    m_rowCount = rowCount;
    m_columnCount = columnCount;
    if (newArray) {
        m_dataArray = *newArray;
        delete newArray;
    }

    int oldSize = m_dataArray.size();

    if (oldSize < m_rowCount) {
        m_dataArray.reserve(m_rowCount);
        for (int i = oldSize; i < m_rowCount; i++)
            m_dataArray.append(0);
    } else if (oldSize > m_rowCount) {
        for (int i = oldSize - 1; i >= m_rowCount; i--) {
            clearRow(i);
            m_dataArray.removeLast();
        }
    }

    for (int i = 0; i < m_dataArray.size(); i++)
        fixRow(m_dataArray.at(i));
}

void QBarDataProxyPrivate::setRow(int rowIndex, QBarDataRow *row)
{
    QMutexLocker locker(&m_mutex);
    clearRow(rowIndex);
    fixRow(row);
    m_dataArray[rowIndex] = row;
}

int QBarDataProxyPrivate::addRow(QBarDataRow *row)
{
    QMutexLocker locker(&m_mutex);
    int currentSize = m_rowCount;
    // Init column count to row size if not yet explicitly set
    if (!m_columnCount)
        m_columnCount = row->size();
    fixRow(row);
    m_dataArray.append(row);
    m_rowCount++;
    return currentSize;
}

int QBarDataProxyPrivate::addRows(QBarDataArray *rows)
{
    QMutexLocker locker(&m_mutex);
    int currentSize = m_rowCount;
    // Init column count to first row size if not yet explicitly set
    if (!m_columnCount && rows->size())
        m_columnCount = rows->at(0)->size();
    for (int i = 0; i < rows->size(); i++) {
        fixRow(rows->at(i));
        m_dataArray.append(rows->at(i));
    }
    m_rowCount += rows->size();
    delete rows;
    return currentSize;
}

void QBarDataProxyPrivate::insertRow(int rowIndex, QBarDataRow *row)
{
    QMutexLocker locker(&m_mutex);
    // Init column count to row size if not yet explicitly set
    if (!m_columnCount)
        m_columnCount = row->size();
    fixRow(row);
    m_dataArray.insert(rowIndex, row);
    m_rowCount++;
}

void QBarDataProxyPrivate::insertRows(int rowIndex, QBarDataArray *rows)
{
    QMutexLocker locker(&m_mutex);
    // Init column count to first row size if not yet explicitly set
    if (!m_columnCount && rows->size())
        m_columnCount = rows->at(0)->size();
    for (int i = 0; i < rows->size(); i++) {
        fixRow(rows->at(i));
        m_dataArray.insert(rowIndex, rows->at(i));
    }
    m_rowCount += rows->size();
    delete rows;
}

void QBarDataProxyPrivate::setRowCount(int count)
{
    // Locked already in public implementatation to avoid multiple lockings per call
    if (m_rowCount != count) {
        if (count < m_columnCount) {
            for (int i = m_columnCount - 1; i >= count; i--) {
                clearRow(i);
                m_dataArray.removeLast();
            }
        } else if (count > m_columnCount) {
            for (int i = m_columnCount; i < count; i++)
                m_dataArray.append(0);
        }
        m_rowCount = count;
    }
}

void QBarDataProxyPrivate::setColumnCount(int count)
{
    QMutexLocker locker(&m_mutex);
    if (m_columnCount != count) {
        m_columnCount = count;
        for (int i = 0; i < m_rowCount; i++)
            fixRow(m_dataArray.at(i));
    }
}

// Protected & private functions. Do not mutex as these are used from mutexed functions.

void QBarDataProxyPrivate::clearRow(int rowIndex)
{
    if (m_dataArray[rowIndex]) {
        for (int i = 0; i < m_columnCount; i++)
            delete m_dataArray[rowIndex]->at(i);
        delete m_dataArray[rowIndex];
        m_dataArray[rowIndex] = 0;
    }
}

void QBarDataProxyPrivate::clearArray()
{
    for (int i = 0; i < m_rowCount; i++)
        clearRow(i);
    m_dataArray.clear();
}

void QBarDataProxyPrivate::fixRow(QBarDataRow *row)
{
    if (!row)
        return;

    int rowSize = row->size();
    if (rowSize < m_columnCount) {
        for (int i = rowSize; i < m_columnCount; i++)
            row->append(new QBarDataItem);
    } else if (rowSize > m_columnCount) {
        for (int i = m_columnCount; i < rowSize; i++)
            delete row->takeLast();
    }
    for (int i = 0; i < m_columnCount; i++) {
        if (row->at(i))
            row->at(i)->d_ptr->setDataProxy(q_ptr);
    }
}

QPair<GLfloat, GLfloat> QBarDataProxyPrivate::limitValues(int startRow, int endRow, int startColumn, int endColumn)
{
    QMutexLocker locker(&m_mutex);
    QPair<GLfloat, GLfloat> limits = qMakePair(100.0f, -100.0f);
    endRow = qMin(endRow, m_rowCount - 1);
    endColumn = qMin(endColumn, m_columnCount - 1);
    for (int i = startRow; i <= endRow; i++) {
        if (!m_dataArray.at(i))
            continue;
        for (int j = startColumn; j <= endColumn; j++) {
            QBarDataItem *item = m_dataArray.at(i)->at(j);
            if (item) {
                float itemValue = item->value();
                if (limits.second < itemValue)
                    limits.second = itemValue;
                if (limits.first > itemValue)
                    limits.first = itemValue;
            }
        }
    }
    return limits;
}

QT_DATAVIS3D_END_NAMESPACE
