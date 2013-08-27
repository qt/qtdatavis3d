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

#include "qcategoryaxis.h"
#include "qcategoryaxis_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QCategoryAxis
 * \inmodule QtDataVis3D
 * \brief The QCategoryAxis class is used for manipulating an axis of a graph.
 * \since 1.0.0
 *
 * QCategoryAxis provides an axis that can be given labels. The axis is divided into equal-sized
 * categories based on \l {Q3DBars::setDataWindow()}{data window} size.
 *
 * Grid lines are drawn between categories, if visible. Labels are drawn to positions of categories
 * if provided.
 */

/*!
 * Constructs QCategoryAxis with \a parent.
 */
QCategoryAxis::QCategoryAxis(QObject *parent) :
    QAbstractAxis(new QCategoryAxisPrivate(this), parent)
{
}

/*!
 * Destroys QCategoryAxis.
 */
QCategoryAxis::~QCategoryAxis()
{
}

/*!
 * \property QCategoryAxis::categoryLabels
 *
 * Defines labels for axis applied to categories. If there are fewer labels than categories, the
 * remaining ones do not have a label.
 */
QStringList QCategoryAxis::categoryLabels() const
{
    return labels();
}

void QCategoryAxis::setCategoryLabels(const QStringList &labels)
{
    if (d_ptr->m_labels != labels) {
        d_ptr->m_labels = labels;
        emit labelsChanged();
    }
}

/*!
 * \internal
 */
QCategoryAxisPrivate *QCategoryAxis::dptr()
{
    return static_cast<QCategoryAxisPrivate *>(d_ptr.data());
}

QCategoryAxisPrivate::QCategoryAxisPrivate(QCategoryAxis *q)
    : QAbstractAxisPrivate(q, QAbstractAxis::AxisTypeCategory)
{
}

QCategoryAxisPrivate::~QCategoryAxisPrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE
