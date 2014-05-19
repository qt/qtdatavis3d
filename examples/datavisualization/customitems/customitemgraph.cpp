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
#include <QtDataVisualization/QCustom3DItem>
#include <QtGui/QImage>

using namespace QtDataVisualization;

CustomItemGraph::CustomItemGraph(Q3DSurface *surface, QLabel *label)
    : m_graph(surface),
      m_textField(label),
      m_previouslyAnimatedItem(0)
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

    connect(m_graph, &QAbstract3DGraph::elementSelected,
            this, &CustomItemGraph::handleElementSelected);

    m_selectionAnimation = new QPropertyAnimation(this);
    m_selectionAnimation->setPropertyName("scaling");
    m_selectionAnimation->setDuration(500);
    m_selectionAnimation->setLoopCount(-1);
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
    QVector3D positionOnePipe = QVector3D(39.0f, 45.0f, 19.2f);
    if (show) {
        //! [0]
        QImage color = QImage(2, 2, QImage::Format_RGB32);
        color.fill(Qt::red);
        //! [0]
        //! [2]
        QCustom3DItem *item = new QCustom3DItem(":/items/oilrig.obj", positionOne,
                                                QVector3D(0.025f, 0.025f, 0.025f),
                                                QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 45.0f),
                                                color);
        //! [2]
        //! [3]
        m_graph->addCustomItem(item);
        //! [3]
        item = new QCustom3DItem(":/items/pipe.obj", positionOnePipe,
                                 QVector3D(0.005f, 0.5f, 0.005f),
                                 QQuaternion(),
                                 color);
        item->setShadowCasting(false);
        m_graph->addCustomItem(item);
    } else {
        resetSelection();
        //! [4]
        m_graph->removeCustomItemAt(positionOne);
        //! [4]
        m_graph->removeCustomItemAt(positionOnePipe);
    }
}

void CustomItemGraph::toggleItemTwo(bool show)
{
    QVector3D positionTwo = QVector3D(34.5f, 77.0f, 23.4f);
    QVector3D positionTwoPipe = QVector3D(34.5f, 45.0f, 23.4f);
    if (show) {
        QImage color = QImage(2, 2, QImage::Format_RGB32);
        color.fill(Qt::red);
        QCustom3DItem *item = new QCustom3DItem();
        item->setMeshFile(":/items/oilrig.obj");
        item->setPosition(positionTwo);
        item->setScaling(QVector3D(0.025f, 0.025f, 0.025f));
        item->setRotation(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 25.0f));
        item->setTextureImage(color);
        m_graph->addCustomItem(item);
        item = new QCustom3DItem(":/items/pipe.obj", positionTwoPipe,
                                 QVector3D(0.005f, 0.5f, 0.005f),
                                 QQuaternion(),
                                 color);
        item->setShadowCasting(false);
        m_graph->addCustomItem(item);
    } else {
        resetSelection();
        m_graph->removeCustomItemAt(positionTwo);
        m_graph->removeCustomItemAt(positionTwoPipe);
    }
}

void CustomItemGraph::toggleItemThree(bool show)
{
    QVector3D positionThree = QVector3D(34.5f, 86.0f, 19.1f);
    if (show) {
        QImage color = QImage(2, 2, QImage::Format_RGB32);
        color.fill(Qt::darkMagenta);
        QCustom3DItem *item = new QCustom3DItem();
        item->setMeshFile(":/items/refinery.obj");
        item->setPosition(positionThree);
        item->setScaling(QVector3D(0.04f, 0.04f, 0.04f));
        item->setRotation(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 75.0f));
        item->setTextureImage(color);
        m_graph->addCustomItem(item);
    } else {
        resetSelection();
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

void CustomItemGraph::handleElementSelected(QAbstract3DGraph::ElementType type)
{
    resetSelection();
    if (type == QAbstract3DGraph::ElementCustomItem) {
        int index = m_graph->selectedCustomItemIndex();
        QCustom3DItem *item = m_graph->selectedCustomItem();
        m_previouslyAnimatedItem = item;
        m_previousScaling = item->scaling();
        QString text;
        text.setNum(index);
        text.append(": ");
        QStringList split = item->meshFile().split("/");
        text.append(split.last());
        m_textField->setText(text);
        m_selectionAnimation->setTargetObject(item);
        m_selectionAnimation->setStartValue(item->scaling());
        m_selectionAnimation->setEndValue(item->scaling() * 1.5f);
        m_selectionAnimation->start();
        item->setShadowCasting(false);
    } else if (type == QAbstract3DGraph::ElementSeries) {
        QString text = "Surface (";
        QSurface3DSeries *series = m_graph->selectedSeries();
        if (series) {
            QPoint point = series->selectedPoint();
            QString posStr;
            posStr.setNum(point.x());
            text.append(posStr);
            text.append(", ");
            posStr.setNum(point.y());
            text.append(posStr);
        }
        text.append(")");
        m_textField->setText(text);
    } else if (type > QAbstract3DGraph::ElementSeries
               && type < QAbstract3DGraph::ElementCustomItem) {
        m_textField->setText("Axis");
    } else {
        m_textField->setText("Nothing");
    }
}

void CustomItemGraph::resetSelection()
{
    m_selectionAnimation->stop();
    if (m_previouslyAnimatedItem)
        m_previouslyAnimatedItem->setScaling(m_previousScaling);
    m_previouslyAnimatedItem = 0;
}
