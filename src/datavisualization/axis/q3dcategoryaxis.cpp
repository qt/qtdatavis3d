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
#include "bars3dcontroller_p.h"
#include "qbardataproxy.h"

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
 * \qmltype CategoryAxis3D
 * \instantiates Q3DCategoryAxis
 * \inherits AbstractAxis3D
 *
 * This type provides an axis that can be given labels.
 */

/*!
 * \qmlproperty list CategoryAxis3D::categoryLabels
 * Defines labels for axis applied to categories. If there are fewer labels than categories, the
 * remaining ones do not have a label. If category labels are not explicitly defined, labels are
 * generated from the data row and column labels.
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
 * remaining ones do not have a label. If category labels are not explicitly defined, labels are
 * generated from the data row and column labels.
 *
 * \note CategoryLabels actually reads/writes the Q3DAbstractAxis::labels property,
 * which is read only there. Since subclass cannot have property with same name,
 * this partially duplicate property is necessary.
 */
QStringList Q3DCategoryAxis::categoryLabels() const
{
    return labels();
}

void Q3DCategoryAxis::setCategoryLabels(const QStringList &labels)
{
    dptr()->m_labelsExplicitlySet = !labels.isEmpty();
    bool labelsFromData = false;

    // Get labels from data proxy if axis is attached to a bar controller and an active axis there
    if (labels.isEmpty()) {
        Bars3DController *controller = qobject_cast<Bars3DController *>(parent());
        if (controller) {
            if (controller->axisX() == this) {
                controller->handleDataRowLabelsChanged();
                labelsFromData = true;
            } else if (controller->axisZ() == this) {
                controller->handleDataColumnLabelsChanged();
                labelsFromData = true;
            }
        }
    }

    if (!labelsFromData && d_ptr->m_labels != labels) {
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
    : Q3DAbstractAxisPrivate(q, Q3DAbstractAxis::AxisTypeCategory),
      m_labelsExplicitlySet(false)
{
    m_onlyPositiveValues = true;
    m_allowMinMaxSame = true;
}

Q3DCategoryAxisPrivate::~Q3DCategoryAxisPrivate()
{
}

/*!
 * \internal
 * Controller uses this function to set labels from data proxy as category labels.
 * If the labels have been explicitly set by user, data proxy labels are not used.
 */
void Q3DCategoryAxisPrivate::setDataLabels(const QStringList &labels)
{
    if (!m_labelsExplicitlySet && m_labels != labels) {
        m_labels = labels;
        emit qptr()->labelsChanged();
    }
}

Q3DCategoryAxis *Q3DCategoryAxisPrivate::qptr()
{
    return static_cast<Q3DCategoryAxis *>(q_ptr);
}

QT_DATAVISUALIZATION_END_NAMESPACE
