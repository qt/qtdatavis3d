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

/*!
 * \class QSurfaceDataProxy
 * \inmodule QtDataVisualization
 * \brief Base proxy class for Q3DSurface.
 * \since Qt Data Visualization 1.0
 *
 * QSurfaceDataProxy takes care of surface related data handling. The QSurfaceDataProxy handles the data
 * in rows and for this it provides two auxiliary typedefs. QSurfaceDataArray is a QList for
 * controlling the rows. For rows there is a QVector QSurfaceDataRow which contains QSurfaceDataItem
 * objects. See Q3DSurface documentation and basic sample code there how to feed the data for the
 * QSurfaceDataProxy.
 *
 * All rows must have the same number of items.
 *
 * When determining what rows and columns are visible, the first item in each row and the first item in
 * each column determine if the whole row or column is visible, even if other items in the row or column
 * individually have different X- or Z-coordinates.
 *
 * \note Surfaces with less than two rows or columns are not considered valid surfaces and will
 * not get rendered.
 *
 * QSurfaceDataProxy supports the following format tags for QAbstractDataProxy::setItemLabelFormat():
 * \table
 *   \row
 *     \li @xTitle    \li Title from X axis
 *   \row
 *     \li @yTitle    \li Title from Y axis
 *   \row
 *     \li @zTitle    \li Title from Z axis
 *   \row
 *     \li @xLabel    \li Item value formatted using the same format as the X axis attached to the graph uses,
 *                            see \l{Q3DValueAxis::setLabelFormat()} for more information.
 *   \row
 *     \li @yLabel    \li Item value formatted using the same format as the Y axis attached to the graph uses,
 *                            see \l{Q3DValueAxis::setLabelFormat()} for more information.
 *   \row
 *     \li @zLabel    \li Item value formatted using the same format as the Z axis attached to the graph uses,
 *                            see \l{Q3DValueAxis::setLabelFormat()} for more information.
 * \endtable
 *
 * \sa {Qt Data Visualization Data Handling}
 */

/*!
 * \qmltype SurfaceDataProxy
 * \inqmlmodule com.digia.QtDataVisualization
 * \since com.digia.QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QSurfaceDataProxy
 * \inherits AbstractDataProxy
 * \brief Base proxy class for Surface3D.
 *
 * This type handles surface data items. The data is arranged into rows and columns, and all rows must have
 * the same number of columns.
 *
 * This type is uncreatable, but contains properties that are exposed via subtypes.
 *
 * For a more complete description, see QSurfaceDataProxy.
 *
 * \sa ItemModelSurfaceDataProxy, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmlproperty int SurfaceDataProxy::rowCount
 * Number of the rows in the array.
 */

/*!
 * \qmlproperty int SurfaceDataProxy::columnCount
 * Number of the columns in the array.
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
 * Takes ownership of the \a newArray. Clears the existing array if the \a newArray is
 * different from the existing array. If it's the same array, this just triggers arrayReset()
 * signal.
 * Passing a null array deletes the old array and creates a new empty array.
 * All rows in \a newArray must be of same length.
 */
void QSurfaceDataProxy::resetArray(QSurfaceDataArray *newArray)
{
    if (dptr()->m_dataArray != newArray) {
        dptr()->resetArray(newArray);
    }
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
 * \return pointer to the item at \a index. It is guaranteed to be valid only until the next call that
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
 * Emitted when the data array is reset.
 * If you change the whole array contents without calling resetArray(), you need to
 * emit this signal yourself or the graph won't get updated.
 */

//
//  QSurfaceDataProxyPrivate
//

QSurfaceDataProxyPrivate::QSurfaceDataProxyPrivate(QSurfaceDataProxy *q)
    : QAbstractDataProxyPrivate(q, QAbstractDataProxy::DataTypeSurface),
      m_dataArray(new QSurfaceDataArray)
{
    m_itemLabelFormat = QStringLiteral("@yLabel (@xLabel, @zLabel)");
}

QSurfaceDataProxyPrivate::~QSurfaceDataProxyPrivate()
{
    clearArray();
}

void QSurfaceDataProxyPrivate::resetArray(QSurfaceDataArray *newArray)
{
    if (!newArray)
        newArray = new QSurfaceDataArray;

    if (newArray != m_dataArray) {
        clearArray();
        m_dataArray = newArray;
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
        min = m_dataArray->at(0)->at(0).y();
        max = m_dataArray->at(0)->at(0).y();
    }

    for (int i = 0; i < rows; i++) {
        QSurfaceDataRow *row = m_dataArray->at(i);
        if (row) {
            for (int j = 0; j < columns; j++) {
                qreal itemValue = m_dataArray->at(i)->at(j).y();
                if (min > itemValue)
                    min = itemValue;
                if (max < itemValue)
                    max = itemValue;
            }
        }
    }

    minValues.setY(min);
    maxValues.setY(max);
    if (columns) {
        minValues.setX(m_dataArray->at(0)->at(0).x());
        minValues.setZ(m_dataArray->at(0)->at(0).z());
        maxValues.setX(m_dataArray->at(0)->last().x());
        maxValues.setZ(m_dataArray->last()->at(0).z());
    } else {
        minValues.setX(0.0f);
        minValues.setZ(0.0f);
        maxValues.setX(0.0f);
        maxValues.setZ(0.0f);
    }
}

void QSurfaceDataProxyPrivate::clearRow(int rowIndex)
{
    if (m_dataArray->at(rowIndex)) {
        delete m_dataArray->at(rowIndex);
        (*m_dataArray)[rowIndex] = 0;
    }
}

void QSurfaceDataProxyPrivate::clearArray()
{
    for (int i = 0; i < m_dataArray->size(); i++)
        clearRow(i);
    m_dataArray->clear();
    delete m_dataArray;
}

QT_DATAVISUALIZATION_END_NAMESPACE
