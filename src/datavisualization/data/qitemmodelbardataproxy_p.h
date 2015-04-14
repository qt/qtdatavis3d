/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
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

#ifndef QITEMMODELBARDATAPROXY_P_H
#define QITEMMODELBARDATAPROXY_P_H

#include "qitemmodelbardataproxy.h"
#include "qbardataproxy_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class BarItemModelHandler;

class QItemModelBarDataProxyPrivate : public QBarDataProxyPrivate
{
    Q_OBJECT
public:
    QItemModelBarDataProxyPrivate(QItemModelBarDataProxy *q);
    virtual ~QItemModelBarDataProxyPrivate();

    void connectItemModelHandler();

private:
    QItemModelBarDataProxy *qptr();

    BarItemModelHandler *m_itemModelHandler;

    QString m_rowRole;
    QString m_columnRole;
    QString m_valueRole;
    QString m_rotationRole;

    // For row/column items, sort items into these categories. Other categories are ignored.
    QStringList m_rowCategories;
    QStringList m_columnCategories;

    bool m_useModelCategories;
    bool m_autoRowCategories;
    bool m_autoColumnCategories;

    QRegExp m_rowRolePattern;
    QRegExp m_columnRolePattern;
    QRegExp m_valueRolePattern;
    QRegExp m_rotationRolePattern;

    QString m_rowRoleReplace;
    QString m_columnRoleReplace;
    QString m_valueRoleReplace;
    QString m_rotationRoleReplace;

    QItemModelBarDataProxy::MultiMatchBehavior m_multiMatchBehavior;

    friend class BarItemModelHandler;
    friend class QItemModelBarDataProxy;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
