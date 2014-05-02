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

#include "surfaceitemmodelhandler_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

static const int noRoleIndex = -1;

SurfaceItemModelHandler::SurfaceItemModelHandler(QItemModelSurfaceDataProxy *proxy, QObject *parent)
    : AbstractItemModelHandler(parent),
      m_proxy(proxy),
      m_proxyArray(0),
      m_xPosRole(noRoleIndex),
      m_yPosRole(noRoleIndex),
      m_zPosRole(noRoleIndex)
{
}

SurfaceItemModelHandler::~SurfaceItemModelHandler()
{
}

void SurfaceItemModelHandler::handleDataChanged(const QModelIndex &topLeft,
                                                const QModelIndex &bottomRight,
                                                const QVector<int> &roles)
{
    // Do nothing if full reset already pending
    if (!m_fullReset) {
        if (!m_proxy->useModelCategories()) {
            // If the data model doesn't directly map rows and columns, we cannot optimize
            AbstractItemModelHandler::handleDataChanged(topLeft, bottomRight, roles);
        } else {
            int startRow = qMin(topLeft.row(), bottomRight.row());
            int endRow = qMax(topLeft.row(), bottomRight.row());
            int startCol = qMin(topLeft.column(), bottomRight.column());
            int endCol = qMax(topLeft.column(), bottomRight.column());

            for (int i = startRow; i <= endRow; i++) {
                for (int j = startCol; j <= endCol; j++) {
                    QSurfaceDataItem item;
                    const QSurfaceDataItem *oldItem = m_proxy->itemAt(i, j);
                    float xPos;
                    float zPos;
                    if (m_xPosRole != noRoleIndex)
                        xPos = m_itemModel->index(i, j).data(m_xPosRole).toFloat();
                    else
                        xPos = oldItem->x();
                    if (m_zPosRole != noRoleIndex)
                        zPos = m_itemModel->index(i, j).data(m_zPosRole).toFloat();
                    else
                        zPos = oldItem->z();
                    item.setPosition(QVector3D(xPos,
                                               m_itemModel->index(i, j).data(m_yPosRole).toFloat(),
                                               zPos));
                    m_proxy->setItem(i, j, item);
                }
            }
        }
    }
}

// Resolve entire item model into QSurfaceDataArray.
void SurfaceItemModelHandler::resolveModel()
{
    if (m_itemModel.isNull()) {
        m_proxy->resetArray(0);
        m_proxyArray = 0;
        return;
    }

    if (!m_proxy->useModelCategories()
            && (m_proxy->rowRole().isEmpty() || m_proxy->columnRole().isEmpty())) {
        m_proxy->resetArray(0);
        m_proxyArray = 0;
        return;
    }

    QHash<int, QByteArray> roleHash = m_itemModel->roleNames();

    // Default to display role if no mapping
    m_xPosRole = roleHash.key(m_proxy->xPosRole().toLatin1(), noRoleIndex);
    m_yPosRole = roleHash.key(m_proxy->yPosRole().toLatin1(), Qt::DisplayRole);
    m_zPosRole = roleHash.key(m_proxy->zPosRole().toLatin1(), noRoleIndex);
    int rowCount = m_itemModel->rowCount();
    int columnCount = m_itemModel->columnCount();

    if (m_proxy->useModelCategories()) {
        // If dimensions have changed, recreate the array
        if (m_proxyArray != m_proxy->array() || columnCount != m_proxy->columnCount()
                || rowCount != m_proxyArray->size()) {
            m_proxyArray = new QSurfaceDataArray;
            m_proxyArray->reserve(rowCount);
            for (int i = 0; i < rowCount; i++)
                m_proxyArray->append(new QSurfaceDataRow(columnCount));
        }
        for (int i = 0; i < rowCount; i++) {
            QSurfaceDataRow &newProxyRow = *m_proxyArray->at(i);
            for (int j = 0; j < columnCount; j++) {
                float xPos = float(j);
                float zPos = float(i);
                if (m_xPosRole != noRoleIndex) {
                    xPos = m_itemModel->index(i, j).data(m_xPosRole).toFloat();
                } else {
                    QString header = m_itemModel->headerData(j, Qt::Horizontal).toString();
                    bool ok = false;
                    float headerValue = header.toFloat(&ok);
                    if (ok)
                        xPos = headerValue;
                }

                if (m_zPosRole != noRoleIndex) {
                    zPos = m_itemModel->index(i, j).data(m_zPosRole).toFloat();
                } else {
                    QString header = m_itemModel->headerData(i, Qt::Vertical).toString();
                    bool ok = false;
                    float headerValue = header.toFloat(&ok);
                    if (ok)
                        zPos = headerValue;
                }

                newProxyRow[j].setPosition(
                            QVector3D(xPos,
                                      m_itemModel->index(i, j).data(m_yPosRole).toFloat(),
                                      zPos));
            }
        }
    } else {
        int rowRole = roleHash.key(m_proxy->rowRole().toLatin1());
        int columnRole = roleHash.key(m_proxy->columnRole().toLatin1());
        if (m_xPosRole == noRoleIndex)
            m_xPosRole = columnRole;
        if (m_zPosRole == noRoleIndex)
            m_zPosRole = rowRole;

        bool generateRows = m_proxy->autoRowCategories();
        bool generateColumns = m_proxy->autoColumnCategories();

        QStringList rowList;
        QStringList columnList;
        // For detecting duplicates in categories generation, using QHashes should be faster than
        // simple QStringList::contains() check.
        QHash<QString, bool> rowListHash;
        QHash<QString, bool> columnListHash;

        // Sort values into rows and columns
        typedef QHash<QString, QVector3D> ColumnValueMap;
        QHash <QString, ColumnValueMap> itemValueMap;
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < columnCount; j++) {
                QModelIndex index = m_itemModel->index(i, j);
                QString rowRoleStr = index.data(rowRole).toString();
                QString columnRoleStr = index.data(columnRole).toString();
                QVector3D itemPos(index.data(m_xPosRole).toFloat(),
                                  index.data(m_yPosRole).toFloat(),
                                  index.data(m_zPosRole).toFloat());
                itemValueMap[rowRoleStr][columnRoleStr] = itemPos;
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
            m_proxy->dptr()->m_rowCategories = rowList;
        else
            rowList = m_proxy->rowCategories();

        if (generateColumns)
            m_proxy->dptr()->m_columnCategories = columnList;
        else
            columnList = m_proxy->columnCategories();

        // If dimensions have changed, recreate the array
        if (m_proxyArray != m_proxy->array() || columnList.size() != m_proxy->columnCount()
                || rowList.size() != m_proxyArray->size()) {
            m_proxyArray = new QSurfaceDataArray;
            m_proxyArray->reserve(rowList.size());
            for (int i = 0; i < rowList.size(); i++)
                m_proxyArray->append(new QSurfaceDataRow(columnList.size()));
        }
        // Create data array from itemValueMap
        for (int i = 0; i < rowList.size(); i++) {
            QString rowKey = rowList.at(i);
            QSurfaceDataRow &newProxyRow = *m_proxyArray->at(i);
            for (int j = 0; j < columnList.size(); j++)
                newProxyRow[j].setPosition(itemValueMap[rowKey][columnList.at(j)]);
        }
    }

    m_proxy->resetArray(m_proxyArray);
}

QT_END_NAMESPACE_DATAVISUALIZATION
