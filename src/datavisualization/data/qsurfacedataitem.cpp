/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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
****************************************************************************/

#include "qsurfacedataitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QSurfaceDataItem
 * \inmodule QtDataVisualization
 * \brief The QSurfaceDataItem class provides a container for resolved data to be added to surface
 * graphs.
 * \since QtDataVisualization 1.0
 *
 * A surface data item holds the data for a single vertex in a surface graph.
 * Surface data proxies parse data into QSurfaceDataItem instances for
 * visualization.
 *
 * \sa QSurfaceDataProxy, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs a surface data item.
 */
QSurfaceDataItem::QSurfaceDataItem()
    : d_ptr(0) // private data doesn't exist by default (optimization)

{
}

/*!
 * Constructs a surface data item at the position \a position.
 */
QSurfaceDataItem::QSurfaceDataItem(const QVector3D &position)
    : d_ptr(0),
      m_position(position)
{
}

/*!
 * Constructs a copy of \a other.
 */
QSurfaceDataItem::QSurfaceDataItem(const QSurfaceDataItem &other)
{
    operator=(other);
}

/*!
 * Deletes a surface data item.
 */
QSurfaceDataItem::~QSurfaceDataItem()
{
}

/*!
 *  Assigns a copy of \a other to this object.
 */
QSurfaceDataItem &QSurfaceDataItem::operator=(const QSurfaceDataItem &other)
{
    m_position = other.m_position;

    if (other.d_ptr)
        createExtraData();
    else
        d_ptr = 0;

    return *this;
}

/*!
 * \fn void QSurfaceDataItem::setPosition(const QVector3D &pos)
 * Sets the position \a pos to this data item.
 */

/*!
 * \fn QVector3D QSurfaceDataItem::position() const
 * Returns the position of this data item.
 */

/*!
 * \fn void QSurfaceDataItem::setX(float value)
 * Sets the x-coordinate of the item position to the value \a value.
 */

/*!
 * \fn void QSurfaceDataItem::setY(float value)
 * Sets the y-coordinate of the item position to the value \a value.
 */

/*!
 * \fn void QSurfaceDataItem::setZ(float value)
 * Sets the z-coordinate of the item position to the value \a value.
 */

/*!
 * \fn float QSurfaceDataItem::x() const
 * Returns the x-coordinate of the position of this data item.
 */

/*!
 * \fn float QSurfaceDataItem::y() const
 * Returns the y-coordinate of the position of this data item.
 */

/*!
 * \fn float QSurfaceDataItem::z() const
 * Returns the z-coordinate of the position of this data item.
 */

/*!
 * \internal
 */
void QSurfaceDataItem::createExtraData()
{
    if (!d_ptr)
        d_ptr = new QSurfaceDataItemPrivate;
}

QSurfaceDataItemPrivate::QSurfaceDataItemPrivate()
{
}

QSurfaceDataItemPrivate::~QSurfaceDataItemPrivate()
{
}

QT_END_NAMESPACE_DATAVISUALIZATION
