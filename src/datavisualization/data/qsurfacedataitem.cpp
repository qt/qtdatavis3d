/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include "qsurfacedataitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QSurfaceDataItem
 * \inmodule QtDataVisualization
 * \brief The QSurfaceDataItem class provides a container for resolved data to be added to surface
 * graphs.
 * \since QtDataVisualization 1.0
 *
 * A QSurfaceDataItem holds data for a single vertex in surface graph.
 * Surface data proxies parse data into QSurfaceDataItem instances for visualizing.
 *
 * \sa QSurfaceDataProxy, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs QSurfaceDataItem.
 */
QSurfaceDataItem::QSurfaceDataItem()
    : d_ptr(0) // private data doesn't exist by default (optimization)

{
}

/*!
 * Constructs QSurfaceDataItem with \a position.
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
 * Destroys QSurfaceDataItem.
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
 * Sets position \a pos to this data item.
 */

/*!
 * \fn QVector3D QSurfaceDataItem::position() const
 * \return position of this data item.
 */

/*!
 * \fn void QSurfaceDataItem::setX(float value)
 * Sets the X component of the item position to the \a value.
 */

/*!
 * \fn void QSurfaceDataItem::setY(float value)
 * Sets the Y component of the item position to the \a value.
 */

/*!
 * \fn void QSurfaceDataItem::setZ(float value)
 * Sets the Z component of the item position to the \a value.
 */

/*!
 * \fn float QSurfaceDataItem::x() const
 * \return the X component of the position of this data item.
 */

/*!
 * \fn float QSurfaceDataItem::y() const
 * \return the Y component of the position of this data item.
 */

/*!
 * \fn float QSurfaceDataItem::z() const
 * \return the Z component of the position of this data item.
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
