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

#include "highlightseries.h"

#include <QDebug>

using namespace QtDataVisualization;

HighlightSeries::HighlightSeries()
    : m_width(100),
      m_height(100)
{
    setDrawMode(QSurface3DSeries::DrawSurface);
    setFlatShadingEnabled(true);
    setVisible(false);
}

HighlightSeries::~HighlightSeries()
{
}

void HighlightSeries::setTopographicSeries(TopographicSeries *series)
{
    m_topographicSeries = series;
    m_srcWidth = m_topographicSeries->dataProxy()->array()->at(0)->size();
    m_srcHeight = m_topographicSeries->dataProxy()->array()->size();

    QObject::connect(m_topographicSeries, &QSurface3DSeries::selectedPointChanged,
                     this, &HighlightSeries::handlePositionChange);
}

void HighlightSeries::handlePositionChange(const QPoint &position)
{
    m_position = position;

    if (position == invalidSelectionPosition()) {
        setVisible(false);

        return;
    }

    int halfWidth = m_width / 2;
    int halfHeight = m_height / 2;

    int startX = position.y() - halfWidth;
    if (startX < 0 )
        startX = 0;
    int endX = position.y() + halfWidth;
    if (endX > (m_srcWidth - 1))
        endX = m_srcWidth - 1;
    int startZ = position.x() - halfHeight;
    if (startZ < 0 )
        startZ = 0;
    int endZ = position.x() + halfHeight;
    if (endZ > (m_srcHeight - 1))
        endZ = m_srcHeight - 1;

    QSurfaceDataProxy *srcProxy = m_topographicSeries->dataProxy();
    const QSurfaceDataArray &srcArray = *srcProxy->array();

    QSurfaceDataArray *dataArray = new QSurfaceDataArray;
    dataArray->reserve(endZ - startZ);
    for (int i = startZ; i < endZ; i++) {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(endX - startX);
        QSurfaceDataRow *srcRow = srcArray.at(i);
        for (int j = startX, p = 0; j < endX; j++, p++) {
            QVector3D pos = srcRow->at(j).position();
            (*newRow)[p].setPosition(QVector3D(pos.x(), pos.y() + 0.1f, pos.z()));
        }
        *dataArray << newRow;
    }

    dataProxy()->resetArray(dataArray);
    setVisible(true);
}
