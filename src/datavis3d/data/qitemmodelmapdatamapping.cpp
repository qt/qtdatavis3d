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

