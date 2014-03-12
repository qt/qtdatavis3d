/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#include "qvalue3daxisformatter_p.h"
#include "qvalue3daxis_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QValue3DAxisFormatter
 * \inmodule QtDataVisualization
 * \brief QValue3DAxisFormatter is base class for value axis formatters.
 * \since Qt Data Visualization 1.1
 *
 * This class provides formatting rules for a linear QValue3DAxis. Subclass it if you
 * want to implement custom value axes.
 *
 * \sa QLog3DAxisFormatter
 */

/*!
 * \qmltype ValueAxis3DFormatter
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QValue3DAxisFormatter
 * \brief ValueAxis3DFormatter is base type for value axis formatters.
 *
 * This type provides formatting rules for a linear ValueAxis3D.
 * This type is the default type for ValueAxis3D and thus never needs to be explicitly created.
 */

/*!
 * \internal
 */
QValue3DAxisFormatter::QValue3DAxisFormatter(QValue3DAxisFormatterPrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

/*!
 * Constructs a new QValue3DAxisFormatter instance with optional \a parent.
 */
QValue3DAxisFormatter::QValue3DAxisFormatter(QObject *parent) :
    QObject(parent),
    d_ptr(new QValue3DAxisFormatterPrivate(this))
{
}

/*!
 * Destroys QValue3DAxisFormatter.
 */
QValue3DAxisFormatter::~QValue3DAxisFormatter()
{
}

/*!
 * \return \c true if negative values are valid values for parent axis.
 * The default implementation always returns true.
 */
bool QValue3DAxisFormatter::allowNegatives() const
{
    return true;
}

/*!
 * \return \c true if zero is a valid value for parent axis.
 * The default implementation always returns true.
 */
bool QValue3DAxisFormatter::allowZero() const
{
    return true;
}

/*!
 * \return the normalized position along the axis for the grid line indicated by the \a index.
 * The total amount of grid lines is equal to segment count plus one. The returned value should be
 * between -1.0 (for minimum value) and 1.0 (for maximum value), inclusive.
 * The grid line at the index zero corresponds to the minimum value of the axis.
 * This value is used to position the segment grid lines when the graph is rendered.
 *
 * \sa doRecalculate(), gridPositions()
 */
float QValue3DAxisFormatter::gridPosition(int index)
{
    d_ptr->recalculate();

    Q_ASSERT(d_ptr->m_gridPositions.size() > index);

    return d_ptr->m_gridPositions.at(index);
}

/*!
 * \return the normalized position along the axis for a subsegment grid line of a segment.
 * The \a subGridIndex and \a segmentIndex specify the desired subgrid line.
 * The returned value should be between -1.0 (for minimum value) and 1.0 (for maximum value),
 * inclusive.
 * The amount of subgrid lines per segment is equal to subsegment count minus one.
 * This value is used to position the subsegment grid lines when the graph is rendered.
 *
 * \sa doRecalculate(), subGridPositions()
 */
float QValue3DAxisFormatter::subGridPosition(int subGridIndex, int segmentIndex)
{
    d_ptr->recalculate();

    Q_ASSERT(d_ptr->m_subGridPositions.size() > subGridIndex);
    Q_ASSERT(d_ptr->m_subGridPositions.at(0).size() > segmentIndex);

    return d_ptr->m_subGridPositions.at(segmentIndex).at(subGridIndex);
}

/*!
 * \return the normalized position along the axis for given label \a index.
 * The total amount of labels is equal to segment count plus one. The label \a index zero
 * corresponds to the minimum value of the axis.
 * The returned value should be between -1.0 (for minimum value) and 1.0 (for maximum value),
 * inclusive. This value is used to position the axis labels when the graph is rendered.
 *
 * \sa doRecalculate(), labelPositions()
 */
float QValue3DAxisFormatter::labelPosition(int index)
{
    d_ptr->recalculate();

    Q_ASSERT(d_ptr->m_labelPositions.size() > index);

    return d_ptr->m_labelPositions.at(index);
}

/*!
 * This method can be used to convert a value to a string using formatter specific formatting
 * rules supported by the labelFormat property of the parent axis.
 *
 * \return the formatted label string using \a value and \a format.
 *
 * \sa doRecalculate(), doStringForValue(), QValue3DAxis::labelFormat
 */
QString QValue3DAxisFormatter::stringForValue(float value, const QString &format)
{
    d_ptr->recalculate();
    return doStringForValue(value, format);
}

/*!
 * \return the normalized position along the axis for the given \a value.
 * The returned value should be between -1.0 (for minimum value) and 1.0 (for maximum value),
 * inclusive. This value is used to position the items when the graph is rendered.
 *
 * \sa doRecalculate(), doPositionAt()
 */
float QValue3DAxisFormatter::positionAt(float value)
{
    d_ptr->recalculate();
    return doPositionAt(value);
}

/*!
 * \return the value at the normalized \a position along the axis.
 * The \a position value should be between -1.0 (for minimum value) and 1.0 (for maximum value),
 * inclusive.
 *
 * \sa doRecalculate(), doValueAt()
 */
float QValue3DAxisFormatter::valueAt(float position)
{
    d_ptr->recalculate();
    return doValueAt(position);
}

/*!
 * Creates a new empty instance of this formatter. Must be reimplemented in a subclass.
 *
 * \return the new instance. The renderer uses this method to cache a copy of the
 * the formatter. The ownership of the new copy transfers to the caller.
 */
QValue3DAxisFormatter *QValue3DAxisFormatter::createNewInstance()
{
    return new QValue3DAxisFormatter();
}

/*!
 * Populates the \a copy with the values of this formatter.
 *
 * \sa doPopulateCopy()
 */
void QValue3DAxisFormatter::populateCopy(QValue3DAxisFormatter &copy)
{
    d_ptr->recalculate();
    doPopulateCopy(copy);
}

/*!
 * Marks this formatter dirty, prompting the renderer to make a new copy of its cache on the next
 * renderer synchronization. Recalculation is also triggered on next access to the values.
 * This method should be called by a subclass whenever the formatter
 * is changed in a way that affects the resolved values.
 */
void QValue3DAxisFormatter::markDirty()
{
    d_ptr->m_needsRecalculate = true;
    if (d_ptr->m_axis)
        emit d_ptr->m_axis->dptr()->formatterDirty();
}

/*!
 * \return the parent axis. The parent axis must only be accessed in doRecalculate()
 * method to maintain thread safety in environments using a threaded renderer.
 *
 * \sa doRecalculate()
 */
QValue3DAxis *QValue3DAxisFormatter::axis() const
{
    return d_ptr->m_axis;
}

/*!
 * This method populates the label and grid line position arrays, as well as calculates
 * any values needed for mapping between value and position. It is allowed to access
 * the parent axis from inside this function.
 *
 * This method must be reimplemented in a subclass.
 *
 * \sa gridPositions(), subGridPositions(), labelPositions(), axis()
 */
void QValue3DAxisFormatter::doRecalculate()
{
    d_ptr->doRecalculate();
}

/*!
 * Reimplement this method in a subclass to resolve the formatted string for a given \a value
 * if the default formatting provided by QValue3DAxis::labelFormat property is not sufficient.
 *
 * \return the formatted label string using \a value and \a format.
 *
 * \sa doRecalculate(), stringForValue(), QValue3DAxis::labelFormat
 */
QString QValue3DAxisFormatter::doStringForValue(float value, const QString &format)
{
    return d_ptr->stringForValue(value, format);
}

/*!
 * Reimplement this method if the position cannot be resolved by linear interpolation
 * between the parent axis minimum and maximum values.
 *
 * \return the normalized position along the axis for the given \a value.
 * The returned value should be between -1.0 (for minimum value) and 1.0 (for maximum value),
 * inclusive, if the value is within the parent axis range.
 *
 * \sa doRecalculate(), positionAt()
 */
float QValue3DAxisFormatter::doPositionAt(float value)
{
    return d_ptr->positionAt(value);
}

/*!
 * Reimplement this method if the value cannot be resolved by linear interpolation
 * between the parent axis minimum and maximum values.
 *
 * \return the value at the normalized \a position along the axis.
 * The \a position value should be between -1.0 (for minimum value) and 1.0 (for maximum value),
 * inclusive to obtain values within the parent axis range.
 *
 * \sa doRecalculate(), positionAt()
 */
float QValue3DAxisFormatter::doValueAt(float position)
{
    return d_ptr->valueAt(position);
}

/*!
 * Copies all relevant values from this formatter to the \a copy.
 * When reimplementing this method in a subclass, call the the superclass version at some point.
 * The renderer uses this method to cache a copy of the the formatter.
 *
 * \return the new copy. The ownership of the new copy transfers to the caller.
 */
void QValue3DAxisFormatter::doPopulateCopy(QValue3DAxisFormatter &copy)
{
    copy.d_ptr->m_min = d_ptr->m_min;
    copy.d_ptr->m_max = d_ptr->m_max;
}

/*!
 * \return Returns a reference to the array of normalized grid positions.
 * The array size is equal to the segment count of the parent axis plus one.
 * The grid line at the index zero corresponds to the minimum value of the axis.
 *
 * \sa gridPosition()
 */
QVector<float> &QValue3DAxisFormatter::gridPositions()
{
    return d_ptr->m_gridPositions;
}

/*!
 * \return Returns a reference to the array of normalized subgrid positions.
 * The array size is equal to subsegment count of the parent axis minus one.
 *
 * \sa subGridPosition()
 */
QVector<QVector<float> > &QValue3DAxisFormatter::subGridPositions()
{
    return d_ptr->m_subGridPositions;
}

/*!
 * \return Returns a reference to the array of normalized label positions.
 * The array size is equal to the segment count of the parent axis plus one.
 *
 * \sa labelPosition()
 */
QVector<float> &QValue3DAxisFormatter::labelPositions()
{
    return d_ptr->m_labelPositions;
}

// QValue3DAxisFormatterPrivate
QValue3DAxisFormatterPrivate::QValue3DAxisFormatterPrivate(QValue3DAxisFormatter *q)
    : QObject(0),
      q_ptr(q),
      m_needsRecalculate(true),
      m_min(0.0f),
      m_max(0.0f),
      m_rangeNormalizer(0.0f),
      m_axis(0),
      m_preparsedParamType(Utils::ParamTypeUnknown)
{
}

QValue3DAxisFormatterPrivate::~QValue3DAxisFormatterPrivate()
{
}

void QValue3DAxisFormatterPrivate::recalculate()
{
    // Only recalculate if we need to and have m_axis pointer. If we do not have
    // m_axis, either we are not attached to an axis or this is a renderer cache.
    if (m_axis && m_needsRecalculate)
        q_ptr->doRecalculate();
}

void QValue3DAxisFormatterPrivate::doRecalculate()
{
    m_needsRecalculate = false;

    m_gridPositions.clear();
    m_subGridPositions.clear();
    m_labelPositions.clear();

    int segmentCount = m_axis->segmentCount();
    int subGridCount = m_axis->subSegmentCount() - 1;
    m_min = m_axis->min();
    m_max = m_axis->max();
    m_rangeNormalizer = (m_max - m_min) / 2.0f;

    m_gridPositions.resize(segmentCount + 1);

    float segmentStep = 2.0f / float(segmentCount);
    float subSegmentStep = 0;
    if (subGridCount > 0) {
        subSegmentStep = segmentStep / (subGridCount + 1);
        m_subGridPositions.resize(segmentCount);
        for (int i = 0; i < segmentCount; i++)
            m_subGridPositions[i].resize(subGridCount);
    }

    m_labelPositions.resize(segmentCount + 1);

    // Calculate positions

    for (int i = 0; i < segmentCount; i++) {
        float gridValue = -1.0f + segmentStep * i;
        m_gridPositions[i] = gridValue;
        m_labelPositions[i] = gridValue;
        if (m_subGridPositions.size()) {
            for (int j = 0; j < subGridCount; j++)
                m_subGridPositions[i][j] = gridValue + subSegmentStep * i;
        }
    }

    // Ensure max value doesn't suffer from any rounding errors
    m_gridPositions[segmentCount] = 1.0f;
    m_labelPositions[segmentCount] = 1.0f;
}

QString QValue3DAxisFormatterPrivate::stringForValue(float value, const QString &format)
{
    if (m_previousLabelFormat.compare(format)) {
        // Format string different than the previous one used, reparse it
        m_previousLabelFormat = format;
        m_preparsedParamType = Utils::findFormatParamType(format);
        m_labelFormatArray = format.toUtf8();
    }

    return Utils::formatLabel(m_labelFormatArray, m_preparsedParamType, value);
}

float QValue3DAxisFormatterPrivate::positionAt(float value) const
{
    return (((value - m_min) / m_rangeNormalizer) - 1.0f);
}

float QValue3DAxisFormatterPrivate::valueAt(float position) const
{
    return (((position + 1.0f) * m_rangeNormalizer) + m_min);
}

void QValue3DAxisFormatterPrivate::setAxis(QValue3DAxis *axis)
{
    Q_ASSERT(axis);

    connect(axis, &QValue3DAxis::segmentCountChanged,
            this, &QValue3DAxisFormatterPrivate::setNeedsRecalculate);
    connect(axis, &QValue3DAxis::subSegmentCountChanged,
            this, &QValue3DAxisFormatterPrivate::setNeedsRecalculate);
    connect(axis, &QAbstract3DAxis::rangeChanged,
            this, &QValue3DAxisFormatterPrivate::setNeedsRecalculate);

    m_axis = axis;
}

void QValue3DAxisFormatterPrivate::setNeedsRecalculate()
{
    m_needsRecalculate = true;
}

QT_END_NAMESPACE_DATAVISUALIZATION
