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

#include "qolddataproxy.h"
#include "qdataset_p.h"
#include "qdatarow_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

QOldDataProxy::QOldDataProxy() :
    QBarDataProxy(),
    m_dataSet(new QDataSet())
{
}

QOldDataProxy::~QOldDataProxy()
{
}

void QOldDataProxy::addDataRow(const QVector<float> &dataRow)
{
    // Convert to QDataRow and add to QDataSet
    QDataRow *row = new QDataRow();
    for (int i = 0; i < dataRow.size(); i++)
        row->addItem(new QDataItem(dataRow.at(i)));
    //row->d_ptr->verifySize(columnCount());
    m_dataSet->addRow(row);
    //m_dataSet->d_ptr->verifySize(rowCount());
    addProxyRow(row);
}

void QOldDataProxy::addDataRow(const QVector<QDataItem*> &dataRow)
{
    // Convert to QDataRow and add to QDataSet
    QDataRow *row = new QDataRow();
    for (int i = 0; i < dataRow.size(); i++)
        row->addItem(dataRow.at(i));
    //row->d_ptr->verifySize(columnCount());
    m_dataSet->addRow(row);
    //m_dataSet->d_ptr->verifySize(rowCount());
    addProxyRow(row);
}

void QOldDataProxy::addDataRow(QDataRow *dataRow)
{
    // Check that the input data fits into sample space, and resize if it doesn't
    //dataRow->d_ptr->verifySize(columnCount());
    // With each new row, the previous data row must be moved back
    // ie. we need as many vectors as we have rows in the sample space
    m_dataSet->addRow(dataRow);
    // if the added data pushed us over sample space, remove the oldest data set
    //m_dataSet->d_ptr->verifySize(rowCount());
    addProxyRow(dataRow);
}

void QOldDataProxy::addDataSet(const QVector< QVector<float> > &data)
{
    // Delete old data set
    delete m_dataSet;
    m_dataSet = new QDataSet();

    // Convert to QDataRow and add to QDataSet
    QDataRow *row;
    for (int rowNr = 0; rowNr < data.size(); rowNr++) {
        row = new QDataRow();
        for (int colNr = 0; colNr < data.at(rowNr).size(); colNr++)
            row->addItem(new QDataItem(data.at(rowNr).at(colNr)));
        //row->d_ptr->verifySize(columnCount());
        m_dataSet->addRow(row);
        row++;
    }
    //m_dataSet->d_ptr->verifySize(rowCount());
    addProxySet(m_dataSet);
}

void QOldDataProxy::addDataSet(const QVector< QVector<QDataItem*> > &data)
{
    // Delete old data set
    delete m_dataSet;
    m_dataSet = new QDataSet();

    // Convert to QDataRow and add to QDataSet
    QDataRow *row;
    for (int rowNr = 0; rowNr < data.size(); rowNr++) {
        row = new QDataRow();
        for (int colNr = 0; colNr < data.at(rowNr).size(); colNr++)
            row->addItem(data.at(rowNr).at(colNr));
        //row->d_ptr->verifySize(columnCount());
        m_dataSet->addRow(row);
        row++;
    }
    //m_dataSet->d_ptr->verifySize(rowCount());
    addProxySet(m_dataSet);
}

void QOldDataProxy::addDataSet(QDataSet* dataSet)
{
    // Check sizes
    //dataSet->d_ptr->verifySize(rowCount(), columnCount());

    // Delete old data set
    delete m_dataSet;
    // Take ownership of given set
    m_dataSet = dataSet;

    addProxySet(m_dataSet);
}

void QOldDataProxy::addProxyRow(QDataRow *dataRow)
{
    QBarDataRow *newProxyRow = new QBarDataRow;
    for (int i = 0; i < dataRow->d_ptr->m_row.size(); i++) {
        QDataItem *oldItem = dataRow->d_ptr->getItem(i);
        QBarDataItem *newItem = new QBarDataItem;
        newItem->setValue(oldItem->value());
        newProxyRow->append(newItem);
    }

    QBarDataArray *newProxyArray = new QBarDataArray;
    newProxyArray->append(newProxyRow);
    insertRows(0, newProxyArray);
}

void QOldDataProxy::addProxySet(QDataSet *dataSet)
{
    QBarDataArray *newProxyArray = new QBarDataArray;

    QVector<QDataRow *> oldSet = dataSet->d_ptr->set();
    for (int i = 0; i < oldSet.size(); i++) {
        QBarDataRow *newProxyRow = new QBarDataRow;
        QDataRow *oldRow = oldSet.at(i);
        for (int j = 0; j < oldRow->d_ptr->m_row.size(); j++) {
            QDataItem *oldItem = oldRow->d_ptr->getItem(j);
            QBarDataItem *newItem = new QBarDataItem;
            newItem->setValue(oldItem->value());
            newProxyRow->append(newItem);
        }
        newProxyArray->append(newProxyRow);
    }

    resetArray(rowCount(), columnCount());
    addRows(newProxyArray);
}

QT_DATAVIS3D_END_NAMESPACE
