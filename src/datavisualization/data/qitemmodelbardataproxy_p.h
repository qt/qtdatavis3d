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

#ifndef QITEMMODELBARDATAPROXY_P_H
#define QITEMMODELBARDATAPROXY_P_H

#include "qitemmodelbardataproxy.h"
#include "qbardataproxy_p.h"

QT_BEGIN_NAMESPACE

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

    QRegularExpression m_rowRolePattern;
    QRegularExpression m_columnRolePattern;
    QRegularExpression m_valueRolePattern;
    QRegularExpression m_rotationRolePattern;

    QString m_rowRoleReplace;
    QString m_columnRoleReplace;
    QString m_valueRoleReplace;
    QString m_rotationRoleReplace;

    QItemModelBarDataProxy::MultiMatchBehavior m_multiMatchBehavior;

    friend class BarItemModelHandler;
    friend class QItemModelBarDataProxy;
};

QT_END_NAMESPACE

#endif
