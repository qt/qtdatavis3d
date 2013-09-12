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

#include "qitemmodelsurfacedataproxy_p.h"
#include "surfaceitemmodelhandler_p.h"
#include <QTimer>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

// TODO: CHECK DOCUMENTATION!

/*!
 * \class QItemModelSurfaceDataProxy
 * \inmodule QtDataVisualization
 * \brief Proxy class for Q3DSurface data model mapping.
 * \since 1.0.0
 *
 * QItemModelSurfaceDataProxy allows you to use QAbstractItemModel derived models as a data source
 * for Q3DSurface. It maps roles defined in QItemModelSurfaceDataMapping to roles in the model.
 */

/*!
 * \qmltype ItemModelSurfaceDataProxy
 * \instantiates QItemModelSurfaceDataProxy
 * \inherits SurfaceDataProxy
 *
 * This type allows you to use AbstractItemModel derived models as a data source for Surface3D.
 *
 * Usage example:
 *
 * \snippet doc_src_qmldatavisualization.cpp 9
 *
 * \sa SurfaceDataProxy, SurfaceDataMapping
 */

/*!
 * \qmlproperty list ItemModelSurfaceDataProxy::itemModel
 * The item model.
 */

/*!
 * \qmlproperty list ItemModelSurfaceDataProxy::activeMapping
 * The active mapping. Modifying a mapping that is set to the proxy will trigger data set
 * re-resolving.
 */

/*!
 * Constructs QItemModelSurfaceDataProxy.
 */
QItemModelSurfaceDataProxy::QItemModelSurfaceDataProxy() :
    QSurfaceDataProxy(new QItemModelSurfaceDataProxyPrivate(this))
{
}

/*!
 * Constructs QItemModelSurfaceDataProxy with \a itemModel and \a mapping. Does not take ownership
 * of the model or the mapping, but does connect to them to listen for changes.
 */
QItemModelSurfaceDataProxy::QItemModelSurfaceDataProxy(const QAbstractItemModel *itemModel,
                                                       QItemModelSurfaceDataMapping *mapping) :
    QSurfaceDataProxy(new QItemModelSurfaceDataProxyPrivate(this))
{
    dptr()->m_itemModelHandler->setItemModel(itemModel);
    dptr()->m_itemModelHandler->setActiveMapping(mapping);
}

/*!
 * Destroys QItemModelSurfaceDataProxy.
 */
QItemModelSurfaceDataProxy::~QItemModelSurfaceDataProxy()
{
}

/*!
 * \property QItemModelSurfaceDataProxy::itemModel
 *
 * Defines item model. Does not take ownership of the model, but does connect to it to listen for
 * changes.
 */
void QItemModelSurfaceDataProxy::setItemModel(const QAbstractItemModel *itemModel)
{
    dptr()->m_itemModelHandler->setItemModel(itemModel);
}

const QAbstractItemModel *QItemModelSurfaceDataProxy::itemModel() const
{
    return dptrc()->m_itemModelHandler->itemModel();
}

/*!
 * \property QItemModelSurfaceDataProxy::activeMapping
 *
 * Defines data mapping. Proxy takes ownership of the \a mapping.
 * Modifying a mapping that is set to the proxy will trigger data set re-resolving.
 */
void QItemModelSurfaceDataProxy::setActiveMapping(QItemModelSurfaceDataMapping *mapping)
{
    dptr()->m_itemModelHandler->setActiveMapping(mapping);
}

QItemModelSurfaceDataMapping *QItemModelSurfaceDataProxy::activeMapping() const
{
    return static_cast<QItemModelSurfaceDataMapping *>(dptrc()->m_itemModelHandler->activeMapping());
}

/*!
 * Transfers the ownership of the \a mapping to this proxy. The mapping is not taken to use yet.
 * \sa setActiveMapping(), releaseMapping()
 */
void QItemModelSurfaceDataProxy::addMapping(QItemModelSurfaceDataMapping *mapping)
{
    dptr()->m_itemModelHandler->addMapping(mapping);
}

/*!
 * Releases the ownership of the \a mapping back to the caller. If the mapping was the currently
 * active one, no mapping remains active after this call.
 */
void QItemModelSurfaceDataProxy::releaseMapping(QItemModelSurfaceDataMapping *mapping)
{
    dptr()->m_itemModelHandler->releaseMapping(mapping);
}

/*!
 * \return list of mappings owned by the proxy.
 */
QList<QItemModelSurfaceDataMapping *> QItemModelSurfaceDataProxy::mappings() const
{
    QList<QItemModelSurfaceDataMapping *> retList;
    QList<QAbstractDataMapping *> abstractList = dptrc()->m_itemModelHandler->mappings();
    foreach (QAbstractDataMapping *mapping, abstractList)
        retList.append(static_cast<QItemModelSurfaceDataMapping *>(mapping));

    return retList;
}

/*!
 * \internal
 */
QItemModelSurfaceDataProxyPrivate *QItemModelSurfaceDataProxy::dptr()
{
    return static_cast<QItemModelSurfaceDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QItemModelSurfaceDataProxyPrivate *QItemModelSurfaceDataProxy::dptrc() const
{
    return static_cast<const QItemModelSurfaceDataProxyPrivate *>(d_ptr.data());
}

// QItemModelSurfaceDataProxyPrivate

QItemModelSurfaceDataProxyPrivate::QItemModelSurfaceDataProxyPrivate(QItemModelSurfaceDataProxy *q)
    : QSurfaceDataProxyPrivate(q),
      m_itemModelHandler(new SurfaceItemModelHandler(q))
{
}

QItemModelSurfaceDataProxyPrivate::~QItemModelSurfaceDataProxyPrivate()
{
    delete m_itemModelHandler;
}

QItemModelSurfaceDataProxy *QItemModelSurfaceDataProxyPrivate::qptr()
{
    return static_cast<QItemModelSurfaceDataProxy *>(q_ptr);
}

QT_DATAVISUALIZATION_END_NAMESPACE
