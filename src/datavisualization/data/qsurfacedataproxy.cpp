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

#include "qsurfacedataproxy.h"
#include "qsurfacedataproxy_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

// Default ranges correspond value axis defaults
const qreal defaultMinValue = 0.0;
const qreal defaultMaxValue = 10.0;

/*!
 * \class QSurfaceDataProxy
 * \inmodule QtDataVisualization
 * \brief Proxy class for Q3DSurface.
 * \since 1.0.0
 *
 * QSurfaceDataProxy takes care of surface related data handling.
 *
 * Make sure that amount of items on QSurfaceDataRow is the same for all rows.
 */

/*!
 * \qmltype SurfaceDataProxy
 * \instantiates QSurfaceDataProxy
 * \inherits AbstractDataProxy
 *
 * DOCUMENTATION GOES HERE TOO
 */

/*!
 * \qmlproperty int SurfaceDataProxy::itemCount
 * Item count in the array.
 */

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

/*!
 * Clears the existing array and takes ownership of the \a newArray. Do not use \a newArray pointer
 * to further modify data after QSurfaceDataProxy assumes ownership of it, as such modifications will
 * not trigger proper signals.
 * Passing null array clears all data.
 * Row and column ranges are reset to defaults.
 */
void QSurfaceDataProxy::resetArray(QSurfaceDataArray *newArray)
{
    if (dptr()->resetArray(newArray, defaultMinValue, defaultMaxValue, defaultMinValue, defaultMaxValue))
        emit arrayReset();
}

/*!
 * Clears the existing array and takes ownership of the \a newArray. Do not use \a newArray pointer
 * to further modify data after QSurfaceDataProxy assumes ownership of it, as such modifications will
 * not trigger proper signals.
 * Passing null array clears all data.
 * Row and column ranges are set to values defined by the rest of the parameters.
 */
void QSurfaceDataProxy::resetArray(QSurfaceDataArray *newArray, qreal minValueRows,
                                   qreal maxValueRows, qreal minValueColumns,
                                   qreal maxValueColumns)
{
    if (dptr()->resetArray(newArray, minValueRows, maxValueRows, minValueColumns, maxValueColumns))
        emit arrayReset();
}

/*!
 * \return pointer to the data array.
 */
const QSurfaceDataArray *QSurfaceDataProxy::array() const
{
    return dptrc()->m_dataArray;
}

/*!
 * \property QSurfaceDataProxy::rowCount
 *
 * \return number of rows in the data.
 */
int QSurfaceDataProxy::rowCount() const
{
    return dptrc()->m_dataArray->size();
}

/*!
 * \property QSurfaceDataProxy::columnCount
 *
 * \return number of items in the columns.
 */
int QSurfaceDataProxy::columnCount() const
{
    if (dptrc()->m_dataArray->size() > 0)
        return dptrc()->m_dataArray->at(0)->size();
    else
        return 0;
}

/*!
 * \return pointer to the item at \a index. It is guaranteed to be valid only until next call that
 * modifies data.
 */
const QSurfaceDataItem *QSurfaceDataProxy::itemAt(int index) const
{
    return &dptrc()->m_dataArray->at(index)->at(2);
}

/*!
 * \internal
 */
QSurfaceDataProxyPrivate *QSurfaceDataProxy::dptr()
{
    return static_cast<QSurfaceDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QSurfaceDataProxyPrivate *QSurfaceDataProxy::dptrc() const
{
    return static_cast<const QSurfaceDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \fn void QSurfaceDataProxy::arrayReset()
 *
 * Emitted when data array is reset.
 */

/*!
 * Sets the value range for rows from \a min to \a max.
 * When setting the range, the max is adjusted if necessary, to ensure that the range remains valid.
 */
void QSurfaceDataProxy::setValueRangeRows(qreal min, qreal max)
{
    dptr()->setValueRangeRows(min, max);
}

/*!
 * Sets the value range for columns from \a min to \a max.
 * When setting the range, the max is adjusted if necessary, to ensure that the range remains valid.
 */
void QSurfaceDataProxy::setValueRangeColumns(qreal min, qreal max)
{
    dptr()->setValueRangeColumns(min, max);
}

/*!
 * \property QSurfaceDataProxy::minValueRows
 *
 * Defines the minimum value of the range for rows.
 * When setting this property the max is adjusted if necessary, to ensure that the range remains
 * valid.
 */
void QSurfaceDataProxy::setMinValueRows(qreal min)
{
    dptr()->setMinValueRows(min);
}

qreal QSurfaceDataProxy::minValueRows() const
{
    return dptrc()->m_minValueRows;
}

/*!
 * \property QSurfaceDataProxy::maxValueRows
 *
 * Defines the maximum value of the range for rows.
 * When setting this property the min is adjusted if necessary, to ensure that the range remains
 * valid.
 */
void QSurfaceDataProxy::setMaxValueRows(qreal max)
{
    dptr()->setMaxValueRows(max);
}

qreal QSurfaceDataProxy::maxValueRows() const
{
    return dptrc()->m_maxValueRows;
}

/*!
 * \property QSurfaceDataProxy::minValueColumns
 *
 * Defines the minimum value of the range for columns.
 * When setting this property the min is adjusted if necessary, to ensure that the range remains
 * valid.
 */
void QSurfaceDataProxy::setMinValueColumns(qreal min)
{
    dptr()->setMinValueColumns(min);
}

qreal QSurfaceDataProxy::minValueColumns() const
{
    return dptrc()->m_minValueColumns;
}

/*!
 * \property QSurfaceDataProxy::maxValueColumns
 *
 * Defines the maximum value of the range for columns.
 * When setting this property the min is adjusted if necessary, to ensure that the range remains
 * valid.
 */
void QSurfaceDataProxy::setMaxValueColumns(qreal max)
{
    dptr()->setMaxValueColumns(max);
}

qreal QSurfaceDataProxy::maxValueColumns() const
{
    return dptrc()->m_maxValueColumns;
}


//
//  QSurfaceDataProxyPrivate
//

QSurfaceDataProxyPrivate::QSurfaceDataProxyPrivate(QSurfaceDataProxy *q)
    : QAbstractDataProxyPrivate(q, QAbstractDataProxy::DataTypeSurface),
      m_dataArray(new QSurfaceDataArray),
      m_minValueRows(defaultMinValue),
      m_maxValueRows(defaultMaxValue),
      m_minValueColumns(defaultMinValue),
      m_maxValueColumns(defaultMaxValue)
{
    m_itemLabelFormat = QStringLiteral("@yLabel (@xLabel, @zLabel)");
}

QSurfaceDataProxyPrivate::~QSurfaceDataProxyPrivate()
{
    m_dataArray->clear();
    delete m_dataArray;
}

bool QSurfaceDataProxyPrivate::resetArray(QSurfaceDataArray *newArray, qreal minValueRows,
                                          qreal maxValueRows, qreal minValueColumns,
                                          qreal maxValueColumns)
{
    if (!m_dataArray->size() && (!newArray || !newArray->size()))
        return false;

    m_dataArray->clear();
    delete m_dataArray;

    if (newArray) {
        for (int i = 0; i < newArray->size(); i++) {
            Q_ASSERT_X((newArray->at(i) && newArray->at(i)->size() == newArray->at(0)->size()),
                       __FUNCTION__,
                       "All rows of QSurfaceDataArray mustn't be NULL and must be of equal size.");
        }
        m_dataArray = newArray;
    } else {
        m_dataArray = new QSurfaceDataArray;
    }

    setValueRangeRows(minValueRows, maxValueRows);
    setValueRangeColumns(minValueColumns, maxValueColumns);

    return true;
}

void QSurfaceDataProxyPrivate::setValueRangeRows(qreal min, qreal max)
{
    bool changed = false;
    if (m_minValueRows != min) {
        m_minValueRows = min;
        changed = true;
    }
    if (m_maxValueRows != max || min >= max) {
        if (min >= max) {
            m_maxValueRows = min + 1.0;
            qWarning() << "Warning: Tried to set invalid range for value range."
                          " Range automatically adjusted to a valid one:"
                       << min << "-" << max << "-->" << m_minValueRows << "-" << m_maxValueRows;
        } else {
            m_maxValueRows = max;
        }
        changed = true;
    }

    if (changed)
        emit qptr()->valueRangeRowsChanged(min, max);
}

void QSurfaceDataProxyPrivate::setValueRangeColumns(qreal min, qreal max)
{
    bool changed = false;
    if (m_minValueColumns != min) {
        m_minValueColumns = min;
        changed = true;
    }
    if (m_maxValueColumns != max || min >= max) {
        if (min >= max) {
            m_maxValueColumns = min + 1.0;
            qWarning() << "Warning: Tried to set invalid range for value range."
                          " Range automatically adjusted to a valid one:"
                       << min << "-" << max << "-->" << m_minValueColumns << "-" << m_maxValueColumns;
        } else {
            m_maxValueColumns = max;
        }
        changed = true;
    }

    if (changed)
        emit qptr()->valueRangeColumnsChanged(min, max);
}

void QSurfaceDataProxyPrivate::setMinValueRows(qreal min)
{
    if (min != m_minValueRows) {
        if (min >= m_maxValueRows) {
            qreal oldMax = m_maxValueRows;
            m_maxValueRows = min + 1.0;
            qWarning() << "Warning: Tried to set minimum to equal or larger than maximum for"
                          " value range. Maximum automatically adjusted to a valid one:"
                       << oldMax <<  "-->" << m_maxValueRows;
        }
        m_minValueRows = min;
        emit qptr()->valueRangeRowsChanged(m_minValueRows, m_maxValueRows);
    }
}

void QSurfaceDataProxyPrivate::setMaxValueRows(qreal max)
{
    if (m_maxValueRows != max) {
        if (max <= m_minValueRows) {
            qreal oldMin = m_minValueRows;
            m_minValueRows = max - 1.0;
            qWarning() << "Warning: Tried to set maximum to equal or smaller than minimum for"
                          " value range. Minimum automatically adjusted to a valid one:"
                       << oldMin <<  "-->" << m_minValueRows;
        }
        m_maxValueRows = max;
        emit qptr()->valueRangeRowsChanged(m_minValueRows, m_maxValueRows);
    }
}

void QSurfaceDataProxyPrivate::setMinValueColumns(qreal min)
{
    if (min != m_minValueColumns) {
        if (min >= m_maxValueColumns) {
            qreal oldMax = m_maxValueColumns;
            m_maxValueColumns = min + 1.0;
            qWarning() << "Warning: Tried to set minimum to equal or larger than maximum for"
                          " value range. Maximum automatically adjusted to a valid one:"
                       << oldMax <<  "-->" << m_maxValueColumns;
        }
        m_minValueColumns = min;
        emit qptr()->valueRangeColumnsChanged(m_minValueColumns, m_maxValueRows);
    }
}

void QSurfaceDataProxyPrivate::setMaxValueColumns(qreal max)
{
    if (m_maxValueColumns != max) {
        if (max <= m_minValueColumns) {
            qreal oldMin = m_minValueColumns;
            m_minValueColumns = max - 1.0;
            qWarning() << "Warning: Tried to set maximum to equal or smaller than minimum for"
                          " value range. Minimum automatically adjusted to a valid one:"
                       << oldMin <<  "-->" << m_minValueColumns;
        }
        m_maxValueColumns = max;
        emit qptr()->valueRangeColumnsChanged(m_minValueColumns, m_maxValueColumns);
    }
}

QSurfaceDataProxy *QSurfaceDataProxyPrivate::qptr()
{
    return static_cast<QSurfaceDataProxy *>(q_ptr);
}

void QSurfaceDataProxyPrivate::limitValues(QVector3D &minValues, QVector3D &maxValues)
{
    qreal min = 0.0;
    qreal max = 0.0;

    int rows = m_dataArray->size();
    int columns = 0;
    if (rows)
        columns = m_dataArray->at(0)->size();

    if (rows && columns) {
        min = m_dataArray->at(0)->at(0);
        max = m_dataArray->at(0)->at(0);
    }

    for (int i = 0; i < rows; i++) {
        QSurfaceDataRow *row = m_dataArray->at(i);
        if (row) {
            for (int j = 0; j < columns; j++) {
                qreal itemValue = m_dataArray->at(i)->at(j);
                if (min > itemValue)
                    min = itemValue;
                if (max < itemValue)
                    max = itemValue;
            }
        }
    }

    minValues.setX(m_minValueColumns);
    minValues.setY(min);
    minValues.setZ(m_minValueRows);
    maxValues.setX(m_maxValueColumns);
    maxValues.setY(max);
    maxValues.setZ(m_maxValueRows);
}


QT_DATAVISUALIZATION_END_NAMESPACE
