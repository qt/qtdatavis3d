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

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QItemModelScatterDataProxy
 * \inmodule QtDataVisualization
 * \brief Proxy class for presenting data in item models with Q3DScatter.
 * \since Qt Data Visualization 1.0
 *
 * QItemModelScatterDataProxy allows you to use QAbstractItemModel derived models as a data source
 * for Q3DScatter. It maps roles of QAbstractItemModel to the XYZ-values of Q3DScatter points.
 *
 * The data is resolved asynchronously whenever the mapping or the model changes.
 * QScatterDataProxy::arrayReset() is emitted when the data has been resolved. However, inserts,
 * removes, and single data item changes after the model initialization are resolved synchronously,
 * unless the same frame also contains a change that causes the whole model to be resolved.
 *
 * Mapping ignores rows and columns of the QAbstractItemModel and treats
 * all items equally. It requires the model to provide at least three roles for the data items
 * that can be mapped to X, Y, and Z-values for the scatter points.
 *
 * For example, assume that you have a custom QAbstractItemModel for storing various measurements
 * done on material samples, providing data for roles such as "density", "hardness", and
 * "conductivity". You could visualize these properties on a scatter graph using this proxy:
 *
 * \snippet doc_src_qtdatavisualization.cpp 4
 *
 * \sa {Qt Data Visualization Data Handling}
 */

/*!
 * \qmltype ItemModelScatterDataProxy
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QItemModelScatterDataProxy
 * \inherits ScatterDataProxy
 * \brief Proxy class for presenting data in item models with Scatter3D.
 *
 * This type allows you to use AbstractItemModel derived models as a data source for Scatter3D.
 *
 * The data is resolved asynchronously whenever the mapping or the model changes.
 * QScatterDataProxy::arrayReset() is emitted when the data has been resolved.
 *
 * For more details, see QItemModelScatterDataProxy documentation.
 *
 * Usage example:
 *
 * \snippet doc_src_qmldatavisualization.cpp 8
 *
 * \sa ScatterDataProxy, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmlproperty list ItemModelScatterDataProxy::itemModel
 * The item model.
 */

/*!
 * \qmlproperty string ItemModelScatterDataProxy::xPosRole
 * The X position role of the mapping.
 */

/*!
 * \qmlproperty string ItemModelScatterDataProxy::yPosRole
 * The Y position role of the mapping.
 */

/*!
 * \qmlproperty string ItemModelScatterDataProxy::zPosRole
 * The Z position role of the mapping.
 */

/*!
 * Constructs QItemModelScatterDataProxy with optional \a parent.
 */
QItemModelScatterDataProxy::QItemModelScatterDataProxy(QObject *parent)
    : QScatterDataProxy(new QItemModelScatterDataProxyPrivate(this), parent)
{
    dptr()->connectItemModelHandler();
}

/*!
 * Constructs QItemModelScatterDataProxy with \a itemModel and optional \a parent. Proxy doesn't take
 * ownership of the \a itemModel, as typically item models are owned by other controls.
 */
QItemModelScatterDataProxy::QItemModelScatterDataProxy(const QAbstractItemModel *itemModel,
                                                       QObject *parent)
    : QScatterDataProxy(new QItemModelScatterDataProxyPrivate(this), parent)
{
    dptr()->m_itemModelHandler->setItemModel(itemModel);
    dptr()->connectItemModelHandler();
}

/*!
 * Constructs QItemModelScatterDataProxy with \a itemModel and optional \a parent. Proxy doesn't take
 * ownership of the \a itemModel, as typically item models are owned by other controls.
 * The xPosRole property is set to \a xPosRole, yPosRole property to \a yPosRole, and zPosRole property
 * to \a zPosRole.
 */
QItemModelScatterDataProxy::QItemModelScatterDataProxy(const QAbstractItemModel *itemModel,
                                                       const QString &xPosRole,
                                                       const QString &yPosRole,
                                                       const QString &zPosRole,
                                                       QObject *parent)
    : QScatterDataProxy(new QItemModelScatterDataProxyPrivate(this), parent)
{
    dptr()->m_itemModelHandler->setItemModel(itemModel);
    dptr()->m_xPosRole = xPosRole;
    dptr()->m_yPosRole = yPosRole;
    dptr()->m_zPosRole = zPosRole;
    dptr()->connectItemModelHandler();
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
 * Defines the item model. Does not take ownership of the model, but does connect to it to listen for
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
 * \property QItemModelScatterDataProxy::xPosRole
 *
 * Defines the X position role for the mapping.
 */
void QItemModelScatterDataProxy::setXPosRole(const QString &role)
{
    if (dptr()->m_xPosRole != role) {
        dptr()->m_xPosRole = role;
        emit xPosRoleChanged(role);
    }
}

QString QItemModelScatterDataProxy::xPosRole() const
{
    return dptrc()->m_xPosRole;
}

/*!
 * \property QItemModelScatterDataProxy::yPosRole
 *
 * Defines the Y position role for the mapping.
 */
void QItemModelScatterDataProxy::setYPosRole(const QString &role)
{
    if (dptr()->m_yPosRole != role) {
        dptr()->m_yPosRole = role;
        emit yPosRoleChanged(role);
    }
}

QString QItemModelScatterDataProxy::yPosRole() const
{
    return dptrc()->m_yPosRole;
}

/*!
 * \property QItemModelScatterDataProxy::zPosRole
 *
 * Defines the Z position role for the mapping.
 */
void QItemModelScatterDataProxy::setZPosRole(const QString &role)
{
    if (dptr()->m_zPosRole != role) {
        dptr()->m_zPosRole = role;
        emit zPosRoleChanged(role);
    }
}

QString QItemModelScatterDataProxy::zPosRole() const
{
    return dptrc()->m_zPosRole;
}

/*!
 * Changes \a xPosRole, \a yPosRole and \a zPosRole mapping.
 */
void QItemModelScatterDataProxy::remap(const QString &xPosRole, const QString &yPosRole,
                                       const QString &zPosRole)
{
    setXPosRole(xPosRole);
    setYPosRole(yPosRole);
    setZPosRole(zPosRole);
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

void QItemModelScatterDataProxyPrivate::connectItemModelHandler()
{
    QObject::connect(m_itemModelHandler, &ScatterItemModelHandler::itemModelChanged,
                     qptr(), &QItemModelScatterDataProxy::itemModelChanged);
    QObject::connect(qptr(), &QItemModelScatterDataProxy::xPosRoleChanged,
                     m_itemModelHandler, &AbstractItemModelHandler::handleMappingChanged);
    QObject::connect(qptr(), &QItemModelScatterDataProxy::yPosRoleChanged,
                     m_itemModelHandler, &AbstractItemModelHandler::handleMappingChanged);
    QObject::connect(qptr(), &QItemModelScatterDataProxy::zPosRoleChanged,
                     m_itemModelHandler, &AbstractItemModelHandler::handleMappingChanged);
}

QT_END_NAMESPACE_DATAVISUALIZATION
