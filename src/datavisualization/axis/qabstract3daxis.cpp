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

#include "qabstract3daxis.h"
#include "qabstract3daxis_p.h"

namespace QtDataVisualization {

/*!
 * \class QAbstract3DAxis
 * \inmodule QtDataVisualization
 * \brief QAbstract3DAxis is base class for axes of a graph.
 * \since Qt Data Visualization 1.0
 *
 * You should not need to use this class directly, but one of its subclasses instead.
 *
 * \sa QCategory3DAxis, QValue3DAxis
 */

/*!
 * \qmltype AbstractAxis3D
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QAbstract3DAxis
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
 * \note Setting this property for ValueAxis3D does nothing, as it generates labels automatically.
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
 * \enum QAbstract3DAxis::AxisOrientation
 *
 * The orientation of the axis object.
 *
 * \value AxisOrientationNone
 * \value AxisOrientationX
 * \value AxisOrientationY
 * \value AxisOrientationZ
 */

/*!
 * \enum QAbstract3DAxis::AxisType
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
QAbstract3DAxis::QAbstract3DAxis(QAbstract3DAxisPrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

/*!
 * Destroys QAbstract3DAxis.
 */
QAbstract3DAxis::~QAbstract3DAxis()
{
}

/*!
 * \property QAbstract3DAxis::orientation
 *
 * Defines the orientation of the axis, one of \c QAbstract3DAxis::AxisOrientation.
 */
QAbstract3DAxis::AxisOrientation QAbstract3DAxis::orientation() const
{
    return d_ptr->m_orientation;
}

/*!
 * \property QAbstract3DAxis::type
 *
 * Defines the type of the axis, one of \c QAbstract3DAxis::AxisType.
 */
QAbstract3DAxis::AxisType QAbstract3DAxis::type() const
{
    return d_ptr->m_type;
}

/*!
 * \property QAbstract3DAxis::title
 *
 * Defines the title for the axis.
 */
void QAbstract3DAxis::setTitle(QString title)
{
    if (d_ptr->m_title != title) {
        d_ptr->m_title = title;
        emit titleChanged(title);
    }
}

QString QAbstract3DAxis::title() const
{
    return d_ptr->m_title;
}

/*!
 * \property QAbstract3DAxis::labels
 *
 * Defines the labels for the axis.
 * \note Setting this property for QValue3DAxis does nothing, as it generates labels automatically.
 */
void QAbstract3DAxis::setLabels(const QStringList &labels)
{
    Q_UNUSED(labels)
}

QStringList QAbstract3DAxis::labels() const
{
    d_ptr->updateLabels();
    return d_ptr->m_labels;
}

/*!
 * Sets value range of the axis from \a min to \a max.
 * When setting the range, the max is adjusted if necessary, to ensure that the range remains valid.
 * \note For QCategory3DAxis this specifies the index range of rows or columns to show.
 */
void QAbstract3DAxis::setRange(float min, float max)
{
    d_ptr->setRange(min, max);
    setAutoAdjustRange(false);
}

/*!
 * \property QAbstract3DAxis::min
 *
 * Defines the minimum value on the axis.
 * When setting this property the max is adjusted if necessary, to ensure that the range remains
 * valid.
 * \note For QCategory3DAxis this specifies the index of the first row or column to show.
 */
void QAbstract3DAxis::setMin(float min)
{
    d_ptr->setMin(min);
    setAutoAdjustRange(false);
}

/*!
 * \property QAbstract3DAxis::max
 *
 * Defines the maximum value on the axis.
 * When setting this property the min is adjusted if necessary, to ensure that the range remains
 * valid.
 * \note For QCategory3DAxis this specifies the index of the last row or column to show.
 */
void QAbstract3DAxis::setMax(float max)
{
    d_ptr->setMax(max);
    setAutoAdjustRange(false);
}

float QAbstract3DAxis::min() const
{
    return d_ptr->m_min;
}

float QAbstract3DAxis::max() const
{
    return d_ptr->m_max;
}

/*!
 * \property QAbstract3DAxis::autoAdjustRange
 *
 * If set, the axis will automatically adjust the range so that all data fits in it.
 *
 * \sa setRange(), setMin(), setMax()
 */
void QAbstract3DAxis::setAutoAdjustRange(bool autoAdjust)
{
    if (d_ptr->m_autoAdjust != autoAdjust) {
        d_ptr->m_autoAdjust = autoAdjust;
        emit autoAdjustRangeChanged(autoAdjust);
    }
}

bool QAbstract3DAxis::isAutoAdjustRange() const
{
    return d_ptr->m_autoAdjust;
}

/*!
 * \fn QAbstract3DAxis::rangeChanged(float min, float max)
 *
 * Emits range \a min and \a max values when range changes.
 */

// QAbstract3DAxisPrivate

QAbstract3DAxisPrivate::QAbstract3DAxisPrivate(QAbstract3DAxis *q, QAbstract3DAxis::AxisType type)
    : QObject(0),
      q_ptr(q),
      m_orientation(QAbstract3DAxis::AxisOrientationNone),
      m_type(type),
      m_isDefaultAxis(false),
      m_min(0.0f),
      m_max(10.0f),
      m_autoAdjust(true),
      m_onlyPositiveValues(false),
      m_allowMinMaxSame(false)
{
}

QAbstract3DAxisPrivate::~QAbstract3DAxisPrivate()
{
}

void QAbstract3DAxisPrivate::setOrientation(QAbstract3DAxis::AxisOrientation orientation)
{
    if (m_orientation == QAbstract3DAxis::AxisOrientationNone) {
        m_orientation = orientation;
        emit q_ptr->orientationChanged(orientation);
    } else {
        Q_ASSERT("Attempted to reset axis orientation.");
    }
}

void QAbstract3DAxisPrivate::updateLabels()
{
    // Default implementation does nothing
}

void QAbstract3DAxisPrivate::setRange(float min, float max)
{
    bool adjusted = false;
    if (m_onlyPositiveValues) {
        if (min < 0.0f) {
            min = 0.0f;
            adjusted = true;
        }
        if (max < 0.0f) {
            max = 0.0f;
            adjusted = true;
        }
    }
    // If min >= max, we adjust ranges so that
    // m_max becomes (min + 1.0f)
    // as axes need some kind of valid range.
    bool minDirty = false;
    bool maxDirty = false;
    if (m_min != min) {
        m_min = min;
        minDirty = true;
    }
    if (m_max != max || min > max || (!m_allowMinMaxSame && min == max)) {
        if (min > max || (!m_allowMinMaxSame && min == max)) {
            m_max = min + 1.0f;
            adjusted = true;
        } else {
            m_max = max;
        }
        maxDirty = true;
    }

    if (minDirty || maxDirty) {
        if (adjusted) {
            qWarning() << "Warning: Tried to set invalid range for axis."
                          " Range automatically adjusted to a valid one:"
                       << min << "-" << max << "-->" << m_min << "-" << m_max;
        }
        emit q_ptr->rangeChanged(m_min, m_max);
    }

    if (minDirty)
        emit q_ptr->minChanged(m_min);
    if (maxDirty)
        emit q_ptr->maxChanged(m_max);
}

void QAbstract3DAxisPrivate::setMin(float min)
{
    if (m_onlyPositiveValues) {
        if (min < 0.0f) {
            min = 0.0f;
            qWarning() << "Warning: Tried to set negative minimum for an axis that only supports"
                          " positive values:" << min;
        }
    }

    if (m_min != min) {
        bool maxChanged = false;
        if (min > m_max || (!m_allowMinMaxSame && min == m_max)) {
            float oldMax = m_max;
            m_max = min + 1.0f;
            qWarning() << "Warning: Tried to set minimum to equal or larger than maximum for"
                          " value axis. Maximum automatically adjusted to a valid one:"
                       << oldMax <<  "-->" << m_max;
            maxChanged = true;
        }
        m_min = min;

        emit q_ptr->rangeChanged(m_min, m_max);
        emit q_ptr->minChanged(m_min);
        if (maxChanged)
            emit q_ptr->maxChanged(m_max);
    }
}

void QAbstract3DAxisPrivate::setMax(float max)
{
    if (m_onlyPositiveValues) {
        if (max < 0.0f) {
            max = 0.0f;
            qWarning() << "Warning: Tried to set negative maximum for an axis that only supports"
                          " positive values:" << max;
        }
    }

    if (m_max != max) {
        bool minChanged = false;
        if (m_min > max || (!m_allowMinMaxSame && m_min == max)) {
            float oldMin = m_min;
            m_min = max - 1.0f;
            if (m_onlyPositiveValues && m_min < 0.0f) {
                m_min = 0.0f;
                if (!m_allowMinMaxSame && max == 0.0f) {
                    m_min = oldMin;
                    qWarning() << "Unable to set maximum value to zero.";
                    return;
                }
            }
            qWarning() << "Warning: Tried to set maximum to equal or smaller than minimum for"
                          " value axis. Minimum automatically adjusted to a valid one:"
                       << oldMin <<  "-->" << m_min;
            minChanged = true;
        }
        m_max = max;
        emit q_ptr->rangeChanged(m_min, m_max);
        emit q_ptr->maxChanged(m_max);
        if (minChanged)
            emit q_ptr->minChanged(m_min);
    }
}

}
