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

#include "qbardataitem_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QBarDataItem
 * \inmodule QtDataVisualization
 * \brief The QBarDataItem class provides a container for resolved data to be added to bar graphs.
 * \since Qt Data Visualization 1.0
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
      m_value(0.0)
{
}

/*!
 * Constructs QBarDataItem with \a value.
 */
QBarDataItem::QBarDataItem(qreal value)
    : d_ptr(0),
      m_value(value)
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
    if (other.d_ptr)
        createExtraData();
    else
        d_ptr = 0;
    // TODO set extra data
    return *this;
}

/*!
 * \fn void QBarDataItem::setValue(qreal value)
 * Sets \a value to this data item.
 */

/*!
 * \fn qreal QBarDataItem::value() const
 * \return value of this data item.
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

QT_DATAVISUALIZATION_END_NAMESPACE
