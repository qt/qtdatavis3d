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

#include "scatteritemmodelhandler_p.h"
#include <QTimer>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

static const int noRoleIndex = -1;

ScatterItemModelHandler::ScatterItemModelHandler(QItemModelScatterDataProxy *proxy, QObject *parent)
    : AbstractItemModelHandler(parent),
      m_proxy(proxy),
      m_proxyArray(0)
{
}

ScatterItemModelHandler::~ScatterItemModelHandler()
{
}

void ScatterItemModelHandler::handleDataChanged(const QModelIndex &topLeft,
                                                const QModelIndex &bottomRight,
                                                const QVector<int> &roles)
{
    // Do nothing if full reset already pending
    if (!m_fullReset) {
        if (m_itemModel->columnCount() > 1) {
            // If the data model is multi-column, do full asynchronous reset to simplify things
            AbstractItemModelHandler::handleDataChanged(topLeft, bottomRight, roles);
        } else {
            int start = qMin(topLeft.row(), bottomRight.row());
            int end = qMax(topLeft.row(), bottomRight.row());

            QScatterDataArray array(end - start + 1);
            int count = 0;
            for (int i = start; i <= end; i++)
                modelPosToScatterItem(i, 0, array[count++]);

            m_proxy->setItems(start, array);
        }
    }
}

void ScatterItemModelHandler::handleRowsInserted(const QModelIndex &parent, int start, int end)
{
    // Do nothing if full reset already pending
    if (!m_fullReset) {
        if (!m_proxy->itemCount() || m_itemModel->columnCount() > 1) {
            // If inserting into an empty array, do full asynchronous reset to avoid multiple
            // separate inserts when initializing the model.
            // If the data model is multi-column, do full asynchronous reset to simplify things
            AbstractItemModelHandler::handleRowsInserted(parent, start, end);
        } else {
            QScatterDataArray array(end - start + 1);
            int count = 0;
            for (int i = start; i <= end; i++)
                modelPosToScatterItem(i, 0, array[count++]);

            m_proxy->insertItems(start, array);
        }
    }
}

void ScatterItemModelHandler::handleRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)

    // Do nothing if full reset already pending
    if (!m_fullReset) {
        if (m_itemModel->columnCount() > 1) {
            // If the data model is multi-column, do full asynchronous reset to simplify things
            AbstractItemModelHandler::handleRowsRemoved(parent, start, end);
        } else {
            m_proxy->removeItems(start, end - start + 1);
        }
    }
}

void ScatterItemModelHandler::modelPosToScatterItem(int modelRow, int modelColumn, QScatterDataItem &item)
{
    QModelIndex index = m_itemModel->index(modelRow, modelColumn);
    float xPos(0.0f);
    float yPos(0.0f);
    float zPos(0.0f);
    if (m_xPosRole != noRoleIndex)
        xPos = index.data(m_xPosRole).toFloat();
    if (m_yPosRole != noRoleIndex)
        yPos = index.data(m_yPosRole).toFloat();
    if (m_zPosRole != noRoleIndex)
        zPos = index.data(m_zPosRole).toFloat();
    item.setPosition(QVector3D(xPos, yPos, zPos));
}

// Resolve entire item model into QScatterDataArray.
void ScatterItemModelHandler::resolveModel()
{
    if (m_itemModel.isNull()) {
        m_proxy->resetArray(0);
        m_proxyArray = 0;
        return;
    }

    QHash<int, QByteArray> roleHash = m_itemModel->roleNames();
    m_xPosRole = roleHash.key(m_proxy->xPosRole().toLatin1(), noRoleIndex);
    m_yPosRole = roleHash.key(m_proxy->yPosRole().toLatin1(), noRoleIndex);
    m_zPosRole = roleHash.key(m_proxy->zPosRole().toLatin1(), noRoleIndex);
    const int columnCount = m_itemModel->columnCount();
    const int rowCount = m_itemModel->rowCount();
    const int totalCount = rowCount * columnCount;
    int runningCount = 0;

    // If dimensions have changed, recreate the array
    if (m_proxyArray != m_proxy->array() || totalCount != m_proxyArray->size())
        m_proxyArray = new QScatterDataArray(totalCount);

    // Parse data into newProxyArray
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < columnCount; j++) {
            modelPosToScatterItem(i, j, (*m_proxyArray)[runningCount]);
            runningCount++;
        }
    }

    m_proxy->resetArray(m_proxyArray);
}

QT_END_NAMESPACE_DATAVISUALIZATION
