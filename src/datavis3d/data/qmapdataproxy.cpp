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

#include "qmapdataproxy.h"
#include "qmapdataproxy_p.h"
#include <QMutexLocker>

QT_DATAVIS3D_BEGIN_NAMESPACE

QMapDataProxy::QMapDataProxy() :
    QAbstractDataProxy(new QMapDataProxyPrivate(this))
{
}

QMapDataProxy::QMapDataProxy(QMapDataProxyPrivate *d) :
    QAbstractDataProxy(d)
{
}

QMapDataProxy::~QMapDataProxy()
{
}

void QMapDataProxy::resetArray(QMapDataArray *newArray)
{
    if (dptr()->resetArray(newArray))
        emit arrayReset();
}


// Mutexing data accessors should be done by user, if needed
int QMapDataProxy::itemCount() const
{
    return dptrc()->m_dataArray.size();
}

const QMapDataArray *QMapDataProxy::array() const
{
    return &dptrc()->m_dataArray;
}

const QMapDataItem *QMapDataProxy::itemAt(int index) const
{
    return &dptrc()->m_dataArray.at(index);
}

QMapDataProxyPrivate *QMapDataProxy::dptr()
{
    return static_cast<QMapDataProxyPrivate *>(d_ptr.data());
}

const QMapDataProxyPrivate *QMapDataProxy::dptrc() const
{
    return static_cast<const QMapDataProxyPrivate *>(d_ptr.data());
}

// QBarDataProxyPrivate

QMapDataProxyPrivate::QMapDataProxyPrivate(QMapDataProxy *q)
    : QAbstractDataProxyPrivate(q, QAbstractDataProxy::DataTypeMap)
{
}

QMapDataProxyPrivate::~QMapDataProxyPrivate()
{
    m_dataArray.clear();
}

bool QMapDataProxyPrivate::resetArray(QMapDataArray *newArray)
{
    QMutexLocker locker(&m_mutex);

    if (!m_dataArray.size() && (!newArray || !newArray->size()))
        return false;

    m_dataArray.clear();

    if (newArray) {
        m_dataArray = *newArray;
        delete newArray;
    }

    return true;
}

// Protected & private functions. Do not mutex as these are used from mutexed functions.

QPair<GLfloat, GLfloat> QMapDataProxyPrivate::limitValues()
{
    QMutexLocker locker(&m_mutex);
    QPair<GLfloat, GLfloat> limits = qMakePair(0.0f, 0.0f);
    for (int i = 0; i < m_dataArray.size(); i++) {
        const QMapDataItem &item = m_dataArray.at(i);
        qreal itemValue = item.value();
        if (limits.second < itemValue)
            limits.second = itemValue;
        if (limits.first > itemValue)
            limits.first = itemValue;
    }
    return limits;
}

QT_DATAVIS3D_END_NAMESPACE
