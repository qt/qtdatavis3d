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

#include "qitemmodelscatterdataproxy_p.h"
#include "scatteritemmodelhandler_p.h"
#include <QTimer>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QItemModelScatterDataProxy
 * \inmodule QtDataVisualization
 * \brief Proxy class for presenting data in item models with Q3DScatter.
 * \since 1.0.0
 *
 * QItemModelScatterDataProxy allows you to use QAbstractItemModel derived models as a data source
 * for Q3DScatter. It maps roles defined in QItemModelScatterDataMapping to roles in the model.
 *
 * Data is resolved asynchronously whenever the mapping or the model changes.
 * QScatterDataProxy::arrayReset() is emitted when the data has been resolved.
 *
 * /sa {Qt Data Visualization Data Handling}
 */

/*!
 * \qmltype ItemModelScatterDataProxy
 * \inqmlmodule com.digia.QtDataVisualization 1.0
 * \since com.digia.QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QItemModelScatterDataProxy
 * \inherits ScatterDataProxy
 * \brief Proxy class for presenting data in item models with Scatter3D.
 *
 * This type allows you to use AbstractItemModel derived models as a data source for Scatter3D.
 *
 * Data is resolved asynchronously whenever the mapping or the model changes.
 * QScatterDataProxy::arrayReset() is emitted when the data has been resolved.
 *
 * Usage example:
 *
 * \snippet doc_src_qmldatavisualization.cpp 8
 *
 * \sa ScatterDataProxy, ScatterDataMapping, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmlproperty list ItemModelScatterDataProxy::itemModel
 * The item model.
 */

/*!
 * \qmlproperty list ItemModelScatterDataProxy::activeMapping
 * The active mapping. Modifying a mapping that is set to the proxy will trigger data set
 * re-resolving.
 */

/*!
 * Constructs QItemModelScatterDataProxy.
 */
QItemModelScatterDataProxy::QItemModelScatterDataProxy() :
    QScatterDataProxy(new QItemModelScatterDataProxyPrivate(this))
{
}

/*!
 * Constructs QItemModelScatterDataProxy with \a itemModel and \a mapping. Does not take ownership
 * of the model or the mapping, but does connect to them to listen for changes.
 */
QItemModelScatterDataProxy::QItemModelScatterDataProxy(const QAbstractItemModel *itemModel,
                                                       QItemModelScatterDataMapping *mapping) :
    QScatterDataProxy(new QItemModelScatterDataProxyPrivate(this))
{
    dptr()->m_itemModelHandler->setItemModel(itemModel);
    dptr()->m_itemModelHandler->setActiveMapping(mapping);
}

/*!
 * Destroys QItemModelScatterDataProxy.
 */
QItemModelScatterDataProxy::~QItemModelScatterDataProxy()
{
}

/*!
 * \property QItemModelScatterDataProxy::itemModel
 *
 * Defines item model. Does not take ownership of the model, but does connect to it to listen for
 * changes.
 */
void QItemModelScatterDataProxy::setItemModel(const QAbstractItemModel *itemModel)
{
    dptr()->m_itemModelHandler->setItemModel(itemModel);
}

const QAbstractItemModel *QItemModelScatterDataProxy::itemModel() const
{
    return dptrc()->m_itemModelHandler->itemModel();
}

/*!
 * \property QItemModelScatterDataProxy::activeMapping
 *
 * Defines data mapping. Proxy takes ownership of the \a mapping.
 * Modifying a mapping that is set to the proxy will trigger data set re-resolving.
 */
void QItemModelScatterDataProxy::setActiveMapping(QItemModelScatterDataMapping *mapping)
{
    dptr()->m_itemModelHandler->setActiveMapping(mapping);
}

QItemModelScatterDataMapping *QItemModelScatterDataProxy::activeMapping() const
{
    return static_cast<QItemModelScatterDataMapping *>(dptrc()->m_itemModelHandler->activeMapping());
}

/*!
 * Transfers the ownership of the \a mapping to this proxy. The mapping is not taken to use yet.
 * \sa setActiveMapping(), releaseMapping()
 */
void QItemModelScatterDataProxy::addMapping(QItemModelScatterDataMapping *mapping)
{
    dptr()->m_itemModelHandler->addMapping(mapping);
}

/*!
 * Releases the ownership of the \a mapping back to the caller. If the mapping was the currently
 * active one, no mapping remains active after this call.
 */
void QItemModelScatterDataProxy::releaseMapping(QItemModelScatterDataMapping *mapping)
{
    dptr()->m_itemModelHandler->releaseMapping(mapping);
}

/*!
 * \return list of mappings owned by the proxy.
 */
QList<QItemModelScatterDataMapping *> QItemModelScatterDataProxy::mappings() const
{
    QList<QItemModelScatterDataMapping *> retList;
    QList<QAbstractDataMapping *> abstractList = dptrc()->m_itemModelHandler->mappings();
    foreach (QAbstractDataMapping *mapping, abstractList)
        retList.append(static_cast<QItemModelScatterDataMapping *>(mapping));

    return retList;
}

/*!
 * \internal
 */
QItemModelScatterDataProxyPrivate *QItemModelScatterDataProxy::dptr()
{
    return static_cast<QItemModelScatterDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QItemModelScatterDataProxyPrivate *QItemModelScatterDataProxy::dptrc() const
{
    return static_cast<const QItemModelScatterDataProxyPrivate *>(d_ptr.data());
}

// QItemModelScatterDataProxyPrivate

QItemModelScatterDataProxyPrivate::QItemModelScatterDataProxyPrivate(QItemModelScatterDataProxy *q)
    : QScatterDataProxyPrivate(q),
      m_itemModelHandler(new ScatterItemModelHandler(q))
{
}

QItemModelScatterDataProxyPrivate::~QItemModelScatterDataProxyPrivate()
{
    delete m_itemModelHandler;
}

QItemModelScatterDataProxy *QItemModelScatterDataProxyPrivate::qptr()
{
    return static_cast<QItemModelScatterDataProxy *>(q_ptr);
}

QT_DATAVISUALIZATION_END_NAMESPACE
