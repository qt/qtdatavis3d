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
#include <QMutexLocker>

QT_DATAVIS3D_BEGIN_NAMESPACE

QBarDataProxy::QBarDataProxy() :
    QAbstractDataProxy(new QBarDataProxyPrivate(this))
{
}

QBarDataProxy::QBarDataProxy(QBarDataProxyPrivate *d) :
    QAbstractDataProxy(d)
{
}

QBarDataProxy::~QBarDataProxy()
{
}

void QBarDataProxy::resetArray(QBarDataArray *newArray)
{
    if (dptr()->resetArray(newArray))
        emit arrayReset();
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

// Mutexing data accessors should be done by user
int QBarDataProxy::rowCount()
{
    return dptrc()->m_dataArray.size();
}

const QBarDataArray &QBarDataProxy::array() const
{
    return dptrc()->m_dataArray;
}

const QBarDataRow *QBarDataProxy::rowAt(int rowIndex) const
{
    return dptrc()->m_dataArray[rowIndex];
}

const QBarDataItem *QBarDataProxy::itemAt(int rowIndex, int columnIndex) const
{
    return &dptrc()->m_dataArray[rowIndex]->at(columnIndex);
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
    : QAbstractDataProxyPrivate(q, QAbstractDataProxy::DataTypeBar)
{
}

QBarDataProxyPrivate::~QBarDataProxyPrivate()
{
    clearArray();
}

bool QBarDataProxyPrivate::resetArray(QBarDataArray *newArray)
{
    QMutexLocker locker(&m_mutex);

    if (!m_dataArray.size() && (!newArray || !newArray->size()))
        return false;

    clearArray();

    if (newArray) {
        m_dataArray = *newArray;
        delete newArray;
    }

    return true;
}

void QBarDataProxyPrivate::setRow(int rowIndex, QBarDataRow *row)
{
    QMutexLocker locker(&m_mutex);
    clearRow(rowIndex);
    m_dataArray[rowIndex] = row;
}

int QBarDataProxyPrivate::addRow(QBarDataRow *row)
{
    QMutexLocker locker(&m_mutex);
    int currentSize = m_dataArray.size();
    m_dataArray.append(row);
    return currentSize;
}

int QBarDataProxyPrivate::addRows(QBarDataArray *rows)
{
    QMutexLocker locker(&m_mutex);
    int currentSize = m_dataArray.size();
    for (int i = 0; i < rows->size(); i++)
        m_dataArray.append(rows->at(i));
    delete rows;
    return currentSize;
}

void QBarDataProxyPrivate::insertRow(int rowIndex, QBarDataRow *row)
{
    QMutexLocker locker(&m_mutex);
    m_dataArray.insert(rowIndex, row);
}

void QBarDataProxyPrivate::insertRows(int rowIndex, QBarDataArray *rows)
{
    QMutexLocker locker(&m_mutex);
    for (int i = 0; i < rows->size(); i++)
        m_dataArray.insert(rowIndex, rows->at(i));
    delete rows;
}

// Protected & private functions. Do not mutex as these are used from mutexed functions.

void QBarDataProxyPrivate::clearRow(int rowIndex)
{
    if (m_dataArray[rowIndex]) {
        delete m_dataArray[rowIndex];
        m_dataArray[rowIndex] = 0;
    }
}

void QBarDataProxyPrivate::clearArray()
{
    for (int i = 0; i < m_dataArray.size(); i++)
        clearRow(i);
    m_dataArray.clear();
}

QPair<GLfloat, GLfloat> QBarDataProxyPrivate::limitValues(int startRow, int endRow, int startColumn, int endColumn)
{
    QMutexLocker locker(&m_mutex);
    QPair<GLfloat, GLfloat> limits = qMakePair(100.0f, -100.0f);
    endRow = qMin(endRow, m_dataArray.size() - 1);
    for (int i = startRow; i <= endRow; i++) {
        QBarDataRow *row = m_dataArray.at(i);
        if (row) {
            endColumn = qMin(endColumn, row->size() - 1);
            for (int j = startColumn; j <= endColumn; j++) {
                const QBarDataItem &item = m_dataArray.at(i)->at(j);
                qreal itemValue = item.value();
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
