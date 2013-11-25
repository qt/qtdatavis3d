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

#include "q3dvalueaxis.h"
#include "q3dabstractaxis_p.h"

#ifndef QVALUEAXIS_P_H
#define QVALUEAXIS_P_H

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DValueAxisPrivate : public Q3DAbstractAxisPrivate
{
    Q_OBJECT

public:
    Q3DValueAxisPrivate(Q3DValueAxis *q);
    virtual ~Q3DValueAxisPrivate();

    virtual void setRange(float min, float max);
    virtual void setMin(float min);
    virtual void setMax (float max);

protected:
    void emitLabelsChanged();
    virtual void updateLabels();

    int m_segmentCount;
    int m_subSegmentCount;
    QString m_labelFormat;
    bool m_labelsDirty;

private:
    Q3DValueAxis *qptr();

    friend class Q3DValueAxis;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QVALUEAXIS_P_H
