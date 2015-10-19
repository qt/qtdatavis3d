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

#include "barseriesrendercache_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

BarSeriesRenderCache::BarSeriesRenderCache(QAbstract3DSeries *series,
                                           Abstract3DRenderer *renderer)
    : SeriesRenderCache(series, renderer),
      m_visualIndex(-1)
{
}

BarSeriesRenderCache::~BarSeriesRenderCache()
{
}

void BarSeriesRenderCache::cleanup(TextureHelper *texHelper)
{
    m_renderArray.clear();
    m_sliceArray.clear();

    SeriesRenderCache::cleanup(texHelper);
}

QT_END_NAMESPACE_DATAVISUALIZATION
