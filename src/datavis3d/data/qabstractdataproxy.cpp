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

QAbstractDataProxy::QAbstractDataProxy(QAbstractDataProxyPrivate *d) :
    QObject(0),
    d_ptr(d)
{
}

QAbstractDataProxy::~QAbstractDataProxy()
{
}

QAbstractDataProxy::DataType QAbstractDataProxy::type() const
{
    return d_ptr->m_type;
}

void QAbstractDataProxy::setItemLabelFormat(const QString &format)
{
    d_ptr->setItemLabelFormat(format);
    emit itemLabelFormatChanged();
}

QString QAbstractDataProxy::itemLabelFormat() const
{
    return d_ptr->m_itemLabelFormat;
}

// QAbstractDataProxyPrivate

QAbstractDataProxyPrivate::QAbstractDataProxyPrivate(QAbstractDataProxy *q, QAbstractDataProxy::DataType type)
    : QObject(0),
      q_ptr(q),
      m_type(type)
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
