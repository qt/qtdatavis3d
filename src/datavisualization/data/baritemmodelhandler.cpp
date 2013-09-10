/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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

#include "baritemmodelhandler_p.h"
#include <QTimer>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

BarItemModelHandler::BarItemModelHandler(QItemModelBarDataProxy *proxy, QObject *parent)
    : AbstractItemModelHandler(parent),
      m_proxy(proxy)
{
}

BarItemModelHandler::~BarItemModelHandler()
{
}

// Resolve entire item model into QBarDataArray.
void BarItemModelHandler::resolveModel()
{
    QItemModelBarDataMapping *mapping = static_cast<QItemModelBarDataMapping *>(m_activeMapping);
    if (m_itemModel.isNull() || !mapping) {
        m_proxy->resetArray(0);
        return;
    }

    bool useModelRows(false);
    if (!mapping->rowCategories().size() || !mapping->columnCategories().size()) {
        useModelRows = true;
    } else if (mapping->rowRole().isEmpty() || mapping->columnRole().isEmpty()) {
        m_proxy->resetArray(0);
        return;
    }

    QBarDataArray *newProxyArray = new QBarDataArray;
    QHash<int, QByteArray> roleHash = m_itemModel->roleNames();
    // Default to display role if no mapping
    int valueRole = roleHash.key(mapping->valueRole().toLatin1(), Qt::DisplayRole);
    int rowCount = m_itemModel->rowCount();
    int columnCount = m_itemModel->columnCount();

    if (useModelRows) {
        for (int i = 0; i < rowCount; i++) {
            QBarDataRow *newProxyRow = new QBarDataRow(columnCount);
            for (int j = 0; j < columnCount; j++)
                (*newProxyRow)[j].setValue(m_itemModel->index(i, j).data(valueRole).toReal());
            newProxyArray->append(newProxyRow);
        }
    } else {
        int rowRole = roleHash.key(mapping->rowRole().toLatin1());
        int columnRole = roleHash.key(mapping->columnRole().toLatin1());
        const QStringList &rowList = mapping->rowCategories();
        const QStringList &columnList = mapping->columnCategories();

        // Sort values into rows and columns
        typedef QHash<QString, qreal> ColumnValueMap;
        QHash <QString, ColumnValueMap> itemValueMap;
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < columnCount; j++) {
                QModelIndex index = m_itemModel->index(i, j);
                itemValueMap[index.data(rowRole).toString()][index.data(columnRole).toString()]
                        = index.data(valueRole).toReal();
            }
        }

        // Create new data array from itemValueMap
        foreach (QString rowKey, rowList) {
            QBarDataRow *newProxyRow = new QBarDataRow(columnList.size());
            for (int i = 0; i < columnList.size(); i++)
                (*newProxyRow)[i].setValue(itemValueMap[rowKey][columnList.at(i)]);
            newProxyArray->append(newProxyRow);
        }
    }

    // Generate labels from headers if using model rows/columns
    QStringList rowLabels;
    QStringList columnLabels;
    if (useModelRows) {
        for (int i = 0; i < rowCount; i++)
            rowLabels << m_itemModel->headerData(i, Qt::Vertical).toString();
        for (int i = 0; i < columnCount; i++)
            columnLabels << m_itemModel->headerData(i, Qt::Horizontal).toString();
    }

    m_proxy->resetArray(newProxyArray, rowLabels, columnLabels);
}

QT_DATAVISUALIZATION_END_NAMESPACE
