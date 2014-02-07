/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#include "qscatterdataitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QScatterDataItem
 * \inmodule QtDataVisualization
 * \brief The QScatterDataItem class provides a container for resolved data to be added to scatter
 * graphs.
 * \since Qt Data Visualization 1.0
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
    : d_ptr(0), // private data doesn't exist by default (optimization)
      m_rotationAxis(upVector),
      m_rotationAngle(0.0f)

{
}

/*!
 * Constructs QScatterDataItem with \a position.
 */
QScatterDataItem::QScatterDataItem(const QVector3D &position)
    : d_ptr(0),
      m_position(position),
      m_rotationAxis(upVector),
      m_rotationAngle(0.0f)
{
}

/*!
 * Constructs QScatterDataItem with \a position, \a rotationAxis and \a rotationAngle.
 */
QScatterDataItem::QScatterDataItem(const QVector3D &position, const QVector3D &rotationAxis,
                                   float rotationAngle)
    : d_ptr(0),
      m_position(position),
      m_rotationAxis(rotationAxis),
      m_rotationAngle(rotationAngle)
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
    m_rotationAxis = other.m_rotationAxis;
    m_rotationAngle = other.m_rotationAngle;

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
 * \fn void QScatterDataItem::setRotationAxis(const QVector3D &axis)
 * Sets rotation \a axis of this data item.
 * If the series also has rotation, the resulting rotation quaternions
 * are multiplied together at render time.
 * Defaults to Y-axis.
 *
 * \sa setRotationAngle()
 */

/*!
 * \fn QVector3D QScatterDataItem::rotationAxis() const
 * \return rotation axis of this data item.
 * \sa setRotationAxis(), setRotationAngle()
 */

/*!
 * \fn void QScatterDataItem::setRotationAngle(float angle)
 * Sets rotation \a angle of this data item.
 * If the series also has rotation, the resulting rotation quaternions
 * are multiplied together at render time.
 * Defaults to no rotation.
 *
 * \sa setRotationAxis()
 */

/*!
 * \fn float QScatterDataItem::rotationAngle() const
 * \return rotation angle of this data item.
 * \sa setRotationAxis(), setRotationAngle()
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
