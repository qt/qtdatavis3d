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

#include "qbardataitem_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QBarDataItem
 * \inmodule QtDataVis3D
 * \brief The QBarDataItem class provides a container for resolved data to be added to bar graphs.
 * \since 1.0.0
 *
 * A QBarDataItem holds data for a single rendered bar in a graph.
 * Bar data proxies parse data into QBarDataItem instances for visualizing.
 *
 * \sa QBarDataProxy, {Qt Data Visualization 3D C++ Classes}
 */

/*!
 * Constructs QBarDataItem.
 */
QBarDataItem::QBarDataItem()
    : d_ptr(0) // private data doesn't exist by default (optimization)
{
}

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

void QBarDataItem::setValue(qreal value)
{
    m_value = value;
}

qreal QBarDataItem::value() const
{
    return m_value;
}

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

QT_DATAVIS3D_END_NAMESPACE
