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

QCategoryAxis::QCategoryAxis() :
    QAbstractAxis(new QCategoryAxisPrivate(this))
{
}

QCategoryAxis::~QCategoryAxis()
{
}

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
