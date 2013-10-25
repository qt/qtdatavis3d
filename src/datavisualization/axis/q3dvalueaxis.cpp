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
 * \since Qt Data Visualization 1.0
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
 * \inqmlmodule com.digia.QtDataVisualization
 * \since com.digia.QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates Q3DValueAxis
 * \inherits AbstractAxis3D
 * \brief The ValueAxis3D type is used for manipulating an axis of a graph.
 *
 * This type provides an axis that can be given a range of values and segment and subsegment
 * counts to divide the range into.
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
      m_segmentCount(5),
      m_subSegmentCount(1),
      m_labelFormat(Utils::defaultLabelFormat()),
      m_labelsDirty(true)
{
}

Q3DValueAxisPrivate::~Q3DValueAxisPrivate()
{
}

void Q3DValueAxisPrivate::setRange(qreal min, qreal max)
{
    bool dirty = (min != m_min || max != m_max);

    Q3DAbstractAxisPrivate::setRange(min, max);

    if (dirty)
        emitLabelsChanged();
}

void Q3DValueAxisPrivate::setMin(qreal min)
{
    bool dirty = (min != m_min);

    Q3DAbstractAxisPrivate::setMin(min);

    if (dirty)
        emitLabelsChanged();
}

void Q3DValueAxisPrivate::setMax(qreal max)
{
    bool dirty = (max != m_max);

    Q3DAbstractAxisPrivate::setMax(max);

    if (dirty)
        emitLabelsChanged();
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
