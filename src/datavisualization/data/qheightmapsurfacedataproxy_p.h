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

#ifndef QHEIGHTMAPSURFACEDATAPROXY_P_H
#define QHEIGHTMAPSURFACEDATAPROXY_P_H

#include "qheightmapsurfacedataproxy.h"
#include "qsurfacedataproxy_p.h"
#include <QTimer>

namespace QtDataVisualization {

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

}

#endif
