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

#include "seriesrendercache_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

SeriesRenderCache::SeriesRenderCache()
    : m_series(0)
{
}

SeriesRenderCache::~SeriesRenderCache()
{
}

void SeriesRenderCache::populate(QAbstract3DSeries *series)
{
    Q_ASSERT(series);

    m_series = series;
    m_itemLabelFormat = series->itemLabelFormat();

    // TODO: Add other visual element extractions
}

QT_DATAVISUALIZATION_END_NAMESPACE
