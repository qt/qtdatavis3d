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

#include "qitemmodelbardatamapping_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QItemModelBarDataMapping
 * \inmodule QtDataVisualization
 * \brief Data model mapping for Q3DBars.
 * \since 1.0.0
 *
 * QItemModelBarDataMapping is used to map roles of QAbstractItemModel to rows, columns, and values
 * of Q3DBars. There are two ways to use QItemModelBarDataMapping:
 *
 * 1) By default, the QItemModelBarDataMapping will map the rows and columns of QAbstractItemModel
 *    to rows and columns of Q3DBars, and uses the value returned for Qt::DisplayRole as bar value.
 *    The value role to be used can be redefined if Qt::DisplayRole is not suitable.
 *
 * 2) For models that do not have data already neatly sorted into rows and columns, such as
 *    QAbstractListModel based models, you can define a list of categories for both rows and columns,
 *    and define a role to map for each of row, column and value.
 *    For example, assume that you have a custom QAbstractItemModel for storing various monthly values
 *    related to a business.
 *    Each item in the model has roles "year", "month", "income", and "expenses".
 *    You could do the following to display the data in a bar chart:
 *
 *    \snippet doc_src_qtdatavisualization.cpp 3
 *
 * \sa QItemModelBarDataProxy
 */

/*!
 * Constructs QItemModelBarDataMapping with the given \a parent.
 */
QItemModelBarDataMapping::QItemModelBarDataMapping(QObject *parent)
    : QAbstractDataMapping(new QItemModelBarDataMappingPrivate(this), parent)
{
}

/*!
 * Constructs QItemModelBarDataMapping with \a valueRole and the given \a parent.
 */
QItemModelBarDataMapping::QItemModelBarDataMapping(const QString &valueRole, QObject *parent)
    : QAbstractDataMapping(new QItemModelBarDataMappingPrivate(this), parent)
{
    dptr()->m_valueRole = valueRole;
}

/*!
 * Constructs QItemModelBarDataMapping with \a rowRole, \a columnRole, \a valueRole,
 * \a rowCategories, \a columnCategories and the given \a parent.
 */
QItemModelBarDataMapping::QItemModelBarDataMapping(const QString &rowRole,
                                                   const QString &columnRole,
                                                   const QString &valueRole,
                                                   const QStringList &rowCategories,
                                                   const QStringList &columnCategories,
                                                   QObject *parent)
    : QAbstractDataMapping(new QItemModelBarDataMappingPrivate(this), parent)
{
    dptr()->m_rowRole = rowRole;
    dptr()->m_columnRole = columnRole;
    dptr()->m_valueRole = valueRole;
    dptr()->m_rowCategories = rowCategories;
    dptr()->m_columnCategories = columnCategories;
}

/*!
 * Destroys QItemModelBarDataMapping.
 */
QItemModelBarDataMapping::~QItemModelBarDataMapping()
{
}

/*!
 * \property QItemModelBarDataMapping::rowRole
 *
 * Defines the row role for the mapping.
 */
void QItemModelBarDataMapping::setRowRole(const QString &role)
{
    dptr()->m_rowRole = role;
    emit mappingChanged();
}

QString QItemModelBarDataMapping::rowRole() const
{
    return dptrc()->m_rowRole;
}

/*!
 * \property QItemModelBarDataMapping::columnRole
 *
 * Defines the column role for the mapping.
 */
void QItemModelBarDataMapping::setColumnRole(const QString &role)
{
    dptr()->m_columnRole = role;
    emit mappingChanged();
}

QString QItemModelBarDataMapping::columnRole() const
{
    return dptrc()->m_columnRole;
}

/*!
 * \property QItemModelBarDataMapping::valueRole
 *
 * Defines the value role for the mapping.
 */
void QItemModelBarDataMapping::setValueRole(const QString &role)
{
    dptr()->m_valueRole = role;
    emit mappingChanged();
}

QString QItemModelBarDataMapping::valueRole() const
{
    return dptrc()->m_valueRole;
}

/*!
 * \property QItemModelBarDataMapping::rowCategories
 *
 * Defines the row categories for the mapping.
 */
void QItemModelBarDataMapping::setRowCategories(const QStringList &categories)
{
    dptr()->m_rowCategories = categories;
    emit mappingChanged();
}

QStringList QItemModelBarDataMapping::rowCategories() const
{
    return dptrc()->m_rowCategories;
}

/*!
 * \property QItemModelBarDataMapping::columnCategories
 *
 * Defines the column categories for the mapping.
 */
void QItemModelBarDataMapping::setColumnCategories(const QStringList &categories)
{
    dptr()->m_columnCategories = categories;
    emit mappingChanged();
}

QStringList QItemModelBarDataMapping::columnCategories() const
{
    return dptrc()->m_columnCategories;
}

/*!
 * Changes \a rowRole, \a columnRole, \a valueRole, \a rowCategories and \a columnCategories to the
 * mapping.
 */
void QItemModelBarDataMapping::remap(const QString &rowRole,
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
int QItemModelBarDataMapping::rowCategoryIndex(const QString &category)
{
    return dptr()->m_rowCategories.indexOf(category);
}

/*!
 * /return index of the specified \a category in column categories list.
 * If the category is not found, -1 is returned.
 */
int QItemModelBarDataMapping::columnCategoryIndex(const QString &category)
{
    return dptr()->m_columnCategories.indexOf(category);
}

/*!
 * \internal
 */
QItemModelBarDataMappingPrivate *QItemModelBarDataMapping::dptr()
{
    return static_cast<QItemModelBarDataMappingPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QItemModelBarDataMappingPrivate *QItemModelBarDataMapping::dptrc() const
{
    return static_cast<const QItemModelBarDataMappingPrivate *>(d_ptr.data());
}

// QItemModelBarDataMappingPrivate

QItemModelBarDataMappingPrivate::QItemModelBarDataMappingPrivate(QItemModelBarDataMapping *q)
    : QAbstractDataMappingPrivate(q, QAbstractDataProxy::DataTypeBar)
{
}

QItemModelBarDataMappingPrivate::~QItemModelBarDataMappingPrivate()
{
}


QT_DATAVISUALIZATION_END_NAMESPACE

