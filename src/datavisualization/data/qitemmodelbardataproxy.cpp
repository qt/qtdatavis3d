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

#include "qitemmodelbardataproxy_p.h"
#include "baritemmodelhandler_p.h"
#include <QTimer>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QItemModelBarDataProxy
 * \inmodule QtDataVisualization
 * \brief Proxy class for presenting data in item models with Q3DBars.
 * \since Qt Data Visualization 1.0
 *
 * QItemModelBarDataProxy allows you to use QAbstractItemModel derived models as a data source
 * for Q3DBars. It uses QItemModelBarDataMapping instance to map data from the model to Q3DBars
 * graph.
 *
 * The data is resolved asynchronously whenever the mapping or the model changes.
 * QBarDataProxy::arrayReset() is emitted when the data has been resolved.
 *
 * \sa QItemModelBarDataMapping, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmltype ItemModelBarDataProxy
 * \inqmlmodule com.digia.QtDataVisualization
 * \since com.digia.QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QItemModelBarDataProxy
 * \inherits BarDataProxy
 * \brief Proxy class for presenting data in item models with Bars3D.
 *
 * This type allows you to use AbstractItemModel derived models as a data source for Bars3D.
 *
 * Data is resolved asynchronously whenever the mapping or the model changes.
 * QBarDataProxy::arrayReset() is emitted when the data has been resolved.
 *
 * Usage example:
 *
 * \snippet doc_src_qmldatavisualization.cpp 7
 *
 * \sa BarDataProxy, BarDataMapping, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmlproperty list ItemModelBarDataProxy::itemModel
 * The item model.
 */

/*!
 * \qmlproperty list ItemModelBarDataProxy::activeMapping
 * The active mapping. Modifying a mapping that is set to the proxy will trigger data set
 * re-resolving.
 */

/*!
 * Constructs QItemModelBarDataProxy.
 */
QItemModelBarDataProxy::QItemModelBarDataProxy() :
    QBarDataProxy(new QItemModelBarDataProxyPrivate(this))
{
}

/*!
 * Constructs QItemModelBarDataProxy with \a itemModel and \a mapping. Proxy takes ownership of the
 * \a mapping, but doesn't take ownership of the \a itemModel, as typically item models are owned
 * by other controls.
 */
QItemModelBarDataProxy::QItemModelBarDataProxy(const QAbstractItemModel *itemModel,
                                               QItemModelBarDataMapping *mapping) :
    QBarDataProxy(new QItemModelBarDataProxyPrivate(this))
{
    dptr()->m_itemModelHandler->setItemModel(itemModel);
    dptr()->m_itemModelHandler->setActiveMapping(mapping);
}

/*!
 * Destroys QItemModelBarDataProxy.
 */
QItemModelBarDataProxy::~QItemModelBarDataProxy()
{
}

/*!
 * \property QItemModelBarDataProxy::itemModel
 *
 * Defines item model. Does not take ownership of the model, but does connect to it to listen for
 * changes.
 */
void QItemModelBarDataProxy::setItemModel(const QAbstractItemModel *itemModel)
{
    dptr()->m_itemModelHandler->setItemModel(itemModel);
}

const QAbstractItemModel *QItemModelBarDataProxy::itemModel() const
{
    return dptrc()->m_itemModelHandler->itemModel();
}

/*!
 * \property QItemModelBarDataProxy::activeMapping
 *
 * Defines data mapping. Proxy takes ownership of the \a mapping.
 * Modifying a mapping that is set to the proxy will trigger data set re-resolving.
 */
void QItemModelBarDataProxy::setActiveMapping(QItemModelBarDataMapping *mapping)
{
    dptr()->m_itemModelHandler->setActiveMapping(mapping);
}

QItemModelBarDataMapping *QItemModelBarDataProxy::activeMapping() const
{
    return static_cast<QItemModelBarDataMapping *>(dptrc()->m_itemModelHandler->activeMapping());
}

/*!
 * Transfers the ownership of the \a mapping to this proxy. The mapping is not taken to use yet.
 * \sa setActiveMapping(), releaseMapping()
 */
void QItemModelBarDataProxy::addMapping(QItemModelBarDataMapping *mapping)
{
    dptr()->m_itemModelHandler->addMapping(mapping);
}

/*!
 * Releases the ownership of the \a mapping back to the caller. If the mapping was the currently
 * active one, no mapping remains active after this call.
 */
void QItemModelBarDataProxy::releaseMapping(QItemModelBarDataMapping *mapping)
{
    dptr()->m_itemModelHandler->releaseMapping(mapping);
}

/*!
 * \return list of mappings owned by the proxy.
 */
QList<QItemModelBarDataMapping *> QItemModelBarDataProxy::mappings() const
{
    QList<QItemModelBarDataMapping *> retList;
    QList<QAbstractDataMapping *> abstractList = dptrc()->m_itemModelHandler->mappings();
    foreach (QAbstractDataMapping *mapping, abstractList)
        retList.append(static_cast<QItemModelBarDataMapping *>(mapping));

    return retList;
}

/*!
 * \internal
 */
QItemModelBarDataProxyPrivate *QItemModelBarDataProxy::dptr()
{
    return static_cast<QItemModelBarDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QItemModelBarDataProxyPrivate *QItemModelBarDataProxy::dptrc() const
{
    return static_cast<const QItemModelBarDataProxyPrivate *>(d_ptr.data());
}

// QItemModelBarDataProxyPrivate

QItemModelBarDataProxyPrivate::QItemModelBarDataProxyPrivate(QItemModelBarDataProxy *q)
    : QBarDataProxyPrivate(q),
      m_itemModelHandler(new BarItemModelHandler(q))
{
}

QItemModelBarDataProxyPrivate::~QItemModelBarDataProxyPrivate()
{
    delete m_itemModelHandler;
}

QItemModelBarDataProxy *QItemModelBarDataProxyPrivate::qptr()
{
    return static_cast<QItemModelBarDataProxy *>(q_ptr);
}

QT_DATAVISUALIZATION_END_NAMESPACE
