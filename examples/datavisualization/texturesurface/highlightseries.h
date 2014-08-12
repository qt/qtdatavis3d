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

public slots:
    void handlePositionChange(const QPoint &position);

private:
    int m_width;
    int m_height;
    int m_srcWidth;
    int m_srcHeight;
    QPoint m_position;
    TopographicSeries *m_topographicSeries;
};

#endif // HIGHLIGHTSERIES_H
