// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef HIGHLIGHTSERIES_H
#define HIGHLIGHTSERIES_H

#include <QtDataVisualization/QSurface3DSeries>

#include "topographicseries.h"

class HighlightSeries : public QSurface3DSeries
{
    Q_OBJECT
public:
    explicit HighlightSeries();
    ~HighlightSeries();

    void setTopographicSeries(TopographicSeries *series);
    inline void setMinHeight(float height) { m_minHeight = height; }

public Q_SLOTS:
    void handlePositionChange(const QPoint &position);
    void handleGradientChange(float value);

private:
    int m_width;
    int m_height;
    int m_srcWidth;
    int m_srcHeight;
    QPoint m_position;
    TopographicSeries *m_topographicSeries;
    float m_minHeight;
};

#endif // HIGHLIGHTSERIES_H
