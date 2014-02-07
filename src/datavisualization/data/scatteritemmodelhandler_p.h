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

public slots:
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
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
