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
      m_scatterBufferPoints(0),
      m_visibilityChanged(false)
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
