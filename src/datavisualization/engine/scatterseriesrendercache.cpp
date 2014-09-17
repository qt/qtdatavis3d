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

#include "scatterseriesrendercache_p.h"
#include "scatterobjectbufferhelper_p.h"
#include "scatterpointbufferhelper_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

ScatterSeriesRenderCache::ScatterSeriesRenderCache(QAbstract3DSeries *series,
                                                   Abstract3DRenderer *renderer)
    : SeriesRenderCache(series, renderer),
      m_itemSize(0.0f),
      m_selectionIndexOffset(0),
      m_staticBufferDirty(false),
      m_oldRenderArraySize(0),
      m_oldMeshFileName(QString()),
      m_scatterBufferObj(0),
      m_scatterBufferPoints(0)
{
}

ScatterSeriesRenderCache::~ScatterSeriesRenderCache()
{
    delete m_scatterBufferObj;
    delete m_scatterBufferPoints;
}

void ScatterSeriesRenderCache::cleanup(TextureHelper *texHelper)
{
    m_renderArray.clear();

    SeriesRenderCache::cleanup(texHelper);
}

QT_END_NAMESPACE_DATAVISUALIZATION
