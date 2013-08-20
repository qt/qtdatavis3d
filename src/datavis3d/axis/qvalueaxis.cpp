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

#include "qvalueaxis.h"
#include "qvalueaxis_p.h"
#include "utils_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QValueAxis
 * \inmodule QtDataVis3D
 * \brief The QValueAxis class is used for manipulating an axis of a graph.
 * \since 1.0.0
 *
 * QValueAxis provides an axis that can be given a range of values and segment and subsegment
 * counts to divide the range into.
 *
 * Labels are drawn between each segment. Grid lines are drawn between each segment and each
 * subsegment. \note If visible, there will always be at least two grid lines and labels indicating
 * the minimum and the maximum values of the range, as there is always at least one segment.
 */

/*!
 * Constructs QValueAxis.
 */
QValueAxis::QValueAxis() :
    QAbstractAxis(new QValueAxisPrivate(this))
{
}

/*!
 * Destroys QValueAxis.
 */
QValueAxis::~QValueAxis()
{
}

/*!
 * Sets value range of the axis from \a min to \a max.
 * When setting the range, the max is adjusted if necessary, to ensure that the range remains valid.
 */
void QValueAxis::setRange(qreal min, qreal max)
{
    dptr()->setRange(min, max);
    setAutoAdjustRange(false);
}

/*!
 * \property QValueAxis::min
 *
 * Defines the minimum value on the axis.
 * When setting this property the max is adjusted if necessary, to ensure that the range remains
 * valid.
 */
void QValueAxis::setMin(qreal min)
{
    dptr()->setMin(min);
    setAutoAdjustRange(false);
}

/*!
 * \property QValueAxis::max
 *
 * Defines the maximum value on the axis.
 * When setting this property the min is adjusted if necessary, to ensure that the range remains
 * valid.
 */
void QValueAxis::setMax(qreal max)
{
    dptr()->setMax(max);
    setAutoAdjustRange(false);
}

qreal QValueAxis::min() const
{
    return dptrc()->m_min;
}

qreal QValueAxis::max() const
{
    return dptrc()->m_max;
}

/*!
 * \property QValueAxis::segmentCount
 *
 * Defines the number of segments on the axis. This indicates how many labels are drawn. The number
 * of grid lines to be drawn is calculated with formula: \c {segments * subsegments + 1}.
 * The preset default is \c 5, and it can not be below \c 1.
 *
 * \sa setSubSegmentCount()
 */
void QValueAxis::setSegmentCount(int count)
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

int QValueAxis::segmentCount() const
{
    return dptrc()->m_segmentCount;
}

/*!
 * \property QValueAxis::subSegmentCount
 *
 * Defines the number of subsegments inside each segment on the axis. Grid lines are drawn between
 * each subsegment, in addition to each segment.
 * The preset default is \c 1, and it can not be below \c 1.
 *
 * \sa setSegmentCount()
 **/
void QValueAxis::setSubSegmentCount(int count)
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

int QValueAxis::subSegmentCount() const
{
    return dptrc()->m_subSegmentCount;
}

/*!
 * \property QValueAxis::autoAdjustRange
 *
 * Tells the axis to automatically calculate the range instead of setting range or adjusting min or
 * max property.
 *
 * \sa setRange(), setMin(), setMax()
 */
void QValueAxis::setAutoAdjustRange(bool autoAdjust)
{
    if (dptr()->m_autoAdjust != autoAdjust) {
        dptr()->m_autoAdjust = autoAdjust;
        emit autoAdjustRangeChanged(autoAdjust);
    }
}

bool QValueAxis::isAutoAdjustRange() const
{
    return dptrc()->m_autoAdjust;
}

/*!
 * \property QValueAxis::labelFormat
 *
 * Defines the label format to be used for the labels on this axis. Supported specifiers are:
 * \c {d, i, o, x, X, f, F, e, E, g, G, c}. See QString::sprintf() for additional details.
 *
 * Usage example:
 *
 * \c {axis->setLabelFormat("%.2f mm");}
 */
void QValueAxis::setLabelFormat(const QString &format)
{
    if (dptr()->m_labelFormat != format) {
        dptr()->m_labelFormat = format;
        dptr()->emitLabelsChanged();
        emit labelFormatChanged(format);
    }
}

QString QValueAxis::labelFormat() const
{
    return dptrc()->m_labelFormat;
}

/*!
 * \internal
 */
QValueAxisPrivate *QValueAxis::dptr()
{
    return static_cast<QValueAxisPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QValueAxisPrivate *QValueAxis::dptrc() const
{
    return static_cast<const QValueAxisPrivate *>(d_ptr.data());
}

QValueAxisPrivate::QValueAxisPrivate(QValueAxis *q)
    : QAbstractAxisPrivate(q, QAbstractAxis::AxisTypeValue),
      m_min(0.0),
      m_max(10.0),
      m_segmentCount(5),
      m_subSegmentCount(1),
      m_autoAdjust(true)
{
}

QValueAxisPrivate::~QValueAxisPrivate()
{
}

void QValueAxisPrivate::setRange(qreal min, qreal max)
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
    if (m_max != max) {
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

void QValueAxisPrivate::setMin(qreal min)
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

void QValueAxisPrivate::setMax(qreal max)
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

void QValueAxisPrivate::emitLabelsChanged()
{
    m_labelsDirty = true;
    emit q_ptr->labelsChanged();
}

void QValueAxisPrivate::updateLabels()
{
    if (!m_labelsDirty)
        return;

    m_labelsDirty = false;

    QStringList newLabels;
    newLabels.reserve(m_segmentCount + 1);

    // First label is at axis min, which is an extra segment
    qreal segmentStep = (m_max - m_min) / m_segmentCount;

    QString formatString(m_labelFormat);
    if (formatString.isEmpty())
        formatString = Utils::defaultLabelFormat();

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

QValueAxis *QValueAxisPrivate::qptr()
{
    return static_cast<QValueAxis *>(q_ptr);
}

QT_DATAVIS3D_END_NAMESPACE
