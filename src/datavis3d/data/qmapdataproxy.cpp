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

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QMapDataProxy
 * \inmodule QtDataVis3D
 * \brief Proxy class for Q3DMaps.
 * \since 1.0.0
 *
 * QMapDataProxy handles adding, inserting, changing and removing data.
 *
 * QMapDataProxy takes ownership of all QMapDataArrays and QMapDataItems passed to it.
 */

/*!
 * Constructs QMapDataProxy with the given \a parent.
 */
QMapDataProxy::QMapDataProxy(QObject *parent) :
    QAbstractDataProxy(new QMapDataProxyPrivate(this), parent)
{
}

/*!
 * \internal
 */
QMapDataProxy::QMapDataProxy(QMapDataProxyPrivate *d, QObject *parent) :
    QAbstractDataProxy(d, parent)
{
}

/*!
 * Destroys QMapDataProxy.
 */
QMapDataProxy::~QMapDataProxy()
{
}

/*!
 * Clears the existing array and takes ownership of the \a newArray. Do not use \a newArray pointer
 * to further modify data after QMapDataProxy assumes ownership of it, as such modifications will
 * not trigger proper signals.
 * Passing null array clears all data.
 */
void QMapDataProxy::resetArray(QMapDataArray *newArray)
{
    if (dptr()->resetArray(newArray))
        emit arrayReset();
}

/*!
 * \return item count in the array.
 */
int QMapDataProxy::itemCount() const
{
    return dptrc()->m_dataArray.size();
}

/*!
 * \return pointer to the data array.
 */
const QMapDataArray *QMapDataProxy::array() const
{
    return &dptrc()->m_dataArray;
}

/*!
 * \return pointer to the item at \a index. It is guaranteed to be valid only until next call
 * that modifies data.
 */
const QMapDataItem *QMapDataProxy::itemAt(int index) const
{
    return &dptrc()->m_dataArray.at(index);
}

/*!
 * \internal
 */
QMapDataProxyPrivate *QMapDataProxy::dptr()
{
    return static_cast<QMapDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QMapDataProxyPrivate *QMapDataProxy::dptrc() const
{
    return static_cast<const QMapDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \fn void QMapDataProxy::arrayReset()
 *
 * Emitted when data array is reset.
 */

/*!
 * \fn void QMapDataProxy::itemsAdded(int startIndex, int count)
 *
 * Emitted when items have been added. Provides \a startIndex and \a count of items added.
 */

/*!
 * \fn void QMapDataProxy::itemsChanged(int startIndex, int count)
 *
 * Emitted when items have changed. Provides \a startIndex and \a count of changed items.
 */

/*!
 * \fn void QMapDataProxy::itemsRemoved(int startIndex, int count)
 *
 * Emitted when items have been removed. Provides \a startIndex and \a count of items removed.
 * Index may be over current array size if removed from end.
 */

/*!
 * \fn void QMapDataProxy::itemsInserted(int startIndex, int count)
 *
 * Emitted when items have been inserted. Provides \a startIndex and \a count of inserted items.
 */

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
    if (!m_dataArray.size() && (!newArray || !newArray->size()))
        return false;

    m_dataArray.clear();

    if (newArray) {
        m_dataArray = *newArray;
        delete newArray;
    }

    return true;
}

QPair<GLfloat, GLfloat> QMapDataProxyPrivate::limitValues()
{
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
