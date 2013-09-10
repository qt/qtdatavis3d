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

#include "qabstractdatamapping_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QAbstractDataMapping
 * \inmodule QtDataVisualization
 * \brief Abstract base class for QtDataVisualization data mapping classes.
 * \since 1.0.0
 *
 * Data mapping classes provide a way to map data from an external data source to one of
 * QtDataVisualization data proxies.
 * \sa QItemModelBarDataMapping, QItemModelScatterDataMapping
 */

/*!
 * \internal
 */
QAbstractDataMapping::QAbstractDataMapping(QAbstractDataMappingPrivate *d, QObject *parent)
    : QObject(parent),
      d_ptr(d)
{
}

/*!
 * Destroys QAbstractDataMapping.
 */
QAbstractDataMapping::~QAbstractDataMapping()
{
}

/*!
 * \fn void QAbstractDataMapping::mappingChanged()
 *
 * Emitted when any mapping has changed.
 */

// QItemModelBarDataMappingPrivate

QAbstractDataMappingPrivate::QAbstractDataMappingPrivate(QAbstractDataMapping *q,
                                                         QAbstractDataProxy::DataType type)
    : QObject(0),
      q_ptr(q),
      m_type(type)
{
}

QAbstractDataMappingPrivate::~QAbstractDataMappingPrivate()
{
}

QT_DATAVISUALIZATION_END_NAMESPACE

