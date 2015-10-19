/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
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

#ifndef QHEIGHTMAPSURFACEDATAPROXY_P_H
#define QHEIGHTMAPSURFACEDATAPROXY_P_H

#include "qheightmapsurfacedataproxy.h"
#include "qsurfacedataproxy_p.h"
#include <QtCore/QTimer>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QHeightMapSurfaceDataProxyPrivate : public QSurfaceDataProxyPrivate
{
    Q_OBJECT

public:
    QHeightMapSurfaceDataProxyPrivate(QHeightMapSurfaceDataProxy *q);
    virtual ~QHeightMapSurfaceDataProxyPrivate();

    void setValueRanges(float minX, float maxX, float minZ, float maxZ);
    void setMinXValue(float min);
    void setMaxXValue(float max);
    void setMinZValue(float min);
    void setMaxZValue(float max);
private:
    QHeightMapSurfaceDataProxy *qptr();
    void handlePendingResolve();

    QImage m_heightMap;
    QString m_heightMapFile;
    QTimer m_resolveTimer;

    float m_minXValue;
    float m_maxXValue;
    float m_minZValue;
    float m_maxZValue;

    friend class QHeightMapSurfaceDataProxy;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
