// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "surfacegraphmodifier.h"
#include "highlightseries.h"
#include "topographicseries.h"
#include "custominputhandler.h"

#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtGui/qimage.h>
#include <QtCore/qmath.h>

using namespace Qt::StringLiterals;

const int sampleCountX = 150;
const int sampleCountZ = 150;
const int heightMapGridStepX = 6;
const int heightMapGridStepZ = 6;
const float sampleMin = -8.f;
const float sampleMax = 8.f;

const float areaWidth = 8000.f;
const float areaHeight = 8000.f;
const float aspectRatio = 0.1389f;
const float minRange = areaWidth * 0.49f;

SurfaceGraphModifier::SurfaceGraphModifier(Q3DSurface *surface, QLabel *label, QObject *parent) :
      QObject(parent),
      m_graph(surface),
      m_textField(label)
{
    m_graph->scene()->activeCamera()->setZoomLevel(85.f);
    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetIsometricRight);
    m_graph->activeTheme()->setType(Q3DTheme::ThemeRetro);

    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);

    //
    // Sqrt Sin
    //
    //! [0]
    m_sqrtSinProxy = new QSurfaceDataProxy();
    m_sqrtSinSeries = new QSurface3DSeries(m_sqrtSinProxy);
    //! [0]
    fillSqrtSinProxy();

    //
    // Multisurface heightmap
    //
    //! [2]
    // Create the first surface layer
    QImage heightMapImageOne(":/data/layer_1.png");
    m_heightMapProxyOne = new QHeightMapSurfaceDataProxy(heightMapImageOne);
    m_heightMapSeriesOne = new QSurface3DSeries(m_heightMapProxyOne);
    m_heightMapSeriesOne->setItemLabelFormat(u"(@xLabel, @zLabel): @yLabel"_s);
    m_heightMapProxyOne->setValueRanges(34.f, 40.f, 18.f, 24.f);
    //! [2]

    // Create the other 2 surface layers
    QImage heightMapImageTwo(":/data/layer_2.png");
    m_heightMapProxyTwo = new QHeightMapSurfaceDataProxy(heightMapImageTwo);
    m_heightMapSeriesTwo = new QSurface3DSeries(m_heightMapProxyTwo);
    m_heightMapSeriesTwo->setItemLabelFormat(u"(@xLabel, @zLabel): @yLabel"_s);
    m_heightMapProxyTwo->setValueRanges(34.f, 40.f, 18.f, 24.f);

    QImage heightMapImageThree(":/data/layer_3.png");
    m_heightMapProxyThree = new QHeightMapSurfaceDataProxy(heightMapImageThree);
    m_heightMapSeriesThree = new QSurface3DSeries(m_heightMapProxyThree);
    m_heightMapSeriesThree->setItemLabelFormat(u"(@xLabel, @zLabel): @yLabel"_s);
    m_heightMapProxyThree->setValueRanges(34.f, 40.f, 18.f, 24.f);

    // The images are the same size, so it's enough to get the dimensions from one
    m_heightMapWidth = heightMapImageOne.width();
    m_heightMapHeight = heightMapImageOne.height();

    // Set the gradients for multi-surface layers
    QLinearGradient grOne;
    grOne.setColorAt(0.f, Qt::black);
    grOne.setColorAt(0.38f, Qt::darkYellow);
    grOne.setColorAt(0.39f, Qt::darkGreen);
    grOne.setColorAt(0.5f, Qt::darkGray);
    grOne.setColorAt(1.f, Qt::gray);
    m_heightMapSeriesOne->setBaseGradient(grOne);
    m_heightMapSeriesOne->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    QLinearGradient grTwo;
    grTwo.setColorAt(0.39f, Qt::blue);
    grTwo.setColorAt(0.4f, Qt::white);
    m_heightMapSeriesTwo->setBaseGradient(grTwo);
    m_heightMapSeriesTwo->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    QLinearGradient grThree;
    grThree.setColorAt(0.f, Qt::white);
    grThree.setColorAt(0.05f, Qt::black);
    m_heightMapSeriesThree->setBaseGradient(grThree);
    m_heightMapSeriesThree->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    // Custom items and label
    connect(m_graph, &QAbstract3DGraph::selectedElementChanged,
            this, &SurfaceGraphModifier::handleElementSelected);

    m_selectionAnimation = new QPropertyAnimation(this);
    m_selectionAnimation->setPropertyName("scaling");
    m_selectionAnimation->setDuration(500);
    m_selectionAnimation->setLoopCount(-1);

    QFont titleFont = QFont("Century Gothic", 30);
    titleFont.setBold(true);
    m_titleLabel = new QCustom3DLabel("Oil Rigs on Imaginary Sea", titleFont,
                                      QVector3D(0.f, 1.2f, 0.f),
                                      QVector3D(1.f, 1.f, 0.f),
                                      QQuaternion());
    m_titleLabel->setPositionAbsolute(true);
    m_titleLabel->setFacingCamera(true);
    m_titleLabel->setBackgroundColor(QColor(0x66cdaa));
    m_graph->addCustomItem(m_titleLabel);
    m_titleLabel->setVisible(false);

    // Make two of the custom object visible
    toggleItemOne(true);
    toggleItemTwo(true);

    //
    // Topographic map
    //
    m_topography = new TopographicSeries();
    m_topography->setTopographyFile(":/data/topography.png", areaWidth, areaHeight);
    m_topography->setItemLabelFormat(u"@yLabel m"_s);

    m_highlight = new HighlightSeries();
    m_highlight->setTopographicSeries(m_topography);
    m_highlight->setMinHeight(minRange * aspectRatio);
    m_highlight->handleGradientChange(areaWidth * aspectRatio);
    //! [16]
    QObject::connect(m_graph->axisY(), &QValue3DAxis::maxChanged,
                     m_highlight, &HighlightSeries::handleGradientChange);
    //! [16]

    m_customInputHandler = new CustomInputHandler(m_graph);
    m_customInputHandler->setHighlightSeries(m_highlight);
    m_customInputHandler->setAxes(m_graph->axisX(), m_graph->axisY(), m_graph->axisZ());
    m_customInputHandler->setLimits(0.f, areaWidth, minRange);
    m_customInputHandler->setAspectRatio(aspectRatio);
}

void SurfaceGraphModifier::fillSqrtSinProxy()
{
    float stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
    float stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);

    //! [1]
    auto *dataArray = new QSurfaceDataArray;
    dataArray->reserve(sampleCountZ);
    for (int i = 0 ; i < sampleCountZ ; ++i) {
        auto *newRow = new QSurfaceDataRow;
        newRow->reserve(sampleCountX);
        // Keep values within range bounds, since just adding step can cause minor drift due
        // to the rounding errors.
        float z = qMin(sampleMax, (i * stepZ + sampleMin));
        for (int j = 0; j < sampleCountX; ++j) {
            float x = qMin(sampleMax, (j * stepX + sampleMin));
            float R = qSqrt(z * z + x * x) + 0.01f;
            float y = (qSin(R) / R + 0.24f) * 1.61f;
            newRow->append(QSurfaceDataItem({x, y, z}));
        }
        dataArray->append(newRow);
    }

    m_sqrtSinProxy->resetArray(dataArray);
    //! [1]
}

void SurfaceGraphModifier::enableSqrtSinModel(bool enable)
{
    if (enable) {
        //! [3]
        m_sqrtSinSeries->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
        m_sqrtSinSeries->setFlatShadingEnabled(true);

        m_graph->axisX()->setLabelFormat("%.2f");
        m_graph->axisZ()->setLabelFormat("%.2f");
        m_graph->axisX()->setRange(sampleMin, sampleMax);
        m_graph->axisY()->setRange(0.f, 2.f);
        m_graph->axisZ()->setRange(sampleMin, sampleMax);
        m_graph->axisX()->setLabelAutoRotation(30.f);
        m_graph->axisY()->setLabelAutoRotation(90.f);
        m_graph->axisZ()->setLabelAutoRotation(30.f);

        m_graph->removeSeries(m_heightMapSeriesOne);
        m_graph->removeSeries(m_heightMapSeriesTwo);
        m_graph->removeSeries(m_heightMapSeriesThree);
        m_graph->removeSeries(m_topography);
        m_graph->removeSeries(m_highlight);

        m_graph->addSeries(m_sqrtSinSeries);
        //! [3]

        m_titleLabel->setVisible(false);
        m_graph->axisX()->setTitleVisible(false);
        m_graph->axisY()->setTitleVisible(false);
        m_graph->axisZ()->setTitleVisible(false);

        m_graph->axisX()->setTitle({});
        m_graph->axisY()->setTitle({});
        m_graph->axisZ()->setTitle({});

        m_graph->setActiveInputHandler(m_defaultInputHandler);

        //! [6]
        // Reset range sliders for Sqrt & Sin
        m_rangeMinX = sampleMin;
        m_rangeMinZ = sampleMin;
        m_stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
        m_stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);
        m_axisMinSliderX->setMinimum(0);
        m_axisMinSliderX->setMaximum(sampleCountX - 2);
        m_axisMinSliderX->setValue(0);
        m_axisMaxSliderX->setMinimum(1);
        m_axisMaxSliderX->setMaximum(sampleCountX - 1);
        m_axisMaxSliderX->setValue(sampleCountX - 1);
        m_axisMinSliderZ->setMinimum(0);
        m_axisMinSliderZ->setMaximum(sampleCountZ - 2);
        m_axisMinSliderZ->setValue(0);
        m_axisMaxSliderZ->setMinimum(1);
        m_axisMaxSliderZ->setMaximum(sampleCountZ - 1);
        m_axisMaxSliderZ->setValue(sampleCountZ - 1);
        //! [6]
    }
}

void SurfaceGraphModifier::enableHeightMapModel(bool enable)
{
    if (enable) {
        m_heightMapSeriesOne->setDrawMode(QSurface3DSeries::DrawSurface);
        m_heightMapSeriesOne->setFlatShadingEnabled(false);
        m_heightMapSeriesTwo->setDrawMode(QSurface3DSeries::DrawSurface);
        m_heightMapSeriesTwo->setFlatShadingEnabled(false);
        m_heightMapSeriesThree->setDrawMode(QSurface3DSeries::DrawSurface);
        m_heightMapSeriesThree->setFlatShadingEnabled(false);

        m_graph->axisX()->setLabelFormat("%.1f N");
        m_graph->axisZ()->setLabelFormat("%.1f E");
        m_graph->axisX()->setRange(34.f, 40.f);
        //! [4]
        m_graph->axisY()->setAutoAdjustRange(true);
        //! [4]
        m_graph->axisZ()->setRange(18.f, 24.f);

        m_graph->axisX()->setTitle(u"Latitude"_s);
        m_graph->axisY()->setTitle(u"Height"_s);
        m_graph->axisZ()->setTitle(u"Longitude"_s);

        m_graph->removeSeries(m_sqrtSinSeries);
        m_graph->removeSeries(m_topography);
        m_graph->removeSeries(m_highlight);
        m_graph->addSeries(m_heightMapSeriesOne);
        m_graph->addSeries(m_heightMapSeriesTwo);
        m_graph->addSeries(m_heightMapSeriesThree);

        m_graph->setActiveInputHandler(m_defaultInputHandler);

        m_titleLabel->setVisible(true);
        m_graph->axisX()->setTitleVisible(true);
        m_graph->axisY()->setTitleVisible(true);
        m_graph->axisZ()->setTitleVisible(true);

        // Reset range sliders for height map
        int mapGridCountX = m_heightMapWidth / heightMapGridStepX;
        int mapGridCountZ = m_heightMapHeight / heightMapGridStepZ;
        m_rangeMinX = 34.f;
        m_rangeMinZ = 18.f;
        m_stepX = 6.f / float(mapGridCountX - 1);
        m_stepZ = 6.f / float(mapGridCountZ - 1);
        m_axisMinSliderX->setMinimum(0);
        m_axisMinSliderX->setMaximum(mapGridCountX - 2);
        m_axisMinSliderX->setValue(0);
        m_axisMaxSliderX->setMinimum(1);
        m_axisMaxSliderX->setMaximum(mapGridCountX - 1);
        m_axisMaxSliderX->setValue(mapGridCountX - 1);
        m_axisMinSliderZ->setMinimum(0);
        m_axisMinSliderZ->setMaximum(mapGridCountZ - 2);
        m_axisMinSliderZ->setValue(0);
        m_axisMaxSliderZ->setMinimum(1);
        m_axisMaxSliderZ->setMaximum(mapGridCountZ - 1);
        m_axisMaxSliderZ->setValue(mapGridCountZ - 1);
    }
}

void SurfaceGraphModifier::enableTopographyModel(bool enable)
{
    if (enable) {
        m_graph->axisX()->setLabelFormat("%i");
        m_graph->axisZ()->setLabelFormat("%i");
        m_graph->axisX()->setRange(0.f, areaWidth);
        m_graph->axisY()->setRange(100.f, areaWidth * aspectRatio);
        m_graph->axisZ()->setRange(0.f, areaHeight);
        m_graph->axisX()->setLabelAutoRotation(30.f);
        m_graph->axisY()->setLabelAutoRotation(90.f);
        m_graph->axisZ()->setLabelAutoRotation(30.f);

        m_graph->removeSeries(m_heightMapSeriesOne);
        m_graph->removeSeries(m_heightMapSeriesTwo);
        m_graph->removeSeries(m_heightMapSeriesThree);
        m_graph->addSeries(m_topography);
        m_graph->addSeries(m_highlight);

        m_titleLabel->setVisible(false);
        m_graph->axisX()->setTitleVisible(false);
        m_graph->axisY()->setTitleVisible(false);
        m_graph->axisZ()->setTitleVisible(false);

        m_graph->axisX()->setTitle({});
        m_graph->axisY()->setTitle({});
        m_graph->axisZ()->setTitle({});

        //! [5]
        m_graph->setActiveInputHandler(m_customInputHandler);
        //! [5]

        // Reset range sliders for topography map
        m_rangeMinX = 0.f;
        m_rangeMinZ = 0.f;
        m_stepX = 1.f;
        m_stepZ = 1.f;
        m_axisMinSliderX->setMinimum(0);
        m_axisMinSliderX->setMaximum(areaWidth - 200);
        m_axisMinSliderX->setValue(0);
        m_axisMaxSliderX->setMinimum(200);
        m_axisMaxSliderX->setMaximum(areaWidth);
        m_axisMaxSliderX->setValue(areaWidth);
        m_axisMinSliderZ->setMinimum(0);
        m_axisMinSliderZ->setMaximum(areaHeight - 200);
        m_axisMinSliderZ->setValue(0);
        m_axisMaxSliderZ->setMinimum(200);
        m_axisMaxSliderZ->setMaximum(areaHeight);
        m_axisMaxSliderZ->setValue(areaHeight);
    }
}

void SurfaceGraphModifier::adjustXMin(int min)
{
    float minX = m_stepX * float(min) + m_rangeMinX;

    int max = m_axisMaxSliderX->value();
    if (min >= max) {
        max = min + 1;
        m_axisMaxSliderX->setValue(max);
    }
    float maxX = m_stepX * max + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void SurfaceGraphModifier::adjustXMax(int max)
{
    float maxX = m_stepX * float(max) + m_rangeMinX;

    int min = m_axisMinSliderX->value();
    if (max <= min) {
        min = max - 1;
        m_axisMinSliderX->setValue(min);
    }
    float minX = m_stepX * min + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void SurfaceGraphModifier::adjustZMin(int min)
{
    float minZ = m_stepZ * float(min) + m_rangeMinZ;

    int max = m_axisMaxSliderZ->value();
    if (min >= max) {
        max = min + 1;
        m_axisMaxSliderZ->setValue(max);
    }
    float maxZ = m_stepZ * max + m_rangeMinZ;

    setAxisZRange(minZ, maxZ);
}

void SurfaceGraphModifier::adjustZMax(int max)
{
    float maxX = m_stepZ * float(max) + m_rangeMinZ;

    int min = m_axisMinSliderZ->value();
    if (max <= min) {
        min = max - 1;
        m_axisMinSliderZ->setValue(min);
    }
    float minX = m_stepZ * min + m_rangeMinZ;

    setAxisZRange(minX, maxX);
}

//! [7]
void SurfaceGraphModifier::setAxisXRange(float min, float max)
{
    m_graph->axisX()->setRange(min, max);
}
//! [7]

void SurfaceGraphModifier::setAxisZRange(float min, float max)
{
    m_graph->axisZ()->setRange(min, max);
}

void SurfaceGraphModifier::setBlackToYellowGradient()
{
    //! [8]
    QLinearGradient gr;
    gr.setColorAt(0.f, Qt::black);
    gr.setColorAt(0.33f, Qt::blue);
    gr.setColorAt(0.67f, Qt::red);
    gr.setColorAt(1.f, Qt::yellow);

    m_sqrtSinSeries->setBaseGradient(gr);
    m_sqrtSinSeries->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    //! [8]
}

void SurfaceGraphModifier::setGreenToRedGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.f, Qt::darkGreen);
    gr.setColorAt(0.5f, Qt::yellow);
    gr.setColorAt(0.8f, Qt::red);
    gr.setColorAt(1.f, Qt::darkRed);

    m_sqrtSinSeries->setBaseGradient(gr);
    m_sqrtSinSeries->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraphModifier::toggleItemOne(bool show)
{
    //! [10]
    QVector3D positionOne = QVector3D(39.f, 77.f, 19.2f);
    //! [10]
    QVector3D positionOnePipe = QVector3D(39.f, 45.f, 19.2f);
    QVector3D positionOneLabel = QVector3D(39.f, 107.f, 19.2f);
    if (show) {
        //! [9]
        QImage color = QImage(2, 2, QImage::Format_RGB32);
        color.fill(Qt::red);
        //! [9]
        //! [11]
        auto *item = new QCustom3DItem(":/data/oilrig.obj", positionOne,
                                       QVector3D(0.025f, 0.025f, 0.025f),
                                       QQuaternion::fromAxisAndAngle(0.f, 1.f, 0.f, 45.f),
                                       color);
        //! [11]
        //! [12]
        m_graph->addCustomItem(item);
        //! [12]
        item = new QCustom3DItem(":/data/pipe.obj", positionOnePipe,
                                 QVector3D(0.005f, 0.5f, 0.005f),
                                 QQuaternion(),
                                 color);
        item->setShadowCasting(false);
        m_graph->addCustomItem(item);

        //! [13]
        auto *label = new QCustom3DLabel();
        label->setText("Oil Rig One");
        label->setPosition(positionOneLabel);
        label->setScaling(QVector3D(1.f, 1.f, 1.f));
        m_graph->addCustomItem(label);
        //! [13]
    } else {
        resetSelection();
        //! [14]
        m_graph->removeCustomItemAt(positionOne);
        //! [14]
        m_graph->removeCustomItemAt(positionOnePipe);
        m_graph->removeCustomItemAt(positionOneLabel);
    }
}

void SurfaceGraphModifier::toggleItemTwo(bool show)
{
    QVector3D positionTwo = QVector3D(34.5f, 77.f, 23.4f);
    QVector3D positionTwoPipe = QVector3D(34.5f, 45.f, 23.4f);
    QVector3D positionTwoLabel = QVector3D(34.5f, 107.f, 23.4f);
    if (show) {
        QImage color = QImage(2, 2, QImage::Format_RGB32);
        color.fill(Qt::red);
        auto *item = new QCustom3DItem();
        item->setMeshFile(":/data/oilrig.obj");
        item->setPosition(positionTwo);
        item->setScaling(QVector3D(0.025f, 0.025f, 0.025f));
        item->setRotation(QQuaternion::fromAxisAndAngle(0.f, 1.f, 0.f, 25.f));
        item->setTextureImage(color);
        m_graph->addCustomItem(item);
        item = new QCustom3DItem(":/data/pipe.obj", positionTwoPipe,
                                 QVector3D(0.005f, 0.5f, 0.005f),
                                 QQuaternion(),
                                 color);
        item->setShadowCasting(false);
        m_graph->addCustomItem(item);

        auto *label = new QCustom3DLabel();
        label->setText("Oil Rig Two");
        label->setPosition(positionTwoLabel);
        label->setScaling(QVector3D(1.f, 1.f, 1.f));
        m_graph->addCustomItem(label);
    } else {
        resetSelection();
        m_graph->removeCustomItemAt(positionTwo);
        m_graph->removeCustomItemAt(positionTwoPipe);
        m_graph->removeCustomItemAt(positionTwoLabel);
    }
}

void SurfaceGraphModifier::toggleItemThree(bool show)
{
    QVector3D positionThree = QVector3D(34.5f, 86.f, 19.1f);
    QVector3D positionThreeLabel = QVector3D(34.5f, 116.f, 19.1f);
    if (show) {
        QImage color = QImage(2, 2, QImage::Format_RGB32);
        color.fill(Qt::darkMagenta);
        auto *item = new QCustom3DItem();
        item->setMeshFile(":/data/refinery.obj");
        item->setPosition(positionThree);
        item->setScaling(QVector3D(0.04f, 0.04f, 0.04f));
        item->setRotation(QQuaternion::fromAxisAndAngle(0.f, 1.f, 0.f, 75.f));
        item->setTextureImage(color);
        m_graph->addCustomItem(item);

        auto *label = new QCustom3DLabel();
        label->setText("Refinery");
        label->setPosition(positionThreeLabel);
        label->setScaling(QVector3D(1.f, 1.f, 1.f));
        m_graph->addCustomItem(label);
    } else {
        resetSelection();
        m_graph->removeCustomItemAt(positionThree);
        m_graph->removeCustomItemAt(positionThreeLabel);
    }
}

void SurfaceGraphModifier::toggleSeeThrough(bool seethrough)
{
    if (seethrough) {
        m_graph->seriesList().at(0)->setDrawMode(QSurface3DSeries::DrawWireframe);
        m_graph->seriesList().at(1)->setDrawMode(QSurface3DSeries::DrawWireframe);
    } else {
        m_graph->seriesList().at(0)->setDrawMode(QSurface3DSeries::DrawSurface);
        m_graph->seriesList().at(1)->setDrawMode(QSurface3DSeries::DrawSurface);
    }
}

void SurfaceGraphModifier::toggleOilHighlight(bool highlight)
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

void SurfaceGraphModifier::toggleShadows(bool shadows)
{
    if (shadows)
        m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityMedium);
    else
        m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
}

//! [15]
void SurfaceGraphModifier::toggleSurfaceTexture(bool enable)
{
    if (enable)
        m_topography->setTextureFile(":/data/maptexture.jpg");
    else
        m_topography->setTextureFile("");
}
//! [15]

void SurfaceGraphModifier::handleElementSelected(QAbstract3DGraph::ElementType type)
{
    resetSelection();
    if (type == QAbstract3DGraph::ElementCustomItem) {
        QCustom3DItem *item = m_graph->selectedCustomItem();
        QString text;
        if (qobject_cast<QCustom3DLabel *>(item) != 0) {
            text.append("Custom label: ");
        } else {
            QStringList split = item->meshFile().split("/");
            text.append(split.last());
            text.append(": ");
        }
        int index = m_graph->selectedCustomItemIndex();
        text.append(QString::number(index));
        m_textField->setText(text);
        m_previouslyAnimatedItem = item;
        m_previousScaling = item->scaling();
        m_selectionAnimation->setTargetObject(item);
        m_selectionAnimation->setStartValue(item->scaling());
        m_selectionAnimation->setEndValue(item->scaling() * 1.5f);
        m_selectionAnimation->start();
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
        int index = m_graph->selectedLabelIndex();
        QString text;
        if (type == QAbstract3DGraph::ElementAxisXLabel)
            text.append("Axis X label: ");
        else if (type == QAbstract3DGraph::ElementAxisYLabel)
            text.append("Axis Y label: ");
        else
            text.append("Axis Z label: ");
        text.append(QString::number(index));
        m_textField->setText(text);
    } else {
        m_textField->setText("Nothing");
    }
}

void SurfaceGraphModifier::resetSelection()
{
    m_selectionAnimation->stop();
    if (m_previouslyAnimatedItem)
        m_previouslyAnimatedItem->setScaling(m_previousScaling);
    m_previouslyAnimatedItem = nullptr;
}
