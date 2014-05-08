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

#include "baritemmodelhandler_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

static const int noRoleIndex = -1;

BarItemModelHandler::BarItemModelHandler(QItemModelBarDataProxy *proxy, QObject *parent)
    : AbstractItemModelHandler(parent),
      m_proxy(proxy),
      m_proxyArray(0),
      m_columnCount(0),
      m_valueRole(noRoleIndex),
      m_rotationRole(noRoleIndex),
      m_haveValuePattern(false),
      m_haveRotationPattern(false)
{
}

BarItemModelHandler::~BarItemModelHandler()
{
}

void BarItemModelHandler::handleDataChanged(const QModelIndex &topLeft,
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
                    QBarDataItem item;
                    QVariant valueVar = m_itemModel->index(i, j).data(m_valueRole);
                    float value;
                    if (m_haveValuePattern)
                        value = valueVar.toString().replace(m_valuePattern, m_valueReplace).toFloat();
                    else
                        value = valueVar.toFloat();
                    item.setValue(value);
                    if (m_rotationRole != noRoleIndex) {
                        QVariant rotationVar = m_itemModel->index(i, j).data(m_rotationRole);
                        float rotation;
                        if (m_haveRotationPattern) {
                            rotation = rotationVar.toString().replace(m_rotationPattern,
                                                                      m_rotationReplace).toFloat();
                        } else {
                            rotation = rotationVar.toFloat();
                        }
                        item.setRotation(rotation);
                    }
                    m_proxy->setItem(i, j, item);
                }
            }
        }
    }
}

// Resolve entire item model into QBarDataArray.
void BarItemModelHandler::resolveModel()
{
    if (m_itemModel.isNull()) {
        m_proxy->resetArray(0);
        return;
    }

    if (!m_proxy->useModelCategories()
            && (m_proxy->rowRole().isEmpty() || m_proxy->columnRole().isEmpty())) {
        m_proxy->resetArray(0);
        return;
    }

    // Value and rotation patterns can be reused on single item changes,
    // so store them to member variables.
    QRegExp rowPattern(m_proxy->rowRolePattern());
    QRegExp colPattern(m_proxy->columnRolePattern());
    m_valuePattern = m_proxy->valueRolePattern();
    m_rotationPattern = m_proxy->rotationRolePattern();
    QString rowReplace = m_proxy->rowRoleReplace();
    QString colReplace = m_proxy->columnRoleReplace();
    m_valueReplace = m_proxy->valueRoleReplace();
    m_rotationReplace = m_proxy->rotationRoleReplace();
    bool haveRowPattern = !rowPattern.isEmpty() && rowPattern.isValid();
    bool haveColPattern = !colPattern.isEmpty() && colPattern.isValid();
    m_haveValuePattern = !m_valuePattern.isEmpty() && m_valuePattern.isValid();
    m_haveRotationPattern = !m_rotationPattern.isEmpty() && m_rotationPattern.isValid();

    QStringList rowLabels;
    QStringList columnLabels;

    QHash<int, QByteArray> roleHash = m_itemModel->roleNames();

    // Default value role to display role if no mapping
    m_valueRole = roleHash.key(m_proxy->valueRole().toLatin1(), Qt::DisplayRole);
    m_rotationRole = roleHash.key(m_proxy->rotationRole().toLatin1(), noRoleIndex);
    int rowCount = m_itemModel->rowCount();
    int columnCount = m_itemModel->columnCount();

    if (m_proxy->useModelCategories()) {
        // If dimensions have changed, recreate the array
        if (m_proxyArray != m_proxy->array() || columnCount != m_columnCount
                || rowCount != m_proxyArray->size()) {
            m_proxyArray = new QBarDataArray;
            m_proxyArray->reserve(rowCount);
            for (int i = 0; i < rowCount; i++)
                m_proxyArray->append(new QBarDataRow(columnCount));
        }
        for (int i = 0; i < rowCount; i++) {
            QBarDataRow &newProxyRow = *m_proxyArray->at(i);
            for (int j = 0; j < columnCount; j++) {
                QVariant valueVar = m_itemModel->index(i, j).data(m_valueRole);
                float value;
                if (m_haveValuePattern)
                    value = valueVar.toString().replace(m_valuePattern, m_valueReplace).toFloat();
                else
                    value = valueVar.toFloat();
                newProxyRow[j].setValue(value);
                if (m_rotationRole != noRoleIndex) {
                    QVariant rotationVar = m_itemModel->index(i, j).data(m_rotationRole);
                    float rotation;
                    if (m_haveRotationPattern) {
                        rotation = rotationVar.toString().replace(m_rotationPattern,
                                                                  m_rotationReplace).toFloat();
                    } else {
                        rotation = rotationVar.toFloat();
                    }
                    newProxyRow[j].setRotation(rotation);
                }
            }
        }
        // Generate labels from headers if using model rows/columns
        for (int i = 0; i < rowCount; i++)
            rowLabels << m_itemModel->headerData(i, Qt::Vertical).toString();
        for (int i = 0; i < columnCount; i++)
            columnLabels << m_itemModel->headerData(i, Qt::Horizontal).toString();
        m_columnCount = columnCount;
    } else {
        int rowRole = roleHash.key(m_proxy->rowRole().toLatin1());
        int columnRole = roleHash.key(m_proxy->columnRole().toLatin1());

        bool generateRows = m_proxy->autoRowCategories();
        bool generateColumns = m_proxy->autoColumnCategories();
        QStringList rowList;
        QStringList columnList;
        // For detecting duplicates in categories generation, using QHashes should be faster than
        // simple QStringList::contains() check.
        QHash<QString, bool> rowListHash;
        QHash<QString, bool> columnListHash;

        // Sort values into rows and columns
        typedef QHash<QString, float> ColumnValueMap;
        QHash <QString, ColumnValueMap> itemValueMap;
        QHash <QString, ColumnValueMap> itemRotationMap;
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < columnCount; j++) {
                QModelIndex index = m_itemModel->index(i, j);
                QString rowRoleStr = index.data(rowRole).toString();
                if (haveRowPattern)
                    rowRoleStr.replace(rowPattern, rowReplace);
                QString columnRoleStr = index.data(columnRole).toString();
                if (haveColPattern)
                    columnRoleStr.replace(colPattern, colReplace);
                QVariant valueVar = index.data(m_valueRole);
                float value;
                if (m_haveValuePattern)
                    value = valueVar.toString().replace(m_valuePattern, m_valueReplace).toFloat();
                else
                    value = valueVar.toFloat();
                itemValueMap[rowRoleStr][columnRoleStr] = value;
                if (m_rotationRole != noRoleIndex) {
                    QVariant rotationVar = index.data(m_rotationRole);
                    float rotation;
                    if (m_haveRotationPattern) {
                        rotation = rotationVar.toString().replace(m_rotationPattern,
                                                                  m_rotationReplace).toFloat();
                    } else {
                        rotation = rotationVar.toFloat();
                    }
                    itemRotationMap[rowRoleStr][columnRoleStr] = rotation;
                }
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
        if (m_proxyArray != m_proxy->array() || columnList.size() != m_columnCount
                || rowList.size() != m_proxyArray->size()) {
            m_proxyArray = new QBarDataArray;
            m_proxyArray->reserve(rowList.size());
            for (int i = 0; i < rowList.size(); i++)
                m_proxyArray->append(new QBarDataRow(columnList.size()));
        }
        // Create new data array from itemValueMap
        for (int i = 0; i < rowList.size(); i++) {
            QString rowKey = rowList.at(i);
            QBarDataRow &newProxyRow = *m_proxyArray->at(i);
            for (int j = 0; j < columnList.size(); j++) {
                newProxyRow[j].setValue(itemValueMap[rowKey][columnList.at(j)]);
                if (m_rotationRole != noRoleIndex)
                    newProxyRow[j].setRotation(itemRotationMap[rowKey][columnList.at(j)]);
            }
        }

        rowLabels = rowList;
        columnLabels = columnList;
        m_columnCount = columnList.size();
    }

    m_proxy->resetArray(m_proxyArray, rowLabels, columnLabels);
}

QT_END_NAMESPACE_DATAVISUALIZATION
