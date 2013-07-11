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

#include "qitemmodelbardatamapping_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

QItemModelBarDataMapping::QItemModelBarDataMapping()
    : QObject(0),
      d_ptr(new QItemModelBarDataMappingPrivate(this))
{
}

QItemModelBarDataMapping::QItemModelBarDataMapping(const QItemModelBarDataMapping &other)
    : QObject(0),
      d_ptr(new QItemModelBarDataMappingPrivate(this))
{
    operator=(other);
}

QItemModelBarDataMapping::QItemModelBarDataMapping(const QString &rowRole,
                                                   const QString &columnRole,
                                                   const QString &valueRole,
                                                   const QStringList &rowCategories,
                                                   const QStringList &columnCategories)
    : QObject(0),
      d_ptr(new QItemModelBarDataMappingPrivate(this))
{
    d_ptr->m_rowRole = rowRole;
    d_ptr->m_columnRole = columnRole;
    d_ptr->m_valueRole = valueRole;
    d_ptr->m_rowCategories = rowCategories;
    d_ptr->m_columnCategories = columnCategories;
}

QItemModelBarDataMapping::~QItemModelBarDataMapping()
{
}

QItemModelBarDataMapping &QItemModelBarDataMapping::operator=(const QItemModelBarDataMapping &other)
{
    d_ptr->m_rowRole = other.d_ptr->m_rowRole;
    d_ptr->m_columnRole = other.d_ptr->m_columnRole;
    d_ptr->m_valueRole = other.d_ptr->m_valueRole;
    d_ptr->m_rowCategories = other.d_ptr->m_rowCategories;
    d_ptr->m_columnCategories = other.d_ptr->m_columnCategories;

    return *this;
}

void QItemModelBarDataMapping::setRowRole(const QString &role)
{
    d_ptr->m_rowRole = role;
    emit mappingChanged();
}

QString QItemModelBarDataMapping::rowRole() const
{
    return d_ptr->m_rowRole;
}

void QItemModelBarDataMapping::setColumnRole(const QString &role)
{
    d_ptr->m_columnRole = role;
    emit mappingChanged();
}

QString QItemModelBarDataMapping::columnRole() const
{
    return d_ptr->m_columnRole;
}

void QItemModelBarDataMapping::setValueRole(const QString &role)
{
    d_ptr->m_valueRole = role;
    emit mappingChanged();
}

QString QItemModelBarDataMapping::valueRole() const
{
    return d_ptr->m_valueRole;
}

void QItemModelBarDataMapping::setRowCategories(const QStringList &categories)
{
    d_ptr->m_rowCategories = categories;
    emit mappingChanged();
}

const QStringList &QItemModelBarDataMapping::rowCategories() const
{
    return d_ptr->m_rowCategories;
}

void QItemModelBarDataMapping::setColumnCategories(const QStringList &categories)
{
    d_ptr->m_columnCategories = categories;
    emit mappingChanged();
}

const QStringList &QItemModelBarDataMapping::columnCategories() const
{
    return d_ptr->m_columnCategories;
}

void QItemModelBarDataMapping::remap(const QString &rowRole,
                                     const QString &columnRole,
                                     const QString &valueRole,
                                     const QStringList &rowCategories,
                                     const QStringList &columnCategories)
{
    d_ptr->m_rowRole = rowRole;
    d_ptr->m_columnRole = columnRole;
    d_ptr->m_valueRole = valueRole;
    d_ptr->m_rowCategories = rowCategories;
    d_ptr->m_columnCategories = columnCategories;

    emit mappingChanged();
}

// QItemModelBarDataMappingPrivate

QItemModelBarDataMappingPrivate::QItemModelBarDataMappingPrivate(QItemModelBarDataMapping *q)
    : QObject(0),
      q_ptr(q)
{
}

QItemModelBarDataMappingPrivate::~QItemModelBarDataMappingPrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE

