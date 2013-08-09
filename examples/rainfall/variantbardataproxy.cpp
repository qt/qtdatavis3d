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

#include "variantbardataproxy.h"

using namespace QtDataVis3D;

VariantBarDataProxy::VariantBarDataProxy() :
    QBarDataProxy()
{
}

VariantBarDataProxy::VariantBarDataProxy(VariantDataSet *newSet,
                                         VariantBarDataMapping *mapping) :
    QBarDataProxy()
{
    setDataSet(newSet);
    setMapping(mapping);
}

VariantBarDataProxy::~VariantBarDataProxy()
{
}

void VariantBarDataProxy::setDataSet(VariantDataSet *newSet)
{
    if (!m_dataSet.isNull())
        QObject::disconnect(m_dataSet.data(), 0, this, 0);

    m_dataSet = newSet;

    if (!m_dataSet.isNull()) {
        QObject::connect(m_dataSet.data(), &VariantDataSet::itemsAdded, this, &VariantBarDataProxy::handleItemsAdded);
        QObject::connect(m_dataSet.data(), &VariantDataSet::dataCleared, this, &VariantBarDataProxy::handleDataCleared);
    }
    resolveDataSet();
}

VariantDataSet *VariantBarDataProxy::dataSet()
{
    return m_dataSet.data();
}

void VariantBarDataProxy::setMapping(VariantBarDataMapping *mapping)
{
    if (!m_mapping.isNull())
        QObject::disconnect(m_mapping.data(), &VariantBarDataMapping::mappingChanged, this, &VariantBarDataProxy::handleMappingChanged);

    m_mapping = mapping;

    if (!m_mapping.isNull())
        QObject::connect(m_mapping.data(), &VariantBarDataMapping::mappingChanged, this, &VariantBarDataProxy::handleMappingChanged);

    resolveDataSet();
}

VariantBarDataMapping *VariantBarDataProxy::mapping()
{
    return m_mapping.data();
}

void VariantBarDataProxy::handleItemsAdded(int index, int count)
{
    Q_UNUSED(index)
    Q_UNUSED(count)

    // Resolve new items
    resolveDataSet(); // TODO Resolving entire dataset is inefficient
}

void VariantBarDataProxy::handleDataCleared()
{
    // Data cleared, reset array
    resetArray(0);
}

void VariantBarDataProxy::handleMappingChanged()
{
    resolveDataSet();
}

// Resolve entire dataset into QBarDataArray.
void VariantBarDataProxy::resolveDataSet()
{
    if (m_dataSet.isNull() || m_mapping.isNull() || !m_mapping->rowCategories().size() || !m_mapping->columnCategories().size()) {
        resetArray(0);
        return;
    }
    const VariantDataItemList &itemList = m_dataSet->itemList();

    int rowIndex = m_mapping->rowIndex();
    int columnIndex = m_mapping->columnIndex();
    int valueIndex = m_mapping->valueIndex();
    const QStringList &rowList = m_mapping->rowCategories();
    const QStringList &columnList = m_mapping->columnCategories();

    // Sort values into rows and columns
    typedef QHash<QString, qreal> ColumnValueMap;
    QHash <QString, ColumnValueMap> itemValueMap;
    foreach (const VariantDataItem *item, itemList)
        itemValueMap[item->at(rowIndex).toString()][item->at(columnIndex).toString()] = item->at(valueIndex).toReal();

    // Create new data array from itemValueMap
    QBarDataArray *newProxyArray = new QBarDataArray;
    foreach (QString rowKey, rowList) {
        QBarDataRow *newProxyRow = new QBarDataRow(columnList.size());
        for (int i = 0; i < columnList.size(); i++)
            (*newProxyRow)[i].setValue(itemValueMap[rowKey][columnList.at(i)]);
        newProxyArray->append(newProxyRow);
    }

    resetArray(newProxyArray);
}
