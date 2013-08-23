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

#include "qitemmodelscatterdatamapping_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QItemModelScatterDataMapping
 * \inmodule QtDataVis3D
 * \brief Data model mapping for Q3DScatter.
 * \since 1.0.0
 *
 * QItemModelScatterDataMapping is used to define roles for mapping data in QAbstractItemModel to
 * Q3DScatter.
 *
 * TO BE CHECKED (add more explanations and/or example)
 *
 * \sa QItemModelScatterDataProxy
 */

/*!
 * Constructs QItemModelScatterDataMapping.
 */
QItemModelScatterDataMapping::QItemModelScatterDataMapping()
    : QObject(0),
      d_ptr(new QItemModelScatterDataMappingPrivate(this))
{
}

/*!
 * Constructs a copy of \a other.
 */
QItemModelScatterDataMapping::QItemModelScatterDataMapping(
        const QItemModelScatterDataMapping &other)
    : QObject(0),
      d_ptr(new QItemModelScatterDataMappingPrivate(this))
{
    operator=(other);
}

/*!
 * Constructs QItemModelScatterDataMapping with \a xPosRole, \a yPosRole, \a zPosRole and
 * \a valueRole.
 */
QItemModelScatterDataMapping::QItemModelScatterDataMapping(const QString &xPosRole,
                                                           const QString &yPosRole,
                                                           const QString &zPosRole,
                                                           const QString &valueRole)
    : QObject(0),
      d_ptr(new QItemModelScatterDataMappingPrivate(this))
{
    Q_UNUSED(valueRole);
    d_ptr->m_xPosRole = xPosRole;
    d_ptr->m_yPosRole = yPosRole;
    d_ptr->m_zPosRole = zPosRole;
    //d_ptr->m_valueRole = valueRole;
}

/*!
 * Destroys QItemModelScatterDataMapping.
 */
QItemModelScatterDataMapping::~QItemModelScatterDataMapping()
{
}

/*!
 *  Assigns a copy of \a other to this object.
 */
QItemModelScatterDataMapping &QItemModelScatterDataMapping::operator=(
        const QItemModelScatterDataMapping &other)
{
    d_ptr->m_xPosRole = other.d_ptr->m_xPosRole;
    d_ptr->m_yPosRole = other.d_ptr->m_yPosRole;
    d_ptr->m_zPosRole = other.d_ptr->m_zPosRole;
    //d_ptr->m_valueRole = other.d_ptr->m_valueRole;

    return *this;
}

/*!
 * \property QItemModelScatterDataMapping::xPosRole
 *
 * Defines the x position role for the mapping.
 */
void QItemModelScatterDataMapping::setXPosRole(const QString &role)
{
    d_ptr->m_xPosRole = role;
    emit mappingChanged();
}

QString QItemModelScatterDataMapping::xPosRole() const
{
    return d_ptr->m_xPosRole;
}

/*!
 * \property QItemModelScatterDataMapping::yPosRole
 *
 * Defines the y position role for the mapping.
 */
void QItemModelScatterDataMapping::setYPosRole(const QString &role)
{
    d_ptr->m_yPosRole = role;
    emit mappingChanged();
}

QString QItemModelScatterDataMapping::yPosRole() const
{
    return d_ptr->m_yPosRole;
}

/*!
 * \property QItemModelScatterDataMapping::zPosRole
 *
 * Defines the z position role for the mapping.
 */
void QItemModelScatterDataMapping::setZPosRole(const QString &role)
{
    d_ptr->m_zPosRole = role;
    emit mappingChanged();
}

QString QItemModelScatterDataMapping::zPosRole() const
{
    return d_ptr->m_zPosRole;
}

//void QItemModelScatterDataMapping::setValueRole(const QString &role)
//{
//    d_ptr->m_valueRole = role;
//    emit mappingChanged();
//}

//QString QItemModelScatterDataMapping::valueRole() const
//{
//    return d_ptr->m_valueRole;
//}

/*!
 * Changes \a xPosRole, \a yPosRole, \a zPosRole and \a valueRole to the mapping.
 *
 * Emits mappingChanged() signal after remapping.
 */
void QItemModelScatterDataMapping::remap(const QString &xPosRole, const QString &yPosRole,
                                         const QString &zPosRole, const QString &valueRole)
{
    Q_UNUSED(valueRole);
    d_ptr->m_xPosRole = xPosRole;
    d_ptr->m_yPosRole = yPosRole;
    d_ptr->m_zPosRole = zPosRole;
    //d_ptr->m_valueRole = valueRole;

    emit mappingChanged();
}

// QItemModelScatterDataMappingPrivate

QItemModelScatterDataMappingPrivate::QItemModelScatterDataMappingPrivate(
        QItemModelScatterDataMapping *q)
    : QObject(0),
      q_ptr(q)
{
}

QItemModelScatterDataMappingPrivate::~QItemModelScatterDataMappingPrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE

