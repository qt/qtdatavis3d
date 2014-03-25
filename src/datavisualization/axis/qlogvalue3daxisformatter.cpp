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

#include "qlogvalue3daxisformatter_p.h"
#include "qvalue3daxis_p.h"
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QLogValue3DAxisFormatter
 * \inmodule QtDataVisualization
 * \brief QLogValue3DAxisFormatter implements logarithmic value axis formatter.
 * \since Qt Data Visualization 1.1
 *
 * This class provides formatting rules for a logarithmic QValue3DAxis.
 *
 * When a QLogValue3DAxisFormatter is attached to a QValue3DAxis, the axis range
 * cannot include negative values or the zero.
 *
 * \sa QValue3DAxisFormatter
 */

/*!
 * \qmltype LogValueAxis3DFormatter
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.1
 * \ingroup datavisualization_qml
 * \instantiates QLogValue3DAxisFormatter
 * \brief LogValueAxis3DFormatter implements logarithmic value axis formatter.
 *
 * This type provides formatting rules for a logarithmic ValueAxis3D.
 * When a LogValueAxis3DFormatter is attached to a ValueAxis3D, the axis range
 * cannot include negative values or the zero.
 */

/*!
 * \qmlproperty real LogValueAxis3DFormatter::base
 *
 * The \a base of the logarithm used to map axis values. If the base is non-zero, the parent axis
 * segment count will be automatically adjusted whenever the axis formatter is recalculated.
 * If you want the range to be divided into equal segments like normal value axis, set this
 * property value to zero.
 *
 * The \a base has to be zero or positive value and not equal to one.
 * Defaults to ten.
 *
 * \sa ValueAxis3D::segmentCount
 */

/*!
 * \qmlproperty bool LogValueAxis3DFormatter::autoSubGrid
 *
 * If this property value is set to \c true, the parent axis sub-segment count is adjusted
 * automatically according to the base property value. The number of subsegments is set to
 * base value minus one, rounded down.
 * Defaults to \c true.
 *
 * \sa base, ValueAxis3D::subSegmentCount
 */

/*!
 * \qmlproperty bool LogValueAxis3DFormatter::showMaxLabel
 *
 * When the base property is non-zero, the segments that get automatically generated and depending
 * on the range, the last segment is often smaller than the other segments. In extreme cases this
 * can lead to overlapping labels on the last two grid lines. By setting this property to \c false,
 * you can suppress showing the max label for the axis in cases where the segments do not exactly
 * fit the axis.
 * Defaults to \c true.
 *
 * \sa base, AbstractAxis3D::labels
 */

/*!
 * \internal
 */
QLogValue3DAxisFormatter::QLogValue3DAxisFormatter(QLogValue3DAxisFormatterPrivate *d,
                                                   QObject *parent) :
    QValue3DAxisFormatter(d, parent)
{
}

/*!
 * Constructs a new QLogValue3DAxisFormatter instance with optional \a parent.
 */
QLogValue3DAxisFormatter::QLogValue3DAxisFormatter(QObject *parent) :
    QValue3DAxisFormatter(new QLogValue3DAxisFormatterPrivate(this), parent)
{
}

/*!
 * Destroys QLogValue3DAxisFormatter.
 */
QLogValue3DAxisFormatter::~QLogValue3DAxisFormatter()
{
}

/*!
 * \property QLogValue3DAxisFormatter::base
 *
 * The \a base of the logarithm used to map axis values. If the base is non-zero, the parent axis
 * segment count will be automatically adjusted whenever the axis formatter is recalculated.
 * If you want the range to be divided into equal segments like normal value axis, set this
 * property value to zero.
 *
 * The \a base has to be zero or positive value and not equal to one.
 * Defaults to ten.
 *
 * \sa QValue3DAxis::segmentCount
 */
void QLogValue3DAxisFormatter::setBase(qreal base)
{
    if (base < 0.0f || base == 1.0f) {
        qWarning() << "Warning: The logarithm base must be greater than 0 and not equal to 1,"
                   << "attempted:" << base;
        return;
    }
    if (dptr()->m_base != base) {
        dptr()->m_base = base;
        markDirty(true);
        emit baseChanged(base);
    }
}

qreal QLogValue3DAxisFormatter::base() const
{
    return dptrc()->m_base;
}

/*!
 * \property QLogValue3DAxisFormatter::autoSubGrid
 *
 * If this property value is set to \c true, the parent axis sub-segment count is adjusted
 * automatically according to the base property value. The number of subsegments is set to
 * base value minus one, rounded up.
 * Defaults to \c true.
 *
 * \sa base, QValue3DAxis::subSegmentCount
 */
void QLogValue3DAxisFormatter::setAutoSubGrid(bool enabled)
{
    if (dptr()->m_autoSubGrid != enabled) {
        dptr()->m_autoSubGrid = enabled;
        markDirty(false);
        emit autoSubGridChanged(enabled);
    }
}

bool QLogValue3DAxisFormatter::autoSubGrid() const
{
    return dptrc()->m_autoSubGrid;
}

/*!
 * \property QLogValue3DAxisFormatter::showMaxLabel
 *
 * When the base property is non-zero, the segments get automatically generated, and depending
 * on the range, the last segment is often smaller than the other segments. In extreme cases this
 * can lead to overlapping labels on the last two grid lines. By setting this property to \c false,
 * you can suppress showing the max label for the axis in cases where the segments do not exactly
 * fit the axis.
 * Defaults to \c true.
 *
 * \sa base, QAbstract3DAxis::labels
 */
void QLogValue3DAxisFormatter::setShowMaxLabel(bool enabled)
{
    if (dptr()->m_showMaxLabel != enabled) {
        dptr()->m_showMaxLabel = enabled;
        markDirty(true);
        emit showMaxLabelChanged(enabled);
    }
}

bool QLogValue3DAxisFormatter::showMaxLabel() const
{
    return dptrc()->m_showMaxLabel;
}

/*!
 * \internal
 */
bool QLogValue3DAxisFormatter::allowNegatives() const
{
    return false;
}

/*!
 * \internal
 */
bool QLogValue3DAxisFormatter::allowZero() const
{
    return false;
}

/*!
 * \internal
 */
QValue3DAxisFormatter *QLogValue3DAxisFormatter::createNewInstance() const
{
    return new QLogValue3DAxisFormatter();
}

/*!
 * \internal
 */
void QLogValue3DAxisFormatter::recalculate()
{
    dptr()->recalculate();
}

/*!
 * \internal
 */
float QLogValue3DAxisFormatter::positionAt(float value) const
{
    return dptrc()->positionAt(value);
}

/*!
 * \internal
 */
float QLogValue3DAxisFormatter::valueAt(float position) const
{
    return dptrc()->valueAt(position);
}

/*!
 * \internal
 */
void QLogValue3DAxisFormatter::populateCopy(QValue3DAxisFormatter &copy) const
{
    QValue3DAxisFormatter::populateCopy(copy);
    dptrc()->populateCopy(copy);
}

/*!
 * \internal
 */
QString QLogValue3DAxisFormatter::labelForIndex(int index) const
{
    return dptrc()->labelForIndex(index);
}

/*!
 * \internal
 */
QLogValue3DAxisFormatterPrivate *QLogValue3DAxisFormatter::dptr()
{
    return static_cast<QLogValue3DAxisFormatterPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QLogValue3DAxisFormatterPrivate *QLogValue3DAxisFormatter::dptrc() const
{
    return static_cast<const QLogValue3DAxisFormatterPrivate *>(d_ptr.data());
}

// QLogValue3DAxisFormatterPrivate
QLogValue3DAxisFormatterPrivate::QLogValue3DAxisFormatterPrivate(QLogValue3DAxisFormatter *q)
    : QValue3DAxisFormatterPrivate(q),
      m_base(10.0f),
      m_logMin(0.0f),
      m_logMax(0.0f),
      m_logRangeNormalizer(0.0f),
      m_autoSubGrid(true),
      m_showMaxLabel(true),
      m_evenSegments(true)
{
}

QLogValue3DAxisFormatterPrivate::~QLogValue3DAxisFormatterPrivate()
{
}

void QLogValue3DAxisFormatterPrivate::recalculate()
{
    if (m_base > 0.0) {
        // Update parent axis segment counts
        qreal logMin = qLn(qreal(m_min)) / qLn(m_base);
        qreal logMax = qLn(qreal(m_max)) / qLn(m_base);
        qreal logRangeNormalizer = logMax - logMin;

        int segmentCount = qCeil(logRangeNormalizer);

        m_axis->setSegmentCount(segmentCount);
        if (m_autoSubGrid) {
            int subSegmentCount = qCeil(m_base) - 1;
            if (subSegmentCount < 1)
                subSegmentCount = 1;
            m_axis->setSubSegmentCount(subSegmentCount);
        }

        resetPositionArrays();

        // Calculate segment positions
        for (int i = 0; i < segmentCount; i++) {
            float gridValue = float(qreal(i) / logRangeNormalizer);
            m_gridPositions[i] = gridValue;
            m_labelPositions[i] = gridValue;
        }
        // Ensure max value doesn't suffer from any rounding errors
        m_gridPositions[segmentCount] = 1.0f;
        m_labelPositions[segmentCount] = 1.0f;
        float lastDiff = 1.0f - m_labelPositions.at(segmentCount - 1);
        float firstDiff = m_labelPositions.at(1) - m_labelPositions.at(0);
        m_evenSegments = qFuzzyCompare(lastDiff, firstDiff);
    } else {
        // Grid lines and label positions are the same as the parent class, so call parent impl
        // first to populate those
        QValue3DAxisFormatterPrivate::doRecalculate();
        m_evenSegments = true;
    }

    // When doing position/value mappings, base doesn't matter, so just use natural logarithm
    m_logMin = qLn(qreal(m_min));
    m_logMax = qLn(qreal(m_max));
    m_logRangeNormalizer = m_logMax - m_logMin;

    // Subgrid line positions are logarithmically spaced
    int subGridCount = m_axis->subSegmentCount() - 1;
    if (subGridCount > 0) {
        float firstSegmentRange = valueAt(m_gridPositions.at(1)) - m_min;
        float subSegmentStep = firstSegmentRange / float(subGridCount + 1);

        // Since the logarithm has the same curvature across whole axis range, we can just calculate
        // subgrid positions for the first segment and replicate them to other segments.
        QVector<float> actualSubSegmentSteps(subGridCount);

        for (int i = 0; i < subGridCount; i++) {
            float currentSubPosition = positionAt(m_min + ((i + 1) * subSegmentStep));
            actualSubSegmentSteps[i] = currentSubPosition;
        }

        int segmentCount = m_axis->segmentCount();
        for (int i = 0; i < segmentCount; i++) {
            for (int j = 0; j < subGridCount; j++) {
                float position = m_gridPositions.at(i) + actualSubSegmentSteps.at(j);
                if (position > 1.0f)
                    position  = 1.0f;
                m_subGridPositions[i][j] = position;
            }
        }
    }
}

void QLogValue3DAxisFormatterPrivate::populateCopy(QValue3DAxisFormatter &copy) const
{
    QLogValue3DAxisFormatter *logFormatter = static_cast<QLogValue3DAxisFormatter *>(&copy);
    QLogValue3DAxisFormatterPrivate *priv = logFormatter->dptr();

    priv->m_base = m_base;
    priv->m_logMin = m_logMin;
    priv->m_logMax = m_logMax;
    priv->m_logRangeNormalizer = m_logRangeNormalizer;
}

float QLogValue3DAxisFormatterPrivate::positionAt(float value) const
{
    qreal logValue = qLn(qreal(value));
    float retval = float((logValue - m_logMin) / m_logRangeNormalizer);

    return retval;
}

float QLogValue3DAxisFormatterPrivate::valueAt(float position) const
{
    qreal logValue = (qreal(position) * m_logRangeNormalizer) + m_logMin;
    return float(qExp(logValue));
}

QString QLogValue3DAxisFormatterPrivate::labelForIndex(int index) const
{
    if (index == m_gridPositions.size() - 1 && !m_evenSegments && !m_showMaxLabel)
        return QString();
    else
        return QValue3DAxisFormatterPrivate::labelForIndex(index);
}

QT_END_NAMESPACE_DATAVISUALIZATION
