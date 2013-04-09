/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "qdatarow.h"
#include "qdatarow_p.h"
#include "qdataitem.h"
#include "qdataitem_p.h"

#include <QString>

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

QDataRow::QDataRow(const QString &label)
    : d_ptr(new QDataRowPrivate(this, label))
{
    //qDebug("QDataRow");
}

QDataRow::~QDataRow()
{
    //qDebug("~QDataRow");
}

void QDataRow::setLabel(const QString &label)
{
    d_ptr->m_label = label;
}

void QDataRow::addItem(QDataItem *item)
{
    d_ptr->m_row.prepend(item);
}

QDataRowPrivate::QDataRowPrivate(QDataRow *q, const QString &label)
    : q_ptr(q)
    , m_label(label)
{
}

QDataRowPrivate::~QDataRowPrivate()
{
    for (int itemCount = 0; itemCount < m_row.size(); itemCount++) {
        delete m_row.at(itemCount);
    }
    m_row.clear();
}

QVector<QDataItem*> QDataRowPrivate::row()
{
    return m_row;
}

void QDataRowPrivate::clear()
{
    m_row.clear();
}

QDataItem *QDataRowPrivate::getItem(int itemIndex)
{
    QDataItem *item = NULL;
    if (m_row.size() > itemIndex)
        item = m_row.at(itemIndex);
    return item;
}

void QDataRowPrivate::verifySize(int size)
{
    qDebug("verifySize (QDataRow)");
    if (size > m_row.size()) {
        // QVector's resize doesn't delete data contained in it
        // Delete contents of items to be removed
        int nbrToBeRemoved = m_row.size() - size;
        for (int itemCount = 0; itemCount < nbrToBeRemoved; itemCount++) {
            int itemToBeRemoved = m_row.size() - itemCount - 1; // -1 to compensate index 0
            delete m_row.at(itemToBeRemoved);
        }
        // Resize vector
        m_row.resize(size);
    }
    else if (size < m_row.size()) {
        qCritical("Check your sample space size! Your row is too short.");
    }
}

float QDataRowPrivate::highestValue()
{
    float max = 0;
    for (int i = 0; i < m_row.size(); i++) {
        QDataItem *item = m_row.at(i);
        float itemValue = item->d_ptr->value();
        if (max < itemValue)
            max = itemValue;
    }
    return max;
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE
