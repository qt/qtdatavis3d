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

#include "qitemmodelscatterdatamapping_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QItemModelScatterDataMapping
 * \inmodule QtDataVisualization
 * \brief Item model mapping for Q3DScatter.
 * \since Qt Data Visualization 1.0
 *
 * QItemModelScatterDataMapping is used to map roles of QAbstractItemModel to the XYZ-values
 * of Q3DScatter points.
 *
 * QItemModelScatterDataMapping ignores rows and columns of the QAbstractItemModel and treats
 * all items equally. It requires the model to provide at least three roles for the data items
 * that can be mapped to X, Y, and Z-values for the scatter points.
 *
 * For example, assume that you have a custom QAbstractItemModel for storing various measurements
 * done on material samples, providing data for roles such as "density", "hardness", and
 * "conductivity". You could visualize these properties on a scatter graph:
 *
 * \snippet doc_src_qtdatavisualization.cpp 4
 *
 * \sa QItemModelScatterDataProxy, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmltype ScatterDataMapping
 * \inqmlmodule com.digia.QtDataVisualization
 * \since com.digia.QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QItemModelScatterDataMapping
 * \brief Item model mapping for Scatter3D.
 *
 * This type is used to map roles of AbstractItemModel to the XYZ-values of Scatter3D points. For
 * a more complete description, see QItemModelScatterDataMapping.
 *
 * Usage example:
 *
 * \snippet doc_src_qmldatavisualization.cpp 5
 *
 * \sa ItemModelScatterDataProxy, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmlproperty string ScatterDataMapping::xPosRole
 * The X position role of the mapping.
 */

/*!
 * \qmlproperty string ScatterDataMapping::yPosRole
 * The Y position role of the mapping.
 */

/*!
 * \qmlproperty string ScatterDataMapping::zPosRole
 * The Z position role of the mapping.
 */

/*!
 * Constructs QItemModelScatterDataMapping with the given \a parent.
 */
QItemModelScatterDataMapping::QItemModelScatterDataMapping(QObject *parent)
    : QAbstractDataMapping(new QItemModelScatterDataMappingPrivate(this), parent)
{
}

/*!
 * Constructs QItemModelScatterDataMapping with \a xPosRole, \a yPosRole, \a zPosRole
 * and the given \a parent.
 */
QItemModelScatterDataMapping::QItemModelScatterDataMapping(const QString &xPosRole,
                                                           const QString &yPosRole,
                                                           const QString &zPosRole,
                                                           QObject *parent)
    : QAbstractDataMapping(new QItemModelScatterDataMappingPrivate(this), parent)
{
    dptr()->m_xPosRole = xPosRole;
    dptr()->m_yPosRole = yPosRole;
    dptr()->m_zPosRole = zPosRole;
}

/*!
 * Destroys QItemModelScatterDataMapping.
 */
QItemModelScatterDataMapping::~QItemModelScatterDataMapping()
{
}

/*!
 * \property QItemModelScatterDataMapping::xPosRole
 *
 * Defines the X position role for the mapping.
 */
void QItemModelScatterDataMapping::setXPosRole(const QString &role)
{
    dptr()->m_xPosRole = role;
    emit mappingChanged();
}

QString QItemModelScatterDataMapping::xPosRole() const
{
    return dptrc()->m_xPosRole;
}

/*!
 * \property QItemModelScatterDataMapping::yPosRole
 *
 * Defines the Y position role for the mapping.
 */
void QItemModelScatterDataMapping::setYPosRole(const QString &role)
{
    dptr()->m_yPosRole = role;
    emit mappingChanged();
}

QString QItemModelScatterDataMapping::yPosRole() const
{
    return dptrc()->m_yPosRole;
}

/*!
 * \property QItemModelScatterDataMapping::zPosRole
 *
 * Defines the Z position role for the mapping.
 */
void QItemModelScatterDataMapping::setZPosRole(const QString &role)
{
    dptr()->m_zPosRole = role;
    emit mappingChanged();
}

QString QItemModelScatterDataMapping::zPosRole() const
{
    return dptrc()->m_zPosRole;
}

/*!
 * Changes \a xPosRole, \a yPosRole and \a zPosRole to the mapping.
 *
 * Emits mappingChanged() signal after remapping.
 */
void QItemModelScatterDataMapping::remap(const QString &xPosRole, const QString &yPosRole,
                                         const QString &zPosRole)
{
    dptr()->m_xPosRole = xPosRole;
    dptr()->m_yPosRole = yPosRole;
    dptr()->m_zPosRole = zPosRole;

    emit mappingChanged();
}

/*!
 * \internal
 */
QItemModelScatterDataMappingPrivate *QItemModelScatterDataMapping::dptr()
{
    return static_cast<QItemModelScatterDataMappingPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QItemModelScatterDataMappingPrivate *QItemModelScatterDataMapping::dptrc() const
{
    return static_cast<const QItemModelScatterDataMappingPrivate *>(d_ptr.data());
}

// QItemModelScatterDataMappingPrivate

QItemModelScatterDataMappingPrivate::QItemModelScatterDataMappingPrivate(
        QItemModelScatterDataMapping *q)
    : QAbstractDataMappingPrivate(q, QAbstractDataProxy::DataTypeScatter)
{
}

QItemModelScatterDataMappingPrivate::~QItemModelScatterDataMappingPrivate()
{
}

QT_DATAVISUALIZATION_END_NAMESPACE

