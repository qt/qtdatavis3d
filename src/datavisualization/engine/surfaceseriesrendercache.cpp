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

#include "seriesrendercache_p.h"
#include "surfaceseriesrendercache_p.h"
#include "objecthelper_p.h"
#include "abstract3drenderer_p.h"
#include "texturehelper_p.h"
#include "utils_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

SurfaceSeriesRenderCache::SurfaceSeriesRenderCache()
    : m_surfaceVisible(false),
      m_surfaceGridVisible(false),
      m_surfaceFlatShading(true),
      m_surfaceObj(new SurfaceObject),
      m_sliceSurfaceObj(new SurfaceObject),
      m_sampleSpace(QRect(0, 0, 0 , 0)),
      m_selectionTexture(0),
      m_selectionIdStart(0),
      m_selectionIdEnd(0),
      m_flatChangeAllowed(true),
      m_flatStatusDirty(false),
      m_scale(QVector3D(1.0f, 1.0f, 1.0f)),
      m_offset(QVector3D(0.0f, 0.0f, 0.0f)),
      m_valid(false),
      m_objectDirty(true)
{
}

SurfaceSeriesRenderCache::~SurfaceSeriesRenderCache()
{
    delete m_surfaceObj;
    delete m_sliceSurfaceObj;
    for (int i = 0; i < m_dataArray.size(); i++)
        delete m_dataArray.at(i);
    m_dataArray.clear();

    for (int i = 0; i < m_sliceDataArray.size(); i++)
        delete m_sliceDataArray.at(i);
    m_sliceDataArray.clear();
}

void SurfaceSeriesRenderCache::populate(QSurface3DSeries *series, Abstract3DRenderer *renderer)
{
    Q_ASSERT(series);

    SeriesRenderCache::populate(series, renderer);

    QSurface3DSeries::DrawFlags drawMode = series->drawMode();
    m_surfaceVisible = drawMode.testFlag(QSurface3DSeries::DrawSurface);
    m_surfaceGridVisible = drawMode.testFlag(QSurface3DSeries::DrawWireframe);
    if (m_flatChangeAllowed && m_surfaceFlatShading != series->isFlatShadingEnabled()) {
        m_surfaceFlatShading = series->isFlatShadingEnabled();
        m_flatStatusDirty = true;
    }
}

QT_END_NAMESPACE_DATAVISUALIZATION
