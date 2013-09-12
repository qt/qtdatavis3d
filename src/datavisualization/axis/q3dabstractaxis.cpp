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

#include "q3dabstractaxis.h"
#include "q3dabstractaxis_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class Q3DAbstractAxis
 * \inmodule QtDataVisualization
 * \brief Q3DAbstractAxis is base class for axes of a graph.
 * \since 1.0.0
 *
 * You should not need to use this class directly, but one of its subclasses instead.
 *
 * \sa Q3DCategoryAxis, Q3DValueAxis
 */

/*!
 * \qmltype AbstractAxis3D
 * \instantiates Q3DAbstractAxis
 *
 * You should not need to use this type directly.
 */

/*!
 * \qmlproperty string AbstractAxis3D::title
 * Defines the title for the axis.
 */

/*!
 * \qmlproperty list AbstractAxis3D::labels
 * Defines the labels for the axis.
 */

/*!
 * \qmlproperty AbstractAxis3D.AxisOrientation AbstractAxis3D::orientation
 * Defines the orientation of the axis.
 */

/*!
 * \qmlproperty AbstractAxis3D.AxisType AbstractAxis3D::type
 * Defines the type of the axis.
 */

/*!
 * \enum Q3DAbstractAxis::AxisOrientation
 *
 * The orientation of the axis object.
 *
 * \value AxisOrientationNone
 * \value AxisOrientationX
 * \value AxisOrientationY
 * \value AxisOrientationZ
 */

/*!
 * \enum Q3DAbstractAxis::AxisType
 *
 * The type of the axis object.
 *
 * \value AxisTypeNone
 * \value AxisTypeCategory
 * \value AxisTypeValue
 */

/*!
 * \internal
 */
Q3DAbstractAxis::Q3DAbstractAxis(Q3DAbstractAxisPrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

/*!
 * Destroys Q3DAbstractAxis.
 */
Q3DAbstractAxis::~Q3DAbstractAxis()
{
}

/*!
 * \property Q3DAbstractAxis::title
 *
 * Defines the title for the axis.
 */
QString Q3DAbstractAxis::title() const
{
    return d_ptr->m_title;
}

/*!
 * \property Q3DAbstractAxis::labels
 *
 * Defines the labels for the axis.
 */
QStringList Q3DAbstractAxis::labels() const
{
    d_ptr->updateLabels();
    return d_ptr->m_labels;
}

/*!
 * \property Q3DAbstractAxis::orientation
 *
 * Defines the orientation of the axis, one of \c Q3DAbstractAxis::AxisOrientation.
 */
Q3DAbstractAxis::AxisOrientation Q3DAbstractAxis::orientation() const
{
    return d_ptr->m_orientation;
}

/*!
 * \property Q3DAbstractAxis::type
 *
 * Defines the type of the axis, one of \c Q3DAbstractAxis::AxisType.
 */
Q3DAbstractAxis::AxisType Q3DAbstractAxis::type() const
{
    return d_ptr->m_type;
}

void Q3DAbstractAxis::setTitle(QString title)
{
    if (d_ptr->m_title != title) {
        d_ptr->m_title = title;
        emit titleChanged(title);
    }
}

// Q3DAbstractAxisPrivate

Q3DAbstractAxisPrivate::Q3DAbstractAxisPrivate(Q3DAbstractAxis *q, Q3DAbstractAxis::AxisType type)
    : QObject(0),
      q_ptr(q),
      m_orientation(Q3DAbstractAxis::AxisOrientationNone),
      m_type(type),
      m_isDefaultAxis(false)
{
}

Q3DAbstractAxisPrivate::~Q3DAbstractAxisPrivate()
{
}

void Q3DAbstractAxisPrivate::setOrientation(Q3DAbstractAxis::AxisOrientation orientation)
{
    if (m_orientation == Q3DAbstractAxis::AxisOrientationNone)
        m_orientation = orientation;
    else
        Q_ASSERT("Attempted to reset axis orientation.");
}

void Q3DAbstractAxisPrivate::updateLabels()
{
    // Default implementation does nothing
}

QT_DATAVISUALIZATION_END_NAMESPACE
