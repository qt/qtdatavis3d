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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include "datavisualizationglobal_p.h"
#include "q3dabstractaxis.h"
#include "abstract3dcontroller_p.h"

#ifndef Q3DABSTRACTAXIS_P_H
#define Q3DABSTRACTAXIS_P_H

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DAbstractAxisPrivate : public QObject
{
    Q_OBJECT
public:
    Q3DAbstractAxisPrivate(Q3DAbstractAxis *q, Q3DAbstractAxis::AxisType type);
    virtual ~Q3DAbstractAxisPrivate();

    void setOrientation(Q3DAbstractAxis::AxisOrientation orientation);

    inline bool isDefaultAxis() { return m_isDefaultAxis; }
    inline void setDefaultAxis(bool isDefault) { m_isDefaultAxis = isDefault; }

    virtual void setRange(qreal min, qreal max);
    virtual void setMin(qreal min);
    virtual void setMax (qreal max);

protected:
    virtual void updateLabels();

    Q3DAbstractAxis *q_ptr;

    QString m_title;
    QStringList m_labels;
    Q3DAbstractAxis::AxisOrientation m_orientation;
    Q3DAbstractAxis::AxisType m_type;
    bool m_isDefaultAxis;
    qreal m_min;
    qreal m_max;
    bool m_autoAdjust;
    bool m_onlyPositiveValues;
    bool m_allowMinMaxSame;

    friend class Q3DAbstractAxis;
    friend class Q3DValueAxis;
    friend class Q3DCategoryAxis;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QABSTRACTAXIS_P_H
