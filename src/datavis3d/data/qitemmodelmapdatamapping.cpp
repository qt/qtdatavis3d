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

/*!
 * \class QItemModelMapDataMapping
 * \inmodule QtDataVis3D
 * \brief Data model mapping for Q3DMaps.
 * \since 1.0.0
 *
 * QItemModelMapDataMapping is used to define roles for mapping data in QAbstractItemModel to
 * Q3DMaps.
 *
 * TO BE CHECKED (add more explanations and/or example)
 *
 * \sa QItemModelMapDataProxy
 */

/*!
 * Constructs QItemModelMapDataMapping with the given \a parent.
 */
QItemModelMapDataMapping::QItemModelMapDataMapping(QObject *parent)
    : QObject(parent),
      d_ptr(new QItemModelMapDataMappingPrivate(this))
{
}

/*!
 * Constructs QItemModelMapDataMapping with \a labelRole, \a xPosRole, \a zPosRole,
 * \a valueRole and the given \a parent.
 */
QItemModelMapDataMapping::QItemModelMapDataMapping(const QString &labelRole,
                                                   const QString &xPosRole,
                                                   const QString &zPosRole,
                                                   const QString &valueRole,
                                                   QObject *parent)
    : QObject(parent),
      d_ptr(new QItemModelMapDataMappingPrivate(this))
{
    d_ptr->m_labelRole = labelRole;
    d_ptr->m_xPosRole = xPosRole;
    d_ptr->m_zPosRole = zPosRole;
    d_ptr->m_valueRole = valueRole;
}

/*!
 * Destroys QItemModelMapDataMapping.
 */
QItemModelMapDataMapping::~QItemModelMapDataMapping()
{
}

/*!
 * \property QItemModelMapDataMapping::labelRole
 *
 * Defines the label role for the mapping.
 */
void QItemModelMapDataMapping::setLabelRole(const QString &role)
{
    d_ptr->m_labelRole = role;
    emit mappingChanged();
}

QString QItemModelMapDataMapping::labelRole() const
{
    return d_ptr->m_labelRole;
}

/*!
 * \property QItemModelMapDataMapping::xPosRole
 *
 * Defines the x position role for the mapping.
 */
void QItemModelMapDataMapping::setXPosRole(const QString &role)
{
    d_ptr->m_xPosRole = role;
    emit mappingChanged();
}

QString QItemModelMapDataMapping::xPosRole() const
{
    return d_ptr->m_xPosRole;
}

/*!
 * \property QItemModelMapDataMapping::zPosRole
 *
 * Defines the z position role for the mapping.
 */
void QItemModelMapDataMapping::setZPosRole(const QString &role)
{
    d_ptr->m_zPosRole = role;
    emit mappingChanged();
}

QString QItemModelMapDataMapping::zPosRole() const
{
    return d_ptr->m_zPosRole;
}

/*!
 * \property QItemModelMapDataMapping::valueRole
 *
 * Defines the value role for the mapping.
 */
void QItemModelMapDataMapping::setValueRole(const QString &role)
{
    d_ptr->m_valueRole = role;
    emit mappingChanged();
}

QString QItemModelMapDataMapping::valueRole() const
{
    return d_ptr->m_valueRole;
}


/*!
 * Changes \a labelRole, \a xPosRole, \a zPosRole and \a valueRole to the mapping.
 *
 * Emits mappingChanged() signal after remapping.
 */
void QItemModelMapDataMapping::remap(const QString &labelRole, const QString &xPosRole,
                                     const QString &zPosRole, const QString &valueRole)
{
    d_ptr->m_labelRole = labelRole;
    d_ptr->m_xPosRole = xPosRole;
    d_ptr->m_zPosRole = zPosRole;
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

