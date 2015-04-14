/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef HIGHLIGHTSERIES_H
#define HIGHLIGHTSERIES_H

#include <QtDataVisualization/QSurface3DSeries>

#include "topographicseries.h"

using namespace QtDataVisualization;

class HighlightSeries : public QSurface3DSeries
{
    Q_OBJECT
public:
    explicit HighlightSeries();
    ~HighlightSeries();

    void setTopographicSeries(TopographicSeries *series);
    inline void setMinHeight(float height) { m_minHeight = height; }

public slots:
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
