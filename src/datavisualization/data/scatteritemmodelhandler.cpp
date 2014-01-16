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

namespace QtDataVisualization {

/*!
 * \class QtDataVisualization::ScatterItemModelHandler
 * \internal
 */

ScatterItemModelHandler::ScatterItemModelHandler(QItemModelScatterDataProxy *proxy, QObject *parent)
    : AbstractItemModelHandler(parent),
      m_proxy(proxy),
      m_proxyArray(0)
{
}

ScatterItemModelHandler::~ScatterItemModelHandler()
{
}

// Resolve entire item model into QScatterDataArray.
void ScatterItemModelHandler::resolveModel()
{
    if (m_itemModel.isNull()) {
        m_proxy->resetArray(0);
        m_proxyArray = 0;
        return;
    }

    static const int noRoleIndex = -1;

    QHash<int, QByteArray> roleHash = m_itemModel->roleNames();
    const int xPosRole = roleHash.key(m_proxy->xPosRole().toLatin1(), noRoleIndex);
    const int yPosRole = roleHash.key(m_proxy->yPosRole().toLatin1(), noRoleIndex);
    const int zPosRole = roleHash.key(m_proxy->zPosRole().toLatin1(), noRoleIndex);
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
            QModelIndex index = m_itemModel->index(i, j);
            float xPos(0.0f);
            float yPos(0.0f);
            float zPos(0.0f);
            if (xPosRole != noRoleIndex)
                xPos = index.data(xPosRole).toFloat();
            if (yPosRole != noRoleIndex)
                yPos = index.data(yPosRole).toFloat();
            if (zPosRole != noRoleIndex)
                zPos = index.data(zPosRole).toFloat();
            (*m_proxyArray)[runningCount].setPosition(QVector3D(xPos, yPos, zPos));
            runningCount++;
        }
    }

    m_proxy->resetArray(m_proxyArray);
}

}
