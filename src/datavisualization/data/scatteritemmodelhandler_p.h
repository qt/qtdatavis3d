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

#ifndef SCATTERITEMMODELHANDLER_P_H
#define SCATTERITEMMODELHANDLER_P_H

#include "abstractitemmodelhandler_p.h"
#include "qitemmodelscatterdataproxy_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class ScatterItemModelHandler : public AbstractItemModelHandler
{
    Q_OBJECT
public:
    ScatterItemModelHandler(QItemModelScatterDataProxy *proxy, QObject *parent = 0);
    virtual ~ScatterItemModelHandler();

public Q_SLOTS:
    virtual void handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                   const QVector<int> &roles = QVector<int> ());
    virtual void handleRowsInserted(const QModelIndex &parent, int start, int end);
    virtual void handleRowsRemoved(const QModelIndex &parent, int start, int end);

protected:
    void virtual resolveModel();

private:
    void modelPosToScatterItem(int modelRow, int modelColumn, QScatterDataItem &item);

    QItemModelScatterDataProxy *m_proxy; // Not owned
    QScatterDataArray *m_proxyArray; // Not owned
    int m_xPosRole;
    int m_yPosRole;
    int m_zPosRole;
    int m_rotationRole;
    QRegExp m_xPosPattern;
    QRegExp m_yPosPattern;
    QRegExp m_zPosPattern;
    QRegExp m_rotationPattern;
    QString m_xPosReplace;
    QString m_yPosReplace;
    QString m_zPosReplace;
    QString m_rotationReplace;
    bool m_haveXPosPattern;
    bool m_haveYPosPattern;
    bool m_haveZPosPattern;
    bool m_haveRotationPattern;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
