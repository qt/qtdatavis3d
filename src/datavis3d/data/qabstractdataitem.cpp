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

#include "qabstractdataitem_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QAbstractDataItem
 * \inmodule QtDataVis3D
 * \brief The QAbstractDataItem class provides a base container for resolved data to be added to graphs.
 * \since 1.0.0
 *
 * A QAbstractDataItem holds data for a single rendered item in a graph.
 * Data proxies parse data into QAbstractDataItem based instances for visualizing.
 *
 * \sa QAbstractDataProxy, {Qt Data Visualization 3D C++ Classes}
 */

/*!
 * Constructs QAbstractDataItem.
 */
QAbstractDataItem::QAbstractDataItem(QAbstractDataItemPrivate *d)
    : d_ptr(d)
{
}

/*!
 * Destroys QAbstractDataItem.
 */
QAbstractDataItem::~QAbstractDataItem()
{
}

/*!
 * \a label A formatted label for the data item.
 *
 * Sets the formatted label for the data item.
 */
void QAbstractDataItem::setLabel(const QString &label)
{
    d_ptr->m_label = label;
}

QString &QAbstractDataItem::label() const
{
    if (d_ptr->m_label.isNull())
        d_ptr->formatLabel();
    return d_ptr->m_label;
}

QAbstractDataItemPrivate::QAbstractDataItemPrivate()
    : m_dataProxy(0)
{
}

QAbstractDataItemPrivate::~QAbstractDataItemPrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE
