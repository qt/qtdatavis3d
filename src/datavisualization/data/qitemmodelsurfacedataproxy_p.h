/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
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

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class SurfaceItemModelHandler;

class QItemModelSurfaceDataProxyPrivate : public QSurfaceDataProxyPrivate
{
    Q_OBJECT
public:
    QItemModelSurfaceDataProxyPrivate(QItemModelSurfaceDataProxy *q);
    virtual ~QItemModelSurfaceDataProxyPrivate();

    void connectItemModelHandler();

private:
    QItemModelSurfaceDataProxy *qptr();

    SurfaceItemModelHandler *m_itemModelHandler;

    QString m_rowRole;
    QString m_columnRole;
    QString m_xPosRole;
    QString m_yPosRole;
    QString m_zPosRole;

    // For row/column items, sort items into these categories. Other categories are ignored.
    QStringList m_rowCategories;
    QStringList m_columnCategories;

    bool m_useModelCategories;
    bool m_autoRowCategories;
    bool m_autoColumnCategories;

    QRegExp m_rowRolePattern;
    QRegExp m_columnRolePattern;
    QRegExp m_xPosRolePattern;
    QRegExp m_yPosRolePattern;
    QRegExp m_zPosRolePattern;

    QString m_rowRoleReplace;
    QString m_columnRoleReplace;
    QString m_xPosRoleReplace;
    QString m_yPosRoleReplace;
    QString m_zPosRoleReplace;

    QItemModelSurfaceDataProxy::MultiMatchBehavior m_multiMatchBehavior;

    friend class SurfaceItemModelHandler;
    friend class QItemModelSurfaceDataProxy;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
