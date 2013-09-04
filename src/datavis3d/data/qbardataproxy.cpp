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

#include "qbardataproxy.h"
#include "qbardataproxy_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QBarDataProxy
 * \inmodule QtDataVis3D
 * \brief Proxy class for Q3DBars.
 * \since 1.0.0
 *
 * QBarDataProxy handles adding, inserting, changing and removing rows of data.
 *
 * The data array is a list of vectors (rows) of QBarDataItem instances.
 * Each row can contain different amount of items or even be null.
 *
 * QBarDataProxy takes ownership of all QBarDataRows passed to it, whether directly or
 * in a QBarDataArray container.
 * QBarDataRow pointers should not be used to modify data further after they have been passed to
 * the proxy, as such modifications will not trigger proper signals.
 *
 * QBarDataProxy supports the following format tags for QAbstractDataProxy::setItemLabelFormat():
 * \table
 *   \row
 *     \li @rowTitle      \li Title from row axis
 *   \row
 *     \li @colTitle      \li Title from column axis
 *   \row
 *     \li @valueTitle    \li Title from value axis
 *   \row
 *     \li @rowIdx        \li Visible row index
 *   \row
 *     \li @colIdx        \li Visible Column index
 *   \row
 *     \li @rowLabel      \li Label from row axis
 *   \row
 *     \li @colLabel      \li Label from column axis
 *   \row
 *     \li @valueLabel    \li Item value formatted using the same format the value axis attached to the graph uses,
 *                            see \l{Q3DValueAxis::setLabelFormat()} for more information.
 *   \row
 *     \li %<format spec> \li Item value in specified format.
 * \endtable
 *
 * For example:
 * \snippet doc_src_qtdatavis3d.cpp 1
 */

/*!
 * Constructs QBarDataProxy with the given \a parent.
 */
QBarDataProxy::QBarDataProxy(QObject *parent) :
    QAbstractDataProxy(new QBarDataProxyPrivate(this), parent)
{
}

/*!
 * \internal
 */
QBarDataProxy::QBarDataProxy(QBarDataProxyPrivate *d, QObject *parent) :
    QAbstractDataProxy(d, parent)
{
}

/*!
 * Destroys QBarDataProxy.
 */
QBarDataProxy::~QBarDataProxy()
{
}

/*!
 * Clears the existing array and takes ownership of the \a newArray. Do not use \a newArray pointer
 * to further modify data after QBarDataProxy assumes ownership of it, as such modifications will
 * not trigger proper signals.
 * Passing null array clears all data.
 */
void QBarDataProxy::resetArray(QBarDataArray *newArray)
{
    if (dptr()->resetArray(newArray))
        emit arrayReset();
}

/*!
 * Changes existing rows by replacing a row at \a rowIndex with \a row.
 */
void QBarDataProxy::setRow(int rowIndex, QBarDataRow *row)
{
    dptr()->setRow(rowIndex, row);
    emit rowsChanged(rowIndex, 1);
}

/*!
 * Changes existing rows by replacing a rows starting at \a rowIndex with \a rows.
 */
void QBarDataProxy::setRows(int rowIndex, const QBarDataArray &rows)
{
    dptr()->setRows(rowIndex, rows);
    emit rowsChanged(rowIndex, rows.size());
}

/*!
 * Changes a single item at \a rowIndex, \a columnIndex with \a item.
 */
void QBarDataProxy::setItem(int rowIndex, int columnIndex, const QBarDataItem &item)
{
    dptr()->setItem(rowIndex, columnIndex, item);
    emit itemChanged(rowIndex, columnIndex);
}

/*!
 * Adds a new \a row to the end of array.
 *
 * \return index of the added row.
 */
int QBarDataProxy::addRow(QBarDataRow *row)
{
    int addIndex = dptr()->addRow(row);
    emit rowsAdded(addIndex, 1);
    return addIndex;
}

/*!
 * Adds new \a rows to the end of array.
 *
 * \return index of the first added row.
 */
int QBarDataProxy::addRows(const QBarDataArray &rows)
{
    int addIndex = dptr()->addRows(rows);
    emit rowsAdded(addIndex, rows.size());
    return addIndex;
}

/*!
 * Inserts a new \a row to \a rowIndex. If rowIndex is equal to array size, rows are added to end
 * of the array.
 */
void QBarDataProxy::insertRow(int rowIndex, QBarDataRow *row)
{
    dptr()->insertRow(rowIndex, row);
    emit rowsInserted(rowIndex, 1);
}

/*!
 * Inserts new \a rows to \a rowIndex. If rowIndex is equal to array size, rows are added to end
 * of the array.
 */
void QBarDataProxy::insertRows(int rowIndex, const QBarDataArray &rows)
{
    dptr()->insertRows(rowIndex, rows);
    emit rowsInserted(rowIndex, rows.size());
}

/*!
 * Removes \a removeCount rows staring at \a rowIndex. Attempting to remove rows past the end of the
 * array does nothing.
 */
void QBarDataProxy::removeRows(int rowIndex, int removeCount)
{
    if (rowIndex < rowCount()) {
        dptr()->removeRows(rowIndex, removeCount);
        emit rowsRemoved(rowIndex, removeCount);
    }
}

/*!
 * \return row count in the array.
 */
int QBarDataProxy::rowCount() const
{
    return dptrc()->m_dataArray->size();
}

/*!
 * \return pointer to the data array.
 */
const QBarDataArray *QBarDataProxy::array() const
{
    return dptrc()->m_dataArray;
}

/*!
 * \return pointer to the row at \a rowIndex. It is guaranteed to be valid only until next call
 * that modifies data.
 */
const QBarDataRow *QBarDataProxy::rowAt(int rowIndex) const
{
    const QBarDataArray &dataArray = *dptrc()->m_dataArray;
    Q_ASSERT(rowIndex >= 0 && rowIndex < dataArray.size());
    return dataArray[rowIndex];
}

/*!
 * \return pointer to the item at \a rowIndex, \a columnIndex. It is guaranteed to be valid only
 * until next call that modifies data.
 */
const QBarDataItem *QBarDataProxy::itemAt(int rowIndex, int columnIndex) const
{
    const QBarDataArray &dataArray = *dptrc()->m_dataArray;
    Q_ASSERT(rowIndex >= 0 && rowIndex < dataArray.size());
    const QBarDataRow &dataRow = *dataArray[rowIndex];
    Q_ASSERT(columnIndex >= 0 && columnIndex < dataRow.size());
    return &dataRow.at(columnIndex);
}

/*!
 * \internal
 */
QBarDataProxyPrivate *QBarDataProxy::dptr()
{
    return static_cast<QBarDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QBarDataProxyPrivate *QBarDataProxy::dptrc() const
{
    return static_cast<const QBarDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \fn void QBarDataProxy::arrayReset()
 *
 * Emitted when data array is reset.
 */

/*!
 * \fn void QBarDataProxy::rowsAdded(int startIndex, int count)
 *
 * Emitted when rows have been added. Provides \a startIndex and \a count of rows added.
 */

/*!
 * \fn void QBarDataProxy::rowsChanged(int startIndex, int count)
 *
 * Emitted when rows have changed. Provides \a startIndex and \a count of changed rows.
 */

/*!
 * \fn void QBarDataProxy::rowsRemoved(int startIndex, int count)
 *
 * Emitted when rows have been removed. Provides \a startIndex and \a count of rows removed.
 * Index is the current array size if rows were removed from the end of the array.
 */

/*!
 * \fn void QBarDataProxy::rowsInserted(int startIndex, int count)
 *
 * Emitted when rows have been inserted. Provides \a startIndex and \a count of inserted rows.
 */

/*!
 * \fn void QBarDataProxy::itemChanged(int rowIndex, int columnIndex)
 *
 * Emitted when an item has changed. Provides \a rowIndex and \a columnIndex of changed item.
 */

// QBarDataProxyPrivate

QBarDataProxyPrivate::QBarDataProxyPrivate(QBarDataProxy *q)
    : QAbstractDataProxyPrivate(q, QAbstractDataProxy::DataTypeBar),
      m_dataArray(new QBarDataArray)
{
    m_itemLabelFormat = QStringLiteral("@valueTitle: @valueLabel");
}

QBarDataProxyPrivate::~QBarDataProxyPrivate()
{
    clearArray();
}

bool QBarDataProxyPrivate::resetArray(QBarDataArray *newArray)
{
    if (!m_dataArray->size() && (!newArray || !newArray->size()))
        return false;

    clearArray();

    if (newArray)
        m_dataArray = newArray;
    else
        m_dataArray = new QBarDataArray;

    return true;
}

void QBarDataProxyPrivate::setRow(int rowIndex, QBarDataRow *row)
{
    Q_ASSERT(rowIndex >= 0 && rowIndex < m_dataArray->size());
    clearRow(rowIndex);
    (*m_dataArray)[rowIndex] = row;
}

void QBarDataProxyPrivate::setRows(int rowIndex, const QBarDataArray &rows)
{
    QBarDataArray &dataArray = *m_dataArray;
    Q_ASSERT(rowIndex >= 0 && (rowIndex + rows.size()) <= dataArray.size());
    for (int i = 0; i < rows.size(); i++) {
        clearRow(rowIndex);
        dataArray[rowIndex] = rows.at(i);
        rowIndex++;
    }
}

void QBarDataProxyPrivate::setItem(int rowIndex, int columnIndex, const QBarDataItem &item)
{
    Q_ASSERT(rowIndex >= 0 && rowIndex < m_dataArray->size());
    QBarDataRow &row = *(*m_dataArray)[rowIndex];
    Q_ASSERT(columnIndex < row.size());
    row[columnIndex] = item;
}

int QBarDataProxyPrivate::addRow(QBarDataRow *row)
{
    int currentSize = m_dataArray->size();
    m_dataArray->append(row);
    return currentSize;
}

int QBarDataProxyPrivate::addRows(const QBarDataArray &rows)
{
    int currentSize = m_dataArray->size();
    for (int i = 0; i < rows.size(); i++)
        m_dataArray->append(rows.at(i));
    return currentSize;
}

void QBarDataProxyPrivate::insertRow(int rowIndex, QBarDataRow *row)
{
    Q_ASSERT(rowIndex >= 0 && rowIndex <= m_dataArray->size());
    m_dataArray->insert(rowIndex, row);
}

void QBarDataProxyPrivate::insertRows(int rowIndex, const QBarDataArray &rows)
{
    Q_ASSERT(rowIndex >= 0 && rowIndex <= m_dataArray->size());
    for (int i = 0; i < rows.size(); i++)
        m_dataArray->insert(rowIndex++, rows.at(i));
}

void QBarDataProxyPrivate::removeRows(int rowIndex, int removeCount)
{
    Q_ASSERT(rowIndex >= 0);
    int maxRemoveCount = m_dataArray->size() - rowIndex;
    removeCount = qMin(removeCount, maxRemoveCount);
    for (int i = 0; i < removeCount; i++) {
        clearRow(rowIndex);
        m_dataArray->removeAt(rowIndex);
    }
}

void QBarDataProxyPrivate::clearRow(int rowIndex)
{
    if (m_dataArray->at(rowIndex)) {
        delete m_dataArray->at(rowIndex);
        (*m_dataArray)[rowIndex] = 0;
    }
}

void QBarDataProxyPrivate::clearArray()
{
    for (int i = 0; i < m_dataArray->size(); i++)
        clearRow(i);
    m_dataArray->clear();
    delete m_dataArray;
}

QPair<GLfloat, GLfloat> QBarDataProxyPrivate::limitValues(int startRow, int endRow, int startColumn, int endColumn)
{
    QPair<GLfloat, GLfloat> limits = qMakePair(0.0f, 0.0f);
    endRow = qMin(endRow, m_dataArray->size() - 1);
    for (int i = startRow; i <= endRow; i++) {
        QBarDataRow *row = m_dataArray->at(i);
        if (row) {
            endColumn = qMin(endColumn, row->size() - 1);
            for (int j = startColumn; j <= endColumn; j++) {
                const QBarDataItem &item = m_dataArray->at(i)->at(j);
                qreal itemValue = item.value();
                if (limits.second < itemValue)
                    limits.second = itemValue;
                if (limits.first > itemValue)
                    limits.first = itemValue;
            }
        }
    }
    return limits;
}

QT_DATAVIS3D_END_NAMESPACE
