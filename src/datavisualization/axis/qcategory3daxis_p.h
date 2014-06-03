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
