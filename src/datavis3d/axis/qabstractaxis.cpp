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

#include "qabstractaxis.h"
#include "qabstractaxis_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QAbstractAxis
 * \inmodule QtDataVis3D
 * \brief QAbstractAxis is base class for axes of a graph.
 * \since 1.0.0
 *
 * You should not need to use this class directly, but one of its subclasses instead.
 *
 * \sa QCategoryAxis, QValueAxis
 */

/*!
 * \enum QAbstractAxis::AxisOrientation
 *
 * The orientation of the axis object.
 *
 * \value AxisOrientationNone
 * \value AxisOrientationX
 * \value AxisOrientationY
 * \value AxisOrientationZ
 */

/*!
 * \enum QAbstractAxis::AxisType
 *
 * The type of the axis object.
 *
 * \value AxisTypeNone
 * \value AxisTypeCategory
 * \value AxisTypeValue
 */

/*!
 * Constructs QAbstractAxis with \a d.
 */
QAbstractAxis::QAbstractAxis(QAbstractAxisPrivate *d) :
    QObject(0),
    d_ptr(d)
{
}

/*!
 * Destroys QAbstractAxis.
 */
QAbstractAxis::~QAbstractAxis()
{
}

/*!
 * \property QAbstractAxis::title
 *
 * Defines the title for the axis.
 */
QString QAbstractAxis::title() const
{
    return d_ptr->m_title;
}

/*!
 * \property QAbstractAxis::labels
 *
 * Defines the labels for the axis.
 */
QStringList QAbstractAxis::labels() const
{
    d_ptr->updateLabels();
    return d_ptr->m_labels;
}

/*!
 * \property QAbstractAxis::orientation
 *
 * Defines the orientation of the axis, one of \c QAbstractAxis::AxisOrientation.
 */
QAbstractAxis::AxisOrientation QAbstractAxis::orientation() const
{
    return d_ptr->m_orientation;
}

/*!
 * \property QAbstractAxis::type
 *
 * Defines the type of the axis, one of \c QAbstractAxis::AxisType.
 */
QAbstractAxis::AxisType QAbstractAxis::type() const
{
    return d_ptr->m_type;
}

void QAbstractAxis::setTitle(QString title)
{
    if (d_ptr->m_title != title) {
        d_ptr->m_title = title;
        emit titleChanged(title);
    }
}

// QAbstractAxisPrivate

QAbstractAxisPrivate::QAbstractAxisPrivate(QAbstractAxis *q, QAbstractAxis::AxisType type)
    : QObject(0),
      q_ptr(q),
      m_orientation(QAbstractAxis::AxisOrientationNone),
      m_type(type)
{
}

QAbstractAxisPrivate::~QAbstractAxisPrivate()
{
}

void QAbstractAxisPrivate::setOrientation(QAbstractAxis::AxisOrientation orientation)
{
    if (m_orientation == QAbstractAxis::AxisOrientationNone)
        m_orientation = orientation;
    else
        Q_ASSERT("Attempted to reset axis orientation.");
}

void QAbstractAxisPrivate::updateLabels()
{
    // Default implementation does nothing
}

QT_DATAVIS3D_END_NAMESPACE
