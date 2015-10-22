/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

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
