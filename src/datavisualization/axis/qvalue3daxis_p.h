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

#include "qvalue3daxis.h"
#include "qabstract3daxis_p.h"

#ifndef QVALUE3DAXIS_P_H
#define QVALUE3DAXIS_P_H

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QValue3DAxisPrivate : public QAbstract3DAxisPrivate
{
    Q_OBJECT

public:
    QValue3DAxisPrivate(QValue3DAxis *q);
    virtual ~QValue3DAxisPrivate();

    virtual void setRange(float min, float max);
    virtual void setMin(float min);
    virtual void setMax (float max);

    void emitLabelsChanged();

signals:
    void formatterDirty();

protected:
    virtual void updateLabels();

    virtual bool allowZero();
    virtual bool allowNegatives();
    virtual bool allowMinMaxSame();

    int m_segmentCount;
    int m_subSegmentCount;
    QString m_labelFormat;
    bool m_labelsDirty;
    QValue3DAxisFormatter *m_formatter;

private:
    QValue3DAxis *qptr();

    friend class QValue3DAxis;
    friend class Abstract3DController;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
