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

#include "surfacegraph.h"

#include <QtDataVisualization/Q3DValueAxis>
#include <QtGui/QImage>
#include <QtCore/qmath.h>

using namespace QtDataVisualization;

const int sampleCountX = 50;
const int sampleCountZ = 50;
const int heightMapGridStepX = 6;
const int heightMapGridStepZ = 6;

SurfaceGraph::SurfaceGraph(Q3DSurface *surface)
    : m_graph(surface)
{
    m_graph->setAxisX(new Q3DValueAxis);
    m_graph->setAxisY(new Q3DValueAxis);
    m_graph->setAxisZ(new Q3DValueAxis);
    m_graph->setLabelStyle(QDataVis::LabelStyleFromTheme);

    //! [0]
    sqrtSinProxy = new QSurfaceDataProxy();
    //! [0]
    fillSqrtSinProxy();

    //! [2]
    QImage heightMapImage(":/maps/mountain");
    m_heightMapProxy = new QHeightMapSurfaceDataProxy(heightMapImage);
    m_heightMapProxy->setValueRanges(34.0, 40.0, 18.0, 24.0);
    //! [2]
    m_heightMapWidth = heightMapImage.width();
    m_heightMapHeight = heightMapImage.height();
}

SurfaceGraph::~SurfaceGraph()
{
    delete m_graph;
}

//! [1]
void SurfaceGraph::fillSqrtSinProxy()
{
    qreal stepX = 16.0 / qreal(sampleCountX);
    qreal stepZ = 16.0 / qreal(sampleCountZ);

    QSurfaceDataArray *dataArray = new QSurfaceDataArray;
    dataArray->reserve(sampleCountZ);
    for (qreal i = -8.0 + stepZ / 2.0 ; i < 8.0 ; i += stepZ) {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
        int index = 0;
        for (qreal j = -8.0 + stepX / 2.0; j < 8.0; j += stepX) {
            qreal R = qSqrt(i * i + j * j) + 0.01;
            qreal y = (qSin(R) / R + 0.24) * 1.61;
            (*newRow)[index++].setPosition(QVector3D(j, y, i));
        }
        *dataArray << newRow;
    }

    sqrtSinProxy->resetArray(dataArray);
}
//! [1]

void SurfaceGraph::enableSqrtSinModel()
{
    //! [3]
    m_graph->setSurfaceGridEnabled(true);
    m_graph->setSmoothSurfaceEnabled(false);

    m_graph->axisX()->setLabelFormat("%.2f");
    m_graph->axisZ()->setLabelFormat("%.2f");
    m_graph->axisX()->setRange(-8.0, 8.0);
    m_graph->axisY()->setRange(0.0, 2.0);
    m_graph->axisZ()->setRange(-8.0, 8.0);

    m_graph->setActiveDataProxy(sqrtSinProxy);
    //! [3]

    // Reset range sliders for Sqrt&Sin
    m_rangeMinX = -8.0;
    m_rangeMinZ = -8.0;
    m_stepX = 16.0 / qreal(sampleCountX - 1);
    m_stepZ = 16.0 / qreal(sampleCountZ - 1);
    m_axisMinSliderX->setMaximum(sampleCountX - 3);
    m_axisMinSliderX->setValue(0);
    m_axisMaxSliderX->setMaximum(sampleCountX - 1);
    m_axisMaxSliderX->setValue(sampleCountX - 1);
    m_axisMinSliderZ->setMaximum(sampleCountZ - 3);
    m_axisMinSliderZ->setValue(0);
    m_axisMaxSliderZ->setMaximum(sampleCountZ - 1);
    m_axisMaxSliderZ->setValue(sampleCountZ - 1);
}

void SurfaceGraph::enableHeightMapModel()
{
    //! [4]
    m_graph->setSurfaceGridEnabled(false);
    m_graph->setSmoothSurfaceEnabled(true);

    m_graph->axisX()->setLabelFormat("%.1f N");
    m_graph->axisZ()->setLabelFormat("%.1f E");
    m_graph->axisX()->setRange(34.0, 40.0);
    m_graph->axisY()->setAutoAdjustRange(true);
    m_graph->axisZ()->setRange(18.0, 24.0);

    m_graph->setActiveDataProxy(m_heightMapProxy);
    //! [4]

    // Reset range sliders for height map
    int mapGridCountX = m_heightMapWidth / heightMapGridStepX;
    int mapGridCountZ = m_heightMapHeight / heightMapGridStepZ;
    m_rangeMinX = 34.0;
    m_rangeMinZ = 18.0;
    m_stepX = 6.0 / qreal(mapGridCountX - 1);
    m_stepZ = 6.0 / qreal(mapGridCountZ - 1);
    m_axisMinSliderX->setMaximum(mapGridCountX - 3);
    m_axisMinSliderX->setValue(0);
    m_axisMaxSliderX->setMaximum(mapGridCountX - 1);
    m_axisMaxSliderX->setValue(mapGridCountX - 1);
    m_axisMinSliderZ->setMaximum(mapGridCountZ - 3);
    m_axisMinSliderZ->setValue(0);
    m_axisMaxSliderZ->setMaximum(mapGridCountZ - 1);
    m_axisMaxSliderZ->setValue(mapGridCountZ - 1);
}

void SurfaceGraph::adjustXMin(int min)
{
    qreal minX = m_stepX * qreal(min) + m_rangeMinX;

    int max = m_axisMaxSliderX->value();
    if (min >= (max - 1)) {
        max = min + 2;
        m_axisMaxSliderX->setValue(max);
    }
    qreal maxX = m_stepX * max + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustXMax(int max)
{
    qreal maxX = m_stepX * qreal(max) + m_rangeMinX;

    int min = m_axisMinSliderX->value();
    if (max <= (min + 1)) {
        min = max - 2;
        m_axisMinSliderX->setValue(min);
    }
    qreal minX = m_stepX * min + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustZMin(int min)
{
    qreal minZ = m_stepZ * qreal(min) + m_rangeMinZ;

    int max = m_axisMaxSliderZ->value();
    if (min >= (max - 1)) {
        max = min + 2;
        m_axisMaxSliderZ->setValue(max);
    }
    qreal maxZ = m_stepZ * max + m_rangeMinZ;

    setAxisZRange(minZ, maxZ);
}

void SurfaceGraph::adjustZMax(int max)
{
    qreal maxX = m_stepZ * qreal(max) + m_rangeMinZ;

    int min = m_axisMinSliderZ->value();
    if (max <= (min + 1)) {
        min = max - 2;
        m_axisMinSliderZ->setValue(min);
    }
    qreal minX = m_stepZ * min + m_rangeMinZ;

    setAxisZRange(minX, maxX);
}

//! [5]
void SurfaceGraph::setAxisXRange(qreal min, qreal max)
{
    m_graph->axisX()->setRange(min, max);
}

void SurfaceGraph::setAxisZRange(qreal min, qreal max)
{
    m_graph->axisZ()->setRange(min, max);
}
//! [5]

//! [6]
void SurfaceGraph::changeTheme(int theme)
{
    m_graph->setTheme((QDataVis::Theme)theme);
}
//! [6]

void SurfaceGraph::setBlackToYellowGradient()
{
    //! [7]
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.33, Qt::blue);
    gr.setColorAt(0.67, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);

    m_graph->setGradient(gr);
    //! [7]
}

void SurfaceGraph::setGreenToRedGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::darkRed);

    m_graph->setGradient(gr);
}

