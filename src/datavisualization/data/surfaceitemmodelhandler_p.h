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

#ifndef SURFACEITEMMODELHANDLER_P_H
#define SURFACEITEMMODELHANDLER_P_H

#include "abstractitemmodelhandler_p.h"
#include "qitemmodelsurfacedataproxy_p.h"

QT_BEGIN_NAMESPACE

class SurfaceItemModelHandler : public AbstractItemModelHandler
{
    Q_OBJECT
public:
    SurfaceItemModelHandler(QItemModelSurfaceDataProxy *proxy, QObject *parent = 0);
    virtual ~SurfaceItemModelHandler();

public Q_SLOTS:
    void handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                           const QList<int> &roles = QList<int>()) override;

protected:
    void resolveModel() override;

    QItemModelSurfaceDataProxy *m_proxy; // Not owned
    QSurfaceDataArray *m_proxyArray; // Not owned
    int m_xPosRole;
    int m_yPosRole;
    int m_zPosRole;
    QRegularExpression m_xPosPattern;
    QRegularExpression m_yPosPattern;
    QRegularExpression m_zPosPattern;
    QString m_xPosReplace;
    QString m_yPosReplace;
    QString m_zPosReplace;
    bool m_haveXPosPattern;
    bool m_haveYPosPattern;
    bool m_haveZPosPattern;
};

QT_END_NAMESPACE

#endif
