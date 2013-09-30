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

#include "qsurfacedataitem_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QSurfaceDataItem
 * \inmodule QtDataVisualization
 * \brief The QSurfaceDataItem class provides a container for resolved data to be added to surface
 * graphs.
 * \since 1.0.0
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
    //m_size = other.m_size;

    if (other.d_ptr)
        createExtraData();
    else
        d_ptr = 0;
    // TODO set extra data

    return *this;
}

/*!
 * \fn void QSurfaceDataItem::setPosition(const QVector3D &position)
 * Sets \a position to this data item.
 */

/*!
 * \fn QVector3D QSurfaceDataItem::position() const
 * \return position of this data item.
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

QT_DATAVISUALIZATION_END_NAMESPACE
