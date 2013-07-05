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

#include "qvariantbardataproxy_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

QVariantBarDataProxy::QVariantBarDataProxy() :
    QBarDataProxy(new QVariantBarDataProxyPrivate(this))
{
}

QVariantBarDataProxy::~QVariantBarDataProxy()
{
}

void QVariantBarDataProxy::setDataSet(QVariantDataSet *newSet)
{
    dptr()->setDataSet(newSet);
}

void QVariantBarDataProxy::setMappings(const QVariantBarMapping &mappings)
{
    dptr()->setMappings(mappings);
}


QVariantBarDataProxyPrivate *QVariantBarDataProxy::dptr()
{
    return static_cast<QVariantBarDataProxyPrivate *>(d_ptr.data());
}

// QVariantBarDataProxyPrivate

QVariantBarDataProxyPrivate::QVariantBarDataProxyPrivate(QVariantBarDataProxy *q)
    : QBarDataProxyPrivate(q),
      m_dataSet(new QVariantDataSet)
{
    connectDataSet();
}

QVariantBarDataProxyPrivate::~QVariantBarDataProxyPrivate()
{
    delete m_dataSet;
}

void QVariantBarDataProxyPrivate::setDataSet(QVariantDataSet *newSet)
{
    delete m_dataSet;
    m_dataSet = newSet;
    connectDataSet();
    resolveDataSet();
}

void QVariantBarDataProxyPrivate::setMappings(const QVariantBarMapping &mappings)
{
    m_mappings = mappings;
    resolveDataSet();
}

void QVariantBarDataProxyPrivate::handleItemsAdded(int index, int count)
{
    // Resolve new item
    // TODO
}

void QVariantBarDataProxyPrivate::handleDataCleared()
{
    // Data cleared, reset array
    qptr()->resetArray(0);
}

void QVariantBarDataProxyPrivate::connectDataSet()
{
    QObject::connect(m_dataSet, &QVariantDataSet::itemsAdded, this, &QVariantBarDataProxyPrivate::handleItemsAdded);
    QObject::connect(m_dataSet, &QVariantDataSet::dataCleared, this, &QVariantBarDataProxyPrivate::handleDataCleared);
}

// Resolve entire dataset into QBarDataArray.
void QVariantBarDataProxyPrivate::resolveDataSet()
{
    if (!m_mappings.size()) {
        qptr()->resetArray(0);
        return;
    }
    const QVariantDataItemList &itemList = m_dataSet->itemList();
    int totalCount = itemList.size();

    int rowIndex = m_mappings[QVariantBarMappingItem::MapRow].itemIndex;
    int columnIndex = m_mappings[QVariantBarMappingItem::MapColumn].itemIndex;
    int valueIndex = m_mappings[QVariantBarMappingItem::MapValue].itemIndex;
    QStringList rowList = m_mappings[QVariantBarMappingItem::MapRow].itemCategories;
    QStringList columnList = m_mappings[QVariantBarMappingItem::MapColumn].itemCategories;

    // Sort values into rows and columns
    typedef QHash<QString, qreal> ColumnValueMap;
    QHash <QString, ColumnValueMap> itemValueMap;
    foreach (const QVariantDataItem *item, itemList)
        itemValueMap[item->at(rowIndex).toString()][item->at(columnIndex).toString()] = item->at(valueIndex).toReal();

    // Create new data array from itemValueMap
    QBarDataArray *newProxyArray = new QBarDataArray;
    foreach (QString rowKey, rowList) {
        QBarDataRow *newProxyRow = new QBarDataRow(columnList.size());
        for (int i = 0; i < columnList.size(); i++)
            (*newProxyRow)[i].setValue(itemValueMap[rowKey][columnList.at(i)]);
        newProxyArray->append(newProxyRow);
    }

    qDebug() << __FUNCTION__ << "totalCount:" << totalCount << "RowCount:" << newProxyArray->size() << "Column count:" << newProxyArray->at(0)->size();

    qptr()->resetArray(newProxyArray);
}

QVariantBarDataProxy *QVariantBarDataProxyPrivate::qptr()
{
    return static_cast<QVariantBarDataProxy *>(q_ptr);
}

QT_DATAVIS3D_END_NAMESPACE
