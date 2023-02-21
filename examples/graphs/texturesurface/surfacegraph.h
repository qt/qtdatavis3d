// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef SURFACEGRAPH_H
#define SURFACEGRAPH_H

#include <QtGraphs/Q3DSurface>
#include <QtGraphs/QSurface3DSeries>
#include <QtWidgets/QSlider>
#include "topographicseries.h"
#include "highlightseries.h"

#include "custominputhandler.h"

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
