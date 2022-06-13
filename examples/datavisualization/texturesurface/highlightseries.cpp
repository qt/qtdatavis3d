// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "highlightseries.h"

//! [2]
const float darkRedPos = 1.0f;
const float redPos = 0.8f;
const float yellowPos = 0.6f;
const float greenPos = 0.4f;
const float darkGreenPos = 0.2f;
//! [2]

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

//! [0]
void HighlightSeries::setTopographicSeries(TopographicSeries *series)
{
    m_topographicSeries = series;
    m_srcWidth = m_topographicSeries->dataProxy()->array()->at(0)->size();
    m_srcHeight = m_topographicSeries->dataProxy()->array()->size();

    QObject::connect(m_topographicSeries, &QSurface3DSeries::selectedPointChanged,
                     this, &HighlightSeries::handlePositionChange);
}
//! [0]

//! [1]
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
//! [1]

//! [3]
void HighlightSeries::handleGradientChange(float value)
{
    float ratio = m_minHeight / value;

    QLinearGradient gr;
    gr.setColorAt(0.0f, Qt::black);
    gr.setColorAt(darkGreenPos * ratio, Qt::darkGreen);
    gr.setColorAt(greenPos * ratio, Qt::green);
    gr.setColorAt(yellowPos * ratio, Qt::yellow);
    gr.setColorAt(redPos * ratio, Qt::red);
    gr.setColorAt(darkRedPos * ratio, Qt::darkRed);

    setBaseGradient(gr);
    setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}
//! [3]
