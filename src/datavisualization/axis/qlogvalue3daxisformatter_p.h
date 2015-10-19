/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QLOGVALUE3DAXISFORMATTER_P_H
#define QLOGVALUE3DAXISFORMATTER_P_H

#include "datavisualizationglobal_p.h"
#include "qlogvalue3daxisformatter.h"
#include "qvalue3daxisformatter_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QLogValue3DAxisFormatterPrivate : public QValue3DAxisFormatterPrivate
{
    Q_OBJECT

public:
    QLogValue3DAxisFormatterPrivate(QLogValue3DAxisFormatter *q);
    virtual ~QLogValue3DAxisFormatterPrivate();

    void recalculate();
    void populateCopy(QValue3DAxisFormatter &copy) const;

    float positionAt(float value) const;
    float valueAt(float position) const;

protected:
    QLogValue3DAxisFormatter *qptr();

    qreal m_base;
    qreal m_logMin;
    qreal m_logMax;
    qreal m_logRangeNormalizer;
    bool m_autoSubGrid;
    bool m_showEdgeLabels;

private:
    bool m_evenMinSegment;
    bool m_evenMaxSegment;

    friend class QLogValue3DAxisFormatter;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
