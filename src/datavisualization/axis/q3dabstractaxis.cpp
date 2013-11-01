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
 * \since Qt Data Visualization 1.0
 *
 * You should not need to use this class directly, but one of its subclasses instead.
 *
 * \sa Q3DCategoryAxis, Q3DValueAxis
 */

/*!
 * \qmltype AbstractAxis3D
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates Q3DAbstractAxis
 * \brief AbstractAxis3D is base type for axes of a graph.
 *
 * This type is uncreatable, but contains properties that are exposed via subtypes.
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
 * \qmlproperty real AbstractAxis3D::min
 *
 * Defines the minimum value on the axis.
 * When setting this property the max is adjusted if necessary, to ensure that the range remains
 * valid.
 */

/*!
 * \qmlproperty real AbstractAxis3D::max
 *
 * Defines the maximum value on the axis.
 * When setting this property the min is adjusted if necessary, to ensure that the range remains
 * valid.
 */

/*!
 * \qmlproperty bool AbstractAxis3D::autoAdjustRange
 *
 * If set, the axis will automatically adjust the range so that all data fits in it.
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

/*!
 * Sets value range of the axis from \a min to \a max.
 * When setting the range, the max is adjusted if necessary, to ensure that the range remains valid.
 * \note For Q3DCategoryAxis this specifies the index range of rows or columns to show.
 */
void Q3DAbstractAxis::setRange(qreal min, qreal max)
{
    d_ptr->setRange(min, max);
    setAutoAdjustRange(false);
}

/*!
 * \property Q3DAbstractAxis::min
 *
 * Defines the minimum value on the axis.
 * When setting this property the max is adjusted if necessary, to ensure that the range remains
 * valid.
 * \note For Q3DCategoryAxis this specifies the index of the first row or column to show.
 */
void Q3DAbstractAxis::setMin(qreal min)
{
    d_ptr->setMin(min);
    setAutoAdjustRange(false);
}

/*!
 * \property Q3DAbstractAxis::max
 *
 * Defines the maximum value on the axis.
 * When setting this property the min is adjusted if necessary, to ensure that the range remains
 * valid.
 * \note For Q3DCategoryAxis this specifies the index of the last row or column to show.
 */
void Q3DAbstractAxis::setMax(qreal max)
{
    d_ptr->setMax(max);
    setAutoAdjustRange(false);
}

qreal Q3DAbstractAxis::min() const
{
    return d_ptr->m_min;
}

qreal Q3DAbstractAxis::max() const
{
    return d_ptr->m_max;
}

/*!
 * \property Q3DAbstractAxis::autoAdjustRange
 *
 * If set, the axis will automatically adjust the range so that all data fits in it.
 *
 * \sa setRange(), setMin(), setMax()
 */
void Q3DAbstractAxis::setAutoAdjustRange(bool autoAdjust)
{
    if (d_ptr->m_autoAdjust != autoAdjust) {
        d_ptr->m_autoAdjust = autoAdjust;
        emit autoAdjustRangeChanged(autoAdjust);
    }
}

bool Q3DAbstractAxis::isAutoAdjustRange() const
{
    return d_ptr->m_autoAdjust;
}

// Q3DAbstractAxisPrivate

Q3DAbstractAxisPrivate::Q3DAbstractAxisPrivate(Q3DAbstractAxis *q, Q3DAbstractAxis::AxisType type)
    : QObject(0),
      q_ptr(q),
      m_orientation(Q3DAbstractAxis::AxisOrientationNone),
      m_type(type),
      m_isDefaultAxis(false),
      m_min(0.0),
      m_max(10.0),
      m_autoAdjust(true),
      m_onlyPositiveValues(false),
      m_allowMinMaxSame(false)
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

void Q3DAbstractAxisPrivate::setRange(qreal min, qreal max)
{
    bool adjusted = false;
    if (m_onlyPositiveValues) {
        if (min < 0.0) {
            min = 0.0;
            adjusted = true;
        }
        if (max < 0.0) {
            max = 0.0;
            adjusted = true;
        }
    }
    // If min >= max, we adjust ranges so that
    // m_max becomes (min + 1.0)
    // as axes need some kind of valid range.
    bool dirty = false;
    if (m_min != min) {
        m_min = min;
        dirty = true;
    }
    if (m_max != max || min > max || (!m_allowMinMaxSame && min == max)) {
        if (min > max || (!m_allowMinMaxSame && min == max)) {
            m_max = min + 1.0;
            adjusted = true;
        } else {
            m_max = max;
        }
        dirty = true;
    }

    if (dirty) {
        if (adjusted) {
            qWarning() << "Warning: Tried to set invalid range for axis."
                          " Range automatically adjusted to a valid one:"
                       << min << "-" << max << "-->" << m_min << "-" << m_max;
        }
        emit q_ptr->rangeChanged(m_min, m_max);
    }
}

void Q3DAbstractAxisPrivate::setMin(qreal min)
{
    if (m_onlyPositiveValues) {
        if (min < 0.0) {
            min = 0.0;
            qWarning() << "Warning: Tried to set negative minimum for an axis that only supports"
                          " positive values:" << min;
        }
    }

    if (m_min != min) {
        if (min > m_max || (!m_allowMinMaxSame && min == m_max)) {
            qreal oldMax = m_max;
            m_max = min + 1.0;
            qWarning() << "Warning: Tried to set minimum to equal or larger than maximum for"
                          " value axis. Maximum automatically adjusted to a valid one:"
                       << oldMax <<  "-->" << m_max;
        }
        m_min = min;

        emit q_ptr->rangeChanged(m_min, m_max);
    }
}

void Q3DAbstractAxisPrivate::setMax(qreal max)
{
    if (m_onlyPositiveValues) {
        if (max < 0.0) {
            max = 0.0;
            qWarning() << "Warning: Tried to set negative maximum for an axis that only supports"
                          " positive values:" << max;
        }
    }

    if (m_max != max) {
        if (m_min > max || (!m_allowMinMaxSame && m_min == max)) {
            qreal oldMin = m_min;
            m_min = max - 1.0;
            if (m_onlyPositiveValues && m_min < 0.0) {
                m_min = 0.0;
                if (!m_allowMinMaxSame && max == 0.0) {
                    m_min = oldMin;
                    qWarning() << "Unable to set maximum value to zero.";
                    return;
                }
            }
            qWarning() << "Warning: Tried to set maximum to equal or smaller than minimum for"
                          " value axis. Minimum automatically adjusted to a valid one:"
                       << oldMin <<  "-->" << m_min;
        }
        m_max = max;
        emit q_ptr->rangeChanged(m_min, m_max);
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
