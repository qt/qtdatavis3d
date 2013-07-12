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

#include "qitemmodelmapdataproxy_p.h"
#include <QTimer>

QT_DATAVIS3D_BEGIN_NAMESPACE

QItemModelMapDataProxy::QItemModelMapDataProxy() :
    QMapDataProxy(new QItemModelMapDataProxyPrivate(this))
{
}

QItemModelMapDataProxy::QItemModelMapDataProxy(QAbstractItemModel *itemModel,
                                               QItemModelMapDataMapping *mapping) :
    QMapDataProxy(new QItemModelMapDataProxyPrivate(this))
{
    dptr()->setItemModel(itemModel);
    dptr()->setMapping(mapping);
}

QItemModelMapDataProxy::~QItemModelMapDataProxy()
{
}

void QItemModelMapDataProxy::setItemModel(QAbstractItemModel *itemModel)
{
    dptr()->setItemModel(itemModel);
}

QAbstractItemModel *QItemModelMapDataProxy::itemModel()
{
    return dptr()->m_itemModel.data();
}

void QItemModelMapDataProxy::setMapping(QItemModelMapDataMapping *mapping)
{
    dptr()->setMapping(mapping);
}

QItemModelMapDataMapping *QItemModelMapDataProxy::mapping()
{
    return dptr()->m_mapping.data();
}

QItemModelMapDataProxyPrivate *QItemModelMapDataProxy::dptr()
{
    return static_cast<QItemModelMapDataProxyPrivate *>(d_ptr.data());
}

// QItemModelMapDataProxyPrivate

QItemModelMapDataProxyPrivate::QItemModelMapDataProxyPrivate(QItemModelMapDataProxy *q)
    : QMapDataProxyPrivate(q),
      resolvePending(0)
{
    m_resolveTimer.setSingleShot(true);
    QObject::connect(&m_resolveTimer, &QTimer::timeout, this, &QItemModelMapDataProxyPrivate::handlePendingResolve);
}

QItemModelMapDataProxyPrivate::~QItemModelMapDataProxyPrivate()
{
}

void QItemModelMapDataProxyPrivate::setItemModel(QAbstractItemModel *itemModel)
{
    if (!m_itemModel.isNull())
        QObject::disconnect(m_itemModel, 0, this, 0);

    m_itemModel = itemModel;

    if (!m_itemModel.isNull()) {
        QObject::connect(m_itemModel, &QAbstractItemModel::columnsInserted, this, &QItemModelMapDataProxyPrivate::handleColumnsInserted);
        QObject::connect(m_itemModel, &QAbstractItemModel::columnsMoved, this, &QItemModelMapDataProxyPrivate::handleColumnsMoved);
        QObject::connect(m_itemModel, &QAbstractItemModel::columnsRemoved, this, &QItemModelMapDataProxyPrivate::handleColumnsRemoved);
        QObject::connect(m_itemModel, &QAbstractItemModel::dataChanged, this, &QItemModelMapDataProxyPrivate::handleDataChanged);
        QObject::connect(m_itemModel, &QAbstractItemModel::layoutChanged, this, &QItemModelMapDataProxyPrivate::handleLayoutChanged);
        QObject::connect(m_itemModel, &QAbstractItemModel::modelReset, this, &QItemModelMapDataProxyPrivate::handleModelReset);
        QObject::connect(m_itemModel, &QAbstractItemModel::rowsInserted, this, &QItemModelMapDataProxyPrivate::handleRowsInserted);
        QObject::connect(m_itemModel, &QAbstractItemModel::rowsMoved, this, &QItemModelMapDataProxyPrivate::handleRowsMoved);
        QObject::connect(m_itemModel, &QAbstractItemModel::rowsRemoved, this, &QItemModelMapDataProxyPrivate::handleRowsRemoved);
    }
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

void QItemModelMapDataProxyPrivate::setMapping(QItemModelMapDataMapping *mapping)
{
    if (!m_mapping.isNull())
        QObject::disconnect(m_mapping.data(), &QItemModelMapDataMapping::mappingChanged, this, &QItemModelMapDataProxyPrivate::handleMappingChanged);

    m_mapping = mapping;

    if (!m_mapping.isNull())
        QObject::connect(m_mapping.data(), &QItemModelMapDataMapping::mappingChanged, this, &QItemModelMapDataProxyPrivate::handleMappingChanged);

    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

void QItemModelMapDataProxyPrivate::handleColumnsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve new items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(sourceStart)
    Q_UNUSED(sourceEnd)
    Q_UNUSED(destinationParent)
    Q_UNUSED(destinationColumn)

    // Resolve moved items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleColumnsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Remove old items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    Q_UNUSED(roles)

    // Resolve changed items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_UNUSED(parents)
    Q_UNUSED(hint)

    // Resolve moved items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleModelReset()
{
    // Data cleared, reset array
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve new items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(sourceStart)
    Q_UNUSED(sourceEnd)
    Q_UNUSED(destinationParent)
    Q_UNUSED(destinationRow)

    // Resolve moved items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve removed items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleMappingChanged()
{
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

void QItemModelMapDataProxyPrivate::handlePendingResolve()
{
    resolveModel();
}

// Resolve entire item model into QMapDataArray.
void QItemModelMapDataProxyPrivate::resolveModel()
{
    if (m_itemModel.isNull() || m_mapping.isNull()) {
        qptr()->resetArray(0);
        return;
    }

    static const int noRoleIndex = -1;

    QHash<int, QByteArray> roleHash = m_itemModel->roleNames();
    const int labelRole = roleHash.key(m_mapping->labelRole().toLatin1(), noRoleIndex);
    const int xPosRole = roleHash.key(m_mapping->xPosRole().toLatin1(), noRoleIndex);
    const int yPosRole = roleHash.key(m_mapping->yPosRole().toLatin1(), noRoleIndex);
    // Default valueRole to display role if no mapping
    const int valueRole = roleHash.key(m_mapping->valueRole().toLatin1(), Qt::DisplayRole);
    const int columnCount = m_itemModel->columnCount();
    const int rowCount = m_itemModel->rowCount();
    const int totalCount = rowCount * columnCount;
    int runningCount = 0;

    // Parse data into newProxyArray
    QMapDataArray *newProxyArray = new QMapDataArray(totalCount);
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < columnCount; j++) {
            QModelIndex index = m_itemModel->index(i, j);
            if (labelRole != noRoleIndex)
                (*newProxyArray)[runningCount].setLabel(index.data(labelRole).toString());
            qreal xPos(qreal(0.0));
            qreal yPos(qreal(0.0));
            if (xPosRole != noRoleIndex)
                xPos = index.data(xPosRole).toReal();
            if (yPosRole != noRoleIndex)
                yPos = index.data(yPosRole).toReal();
            (*newProxyArray)[runningCount].setMapPosition(QPointF(xPos, yPos));
            (*newProxyArray)[runningCount].setValue(index.data(valueRole).toReal());
            runningCount++;
        }
    }

    qDebug() << __FUNCTION__ << "Total count:" << newProxyArray->size();

    qptr()->resetArray(newProxyArray);
}

QItemModelMapDataProxy *QItemModelMapDataProxyPrivate::qptr()
{
    return static_cast<QItemModelMapDataProxy *>(q_ptr);
}

QT_DATAVIS3D_END_NAMESPACE
