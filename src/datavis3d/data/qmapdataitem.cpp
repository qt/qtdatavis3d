/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
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
