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

#include "qitemmodelmapdatamapping_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

QItemModelMapDataMapping::QItemModelMapDataMapping()
    : QObject(0),
      d_ptr(new QItemModelMapDataMappingPrivate(this))
{
}

QItemModelMapDataMapping::QItemModelMapDataMapping(const QItemModelMapDataMapping &other)
    : QObject(0),
      d_ptr(new QItemModelMapDataMappingPrivate(this))
{
    operator=(other);
}

QItemModelMapDataMapping::QItemModelMapDataMapping(const QString &labelRole, const QString &xPosRole,
                                                   const QString &yPosRole, const QString &valueRole)
    : QObject(0),
      d_ptr(new QItemModelMapDataMappingPrivate(this))
{
    d_ptr->m_labelRole = labelRole;
    d_ptr->m_xPosRole = xPosRole;
    d_ptr->m_yPosRole = yPosRole;
    d_ptr->m_valueRole = valueRole;
}

QItemModelMapDataMapping::~QItemModelMapDataMapping()
{
}

QItemModelMapDataMapping &QItemModelMapDataMapping::operator=(const QItemModelMapDataMapping &other)
{
    d_ptr->m_labelRole = other.d_ptr->m_labelRole;
    d_ptr->m_xPosRole = other.d_ptr->m_xPosRole;
    d_ptr->m_yPosRole = other.d_ptr->m_yPosRole;
    d_ptr->m_valueRole = other.d_ptr->m_valueRole;

    return *this;
}

void QItemModelMapDataMapping::setLabelRole(const QString &role)
{
    d_ptr->m_labelRole = role;
    emit mappingChanged();
}

QString QItemModelMapDataMapping::labelRole() const
{
    return d_ptr->m_labelRole;
}

void QItemModelMapDataMapping::setXPosRole(const QString &role)
{
    d_ptr->m_xPosRole = role;
    emit mappingChanged();
}

QString QItemModelMapDataMapping::xPosRole() const
{
    return d_ptr->m_xPosRole;
}

void QItemModelMapDataMapping::setYPosRole(const QString &role)
{
    d_ptr->m_yPosRole = role;
    emit mappingChanged();
}

QString QItemModelMapDataMapping::yPosRole() const
{
    return d_ptr->m_yPosRole;
}

void QItemModelMapDataMapping::setValueRole(const QString &role)
{
    d_ptr->m_valueRole = role;
    emit mappingChanged();
}

QString QItemModelMapDataMapping::valueRole() const
{
    return d_ptr->m_valueRole;
}


void QItemModelMapDataMapping::remap(const QString &labelRole, const QString &xPosRole,
                                     const QString &yPosRole, const QString &valueRole)
{
    d_ptr->m_labelRole = labelRole;
    d_ptr->m_xPosRole = xPosRole;
    d_ptr->m_yPosRole = yPosRole;
    d_ptr->m_valueRole = valueRole;

    emit mappingChanged();
}

// QItemModelMapDataMappingPrivate

QItemModelMapDataMappingPrivate::QItemModelMapDataMappingPrivate(QItemModelMapDataMapping *q)
    : QObject(0),
      q_ptr(q)
{
}

QItemModelMapDataMappingPrivate::~QItemModelMapDataMappingPrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE

