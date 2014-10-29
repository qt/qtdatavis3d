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

#ifndef SURFACEGRAPH_H
#define SURFACEGRAPH_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QSlider>
#include "topographicseries.h"
#include "highlightseries.h"

#include "custominputhandler.h"

using namespace QtDataVisualization;

class SurfaceGraph : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceGraph(Q3DSurface *surface);
    ~SurfaceGraph();

    void toggleSurfaceTexture(bool enable);

private:
    Q3DSurface *m_graph;

    TopographicSeries *m_topography;
    HighlightSeries *m_highlight;
    int m_highlightWidth;
    int m_highlightHeight;

    CustomInputHandler *m_inputHandler;
};

#endif // SURFACEGRAPH_H
