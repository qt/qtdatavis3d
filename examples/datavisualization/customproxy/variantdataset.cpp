/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
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

#include "variantdataset.h"

VariantDataSet::VariantDataSet()
    : QObject(0)
{
}

VariantDataSet::~VariantDataSet()
{
    clear();
}

void VariantDataSet::clear()
{
    foreach (VariantDataItem *item, m_variantData) {
        item->clear();
        delete item;
    }
    m_variantData.clear();
    emit dataCleared();
}

int VariantDataSet::addItem(VariantDataItem *item)
{
    m_variantData.append(item);
    int addIndex =  m_variantData.size();

    emit itemsAdded(addIndex, 1);
    return addIndex;
}

int VariantDataSet::addItems(VariantDataItemList *itemList)
{
    int newCount = itemList->size();
    int addIndex = m_variantData.size();
    m_variantData.append(*itemList);
    delete itemList;
    emit itemsAdded(addIndex, newCount);
    return addIndex;
}

const VariantDataItemList &VariantDataSet::itemList() const
{
    return m_variantData;
}
