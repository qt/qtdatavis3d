/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

#ifndef QCATEGORY3DAXIS_P_H
#define QCATEGORY3DAXIS_P_H

#include "qcategory3daxis.h"
#include "qabstract3daxis_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QCategory3DAxisPrivate : public QAbstract3DAxisPrivate
{
    Q_OBJECT

public:
    QCategory3DAxisPrivate(QCategory3DAxis *q);
    virtual ~QCategory3DAxisPrivate();

    void setDataLabels(const QStringList &labels);

protected:
    virtual bool allowZero();
    virtual bool allowNegatives();
    virtual bool allowMinMaxSame();

private:
    QCategory3DAxis *qptr();

    bool m_labelsExplicitlySet;
    friend class QCategory3DAxis;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
