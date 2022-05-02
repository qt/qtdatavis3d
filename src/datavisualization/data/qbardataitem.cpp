/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
******************************************************************************/

#include "qbardataitem_p.h"

QT_BEGIN_NAMESPACE

/*!
 * \class QBarDataItem
 * \inmodule QtDataVisualization
 * \brief The QBarDataItem class provides a container for resolved data to be added to bar graphs.
 * \since QtDataVisualization 1.0
 *
 * A bar data item holds the data for a single rendered bar in a graph.
 * Bar data proxies parse data into QBarDataItem instances for visualization.
 *
 * \sa QBarDataProxy, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs a bar data item.
 */
QBarDataItem::QBarDataItem()
    : d_ptr(0), // private data doesn't exist by default (optimization)
      m_value(0.0f),
      m_angle(0.0f)
{
}

/*!
 * Constructs a bar data item with the value \a value.
 */
QBarDataItem::QBarDataItem(float value)
    : d_ptr(0),
      m_value(value),
      m_angle(0.0f)
{
}

/*!
 * Constructs a bar data item with the value \a value and angle \a angle.
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
 * Deletes a bar data item.
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
 * \fn void QBarDataItem::setValue(float val)
 * Sets the value \a val to this data item.
 */

/*!
 * \fn float QBarDataItem::value() const
 * Returns the value of this data item.
 */

/*!
 * \fn void QBarDataItem::setRotation(float angle)
 * Sets the rotation angle \a angle in degrees for this data item.
 */

/*!
 * \fn float QBarDataItem::rotation() const
 * Returns the rotation angle in degrees for this data item.
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

QT_END_NAMESPACE
