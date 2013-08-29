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

#include "qabstractdataproxy.h"
#include "qabstractdataproxy_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QAbstractDataProxy
 * \inmodule QtDataVis3D
 * \brief Base class for all QtDataVis3D data proxies.
 * \since 1.0.0
 *
 * You use the visualization type specific inherited classes instead of the base class.
 * \sa QBarDataProxy, QScatterDataProxy, QSurfaceDataProxy
 */

/*!
 * \enum QAbstractDataProxy::DataType
 *
 * Data type of the proxy.
 *
 * \value DataTypeNone
 *        No data type.
 * \value DataTypeBar
 *        Data type for Q3DBars.
 * \value DataTypeScatter
 *        Data type for Q3DScatter.
 * \value DataTypeSurface
 *        Data type for Q3DSurface.
 */

/*!
 * \internal
 */
QAbstractDataProxy::QAbstractDataProxy(QAbstractDataProxyPrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

/*!
 * Destroys QAbstractDataProxy.
 */
QAbstractDataProxy::~QAbstractDataProxy()
{
}

/*!
 * \property QAbstractDataProxy::type
 */
QAbstractDataProxy::DataType QAbstractDataProxy::type() const
{
    return d_ptr->m_type;
}

/*!
 * Sets label \a format for data items in this proxy. This format is used for single item labels,
 * e.g. when an item is selected. How the format is interpreted depends on proxy type. See
 * each proxy class documentation for more information.
 *
 * \sa QBarDataProxy, QScatterDataProxy, QSurfaceDataProxy
 */
void QAbstractDataProxy::setItemLabelFormat(const QString &format)
{
    d_ptr->setItemLabelFormat(format);
    emit itemLabelFormatChanged();
}

/*!
 * \return label format for data items in this proxy.
 */
QString QAbstractDataProxy::itemLabelFormat() const
{
    return d_ptr->m_itemLabelFormat;
}

/*!
 * \fn void QAbstractDataProxy::itemLabelFormatChanged()
 *
 * Emitted when label format changes.
 */

// QAbstractDataProxyPrivate

QAbstractDataProxyPrivate::QAbstractDataProxyPrivate(QAbstractDataProxy *q, QAbstractDataProxy::DataType type)
    : QObject(0),
      q_ptr(q),
      m_type(type),
      m_isDefaultProxy(false)
{
}

QAbstractDataProxyPrivate::~QAbstractDataProxyPrivate()
{
}

void QAbstractDataProxyPrivate::setItemLabelFormat(const QString &format)
{
    m_itemLabelFormat = format;
}

QT_DATAVIS3D_END_NAMESPACE
