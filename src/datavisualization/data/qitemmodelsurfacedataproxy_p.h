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

#ifndef QITEMMODELSURFACEDATAPROXY_P_H
#define QITEMMODELSURFACEDATAPROXY_P_H

#include "qitemmodelsurfacedataproxy.h"
#include "qsurfacedataproxy_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class SurfaceItemModelHandler;

class QItemModelSurfaceDataProxyPrivate : public QSurfaceDataProxyPrivate
{
    Q_OBJECT
public:
    QItemModelSurfaceDataProxyPrivate(QItemModelSurfaceDataProxy *q);
    virtual ~QItemModelSurfaceDataProxyPrivate();

private:
    QItemModelSurfaceDataProxy *qptr();

    SurfaceItemModelHandler *m_itemModelHandler;

    QString m_rowRole;
    QString m_columnRole;
    QString m_valueRole;

    // For row/column items, sort items into these categories. Other categories are ignored.
    QStringList m_rowCategories;
    QStringList m_columnCategories;

    bool m_useModelCategories;
    bool m_autoRowCategories;
    bool m_autoColumnCategories;

    friend class SurfaceItemModelHandler;
    friend class QItemModelSurfaceDataProxy;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif
