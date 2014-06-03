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
