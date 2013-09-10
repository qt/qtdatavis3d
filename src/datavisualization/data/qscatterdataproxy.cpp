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

#include "qscatterdataproxy.h"
#include "qscatterdataproxy_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QScatterDataProxy
 * \inmodule QtDataVisualization
 * \brief Proxy class for Q3DScatter.
 * \since 1.0.0
 *
 * QScatterDataProxy handles adding, inserting, changing and removing data items.
 *
 * QScatterDataProxy takes ownership of all QScatterDataArrays and QScatterDataItems passed to it.
 *
 * QScatterDataProxy supports the following format tags for QAbstractDataProxy::setItemLabelFormat():
 * \table
 *   \row
 *     \li @xTitle    \li Title from X axis
 *   \row
 *     \li @yTitle    \li Title from Y axis
 *   \row
 *     \li @zTitle    \li Title from Z axis
 *   \row
 *     \li @xLabel    \li Item value formatted using the same format the X axis attached to the graph uses,
 *                            see \l{Q3DValueAxis::setLabelFormat()} for more information.
 *   \row
 *     \li @yLabel    \li Item value formatted using the same format the Y axis attached to the graph uses,
 *                            see \l{Q3DValueAxis::setLabelFormat()} for more information.
 *   \row
 *     \li @zLabel    \li Item value formatted using the same format the Z axis attached to the graph uses,
 *                            see \l{Q3DValueAxis::setLabelFormat()} for more information.
 * \endtable
 *
 * For example:
 * \snippet doc_src_qtdatavisualization.cpp 2
 */

/*!
 * Constructs QScatterDataProxy with the given \a parent.
 */
QScatterDataProxy::QScatterDataProxy(QObject *parent) :
    QAbstractDataProxy(new QScatterDataProxyPrivate(this), parent)
{
}

/*!
 * \internal
 */
QScatterDataProxy::QScatterDataProxy(QScatterDataProxyPrivate *d, QObject *parent) :
    QAbstractDataProxy(d, parent)
{
}

/*!
 * Destroys QScatterDataProxy.
 */
QScatterDataProxy::~QScatterDataProxy()
{
}

/*!
 * Clears the existing array and takes ownership of the \a newArray. Do not use \a newArray pointer
 * to further modify data after QScatterDataProxy assumes ownership of it, as such modifications will
 * not trigger proper signals.
 * Passing null array clears all data.
 */
void QScatterDataProxy::resetArray(QScatterDataArray *newArray)
{
    if (dptr()->resetArray(newArray))
        emit arrayReset();
}

/*!
 * Changes a single item at \a index with \a item.
 */
void QScatterDataProxy::setItem(int index, const QScatterDataItem &item)
{
    dptr()->setItem(index, item);
    emit itemsChanged(index, 1);
}

/*!
 * Changes items starting from \a index with \a items.
 */
void QScatterDataProxy::setItems(int index, const QScatterDataArray &items)
{
    dptr()->setItems(index, items);
    emit itemsChanged(index, items.size());
}

/*!
 * Adds a single \a item to the end of the array.
 *
 * \return index of the added item.
 */
int QScatterDataProxy::addItem(const QScatterDataItem &item)
{
    int addIndex = dptr()->addItem(item);
    emit itemsAdded(addIndex, 1);
    return addIndex;
}

/*!
 * Adds \a items to the end of the array.
 *
 * \return index of the first added item.
 */
int QScatterDataProxy::addItems(const QScatterDataArray &items)
{
    int addIndex = dptr()->addItems(items);
    emit itemsAdded(addIndex, items.size());
    return addIndex;
}

/*!
 * Inserts a single \a item to \a index. If index is equal to data array size, item is added to
 * the array.
 */
void QScatterDataProxy::insertItem(int index, const QScatterDataItem &item)
{
    dptr()->insertItem(index, item);
    emit itemsInserted(index, 1);
}

/*!
 * Inserts \a items to \a index. If index is equal to data array size, items are added to the array.
 */
void QScatterDataProxy::insertItems(int index, const QScatterDataArray &items)
{
    dptr()->insertItems(index, items);
    emit itemsInserted(index, items.size());
}

/*!
 * Removes \a removeCount items starting from \a index. Attempting to remove items past the end of
 * the array does nothing.
 */
void QScatterDataProxy::removeItems(int index, int removeCount)
{
    dptr()->removeItems(index, removeCount);
    emit itemsRemoved(index, removeCount);
}

/*!
 * \return item count in the array.
 */
int QScatterDataProxy::itemCount() const
{
    return dptrc()->m_dataArray->size();
}

/*!
 * \return pointer to the data array.
 */
const QScatterDataArray *QScatterDataProxy::array() const
{
    return dptrc()->m_dataArray;
}

/*!
 * \return pointer to the item at \a index. It is guaranteed to be valid only until next call
 * that modifies data.
 */
const QScatterDataItem *QScatterDataProxy::itemAt(int index) const
{
    return &dptrc()->m_dataArray->at(index);
}

/*!
 * \internal
 */
QScatterDataProxyPrivate *QScatterDataProxy::dptr()
{
    return static_cast<QScatterDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QScatterDataProxyPrivate *QScatterDataProxy::dptrc() const
{
    return static_cast<const QScatterDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \fn void QScatterDataProxy::arrayReset()
 *
 * Emitted when data array is reset.
 */

/*!
 * \fn void QScatterDataProxy::itemsAdded(int startIndex, int count)
 *
 * Emitted when items have been added. Provides \a startIndex and \a count of items added.
 */

/*!
 * \fn void QScatterDataProxy::itemsChanged(int startIndex, int count)
 *
 * Emitted when items have changed. Provides \a startIndex and \a count of changed items.
 */

/*!
 * \fn void QScatterDataProxy::itemsRemoved(int startIndex, int count)
 *
 * Emitted when items have been removed. Provides \a startIndex and \a count of items removed.
 * Index may be over current array size if removed from end.
 */

/*!
 * \fn void QScatterDataProxy::itemsInserted(int startIndex, int count)
 *
 * Emitted when items have been inserted. Provides \a startIndex and \a count of inserted items.
 */

// QScatterDataProxyPrivate

QScatterDataProxyPrivate::QScatterDataProxyPrivate(QScatterDataProxy *q)
    : QAbstractDataProxyPrivate(q, QAbstractDataProxy::DataTypeScatter),
      m_dataArray(new QScatterDataArray)
{
    m_itemLabelFormat = QStringLiteral("(@xLabel, @yLabel, @zLabel)");
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
    return limits;
}

QT_DATAVISUALIZATION_END_NAMESPACE
