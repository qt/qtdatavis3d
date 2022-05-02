/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
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

#ifndef QVALUE3DAXIS_P_H
#define QVALUE3DAXIS_P_H

#include "qvalue3daxis.h"
#include "qabstract3daxis_p.h"

QT_BEGIN_NAMESPACE

class QValue3DAxisPrivate : public QAbstract3DAxisPrivate
{
    Q_OBJECT

public:
    QValue3DAxisPrivate(QValue3DAxis *q);
    virtual ~QValue3DAxisPrivate();

    void setRange(float min, float max, bool suppressWarnings = false) override;
    void setMin(float min) override;
    void setMax (float max) override;

    void emitLabelsChanged();

Q_SIGNALS:
    void formatterDirty();

protected:
    void updateLabels() override;

    bool allowZero() override;
    bool allowNegatives() override;
    bool allowMinMaxSame() override;

    int m_segmentCount;
    int m_subSegmentCount;
    QString m_labelFormat;
    bool m_labelsDirty;
    QValue3DAxisFormatter *m_formatter;
    bool m_reversed;

private:
    QValue3DAxis *qptr();

    friend class QValue3DAxis;
    friend class Abstract3DController;
};

QT_END_NAMESPACE

#endif
