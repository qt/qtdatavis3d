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

#include "q3dvalueaxis.h"
#include "q3dvalueaxis_p.h"
#include "utils_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class Q3DValueAxis
 * \inmodule QtDataVisualization
 * \brief The Q3DValueAxis class is used for manipulating an axis of a graph.
 * \since 1.0.0
 *
 * Q3DValueAxis provides an axis that can be given a range of values and segment and subsegment
 * counts to divide the range into.
 *
 * Labels are drawn between each segment. Grid lines are drawn between each segment and each
 * subsegment. \note If visible, there will always be at least two grid lines and labels indicating
 * the minimum and the maximum values of the range, as there is always at least one segment.
 */

/*!
 * \qmltype ValueAxis3D
 * \instantiates Q3DValueAxis
 * \inherits AbstractAxis3D
 *
 * This type provides an axis that can be given a range of values and segment and subsegment
 * counts to divide the range into.
 */

/*!
 * \qmlproperty real ValueAxis3D::min
 *
 * Defines the minimum value on the axis.
 * When setting this property the max is adjusted if necessary, to ensure that the range remains
 * valid.
 */

/*!
 * \qmlproperty real ValueAxis3D::max
 *
 * Defines the maximum value on the axis.
 * When setting this property the min is adjusted if necessary, to ensure that the range remains
 * valid.
 */

/*!
 * \qmlproperty int ValueAxis3D::segmentCount
 *
 * Defines the number of segments on the axis. This indicates how many labels are drawn. The number
 * of grid lines to be drawn is calculated with formula: \c {segments * subsegments + 1}.
 * The preset default is \c 5, and it can not be below \c 1.
 */

/*!
 * \qmlproperty int ValueAxis3D::subSegmentCount
 *
 * Defines the number of subsegments inside each segment on the axis. Grid lines are drawn between
 * each subsegment, in addition to each segment.
 * The preset default is \c 1, and it can not be below \c 1.
 */

/*!
 * \qmlproperty bool ValueAxis3D::autoAdjustRange
 *
 * Determines if the axis is to automatically calculate the range instead of setting range or
 * adjusting min or max property.
 */

/*!
 * \qmlproperty string ValueAxis3D::labelFormat
 *
 * Defines the label format to be used for the labels on this axis. Supported specifiers are:
 * \c {d, i, o, x, X, f, F, e, E, g, G, c}. See QString::sprintf() for additional details.
 */

/*!
 * Constructs Q3DValueAxis with the given \a parent.
 */
Q3DValueAxis::Q3DValueAxis(QObject *parent) :
    Q3DAbstractAxis(new Q3DValueAxisPrivate(this), parent)
{
}

/*!
 * Destroys Q3DValueAxis.
 */
Q3DValueAxis::~Q3DValueAxis()
{
}

/*!
 * Sets value range of the axis from \a min to \a max.
 * When setting the range, the max is adjusted if necessary, to ensure that the range remains valid.
 */
void Q3DValueAxis::setRange(qreal min, qreal max)
{
    dptr()->setRange(min, max);
    setAutoAdjustRange(false);
}

/*!
 * \property Q3DValueAxis::min
 *
 * Defines the minimum value on the axis.
 * When setting this property the max is adjusted if necessary, to ensure that the range remains
 * valid.
 */
void Q3DValueAxis::setMin(qreal min)
{
    dptr()->setMin(min);
    setAutoAdjustRange(false);
}

/*!
 * \property Q3DValueAxis::max
 *
 * Defines the maximum value on the axis.
 * When setting this property the min is adjusted if necessary, to ensure that the range remains
 * valid.
 */
void Q3DValueAxis::setMax(qreal max)
{
    dptr()->setMax(max);
    setAutoAdjustRange(false);
}

qreal Q3DValueAxis::min() const
{
    return dptrc()->m_min;
}

qreal Q3DValueAxis::max() const
{
    return dptrc()->m_max;
}

/*!
 * \property Q3DValueAxis::segmentCount
 *
 * Defines the number of segments on the axis. This indicates how many labels are drawn. The number
 * of grid lines to be drawn is calculated with formula: \c {segments * subsegments + 1}.
 * The preset default is \c 5, and it can not be below \c 1.
 *
 * \sa setSubSegmentCount()
 */
void Q3DValueAxis::setSegmentCount(int count)
{
    if (count <= 0) {
        qWarning() << "Warning: Illegal segment count automatically adjusted to a legal one:"
                   << count << "-> 1";
        count = 1;
    }
    if (dptr()->m_segmentCount != count){
        dptr()->m_segmentCount = count;
        dptr()->emitLabelsChanged();
        emit segmentCountChanged(count);
    }
}

int Q3DValueAxis::segmentCount() const
{
    return dptrc()->m_segmentCount;
}

/*!
 * \property Q3DValueAxis::subSegmentCount
 *
 * Defines the number of subsegments inside each segment on the axis. Grid lines are drawn between
 * each subsegment, in addition to each segment.
 * The preset default is \c 1, and it can not be below \c 1.
 *
 * \sa setSegmentCount()
 */
void Q3DValueAxis::setSubSegmentCount(int count)
{
    if (count <= 0) {
        qWarning() << "Warning: Illegal subsegment count automatically adjusted to a legal one:"
                   << count << "-> 1";
        count = 1;
    }
    if (dptr()->m_subSegmentCount != count) {
        dptr()->m_subSegmentCount = count;
        emit subSegmentCountChanged(count);
    }
}

int Q3DValueAxis::subSegmentCount() const
{
    return dptrc()->m_subSegmentCount;
}

/*!
 * \property Q3DValueAxis::autoAdjustRange
 *
 * Tells the axis to automatically calculate the range instead of setting range or adjusting min or
 * max property.
 *
 * \sa setRange(), setMin(), setMax()
 */
void Q3DValueAxis::setAutoAdjustRange(bool autoAdjust)
{
    if (dptr()->m_autoAdjust != autoAdjust) {
        dptr()->m_autoAdjust = autoAdjust;
        emit autoAdjustRangeChanged(autoAdjust);
    }
}

bool Q3DValueAxis::isAutoAdjustRange() const
{
    return dptrc()->m_autoAdjust;
}

/*!
 * \property Q3DValueAxis::labelFormat
 *
 * Defines the label format to be used for the labels on this axis. Supported specifiers are:
 * \c {d, i, o, x, X, f, F, e, E, g, G, c}. See QString::sprintf() for additional details.
 *
 * Usage example:
 *
 * \c {axis->setLabelFormat("%.2f mm");}
 */
void Q3DValueAxis::setLabelFormat(const QString &format)
{
    if (dptr()->m_labelFormat != format) {
        dptr()->m_labelFormat = format;
        dptr()->emitLabelsChanged();
        emit labelFormatChanged(format);
    }
}

QString Q3DValueAxis::labelFormat() const
{
    return dptrc()->m_labelFormat;
}

/*!
 * \internal
 */
Q3DValueAxisPrivate *Q3DValueAxis::dptr()
{
    return static_cast<Q3DValueAxisPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const Q3DValueAxisPrivate *Q3DValueAxis::dptrc() const
{
    return static_cast<const Q3DValueAxisPrivate *>(d_ptr.data());
}

Q3DValueAxisPrivate::Q3DValueAxisPrivate(Q3DValueAxis *q)
    : Q3DAbstractAxisPrivate(q, Q3DAbstractAxis::AxisTypeValue),
      m_min(0.0),
      m_max(10.0),
      m_segmentCount(5),
      m_subSegmentCount(1),
      m_autoAdjust(true),
      m_labelFormat(Utils::defaultLabelFormat()),
      m_labelsDirty(true)
{
}

Q3DValueAxisPrivate::~Q3DValueAxisPrivate()
{
}

void Q3DValueAxisPrivate::setRange(qreal min, qreal max)
{
    // If min >= max, we adjust ranges so that
    // m_max becomes (min + 1.0)
    // as axes need some kind of valid range.
    // TODO: Make "reverse" axes work (i.e. min > max)
    bool dirty = false;
    if (m_min != min) {
        m_min = min;
        dirty = true;
    }
    if (m_max != max || min >= max) {
        if (min >= max) {
            m_max = min + 1.0;
            qWarning() << "Warning: Tried to set invalid range for value axis."
                          " Range automatically adjusted to a valid one:"
                       << min << "-" << max << "-->" << m_min << "-" << m_max;
        } else {
            m_max = max;
        }
        dirty = true;
    }
    if (dirty) {
        emitLabelsChanged();
        emit qptr()->rangeChanged(min, max);
    }
}

void Q3DValueAxisPrivate::setMin(qreal min)
{
    if (m_min != min) {
        if (min >= m_max) {
            qreal oldMax = m_max;
            m_max = min + 1.0;
            qWarning() << "Warning: Tried to set minimum to equal or larger than maximum for"
                          " value axis. Maximum automatically adjusted to a valid one:"
                       << oldMax <<  "-->" << m_max;
        }
        m_min = min;
        emitLabelsChanged();
        emit qptr()->rangeChanged(m_min, m_max);
    }
}

void Q3DValueAxisPrivate::setMax(qreal max)
{
    if (m_max != max) {
        if (max <= m_min) {
            qreal oldMin = m_min;
            m_min = max - 1.0;
            qWarning() << "Warning: Tried to set maximum to equal or smaller than minimum for"
                          " value axis. Minimum automatically adjusted to a valid one:"
                       << oldMin <<  "-->" << m_min;
        }
        m_max = max;
        emitLabelsChanged();
        emit qptr()->rangeChanged(m_min, m_max);
    }
}

void Q3DValueAxisPrivate::emitLabelsChanged()
{
    m_labelsDirty = true;
    emit q_ptr->labelsChanged();
}

void Q3DValueAxisPrivate::updateLabels()
{
    if (!m_labelsDirty)
        return;

    m_labelsDirty = false;

    QStringList newLabels;
    newLabels.reserve(m_segmentCount + 1);

    // First label is at axis min, which is an extra segment
    qreal segmentStep = (m_max - m_min) / m_segmentCount;

    QString formatString(m_labelFormat);
    Utils::ParamType paramType = Utils::findFormatParamType(formatString);
    QByteArray formatArray = formatString.toUtf8();

    for (int i = 0; i < m_segmentCount; i++) {
        qreal value = m_min + (segmentStep * i);
        newLabels.append(Utils::formatLabel(formatArray, paramType, value));
    }

    // Ensure max label doesn't suffer from any rounding errors
    newLabels.append(Utils::formatLabel(formatArray, paramType, m_max));

    if (m_labels != newLabels)
        m_labels = newLabels;
}

Q3DValueAxis *Q3DValueAxisPrivate::qptr()
{
    return static_cast<Q3DValueAxis *>(q_ptr);
}

QT_DATAVISUALIZATION_END_NAMESPACE
