/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QScatterDataItem
 * \inmodule QtDataVis3D
 * \brief The QScatterDataItem class provides a container for resolved data to be added to scatter
 * graphs.
 * \since 1.0.0
 *
 * A QScatterDataItem holds data for a single rendered item in a scatter graph.
 * Scatter data proxies parse data into QScatterDataItem instances for visualizing.
 *
 * \sa QScatterDataProxy, {Qt Data Visualization 3D C++ Classes}
 */

/*!
 * Constructs QScatterDataItem.
 */
QScatterDataItem::QScatterDataItem()
{
}

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

QScatterDataItem &QScatterDataItem::operator=(const QScatterDataItem &other)
{
    m_position = other.m_position;
    //m_size = other.m_size;

    return *this;
}

void QScatterDataItem::setPosition(const QVector3D &position)
{
    m_position = position;
}

const QVector3D &QScatterDataItem::position() const
{
    return m_position;
}

//void QScatterDataItem::setSize(qreal size)
//{
//    m_size = size;
//}

//const qreal &QScatterDataItem::size() const
//{
//    return m_size;
//}

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

QT_DATAVIS3D_END_NAMESPACE
