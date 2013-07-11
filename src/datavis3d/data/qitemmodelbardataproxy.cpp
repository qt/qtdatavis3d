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

#include "qitemmodelbardataproxy_p.h"
#include <QTimer>

QT_DATAVIS3D_BEGIN_NAMESPACE

QItemModelBarDataProxy::QItemModelBarDataProxy() :
    QBarDataProxy(new QItemModelBarDataProxyPrivate(this))
{
}

QItemModelBarDataProxy::QItemModelBarDataProxy(QAbstractItemModel *itemModel,
                                               QItemModelBarDataMapping *mapping) :
    QBarDataProxy(new QItemModelBarDataProxyPrivate(this))
{
    dptr()->setItemModel(itemModel);
    dptr()->setMapping(mapping);
}

QItemModelBarDataProxy::~QItemModelBarDataProxy()
{
}

void QItemModelBarDataProxy::setItemModel(QAbstractItemModel *itemModel)
{
    dptr()->setItemModel(itemModel);
}

QAbstractItemModel *QItemModelBarDataProxy::itemModel()
{
    return dptr()->m_itemModel.data();
}

void QItemModelBarDataProxy::setMapping(QItemModelBarDataMapping *mapping)
{
    dptr()->setMapping(mapping);
}

QItemModelBarDataMapping *QItemModelBarDataProxy::mapping()
{
    return dptr()->m_mapping.data();
}

QItemModelBarDataProxyPrivate *QItemModelBarDataProxy::dptr()
{
    return static_cast<QItemModelBarDataProxyPrivate *>(d_ptr.data());
}

// QItemModelBarDataProxyPrivate

QItemModelBarDataProxyPrivate::QItemModelBarDataProxyPrivate(QItemModelBarDataProxy *q)
    : QBarDataProxyPrivate(q),
      resolvePending(0)
{
    m_resolveTimer.setSingleShot(true);
    QObject::connect(&m_resolveTimer, &QTimer::timeout, this, &QItemModelBarDataProxyPrivate::handlePendingResolve);
}

QItemModelBarDataProxyPrivate::~QItemModelBarDataProxyPrivate()
{
}

void QItemModelBarDataProxyPrivate::setItemModel(QAbstractItemModel *itemModel)
{
    if (!m_itemModel.isNull())
        QObject::disconnect(m_itemModel, 0, this, 0);

    m_itemModel = itemModel;

    if (!m_itemModel.isNull()) {
        QObject::connect(m_itemModel, &QAbstractItemModel::columnsInserted, this, &QItemModelBarDataProxyPrivate::handleColumnsInserted);
        QObject::connect(m_itemModel, &QAbstractItemModel::columnsMoved, this, &QItemModelBarDataProxyPrivate::handleColumnsMoved);
        QObject::connect(m_itemModel, &QAbstractItemModel::columnsRemoved, this, &QItemModelBarDataProxyPrivate::handleColumnsRemoved);
        QObject::connect(m_itemModel, &QAbstractItemModel::dataChanged, this, &QItemModelBarDataProxyPrivate::handleDataChanged);
        QObject::connect(m_itemModel, &QAbstractItemModel::layoutChanged, this, &QItemModelBarDataProxyPrivate::handleLayoutChanged);
        QObject::connect(m_itemModel, &QAbstractItemModel::modelReset, this, &QItemModelBarDataProxyPrivate::handleModelReset);
        QObject::connect(m_itemModel, &QAbstractItemModel::rowsInserted, this, &QItemModelBarDataProxyPrivate::handleRowsInserted);
        QObject::connect(m_itemModel, &QAbstractItemModel::rowsMoved, this, &QItemModelBarDataProxyPrivate::handleRowsMoved);
        QObject::connect(m_itemModel, &QAbstractItemModel::rowsRemoved, this, &QItemModelBarDataProxyPrivate::handleRowsRemoved);
    }
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

void QItemModelBarDataProxyPrivate::setMapping(QItemModelBarDataMapping *mapping)
{
    if (!m_mapping.isNull())
        QObject::disconnect(m_mapping.data(), &QItemModelBarDataMapping::mappingChanged, this, &QItemModelBarDataProxyPrivate::handleMappingChanged);

    m_mapping = mapping;

    if (!m_mapping.isNull())
        QObject::connect(m_mapping.data(), &QItemModelBarDataMapping::mappingChanged, this, &QItemModelBarDataProxyPrivate::handleMappingChanged);

    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

void QItemModelBarDataProxyPrivate::handleColumnsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve new items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelBarDataProxyPrivate::handleColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn)
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

void QItemModelBarDataProxyPrivate::handleColumnsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Remove old items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelBarDataProxyPrivate::handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    Q_UNUSED(roles)

    // Resolve changed items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelBarDataProxyPrivate::handleLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_UNUSED(parents)
    Q_UNUSED(hint)

    // Resolve moved items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelBarDataProxyPrivate::handleModelReset()
{
    // Data cleared, reset array
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelBarDataProxyPrivate::handleRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve new items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelBarDataProxyPrivate::handleRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
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

void QItemModelBarDataProxyPrivate::handleRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve removed items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelBarDataProxyPrivate::handleMappingChanged()
{
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

void QItemModelBarDataProxyPrivate::handlePendingResolve()
{
    resolveModel();
}

// Resolve entire item model into QBarDataArray.
void QItemModelBarDataProxyPrivate::resolveModel()
{
    if (m_itemModel.isNull() || m_mapping.isNull()) {
        qptr()->resetArray(0);
        return;
    }

    bool useModelRows(false);
    if (!m_mapping->rowCategories().size() || !m_mapping->columnCategories().size()) {
        useModelRows = true;
    } else if (m_mapping->rowRole().isEmpty() || m_mapping->columnRole().isEmpty()) {
        qptr()->resetArray(0);
        return;
    }

    QBarDataArray *newProxyArray = new QBarDataArray;
    QHash<int, QByteArray> roleHash = m_itemModel->roleNames();
    // Default to display role if no mapping
    int valueRole = roleHash.key(m_mapping->valueRole().toLatin1(), Qt::DisplayRole);
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
        int rowRole = roleHash.key(m_mapping->rowRole().toLatin1());
        int columnRole = roleHash.key(m_mapping->columnRole().toLatin1());
        const QStringList &rowList = m_mapping->rowCategories();
        const QStringList &columnList = m_mapping->columnCategories();

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
    qDebug() << __FUNCTION__ << "RowCount:" << newProxyArray->size() << "Column count:" << (newProxyArray->size() ? newProxyArray->at(0)->size() : 0);

    qptr()->resetArray(newProxyArray);
}

QItemModelBarDataProxy *QItemModelBarDataProxyPrivate::qptr()
{
    return static_cast<QItemModelBarDataProxy *>(q_ptr);
}

QT_DATAVIS3D_END_NAMESPACE
