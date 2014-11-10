/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#include "qbardataitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QBarDataItem
 * \inmodule QtDataVisualization
 * \brief The QBarDataItem class provides a container for resolved data to be added to bar graphs.
 * \since QtDataVisualization 1.0
 *
 * A QBarDataItem holds data for a single rendered bar in a graph.
 * Bar data proxies parse data into QBarDataItem instances for visualizing.
 *
 * \sa QBarDataProxy, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs QBarDataItem.
 */
QBarDataItem::QBarDataItem()
    : d_ptr(0), // private data doesn't exist by default (optimization)
      m_value(0.0f),
      m_angle(0.0f)
{
}

/*!
 * Constructs QBarDataItem with \a value.
 */
QBarDataItem::QBarDataItem(float value)
    : d_ptr(0),
      m_value(value),
      m_angle(0.0f)
{
}

/*!
 * Constructs QBarDataItem with \a value and \a angle
 */
QBarDataItem::QBarDataItem(float value, float angle)
    : d_ptr(0),
      m_value(value),
      m_angle(angle)
{
}

/*!
 * Constructs a copy of \a other.
 */
QBarDataItem::QBarDataItem(const QBarDataItem &other)
{
    operator=(other);
}

/*!
 * Destroys QBarDataItem.
 */
QBarDataItem::~QBarDataItem()
{
    delete d_ptr;
}

/*!
 *  Assigns a copy of \a other to this object.
 */
QBarDataItem &QBarDataItem::operator=(const QBarDataItem &other)
{
    m_value = other.m_value;
    m_angle = other.m_angle;
    if (other.d_ptr)
        createExtraData();
    else
        d_ptr = 0;
    return *this;
}

/*!
 * \fn void QBarDataItem::setValue(float value)
 * Sets \a value to this data item.
 */

/*!
 * \fn float QBarDataItem::value() const
 * \return value of this data item.
 */

/*!
 * \fn void QBarDataItem::setRotation(float angle)
 * Sets rotation \a angle in degrees for this data item.
 */

/*!
 * \fn float QBarDataItem::rotation() const
 * \return rotation angle in degrees for this data item.
 */

/*!
 * \internal
 */
void QBarDataItem::createExtraData()
{
    if (!d_ptr)
        d_ptr = new QBarDataItemPrivate;
}

QBarDataItemPrivate::QBarDataItemPrivate()
{
}

QBarDataItemPrivate::~QBarDataItemPrivate()
{
}

QT_END_NAMESPACE_DATAVISUALIZATION
