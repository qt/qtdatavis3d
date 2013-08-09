/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qvalueaxis.h"
#include "qvalueaxis_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

QValueAxis::QValueAxis() :
    QAbstractAxis(new QValueAxisPrivate(this))
{
}

QValueAxis::~QValueAxis()
{
}

void QValueAxis::setRange(qreal min, qreal max)
{
    dptr()->setRange(min, max);
    setAutoAdjustRange(false);
}

void QValueAxis::setMin(qreal min)
{
    dptr()->setMin(min);
    setAutoAdjustRange(false);
}

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

void QValueAxis::setSegmentCount(int count)
{
    if (dptr()->m_segmentCount != count){
        dptr()->m_segmentCount = count;
        dptr()->recreateLabels();
        emit segmentCountChanged(count);
    }
}

int QValueAxis::segmentCount() const
{
    return dptrc()->m_segmentCount;
}

void QValueAxis::setSubSegmentCount(int count)
{
    if (dptr()->m_subSegmentCount != count) {
        dptr()->m_subSegmentCount = count;
        emit subSegmentCountChanged(count);
    }
}

int QValueAxis::subSegmentCount() const
{
    return dptrc()->m_subSegmentCount;
}

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

void QValueAxis::setLabelFormat(const QString &format)
{
    if (dptr()->m_labelFormat != format) {
        dptr()->m_labelFormat = format;
        dptr()->recreateLabels();
        emit labelFormatChanged(format);
    }
}

QString QValueAxis::labelFormat() const
{
    return dptrc()->m_labelFormat;
}

QValueAxisPrivate *QValueAxis::dptr()
{
    return static_cast<QValueAxisPrivate *>(d_ptr.data());
}

const QValueAxisPrivate *QValueAxis::dptrc() const
{
    return static_cast<const QValueAxisPrivate *>(d_ptr.data());
}

QValueAxisPrivate::QValueAxisPrivate(QValueAxis *q)
    : QAbstractAxisPrivate(q, QAbstractAxis::AxisTypeValue),
      m_min(0.0),
      m_max(10.0),
      m_segmentCount(10),
      m_subSegmentCount(0),
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
        recreateLabels();
        emit qptr()->rangeChanged(min, max);
    }
}

void QValueAxisPrivate::setMin(qreal min)
{
    if (m_min != min) {
        if (min >= m_max) {
            m_min = m_max - 1.0;
            qWarning() << "Warning: Tried to set minimum to equal or larger than maximum for"
                          " value axis. Minimum automatically adjusted to a valid one:"
                       << min <<  "-->" << m_min;
        } else {
            m_min = min;
        }
        recreateLabels();
        emit qptr()->rangeChanged(m_min, m_max);
    }
}

void QValueAxisPrivate::setMax(qreal max)
{
    if (m_max != max) {
        if (max <= m_min) {
            m_max = m_min + 1.0;
            qWarning() << "Warning: Tried to set maximum to equal or smaller than minimum for"
                          " value axis. Maximum automatically adjusted to a valid one:"
                       << max <<  "-->" << m_max;
        } else {
            m_max = max;
        }
        recreateLabels();
        emit qptr()->rangeChanged(m_min, m_max);
    }
}

void QValueAxisPrivate::recreateLabels()
{
    QStringList newLabels;
    newLabels.reserve(m_segmentCount + 1);

    // First label is at axis min, which is an extra segment
    qreal segmentStep = (m_max - m_min) / m_segmentCount;

    for (int i = 0; i < m_segmentCount; i++) {
        // TODO Actually do proper formatting
        newLabels.append(QString::number(m_min + (segmentStep * i)));
    }
    // Ensure max label doesn't suffer from any rounding errors
    newLabels.append(QString::number(m_max));

    if (m_labels != newLabels) {
        m_labels = newLabels;
        emit q_ptr->labelsChanged();
    }
}

QValueAxis *QValueAxisPrivate::qptr()
{
    return static_cast<QValueAxis *>(q_ptr);
}

QT_DATAVIS3D_END_NAMESPACE
