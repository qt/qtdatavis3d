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

#include "qmapdataitem_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QMapDataItem
 * \inmodule QtDataVis3D
 * \brief The QMapDataItem class provides a container for resolved data to be added to maps graphs.
 * \since 1.0.0
 *
 * A QMapDataItem holds data for a single rendered bar in a maps graph.
 * Maps data proxies parse data into QMapDataItem instances for visualizing.
 *
 * \sa QMapDataProxy, {Qt Data Visualization 3D C++ Classes}
 */

/*!
 * Constructs QMapDataItem.
 */
QMapDataItem::QMapDataItem()
    : QBarDataItem()
{
}

QMapDataItem::QMapDataItem(const QMapDataItem &other)
    : QBarDataItem(other)
{
    operator=(other);
}

/*!
 * Destroys QMapDataItem.
 */
QMapDataItem::~QMapDataItem()
{
}

QMapDataItem &QMapDataItem::operator=(const QMapDataItem &other)
{
    QBarDataItem::operator =(other);
    m_mapPosition = other.m_mapPosition;
    m_label = other.m_label;

    return *this;
}

void QMapDataItem::setMapPosition(const QPointF &position)
{
    m_mapPosition = position;
}

QPointF QMapDataItem::mapPosition() const
{
    return m_mapPosition;
}

void QMapDataItem::setLabel(const QString &label)
{
    m_label = label;
}

QString QMapDataItem::label() const
{
    return m_label;
}

void QMapDataItem::createExtraData()
{
    if (!d_ptr)
        d_ptr = new QMapDataItemPrivate;
}

QMapDataItemPrivate::QMapDataItemPrivate()
    : QBarDataItemPrivate()
{
}

QMapDataItemPrivate::~QMapDataItemPrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE
