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

#ifndef SCATTEROBJECTBUFFERHELPER_P_H
#define SCATTEROBJECTBUFFERHELPER_P_H

#include "datavisualizationglobal_p.h"
#include "abstractobjecthelper_p.h"
#include "scatterseriesrendercache_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class ScatterObjectBufferHelper : public AbstractObjectHelper
{
public:
    ScatterObjectBufferHelper();
    ~ScatterObjectBufferHelper();

    void fullLoad(ScatterSeriesRenderCache *cache, qreal dotScale);
    void update(ScatterSeriesRenderCache *cache, qreal dotScale);
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
