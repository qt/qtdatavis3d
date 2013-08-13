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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include "qvalueaxis.h"
#include "qabstractaxis_p.h"

#ifndef QVALUEAXIS_P_H
#define QVALUEAXIS_P_H

QT_DATAVIS3D_BEGIN_NAMESPACE

class QValueAxisPrivate : public QAbstractAxisPrivate
{
    Q_OBJECT

public:
    QValueAxisPrivate(QValueAxis *q);
    virtual ~QValueAxisPrivate();

    void setRange(qreal min, qreal max);
    void setMin(qreal min);
    void setMax (qreal max);

protected:
    void recreateLabels();

    qreal m_min;
    qreal m_max;
    int m_segmentCount;
    int m_subSegmentCount;
    bool m_autoAdjust;
    QString m_labelFormat;

private:
    QValueAxis *qptr();

    friend class QValueAxis;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QVALUEAXIS_P_H
