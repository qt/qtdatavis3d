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

#include "customitemgraph.h"

#include <QtDataVisualization/Q3DTheme>
#include <QtGui/QImage>

using namespace QtDataVisualization;

CustomItemGraph::CustomItemGraph(Q3DSurface *surface)
    : m_graph(surface)
{
    QImage layerOneHMap(":/maps/layer_1.png");
    QHeightMapSurfaceDataProxy *layerOneProxy = new QHeightMapSurfaceDataProxy(layerOneHMap);
    QSurface3DSeries *layerOneSeries = new QSurface3DSeries(layerOneProxy);
    layerOneSeries->setItemLabelFormat(QStringLiteral("(@xLabel, @zLabel): @yLabel"));
    layerOneProxy->setValueRanges(34.0f, 40.0f, 18.0f, 24.0f);
    layerOneSeries->setDrawMode(QSurface3DSeries::DrawSurface);
    layerOneSeries->setFlatShadingEnabled(false);

    QImage layerTwoHMap(":/maps/layer_2.png");
    QHeightMapSurfaceDataProxy *layerTwoProxy = new QHeightMapSurfaceDataProxy(layerTwoHMap);
    QSurface3DSeries *layerTwoSeries = new QSurface3DSeries(layerTwoProxy);
    layerTwoSeries->setItemLabelFormat(QStringLiteral("(@xLabel, @zLabel): @yLabel"));
    layerTwoProxy->setValueRanges(34.0f, 40.0f, 18.0f, 24.0f);
    layerTwoSeries->setDrawMode(QSurface3DSeries::DrawSurface);
    layerTwoSeries->setFlatShadingEnabled(false);

    QImage layerThreeHMap(":/maps/layer_3.png");
    QHeightMapSurfaceDataProxy *layerThreeProxy = new QHeightMapSurfaceDataProxy(layerThreeHMap);
    QSurface3DSeries *layerThreeSeries = new QSurface3DSeries(layerThreeProxy);
    layerThreeSeries->setItemLabelFormat(QStringLiteral("(@xLabel, @zLabel): @yLabel"));
    layerThreeProxy->setValueRanges(34.0f, 40.0f, 18.0f, 24.0f);
    layerThreeSeries->setDrawMode(QSurface3DSeries::DrawSurface);
    layerThreeSeries->setFlatShadingEnabled(false);

    m_graph->axisX()->setLabelFormat("%.1f N");
    m_graph->axisZ()->setLabelFormat("%.1f E");
    m_graph->axisX()->setRange(34.0f, 40.0f);
    m_graph->axisY()->setRange(0.0f, 200.0f);
    m_graph->axisZ()->setRange(18.0f, 24.0f);

    m_graph->axisX()->setTitle(QStringLiteral("Latitude"));
    m_graph->axisY()->setTitle(QStringLiteral("Height"));
    m_graph->axisZ()->setTitle(QStringLiteral("Longitude"));

    m_graph->addSeries(layerOneSeries);
    m_graph->addSeries(layerTwoSeries);
    m_graph->addSeries(layerThreeSeries);

    QLinearGradient grOne;
    grOne.setColorAt(0.0, Qt::black);
    grOne.setColorAt(0.38, Qt::darkYellow);
    grOne.setColorAt(0.39, Qt::darkGreen);
    grOne.setColorAt(0.5, Qt::darkGray);
    grOne.setColorAt(1.0, Qt::gray);
    m_graph->seriesList().at(0)->setBaseGradient(grOne);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    QLinearGradient grTwo;
    grTwo.setColorAt(0.385, Qt::blue);
    grTwo.setColorAt(0.395, Qt::white);
    m_graph->seriesList().at(1)->setBaseGradient(grTwo);
    m_graph->seriesList().at(1)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    QLinearGradient grThree;
    grThree.setColorAt(0.0, Qt::white);
    grThree.setColorAt(0.05, Qt::black);
    m_graph->seriesList().at(2)->setBaseGradient(grThree);
    m_graph->seriesList().at(2)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);
}

CustomItemGraph::~CustomItemGraph()
{
    delete m_graph;
}

void CustomItemGraph::toggleItemOne(bool show)
{
    //! [1]
    QVector3D positionOne = QVector3D(39.0f, 77.0f, 19.2f);
    //! [1]
    if (show) {
        //! [0]
        QImage color = QImage(2, 2, QImage::Format_ARGB32);
        color.fill(Qt::red);
        //! [0]
        //! [2]
        m_graph->addCustomItem(":/items/oilrig.obj", positionOne,
                               QVector3D(0.025f, 0.025f, 0.025f),
                               QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 45.0f),
                               color);
        //! [2]
    } else {
        //! [3]
        m_graph->removeCustomItemAt(positionOne);
        //! [3]
    }
}

void CustomItemGraph::toggleItemTwo(bool show)
{
    QVector3D positionTwo = QVector3D(34.5f, 77.0f, 23.4f);
    if (show) {
        QImage color = QImage(2, 2, QImage::Format_ARGB32);
        color.fill(Qt::red);
        m_graph->addCustomItem(":/items/oilrig.obj", positionTwo,
                               QVector3D(0.025f, 0.025f, 0.025f),
                               QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 25.0f),
                               color);
    } else {
        m_graph->removeCustomItemAt(positionTwo);
    }
}

void CustomItemGraph::toggleItemThree(bool show)
{
    QVector3D positionThree = QVector3D(34.5f, 86.0f, 19.1f);
    if (show) {
        QImage color = QImage(2, 2, QImage::Format_ARGB32);
        color.fill(Qt::darkMagenta);
        m_graph->addCustomItem(":/items/refinery.obj", positionThree,
                               QVector3D(0.066f, 0.066f, 0.066f),
                               QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 75.0f),
                               color);
    } else {
        m_graph->removeCustomItemAt(positionThree);
    }
}

void CustomItemGraph::toggleSeeThrough(bool seethrough)
{
    if (seethrough) {
        m_graph->seriesList().at(0)->setDrawMode(QSurface3DSeries::DrawWireframe);
        m_graph->seriesList().at(1)->setDrawMode(QSurface3DSeries::DrawWireframe);
    } else {
        m_graph->seriesList().at(0)->setDrawMode(QSurface3DSeries::DrawSurface);
        m_graph->seriesList().at(1)->setDrawMode(QSurface3DSeries::DrawSurface);
    }
}

void CustomItemGraph::toggleOilHighlight(bool highlight)
{
    if (highlight) {
        QLinearGradient grThree;
        grThree.setColorAt(0.0, Qt::black);
        grThree.setColorAt(0.05, Qt::red);
        m_graph->seriesList().at(2)->setBaseGradient(grThree);
    } else {
        QLinearGradient grThree;
        grThree.setColorAt(0.0, Qt::white);
        grThree.setColorAt(0.05, Qt::black);
        m_graph->seriesList().at(2)->setBaseGradient(grThree);
    }
}

void CustomItemGraph::toggleShadows(bool shadows)
{
    if (shadows)
        m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityMedium);
    else
        m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
}
