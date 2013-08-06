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

#include "qvalueaxis.h"
#include "qvalueaxis_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

QValueAxis::QValueAxis() :
    QAbstractAxis(new QValueAxisPrivate(this))
{
}

QValueAxis::~QValueAxis()
{
}

void QValueAxis::setRange(qreal min, qreal max)
{
    bool dirty = false;
    if (dptr()->m_min != min) {
        dptr()->m_min = min;
        dirty = true;
    }
    if (dptr()->m_max != max) {
        dptr()->m_max = max;
        dirty = true;
    }
    if (dirty)
        emit rangeChanged(min, max);
}

void QValueAxis::setMin(qreal min)
{
    if (dptr()->m_min != min) {
        dptr()->m_min = min;
        emit rangeChanged(min, dptr()->m_max);
    }
}

void QValueAxis::setMax(qreal max)
{
    if (dptr()->m_max != max) {
        dptr()->m_max = max;
        emit rangeChanged(dptr()->m_min, max);
    }
}

qreal QValueAxis::min() const
{
    return dptrc()->m_min;
}

qreal QValueAxis::max() const
{
    return dptrc()->m_max;
}

QValueAxisPrivate *QValueAxis::dptr()
{
    return static_cast<QValueAxisPrivate *>(d_ptr.data());
}

const QValueAxisPrivate *QValueAxis::dptrc() const
{
    return static_cast<const QValueAxisPrivate *>(d_ptr.data());
}

QValueAxisPrivate::QValueAxisPrivate(QValueAxis *q)
    : QAbstractAxisPrivate(q, QAbstractAxis::AxisTypeValue),
      m_min(0.0),
      m_max(0.0)
{
}

QValueAxisPrivate::~QValueAxisPrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE
