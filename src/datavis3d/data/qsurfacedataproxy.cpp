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

#include "qsurfacedataproxy.h"
#include "qsurfacedataproxy_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * Constructs QSurfaceDataProxy with the given \a parent.
 */
QSurfaceDataProxy::QSurfaceDataProxy(QObject *parent) :
    QAbstractDataProxy(new QSurfaceDataProxyPrivate(this), parent)
{
}

/*!
 * \internal
 */
QSurfaceDataProxy::QSurfaceDataProxy(QSurfaceDataProxyPrivate *d, QObject *parent) :
    QAbstractDataProxy(d, parent)
{
}

/*!
 * Destroys QSurfaceDataProxy.
 */
QSurfaceDataProxy::~QSurfaceDataProxy()
{
}

void QSurfaceDataProxy::resetArray(QSurfaceDataArray *newArray)
{
    if (dptr()->resetArray(newArray))
        emit arrayReset();
}

const QSurfaceDataArray *QSurfaceDataProxy::array() const
{
    return dptrc()->m_dataArray;
}

const QSurfaceDataItem *QSurfaceDataProxy::itemAt(int index) const
{
    return &dptrc()->m_dataArray->at(index)->at(2);
}

QSurfaceDataProxyPrivate *QSurfaceDataProxy::dptr()
{
    return static_cast<QSurfaceDataProxyPrivate *>(d_ptr.data());
}

const QSurfaceDataProxyPrivate *QSurfaceDataProxy::dptrc() const
{
    return static_cast<const QSurfaceDataProxyPrivate *>(d_ptr.data());
}

//
//  QSurfaceDataProxyPrivate
//

QSurfaceDataProxyPrivate::QSurfaceDataProxyPrivate(QSurfaceDataProxy *q)
    : QAbstractDataProxyPrivate(q, QAbstractDataProxy::DataTypeSurface),
      m_dataArray(new QSurfaceDataArray)
{
    m_itemLabelFormat = QStringLiteral("(@yLabel)");
}

QSurfaceDataProxyPrivate::~QSurfaceDataProxyPrivate()
{
    m_dataArray->clear();
    delete m_dataArray;
}

bool QSurfaceDataProxyPrivate::resetArray(QSurfaceDataArray *newArray)
{
    if (!m_dataArray->size() && (!newArray || !newArray->size()))
        return false;

    m_dataArray->clear();
    delete m_dataArray;

    if (newArray)
        m_dataArray = newArray;
    else
        m_dataArray = new QSurfaceDataArray;

    return true;
}

QT_DATAVIS3D_END_NAMESPACE
