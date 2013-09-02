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

#include "qitemmodelmapdataproxy_p.h"
#include <QTimer>

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QItemModelMapDataProxy
 * \inmodule QtDataVis3D
 * \brief Proxy class for Q3DMaps data model mapping.
 * \since 1.0.0
 *
 * QItemModelMapDataProxy allows you to use QAbstractItemModel derived models as a data source
 * for Q3DMaps. It maps roles defined in QItemModelMapDataMapping to roles in the model.
 */

/*!
 * Constructs QItemModelMapDataProxy.
 */
QItemModelMapDataProxy::QItemModelMapDataProxy() :
    QMapDataProxy(new QItemModelMapDataProxyPrivate(this))
{
}

/*!
 * Constructs QItemModelMapDataProxy with \a itemModel and \a mapping. Does not take ownership of
 * the model or the mapping, but does connect to them to listen for changes.
 */
QItemModelMapDataProxy::QItemModelMapDataProxy(QAbstractItemModel *itemModel,
                                               QItemModelMapDataMapping *mapping) :
    QMapDataProxy(new QItemModelMapDataProxyPrivate(this))
{
    dptr()->setItemModel(itemModel);
    dptr()->setActiveMapping(mapping);
}

/*!
 * Destroys QItemModelMapDataProxy.
 */
QItemModelMapDataProxy::~QItemModelMapDataProxy()
{
}

/*!
 * \property QItemModelMapDataProxy::itemModel
 *
 * Defines item model. Does not take ownership of the model, but does connect to it to listen for
 * changes.
 */
void QItemModelMapDataProxy::setItemModel(const QAbstractItemModel *itemModel)
{
    dptr()->setItemModel(itemModel);
}

const QAbstractItemModel *QItemModelMapDataProxy::itemModel() const
{
    return dptrc()->m_itemModel.data();
}

/*!
 * \property QItemModelMapDataProxy::activeMapping
 *
 * Defines data mapping. Proxy takes ownership of the \a mapping.
 * Modifying a mapping that is set to the proxy will trigger data set re-resolving.
 */
void QItemModelMapDataProxy::setActiveMapping(QItemModelMapDataMapping *mapping)
{
    dptr()->setActiveMapping(mapping);
}

QItemModelMapDataMapping *QItemModelMapDataProxy::activeMapping() const
{
    return dptrc()->m_mapping;
}

/*!
 * \internal
 */
QItemModelMapDataProxyPrivate *QItemModelMapDataProxy::dptr()
{
    return static_cast<QItemModelMapDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QItemModelMapDataProxyPrivate *QItemModelMapDataProxy::dptrc() const
{
    return static_cast<const QItemModelMapDataProxyPrivate *>(d_ptr.data());
}

// QItemModelMapDataProxyPrivate

QItemModelMapDataProxyPrivate::QItemModelMapDataProxyPrivate(QItemModelMapDataProxy *q)
    : QMapDataProxyPrivate(q),
      m_mapping(0),
      resolvePending(0)
{
    m_resolveTimer.setSingleShot(true);
    QObject::connect(&m_resolveTimer, &QTimer::timeout,
                     this, &QItemModelMapDataProxyPrivate::handlePendingResolve);
}

QItemModelMapDataProxyPrivate::~QItemModelMapDataProxyPrivate()
{
}

void QItemModelMapDataProxyPrivate::setItemModel(const QAbstractItemModel *itemModel)
{
    if (!m_itemModel.isNull())
        QObject::disconnect(m_itemModel, 0, this, 0);

    m_itemModel = itemModel;

    if (!m_itemModel.isNull()) {
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::columnsInserted,
                         this, &QItemModelMapDataProxyPrivate::handleColumnsInserted);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::columnsMoved,
                         this, &QItemModelMapDataProxyPrivate::handleColumnsMoved);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::columnsRemoved,
                         this, &QItemModelMapDataProxyPrivate::handleColumnsRemoved);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::dataChanged,
                         this, &QItemModelMapDataProxyPrivate::handleDataChanged);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::layoutChanged,
                         this, &QItemModelMapDataProxyPrivate::handleLayoutChanged);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::modelReset,
                         this, &QItemModelMapDataProxyPrivate::handleModelReset);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::rowsInserted,
                         this, &QItemModelMapDataProxyPrivate::handleRowsInserted);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::rowsMoved,
                         this, &QItemModelMapDataProxyPrivate::handleRowsMoved);
        QObject::connect(m_itemModel.data(), &QAbstractItemModel::rowsRemoved,
                         this, &QItemModelMapDataProxyPrivate::handleRowsRemoved);
    }
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

void QItemModelMapDataProxyPrivate::setActiveMapping(QItemModelMapDataMapping *mapping)
{
    if (m_mapping) {
        QObject::disconnect(m_mapping, &QItemModelMapDataMapping::mappingChanged,
                            this, &QItemModelMapDataProxyPrivate::handleMappingChanged);
    }

    m_mapping = mapping;

    if (m_mapping) {
        QObject::connect(m_mapping, &QItemModelMapDataMapping::mappingChanged,
                         this, &QItemModelMapDataProxyPrivate::handleMappingChanged);
    }

    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0);
}

void QItemModelMapDataProxyPrivate::handleColumnsInserted(const QModelIndex &parent,
                                                          int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve new items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleColumnsMoved(const QModelIndex &sourceParent,
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

void QItemModelMapDataProxyPrivate::handleColumnsRemoved(const QModelIndex &parent,
                                                         int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Remove old items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleDataChanged(const QModelIndex &topLeft,
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

void QItemModelMapDataProxyPrivate::handleLayoutChanged(const QList<QPersistentModelIndex> &parents,
                                                        QAbstractItemModel::LayoutChangeHint hint)
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

void QItemModelMapDataProxyPrivate::handleRowsInserted(const QModelIndex &parent,
                                                       int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    // Resolve new items
    if (!m_resolveTimer.isActive())
        m_resolveTimer.start(0); // TODO Resolving entire model is inefficient
}

void QItemModelMapDataProxyPrivate::handleRowsMoved(const QModelIndex &sourceParent,
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
    if (m_itemModel.isNull() || !m_mapping) {
        qptr()->resetArray(0);
        return;
    }

    static const int noRoleIndex = -1;

    QHash<int, QByteArray> roleHash = m_itemModel->roleNames();
    const int labelRole = roleHash.key(m_mapping->labelRole().toLatin1(), noRoleIndex);
    const int xPosRole = roleHash.key(m_mapping->xPosRole().toLatin1(), noRoleIndex);
    const int zPosRole = roleHash.key(m_mapping->zPosRole().toLatin1(), noRoleIndex);
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
            if (zPosRole != noRoleIndex)
                yPos = index.data(zPosRole).toReal(); // z position is mapped to y of QPoint
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
