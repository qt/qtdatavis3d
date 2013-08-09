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

#include "qscatterdataproxy.h"
#include "qscatterdataproxy_p.h"
#include <QMutexLocker>

QT_DATAVIS3D_BEGIN_NAMESPACE

QScatterDataProxy::QScatterDataProxy() :
    QAbstractDataProxy(new QScatterDataProxyPrivate(this))
{
}

QScatterDataProxy::QScatterDataProxy(QScatterDataProxyPrivate *d) :
    QAbstractDataProxy(d)
{
}

QScatterDataProxy::~QScatterDataProxy()
{
}

void QScatterDataProxy::resetArray(QScatterDataArray *newArray)
{
    if (dptr()->resetArray(newArray))
        emit arrayReset();
}

// Mutexing data accessors should be done by user, if needed
int QScatterDataProxy::itemCount()
{
    return dptr()->m_dataArray.size();
}

const QScatterDataArray *QScatterDataProxy::array() const
{
    return &dptrc()->m_dataArray;
}

const QScatterDataItem *QScatterDataProxy::itemAt(int index) const
{
    return &dptrc()->m_dataArray.at(index);
}

QScatterDataProxyPrivate *QScatterDataProxy::dptr()
{
    return static_cast<QScatterDataProxyPrivate *>(d_ptr.data());
}

const QScatterDataProxyPrivate *QScatterDataProxy::dptrc() const
{
    return static_cast<const QScatterDataProxyPrivate *>(d_ptr.data());
}

// QBarDataProxyPrivate

QScatterDataProxyPrivate::QScatterDataProxyPrivate(QScatterDataProxy *q)
    : QAbstractDataProxyPrivate(q, QAbstractDataProxy::DataTypeMap)
{
}

QScatterDataProxyPrivate::~QScatterDataProxyPrivate()
{
    m_dataArray.clear();
}

bool QScatterDataProxyPrivate::resetArray(QScatterDataArray *newArray)
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

QVector3D QScatterDataProxyPrivate::limitValues()
{
    QMutexLocker locker(&m_mutex);
    QVector3D limits;
    for (int i = 0; i < m_dataArray.size(); i++) {
        const QScatterDataItem &item = m_dataArray.at(i);
        float xValue = qAbs(item.position().x());
        if (limits.x() < xValue)
            limits.setX(xValue);
        float yValue = qAbs(item.position().y());
        if (limits.y() < yValue)
            limits.setY(yValue);
        float zValue = qAbs(item.position().z());
        if (limits.z() < zValue)
            limits.setZ(zValue);
    }
    //qDebug() << __FUNCTION__ << limits << m_dataArray.size();
    return limits;
}

QT_DATAVIS3D_END_NAMESPACE
