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

#include "qscatterdataproxy.h"
#include "qscatterdataproxy_p.h"

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

void QScatterDataProxy::setItem(int index, const QScatterDataItem &item)
{
    dptr()->setItem(index, item);
    emit itemsChanged(index, 1);
}

void QScatterDataProxy::setItems(int index, const QScatterDataArray &items)
{
    dptr()->setItems(index, items);
    emit itemsChanged(index, items.size());
}

int QScatterDataProxy::addItem(const QScatterDataItem &item)
{
    int addIndex = dptr()->addItem(item);
    emit itemsAdded(addIndex, 1);
    return addIndex;
}

int QScatterDataProxy::addItems(const QScatterDataArray &items)
{
    int addIndex = dptr()->addItems(items);
    emit itemsAdded(addIndex, items.size());
    return addIndex;
}

void QScatterDataProxy::insertItem(int index, const QScatterDataItem &item)
{
    dptr()->insertItem(index, item);
    emit itemsInserted(index, 1);
}

void QScatterDataProxy::insertItems(int index, const QScatterDataArray &items)
{
    dptr()->insertItems(index, items);
    emit itemsInserted(index, items.size());
}

void QScatterDataProxy::removeItems(int index, int removeCount)
{
    dptr()->removeItems(index, removeCount);
    emit itemsRemoved(index, removeCount);
}

int QScatterDataProxy::itemCount() const
{
    return dptrc()->m_dataArray->size();
}

const QScatterDataArray *QScatterDataProxy::array() const
{
    return dptrc()->m_dataArray;
}

const QScatterDataItem *QScatterDataProxy::itemAt(int index) const
{
    return &dptrc()->m_dataArray->at(index);
}

QScatterDataProxyPrivate *QScatterDataProxy::dptr()
{
    return static_cast<QScatterDataProxyPrivate *>(d_ptr.data());
}

const QScatterDataProxyPrivate *QScatterDataProxy::dptrc() const
{
    return static_cast<const QScatterDataProxyPrivate *>(d_ptr.data());
}

// QScatterDataProxyPrivate

QScatterDataProxyPrivate::QScatterDataProxyPrivate(QScatterDataProxy *q)
    : QAbstractDataProxyPrivate(q, QAbstractDataProxy::DataTypeScatter),
      m_dataArray(new QScatterDataArray)
{
}

QScatterDataProxyPrivate::~QScatterDataProxyPrivate()
{
    m_dataArray->clear();
    delete m_dataArray;
}

bool QScatterDataProxyPrivate::resetArray(QScatterDataArray *newArray)
{
    if (!m_dataArray->size() && (!newArray || !newArray->size()))
        return false;

    m_dataArray->clear();
    delete m_dataArray;

    if (newArray)
        m_dataArray = newArray;
    else
        m_dataArray = new QScatterDataArray;

    return true;
}

void QScatterDataProxyPrivate::setItem(int index, const QScatterDataItem &item)
{
    Q_ASSERT(index >= 0 && index < m_dataArray->size());
    (*m_dataArray)[index] = item;
}

void QScatterDataProxyPrivate::setItems(int index, const QScatterDataArray &items)
{
    Q_ASSERT(index >= 0 && (index + items.size()) <= m_dataArray->size());
    for (int i = 0; i < items.size(); i++)
        (*m_dataArray)[index++] = items[i];
}

int QScatterDataProxyPrivate::addItem(const QScatterDataItem &item)
{
    int currentSize = m_dataArray->size();
    m_dataArray->append(item);
    return currentSize;
}

int QScatterDataProxyPrivate::addItems(const QScatterDataArray &items)
{
    int currentSize = m_dataArray->size();
    (*m_dataArray) += items;
    return currentSize;
}

void QScatterDataProxyPrivate::insertItem(int index, const QScatterDataItem &item)
{
    Q_ASSERT(index >= 0 && index <= m_dataArray->size());
    m_dataArray->insert(index, item);
}

void QScatterDataProxyPrivate::insertItems(int index, const QScatterDataArray &items)
{
    Q_ASSERT(index >= 0 && index <= m_dataArray->size());
    for (int i = 0; i < items.size(); i++)
        m_dataArray->insert(index++, items.at(i));
}

void QScatterDataProxyPrivate::removeItems(int index, int removeCount)
{
    Q_ASSERT(index >= 0);
    int maxRemoveCount = m_dataArray->size() - index;
    removeCount = qMin(removeCount, maxRemoveCount);
    m_dataArray->remove(index, removeCount);
}

QVector3D QScatterDataProxyPrivate::limitValues()
{
    QVector3D limits;
    for (int i = 0; i < m_dataArray->size(); i++) {
        const QScatterDataItem &item = m_dataArray->at(i);
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
