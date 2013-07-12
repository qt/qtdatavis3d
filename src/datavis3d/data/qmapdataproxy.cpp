/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
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
int QMapDataProxy::itemCount()
{
    return dptr()->m_dataArray.size();
}

const QMapDataArray &QMapDataProxy::array() const
{
    return dptrc()->m_dataArray;
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
    QPair<GLfloat, GLfloat> limits = qMakePair(100.0f, -100.0f);
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
