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

#include "qitemmodelbardatamapping.h"
#include "qabstractdatamapping_p.h"

#ifndef QITEMMODELBARDATAMAPPING_P_H
#define QITEMMODELBARDATAMAPPING_P_H

QT_DATAVIS3D_BEGIN_NAMESPACE

class QItemModelBarDataMappingPrivate : public QAbstractDataMappingPrivate
{
    Q_OBJECT
public:
    QItemModelBarDataMappingPrivate(QItemModelBarDataMapping *q);
    virtual ~QItemModelBarDataMappingPrivate();

private:
    QString m_rowRole;
    QString m_columnRole;
    QString m_valueRole;

    // For row/column items, sort items into these categories. Other categories are ignored.
    QStringList m_rowCategories;
    QStringList m_columnCategories;

    friend class QItemModelBarDataMapping;
};

QT_DATAVIS3D_END_NAMESPACE

#endif
