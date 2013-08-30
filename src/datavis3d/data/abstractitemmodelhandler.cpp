/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

#include "abstractitemmodelhandler_p.h"
#include "qabstractdatamapping.h"
#include <QTimer>

QT_DATAVIS3D_BEGIN_NAMESPACE

AbstractItemModelHandler::AbstractItemModelHandler(QObject *parent)
    : QObject(parent),
      m_activeMapping(0),
      resolvePending(0)
{
    m_resolveTimer.setSingleShot(true);
    QObject::connect(&m_resolveTimer, &QTimer::timeout,
                     this, &AbstractItemModelHandler::handlePendingResolve);
}

AbstractItemModelHandler::~AbstractItemModelHandler()
{
}

void AbstractItemModelHandler::setItemModel(const QAbstractItemModel *itemModel)
{
    if (!m_itemModel.isNull())
        QObject::disconnect(m_itemModel, 0, this, 0);

    m_itemModel = itemModel;

    if (!m_itemModel.isNull()) {
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::columnsInserted,
                         this, &AbstractItemModelHandler::handleColumnsInserted);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::columnsMoved,
                         this, &AbstractItemModelHandler::handleColumnsMoved);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::columnsRemoved,
                         this, &AbstractItemModelHandler::handleColumnsRemoved);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::dataChanged,
                         this, &AbstractItemModelHandler::handleDataChanged);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::layoutChanged,
                         this, &AbstractItemModelHandler::handleLayoutChanged);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::modelReset,
                         this, &AbstractItemModelHandler::handleModelReset);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::rowsInserted,
                         this, &AbstractItemModelHandler::handleRowsInserted);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::rowsMoved,
                         this, &AbstractItemModelHandler::handleRowsMoved);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::rowsRemoved,
                         this, &AbstractItemModelHandler::handleRowsRemoved);
    }
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

const QAbstractItemModel *AbstractItemModelHandler::itemModel() const
{
    return m_itemModel.data();
}

void AbstractItemModelHandler::setActiveMapping(QAbstractDataMapping *mapping)
{
    if (m_activeMapping)
        QObject::disconnect(m_activeMapping, 0, this, 0);

    if (mapping)
        addMapping(mapping);

    m_activeMapping = mapping;

    if (m_activeMapping) {
        QObject::connect(m_activeMapping, &QAbstractDataMapping::mappingChanged,
                         this, &AbstractItemModelHandler::handleMappingChanged);
    }

    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

QAbstractDataMapping *AbstractItemModelHandler::activeMapping() const
{
    return m_activeMapping;
}

void AbstractItemModelHandler::addMapping(QAbstractDataMapping *mapping)
{
    Q_ASSERT(mapping);
    AbstractItemModelHandler *owner = qobject_cast<AbstractItemModelHandler *>(mapping->parent());
    if (owner != this) {
        Q_ASSERT_X(!owner, "addMapping", "Mapping already attached to a proxy.");
        mapping->setParent(this);
    }
    if (!m_mappings.contains(mapping))
        m_mappings.append(mapping);
}

void AbstractItemModelHandler::releaseMapping(QAbstractDataMapping *mapping)
{
    if (mapping && m_mappings.contains(mapping)) {
        // If the mapping is in use, clear the existing mapping
        if (m_activeMapping == mapping)
            setActiveMapping(0);

        m_mappings.removeAll(mapping);
        mapping->setParent(0);
    }
}

QList<QAbstractDataMapping *> AbstractItemModelHandler::mappings() const
{
    return m_mappings;
}

void AbstractItemModelHandler::handleColumnsInserted(const QModelIndex &parent,
                                                     int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve new items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void AbstractItemModelHandler::handleColumnsMoved(const QModelIndex &sourceParent,
                                                  int sourceStart,
                                                  int sourceEnd,
                                                  const QModelIndex &destinationParent,
                                                  int destinationColumn)
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

void AbstractItemModelHandler::handleColumnsRemoved(const QModelIndex &parent,
                                                    int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Remove old items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void AbstractItemModelHandler::handleDataChanged(const QModelIndex &topLeft,
                                                 const QModelIndex &bottomRight,
                                                 const QVector<int> &roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    Q_UNUSED(roles)

    // Resolve changed items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void AbstractItemModelHandler::handleLayoutChanged(const QList<QPersistentModelIndex> &parents,
                                                   QAbstractItemModel::LayoutChangeHint hint)
{
    Q_UNUSED(parents)
    Q_UNUSED(hint)

    // Resolve moved items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void AbstractItemModelHandler::handleModelReset()
{
    // Data cleared, reset array
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void AbstractItemModelHandler::handleRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve new items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void AbstractItemModelHandler::handleRowsMoved(const QModelIndex &sourceParent,
                                               int sourceStart,
                                               int sourceEnd,
                                               const QModelIndex &destinationParent,
                                               int destinationRow)
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

void AbstractItemModelHandler::handleRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve removed items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void AbstractItemModelHandler::handleMappingChanged()
{
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

void AbstractItemModelHandler::handlePendingResolve()
{
    resolveModel();
}

QT_DATAVIS3D_END_NAMESPACE
