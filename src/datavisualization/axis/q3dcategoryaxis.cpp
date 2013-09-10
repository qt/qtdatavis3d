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

#include "q3dcategoryaxis.h"
#include "q3dcategoryaxis_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class Q3DCategoryAxis
 * \inmodule QtDataVisualization
 * \brief The Q3DCategoryAxis class is used for manipulating an axis of a graph.
 * \since 1.0.0
 *
 * Q3DCategoryAxis provides an axis that can be given labels. The axis is divided into equal-sized
 * categories based on \l {Q3DBars::setDataWindow()}{data window} size.
 *
 * Grid lines are drawn between categories, if visible. Labels are drawn to positions of categories
 * if provided.
 */

/*!
 * Constructs Q3DCategoryAxis with \a parent.
 */
Q3DCategoryAxis::Q3DCategoryAxis(QObject *parent) :
    Q3DAbstractAxis(new Q3DCategoryAxisPrivate(this), parent)
{
}

/*!
 * Destroys Q3DCategoryAxis.
 */
Q3DCategoryAxis::~Q3DCategoryAxis()
{
}

/*!
 * \property Q3DCategoryAxis::categoryLabels
 *
 * Defines labels for axis applied to categories. If there are fewer labels than categories, the
 * remaining ones do not have a label.
 */
QStringList Q3DCategoryAxis::categoryLabels() const
{
    return labels();
}

void Q3DCategoryAxis::setCategoryLabels(const QStringList &labels)
{
    if (d_ptr->m_labels != labels) {
        d_ptr->m_labels = labels;
        emit labelsChanged();
    }
}

/*!
 * \internal
 */
Q3DCategoryAxisPrivate *Q3DCategoryAxis::dptr()
{
    return static_cast<Q3DCategoryAxisPrivate *>(d_ptr.data());
}

Q3DCategoryAxisPrivate::Q3DCategoryAxisPrivate(Q3DCategoryAxis *q)
    : Q3DAbstractAxisPrivate(q, Q3DAbstractAxis::AxisTypeCategory)
{
}

Q3DCategoryAxisPrivate::~Q3DCategoryAxisPrivate()
{
}

QT_DATAVISUALIZATION_END_NAMESPACE
