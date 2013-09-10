/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
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

#include "qitemmodelsurfacedatamapping_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QItemModelSurfaceDataMapping
 * \inmodule QtDataVisualization
 * \brief Data model mapping for Q3DSurface.
 * \since 1.0.0
 *
 * DOCUMENTATION GOES HERE
 *
 * \sa QItemModelSurfaceDataProxy
 */

/*!
 * Constructs QItemModelSurfaceDataMapping with the given \a parent.
 */
QItemModelSurfaceDataMapping::QItemModelSurfaceDataMapping(QObject *parent)
    : QAbstractDataMapping(new QItemModelSurfaceDataMappingPrivate(this), parent)
{
}

/*!
 * Constructs QItemModelSurfaceDataMapping with \a valueRole and the given \a parent.
 */
QItemModelSurfaceDataMapping::QItemModelSurfaceDataMapping(const QString &valueRole, QObject *parent)
    : QAbstractDataMapping(new QItemModelSurfaceDataMappingPrivate(this), parent)
{
    dptr()->m_valueRole = valueRole;
}

/*!
 * Constructs QItemModelSurfaceDataMapping with \a rowRole, \a columnRole, \a valueRole,
 * \a rowCategories, \a columnCategories and the given \a parent.
 */
QItemModelSurfaceDataMapping::QItemModelSurfaceDataMapping(const QString &rowRole,
                                                           const QString &columnRole,
                                                           const QString &valueRole,
                                                           const QStringList &rowCategories,
                                                           const QStringList &columnCategories,
                                                           QObject *parent)
    : QAbstractDataMapping(new QItemModelSurfaceDataMappingPrivate(this), parent)
{
    dptr()->m_rowRole = rowRole;
    dptr()->m_columnRole = columnRole;
    dptr()->m_valueRole = valueRole;
    dptr()->m_rowCategories = rowCategories;
    dptr()->m_columnCategories = columnCategories;
}

/*!
 * Destroys QItemModelSurfaceDataMapping.
 */
QItemModelSurfaceDataMapping::~QItemModelSurfaceDataMapping()
{
}

/*!
 * \property QItemModelSurfaceDataMapping::rowRole
 *
 * Defines the row role for the mapping.
 */
void QItemModelSurfaceDataMapping::setRowRole(const QString &role)
{
    dptr()->m_rowRole = role;
    emit mappingChanged();
}

QString QItemModelSurfaceDataMapping::rowRole() const
{
    return dptrc()->m_rowRole;
}

/*!
 * \property QItemModelSurfaceDataMapping::columnRole
 *
 * Defines the column role for the mapping.
 */
void QItemModelSurfaceDataMapping::setColumnRole(const QString &role)
{
    dptr()->m_columnRole = role;
    emit mappingChanged();
}

QString QItemModelSurfaceDataMapping::columnRole() const
{
    return dptrc()->m_columnRole;
}

/*!
 * \property QItemModelSurfaceDataMapping::valueRole
 *
 * Defines the value role for the mapping.
 */
void QItemModelSurfaceDataMapping::setValueRole(const QString &role)
{
    dptr()->m_valueRole = role;
    emit mappingChanged();
}

QString QItemModelSurfaceDataMapping::valueRole() const
{
    return dptrc()->m_valueRole;
}

/*!
 * \property QItemModelSurfaceDataMapping::rowCategories
 *
 * Defines the row categories for the mapping.
 */
void QItemModelSurfaceDataMapping::setRowCategories(const QStringList &categories)
{
    dptr()->m_rowCategories = categories;
    emit mappingChanged();
}

QStringList QItemModelSurfaceDataMapping::rowCategories() const
{
    return dptrc()->m_rowCategories;
}

/*!
 * \property QItemModelSurfaceDataMapping::columnCategories
 *
 * Defines the column categories for the mapping.
 */
void QItemModelSurfaceDataMapping::setColumnCategories(const QStringList &categories)
{
    dptr()->m_columnCategories = categories;
    emit mappingChanged();
}

QStringList QItemModelSurfaceDataMapping::columnCategories() const
{
    return dptrc()->m_columnCategories;
}

/*!
 * Changes \a rowRole, \a columnRole, \a valueRole, \a rowCategories and \a columnCategories to the
 * mapping.
 */
void QItemModelSurfaceDataMapping::remap(const QString &rowRole,
                                         const QString &columnRole,
                                         const QString &valueRole,
                                         const QStringList &rowCategories,
                                         const QStringList &columnCategories)
{
    dptr()->m_rowRole = rowRole;
    dptr()->m_columnRole = columnRole;
    dptr()->m_valueRole = valueRole;
    dptr()->m_rowCategories = rowCategories;
    dptr()->m_columnCategories = columnCategories;

    emit mappingChanged();
}

/*!
 * /return index of the specified \a category in row categories list.
 * If the category is not found, -1 is returned.
 */
int QItemModelSurfaceDataMapping::rowCategoryIndex(const QString &category)
{
    return dptr()->m_rowCategories.indexOf(category);
}

/*!
 * /return index of the specified \a category in column categories list.
 * If the category is not found, -1 is returned.
 */
int QItemModelSurfaceDataMapping::columnCategoryIndex(const QString &category)
{
    return dptr()->m_columnCategories.indexOf(category);
}

/*!
 * \internal
 */
QItemModelSurfaceDataMappingPrivate *QItemModelSurfaceDataMapping::dptr()
{
    return static_cast<QItemModelSurfaceDataMappingPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QItemModelSurfaceDataMappingPrivate *QItemModelSurfaceDataMapping::dptrc() const
{
    return static_cast<const QItemModelSurfaceDataMappingPrivate *>(d_ptr.data());
}

// QItemModelSurfaceDataMappingPrivate

QItemModelSurfaceDataMappingPrivate::QItemModelSurfaceDataMappingPrivate(QItemModelSurfaceDataMapping *q)
    : QAbstractDataMappingPrivate(q, QAbstractDataProxy::DataTypeSurface)
{
}

QItemModelSurfaceDataMappingPrivate::~QItemModelSurfaceDataMappingPrivate()
{
}


QT_DATAVISUALIZATION_END_NAMESPACE

