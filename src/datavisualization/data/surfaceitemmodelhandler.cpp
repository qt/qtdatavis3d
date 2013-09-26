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

#include "surfaceitemmodelhandler_p.h"
#include "qitemmodelsurfacedatamapping_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

SurfaceItemModelHandler::SurfaceItemModelHandler(QItemModelSurfaceDataProxy *proxy, QObject *parent)
    : AbstractItemModelHandler(parent),
      m_proxy(proxy),
      m_proxyArray(0)
{
}

SurfaceItemModelHandler::~SurfaceItemModelHandler()
{
}

// Resolve entire item model into QSurfaceDataArray.
void SurfaceItemModelHandler::resolveModel()
{
    QItemModelSurfaceDataMapping *mapping = static_cast<QItemModelSurfaceDataMapping *>(m_activeMapping);
    if (m_itemModel.isNull() || !mapping) {
        m_proxy->resetArray(0);
        return;
    }

    if (!mapping->useModelCategories()
            && (mapping->rowRole().isEmpty() || mapping->columnRole().isEmpty())) {
        m_proxy->resetArray(0);
        return;
    }

    float minRowValue = 0.0f;
    float maxRowValue = 1.0f;
    float minColumnValue = 0.0f;
    float maxColumnValue = 1.0f;

    QHash<int, QByteArray> roleHash = m_itemModel->roleNames();

    // Default to display role if no mapping
    int valueRole = roleHash.key(mapping->valueRole().toLatin1(), Qt::DisplayRole);
    int rowCount = m_itemModel->rowCount();
    int columnCount = m_itemModel->columnCount();

    if (mapping->useModelCategories()) {
        // If dimensions have changed, recreate the array
        if (!m_proxyArray || columnCount != m_proxy->columnCount()
                || rowCount != m_proxyArray->size()) {
            m_proxyArray = new QSurfaceDataArray;
            m_proxyArray->reserve(rowCount);
            for (int i = 0; i < rowCount; i++) {
                QSurfaceDataRow *newProxyRow = new QSurfaceDataRow(columnCount);
                m_proxyArray->append(newProxyRow);
            }
        }
        for (int i = 0; i < rowCount; i++) {
            QSurfaceDataRow &newProxyRow = *m_proxyArray->at(i);
            for (int j = 0; j < columnCount; j++)
                newProxyRow[j] = m_itemModel->index(i, j).data(valueRole).toReal();
        }
        if (rowCount) {
            minRowValue = m_itemModel->headerData(0, Qt::Vertical).toFloat();
            maxRowValue = m_itemModel->headerData(rowCount - 1, Qt::Vertical).toFloat();
        }
        if (columnCount) {
            minColumnValue = m_itemModel->headerData(0, Qt::Horizontal).toFloat();
            maxColumnValue = m_itemModel->headerData(columnCount - 1, Qt::Horizontal).toFloat();
        }
    } else {
        int rowRole = roleHash.key(mapping->rowRole().toLatin1());
        int columnRole = roleHash.key(mapping->columnRole().toLatin1());

        bool generateRows = mapping->autoRowCategories();
        bool generateColumns = mapping->autoColumnCategories();

        QStringList rowList;
        QStringList columnList;
        // For detecting duplicates in categories generation, using QHashes should be faster than
        // simple QStringList::contains() check.
        QHash<QString, bool> rowListHash;
        QHash<QString, bool> columnListHash;

        // Sort values into rows and columns
        typedef QHash<QString, qreal> ColumnValueMap;
        QHash <QString, ColumnValueMap> itemValueMap;
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < columnCount; j++) {
                QModelIndex index = m_itemModel->index(i, j);
                QString rowRoleStr = index.data(rowRole).toString();
                QString columnRoleStr = index.data(columnRole).toString();
                itemValueMap[rowRoleStr][columnRoleStr] = index.data(valueRole).toReal();
                if (generateRows && !rowListHash.value(rowRoleStr, false)) {
                    rowListHash.insert(rowRoleStr, true);
                    rowList << rowRoleStr;
                }
                if (generateColumns && !columnListHash.value(columnRoleStr, false)) {
                    columnListHash.insert(columnRoleStr, true);
                    columnList << columnRoleStr;
                }
            }
        }

        if (generateRows)
            mapping->dptr()->m_rowCategories = rowList;
        else
            rowList = mapping->rowCategories();

        if (generateColumns)
            mapping->dptr()->m_columnCategories = columnList;
        else
            columnList = mapping->columnCategories();

        // If dimensions have changed, recreate the array
        if (!m_proxyArray || columnList.size() != m_proxy->columnCount()
                || rowList.size() != m_proxyArray->size()) {
            m_proxyArray = new QSurfaceDataArray;
            m_proxyArray->reserve(rowList.size());
            for (int i = 0; i < rowList.size(); i++) {
                QSurfaceDataRow *newProxyRow = new QSurfaceDataRow(columnList.size());
                m_proxyArray->append(newProxyRow);
            }
        }
        // Create data array from itemValueMap
        for (int i = 0; i < rowList.size(); i++) {
            QString rowKey = rowList.at(i);
            QSurfaceDataRow &newProxyRow = *m_proxyArray->at(i);
            for (int j = 0; j < columnList.size(); j++)
                newProxyRow[j] = itemValueMap[rowKey][columnList.at(j)];
        }

        // Use first and last roles converted to values for limits
        if (rowList.size()) {
            minRowValue = rowList.first().toFloat();
            maxRowValue = rowList.last().toFloat();
        }
        if (columnList.size()) {
            minColumnValue = columnList.first().toFloat();
            maxColumnValue = columnList.last().toFloat();
        }
    }

    m_proxy->resetArray(m_proxyArray, minRowValue, maxRowValue, minColumnValue, maxColumnValue);
}

QT_DATAVISUALIZATION_END_NAMESPACE
