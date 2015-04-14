/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#include "qscatterdataitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QScatterDataItem
 * \inmodule QtDataVisualization
 * \brief The QScatterDataItem class provides a container for resolved data to be added to scatter
 * graphs.
 * \since QtDataVisualization 1.0
 *
 * A QScatterDataItem holds data for a single rendered item in a scatter graph.
 * Scatter data proxies parse data into QScatterDataItem instances for visualizing.
 *
 * \sa QScatterDataProxy, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs QScatterDataItem.
 */
QScatterDataItem::QScatterDataItem()
    : d_ptr(0) // private data doesn't exist by default (optimization)

{
}

/*!
 * Constructs QScatterDataItem with \a position.
 */
QScatterDataItem::QScatterDataItem(const QVector3D &position)
    : d_ptr(0),
      m_position(position)
{
}

/*!
 * Constructs QScatterDataItem with \a position and \a rotation.
 */
QScatterDataItem::QScatterDataItem(const QVector3D &position, const QQuaternion &rotation)
    : d_ptr(0),
      m_position(position),
      m_rotation(rotation)
{
}

/*!
 * Constructs a copy of \a other.
 */
QScatterDataItem::QScatterDataItem(const QScatterDataItem &other)
{
    operator=(other);
}

/*!
 * Destroys QScatterDataItem.
 */
QScatterDataItem::~QScatterDataItem()
{
}

/*!
 *  Assigns a copy of \a other to this object.
 */
QScatterDataItem &QScatterDataItem::operator=(const QScatterDataItem &other)
{
    m_position = other.m_position;
    m_rotation = other.m_rotation;

    if (other.d_ptr)
        createExtraData();
    else
        d_ptr = 0;

    return *this;
}

/*!
 * \fn void QScatterDataItem::setPosition(const QVector3D &position)
 * Sets \a position to this data item.
 */

/*!
 * \fn QVector3D QScatterDataItem::position() const
 * \return position of this data item.
 */

/*!
 * \fn void QScatterDataItem::setRotation(const QQuaternion &rotation)
 * Sets \a rotation to this data item.
 * The \a rotation should be a normalized QQuaternion.
 * If the series also has rotation, item and series rotations are multiplied together.
 * Defaults to no rotation.
 */

/*!
 * \fn QQuaternion QScatterDataItem::rotation() const
 * \return rotation of this data item.
 * \sa setRotation()
 */

/*!
 * \fn void QScatterDataItem::setX(float value)
 * Sets the X component of the item position to the \a value.
 */

/*!
 * \fn void QScatterDataItem::setY(float value)
 * Sets the Y component of the item position to the \a value.
 */

/*!
 * \fn void QScatterDataItem::setZ(float value)
 * Sets the Z component of the item position to the \a value.
 */

/*!
 * \fn float QScatterDataItem::x() const
 * \return the X component of the position of this data item.
 */

/*!
 * \fn float QScatterDataItem::y() const
 * \return the Y component of the position of this data item.
 */

/*!
 * \fn float QScatterDataItem::z() const
 * \return the Z component of the position of this data item.
 */

/*!
 * \internal
 */
void QScatterDataItem::createExtraData()
{
    if (!d_ptr)
        d_ptr = new QScatterDataItemPrivate;
}

QScatterDataItemPrivate::QScatterDataItemPrivate()
{
}

QScatterDataItemPrivate::~QScatterDataItemPrivate()
{
}

QT_END_NAMESPACE_DATAVISUALIZATION
