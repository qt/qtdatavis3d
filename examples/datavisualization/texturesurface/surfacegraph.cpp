// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "surfacegraph.h"
#include "topographicseries.h"

#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>

const float areaWidth = 8000.0f;
const float areaHeight = 8000.0f;
const float aspectRatio = 0.1389f;
const float minRange = areaWidth * 0.49f;

SurfaceGraph::SurfaceGraph(Q3DSurface *surface)
    : m_graph(surface)
{
    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);
    m_graph->axisX()->setLabelFormat("%i");
    m_graph->axisZ()->setLabelFormat("%i");
    m_graph->axisX()->setRange(0.0f, areaWidth);
    m_graph->axisY()->setRange(100.0f, areaWidth * aspectRatio);
    m_graph->axisZ()->setRange(0.0f, areaHeight);
    m_graph->axisX()->setLabelAutoRotation(30);
    m_graph->axisY()->setLabelAutoRotation(90);
    m_graph->axisZ()->setLabelAutoRotation(30);
    m_graph->activeTheme()->setType(Q3DTheme::ThemePrimaryColors);

    QFont font = m_graph->activeTheme()->font();
    font.setPointSize(20);
    m_graph->activeTheme()->setFont(font);

    m_topography = new TopographicSeries();
    m_topography->setTopographyFile(":/maps/topography", areaWidth, areaHeight);
    m_topography->setItemLabelFormat(QStringLiteral("@yLabel m"));

    m_highlight = new HighlightSeries();
    m_highlight->setTopographicSeries(m_topography);
    m_highlight->setMinHeight(minRange * aspectRatio);
    m_highlight->handleGradientChange(areaWidth * aspectRatio);
//! [1]
    QObject::connect(m_graph->axisY(), &QValue3DAxis::maxChanged,
                     m_highlight, &HighlightSeries::handleGradientChange);
//! [1]

    m_graph->addSeries(m_topography);
    m_graph->addSeries(m_highlight);

    m_inputHandler = new CustomInputHandler(m_graph);
    m_inputHandler->setHighlightSeries(m_highlight);
    m_inputHandler->setAxes(m_graph->axisX(), m_graph->axisY(), m_graph->axisZ());
    m_inputHandler->setLimits(0.0f, areaWidth, minRange);
    m_inputHandler->setAspectRatio(aspectRatio);

    m_graph->setActiveInputHandler(m_inputHandler);
}

SurfaceGraph::~SurfaceGraph()
{
    delete m_graph;
}

//! [0]
void SurfaceGraph::toggleSurfaceTexture(bool enable)
{
    if (enable)
        m_topography->setTextureFile(":/maps/maptexture");
    else
        m_topography->setTextureFile("");
}
//! [0]
