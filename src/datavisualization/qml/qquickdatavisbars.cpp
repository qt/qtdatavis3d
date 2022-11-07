// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickbarseriesvisualizer_p.h"
#include "qquickdatavisbars_p.h"
#include "declarativescene_p.h"
#include "qbar3dseries_p.h"
#include "q3dcamera_p.h"
#include <QtCore/QMutexLocker>
#include "bars3dcontroller_p.h"
#include "utils_p.h"
#include <QColor>
#include <QtQuick3D/private/qquick3drepeater_p.h>
#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>
#include <QtQuick3D/private/qquick3dperspectivecamera_p.h>
#include <private/qquick3ddirectionallight_p.h>
#include <private/qquick3dpointlight_p.h>

QQuickDataVisBars::QQuickDataVisBars(QQuickItem *parent)
    : QQuickDataVisItem(parent),
      m_barsController(0),
      m_cachedRowCount(0),
      m_cachedColumnCount(0),
      m_minRow(0),
      m_maxRow(0),
      m_minCol(0),
      m_maxCol(0),
      m_newRows(0),
      m_newCols(0),
      m_maxSceneSize(40.0f),
      m_rowWidth(0),
      m_columnDepth(0),
      m_maxDimension(0),
      m_scaleFactor(0),
      m_xScaleFactor(1.0f),
      m_zScaleFactor(1.0f),
      m_cachedBarSeriesMargin(0.0f, 0.0f)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlags(ItemHasContents);
    // Create the shared component on the main GUI thread.
    m_barsController = new Bars3DController(boundingRect().toRect(), new Declarative3DScene);
    setSharedController(m_barsController);

    QQuick3DSceneEnvironment *scene = environment();
    scene->setBackgroundMode(QQuick3DSceneEnvironment::QQuick3DEnvironmentBackgroundTypes::Color);
    scene->setClearColor(Qt::blue);

    QObject::connect(m_barsController, &Bars3DController::primarySeriesChanged,
                     this, &QQuickDataVisBars::primarySeriesChanged);
    QObject::connect(m_barsController, &Bars3DController::selectedSeriesChanged,
                     this, &QQuickDataVisBars::selectedSeriesChanged);
}

QQuickDataVisBars::~QQuickDataVisBars()
{
    QMutexLocker locker(m_nodeMutex.data());
    const QMutexLocker locker2(mutex());
    delete m_barsController;
}

QCategory3DAxis *QQuickDataVisBars::rowAxis() const
{
    return static_cast<QCategory3DAxis *>(m_barsController->axisZ());
}

void QQuickDataVisBars::setRowAxis(QCategory3DAxis *axis)
{
    m_barsController->setAxisZ(axis);
}

QValue3DAxis *QQuickDataVisBars::valueAxis() const
{
    return static_cast<QValue3DAxis *>(m_barsController->axisY());
}

void QQuickDataVisBars::setValueAxis(QValue3DAxis *axis)
{
    m_barsController->setAxisY(axis);
}

QCategory3DAxis *QQuickDataVisBars::columnAxis() const
{
    return static_cast<QCategory3DAxis *>(m_barsController->axisX());
}

void QQuickDataVisBars::setColumnAxis(QCategory3DAxis *axis)
{
    m_barsController->setAxisX(axis);
}

void QQuickDataVisBars::setMultiSeriesUniform(bool uniform)
{
    if (uniform != isMultiSeriesUniform()) {
        m_barsController->setMultiSeriesScaling(uniform);
        emit multiSeriesUniformChanged(uniform);
    }
}

bool QQuickDataVisBars::isMultiSeriesUniform() const
{
    return m_barsController->multiSeriesScaling();
}

void QQuickDataVisBars::setBarThickness(float thicknessRatio)
{
    if (thicknessRatio != barThickness()) {
        m_barsController->setBarSpecs(thicknessRatio, barSpacing(),
                                      isBarSpacingRelative());
        emit barThicknessChanged(thicknessRatio);
    }
}

float QQuickDataVisBars::barThickness() const
{
    return m_barsController->barThickness();
}

void QQuickDataVisBars::setBarSpacing(const QSizeF &spacing)
{
    if (spacing != barSpacing()) {
        m_barsController->setBarSpecs(barThickness(), spacing, isBarSpacingRelative());
        emit barSpacingChanged(spacing);
    }
}

QSizeF QQuickDataVisBars::barSpacing() const
{
    return m_barsController->barSpacing();
}

void QQuickDataVisBars::setBarSpacingRelative(bool relative)
{
    if (relative != isBarSpacingRelative()) {
        m_barsController->setBarSpecs(barThickness(), barSpacing(), relative);
        emit barSpacingRelativeChanged(relative);
    }
}

bool QQuickDataVisBars::isBarSpacingRelative() const
{
    return m_barsController->isBarSpecRelative();
}

void QQuickDataVisBars::setBarSeriesMargin(const QSizeF &margin)
{
    if (margin != barSeriesMargin()) {
        m_barsController->setBarSeriesMargin(margin);
        emit barSeriesMarginChanged(barSeriesMargin());
    }
}

QSizeF QQuickDataVisBars::barSeriesMargin() const
{
    return m_barsController->barSeriesMargin();
}

QQmlListProperty<QBar3DSeries> QQuickDataVisBars::seriesList()
{
    return QQmlListProperty<QBar3DSeries>(this, this,
                                          &QQuickDataVisBars::appendSeriesFunc,
                                          &QQuickDataVisBars::countSeriesFunc,
                                          &QQuickDataVisBars::atSeriesFunc,
                                          &QQuickDataVisBars::clearSeriesFunc);
}

void QQuickDataVisBars::appendSeriesFunc(QQmlListProperty<QBar3DSeries> *list, QBar3DSeries *series)
{
    reinterpret_cast<QQuickDataVisBars *>(list->data)->addSeries(series);
}

qsizetype QQuickDataVisBars::countSeriesFunc(QQmlListProperty<QBar3DSeries> *list)
{
    return reinterpret_cast<QQuickDataVisBars *>(list->data)->m_barsController->barSeriesList().size();
}

QBar3DSeries *QQuickDataVisBars::atSeriesFunc(QQmlListProperty<QBar3DSeries> *list, qsizetype index)
{
    return reinterpret_cast<QQuickDataVisBars *>(list->data)->m_barsController->barSeriesList().at(index);
}

void QQuickDataVisBars::clearSeriesFunc(QQmlListProperty<QBar3DSeries> *list)
{
    QQuickDataVisBars *declBars = reinterpret_cast<QQuickDataVisBars *>(list->data);
    QList<QBar3DSeries *> realList = declBars->m_barsController->barSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declBars->removeSeries(realList.at(i));
}

void QQuickDataVisBars::addSeries(QBar3DSeries *series)
{
    m_barsController->addSeries(series);
    auto visualizer = new QQuickBarSeriesVisualizer();
    visualizer->m_controller = m_barsController;
    visualizer->m_qml = this;
    visualizer->m_dataVisBars = this;
    visualizer->setup();
    visualizer->connectSeries(series);
    visualizer->m_helperAxisX = &m_helperAxisX;
    visualizer->m_helperAxisY = &m_helperAxisY;
    visualizer->m_helperAxisZ = &m_helperAxisZ;
    setVisualizerForSeries(series, visualizer);

    if (series->selectedBar() != invalidSelectionPosition())
        setSelectedBar(series->selectedBar(), series, false);
}

void QQuickDataVisBars::removeSeries(QBar3DSeries *series)
{
    m_barsController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

void QQuickDataVisBars::insertSeries(int index, QBar3DSeries *series)
{
    m_barsController->insertSeries(index, series);
}

void QQuickDataVisBars::setPrimarySeries(QBar3DSeries *series)
{
    m_barsController->setPrimarySeries(series);
}

QBar3DSeries *QQuickDataVisBars::primarySeries() const
{
    return m_barsController->primarySeries();
}

QBar3DSeries *QQuickDataVisBars::selectedSeries() const
{
    return m_barsController->selectedSeries();
}

void QQuickDataVisBars::setSelectedBar(const QPoint &position, QBar3DSeries *series, bool enterSlice)
{
    // If the selection targets non-existent bar, clear selection instead.
    QPoint pos = position;

    // Series may already have been removed, so check it before setting the selection.
    if (!m_barsController->m_seriesList.contains(series))
        series = 0;

    m_barsController->adjustSelectionPosition(pos, series);

//    if (m_barsController->selectionMode().testFlag(QAbstract3DGraph::SelectionSlice)) {
//        // If the selected bar is outside data window, or there is no visible selected bar,
//        // disable slicing.
        if (pos.x() < m_barsController->m_axisZ->min() || pos.x() > m_barsController->m_axisZ->max()
                || pos.y() < m_barsController->m_axisX->min() || pos.y() > m_barsController->m_axisX->max()
                || !series->isVisible()) {
            scene()->setSlicingActive(false);
        } else if (enterSlice) {
            scene()->setSlicingActive(true);
        }
//    }

    if (pos != m_selectedBar || series != m_selectedBarSeries) {
        bool seriesChanged = (series != m_selectedBarSeries);
        m_selectedBar = pos;
        m_selectedBarSeries = series;
        m_barsController->m_changeTracker.selectedBarChanged = true;

        // Clear selection from other series and finally set new selection to the specified series
        foreach (QAbstract3DSeries *otherSeries, m_barsController->m_seriesList) {
            QBar3DSeries *barSeries = static_cast<QBar3DSeries *>(otherSeries);
            if (barSeries != m_selectedBarSeries) {
                auto visualizer = visualizerForSeries(barSeries);
                visualizer->clearSelection();
                barSeries->dptr()->setSelectedBar(invalidSelectionPosition());
            }
        }
        if (m_selectedBarSeries)
            m_selectedBarSeries->dptr()->setSelectedBar(m_selectedBar);

        if (seriesChanged)
            emit selectedSeriesChanged(m_selectedBarSeries);
    }
}


void QQuickDataVisBars::setFloorLevel(float level)
{
    if (level != floorLevel()) {
        m_barsController->setFloorLevel(level);
        emit floorLevelChanged(level);
    }
}

float QQuickDataVisBars::floorLevel() const
{
    return m_barsController->floorLevel();
}

void QQuickDataVisBars::componentComplete()
{
    setXFlipped(true);

    QQuickDataVisItem::componentComplete();

    QValue3DAxis *axisY = static_cast<QValue3DAxis *>(m_barsController->axisY());
    m_helperAxisY.setFormatter(axisY->formatter());

    setScaleOffset({2.0f, 1.0f, 2.0f});
    setLineLengthScaleFactor(0.01f);

    m_lineLengthScaleFactor = lineLengthScaleFactor();

    setLabelMargin(0.4f);
}

void QQuickDataVisBars::synchData()
{
    QList<QBar3DSeries *> seriesList = m_barsController->barSeriesList();

    calculateSceneScalingFactors();

    QQuickDataVisItem::synchData();

    if (m_barsController->m_changeTracker.barSpecsChanged) {
        updateBarSpecs(m_barsController->m_barThicknessRatio, m_barsController->m_barSpacing, m_barsController->m_isBarSpecRelative);
        m_barsController->m_changeTracker.barSpecsChanged = false;
    }

    // Floor level update requires data update, so do before abstract sync
    if (m_barsController->m_changeTracker.floorLevelChanged) {
//        updateFloorLevel(m_floorLevel);
        m_barsController->m_changeTracker.floorLevelChanged = false;
    }

    if (m_barsController->m_changeTracker.barSeriesMarginChanged) {
        // m_renderer->updateBarSeriesMargin(m_barSeriesMargin);
        m_barsController->m_changeTracker.barSeriesMarginChanged = false;
    }

    // Needs to be done after data is set, as it needs to know the visual array.
    if (m_barsController->m_changeTracker.selectedBarChanged) {
        // m_renderer->updateSelectedBar(m_selectedBar, m_selectedBarSeries);
        m_barsController->m_changeTracker.selectedBarChanged = false;
    }
}

void QQuickDataVisBars::updateParameters() {
    m_minRow = m_barsController->m_axisZ->min();
    m_maxRow = m_barsController->m_axisZ->max();
    m_minCol = m_barsController->m_axisX->min();
    m_maxCol = m_barsController->m_axisX->max();
    m_newRows = m_maxRow - m_minRow + 1;
    m_newCols = m_maxCol - m_minCol + 1;

    if (m_cachedRowCount!= m_newRows || m_cachedColumnCount != m_newCols) {
        // Force update for selection related items
        //m_sliceCache = 0;
        //m_sliceTitleItem = 0;

        m_cachedColumnCount = m_newCols;
        m_cachedRowCount = m_newRows;

        // Calculate max scene size
        float sceneRatio = qMin(float(m_newCols) / float(m_newRows),
                                  float(m_newRows) / float(m_newCols));
        m_maxSceneSize = 2.0f * qSqrt(sceneRatio * m_newCols * m_newRows);
    }
    calculateSceneScalingFactors();
}

void QQuickDataVisBars::updateGrid()
{
    QVector3D scaleX(m_helperAxisX.scale() * m_lineLengthScaleFactor, m_lineWidthScaleFactor, m_lineWidthScaleFactor);
    QVector3D scaleY(m_lineWidthScaleFactor, m_helperAxisY.scale() * m_lineLengthScaleFactor * m_scaleYWithBackground, m_lineWidthScaleFactor);
    QVector3D scaleZ(m_lineWidthScaleFactor, -m_helperAxisZ.scale() * m_lineLengthScaleFactor, 1);

    QVector3D lineBackRotationX(0, 0, 0);
    QVector3D lineBackRotationY(0, 0, 0);

    float linePosX = 0;
    float linePosY = -m_scaleYWithBackground + m_gridOffset;
    float linePosZ = 0;

    QVector3D lineSideRotationY(0, 90, -90);
    QVector3D lineSideRotationZ(0, 90, 0);

    QVector3D lineFloorRotationX(-90,0,0);
    QVector3D lineFloorRotationZ(-90,0,0);

    const bool xFlipped = isXFlipped();
    const bool zFlipped = isZFlipped();

    // Floor lines: rows
    for (int row = 0; row < segmentLineRepeaterZ()->count(); row++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterZ()->objectAt(row));
        float rowPos = row * m_cachedBarSpacing.height();
        linePosZ = (m_columnDepth - rowPos) / m_scaleFactor;
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineFloorRotationZ);
    }

    // X = Column
    linePosZ = 0;

    for (int col = 0; col < segmentLineRepeaterX()->count(); col++) {
        auto lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterX()->objectAt(col));
        float colPos = col * m_cachedBarSpacing.width();
        linePosX = (m_rowWidth - colPos) / m_scaleFactor;
        positionAndScaleLine(lineNode,scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineFloorRotationX);
    }

    // Wall lines: back wall
    // X = Column
    linePosY = 0;
    if (!zFlipped) {
        linePosZ = -m_scaleZWithBackground + m_gridOffset;
    } else {
        linePosZ = m_scaleZWithBackground - m_gridOffset;
        lineBackRotationX = QVector3D(0, 180, 0);
        lineBackRotationY = QVector3D(0, 180, 0);
    }

    linePosX = 0;
    int gridLineCountY = segmentLineRepeaterY()->count() / 2;
    int subGridLineCountY = subsegmentLineRepeaterY()->count() / 2;
    auto scale = m_yScale * m_scaleYWithBackground + m_yScale;

    for (int i  = 0; i < gridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterY()->objectAt(i));
        linePosY = valueAxis()->gridPositionAt(i) * scale - m_yScale;
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineBackRotationY);
    }

    for (int i = 0; i < subGridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterY()->objectAt(i));
        linePosY = valueAxis()->subGridPositionAt(i) * scale - m_yScale;
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineBackRotationY);
    }

    // Side
    // Y = Row

    linePosZ = 0;
    int k = 0;
    if (!xFlipped) {
        linePosX = -m_scaleXWithBackground + m_gridOffset;
    } else {
        linePosX = m_scaleXWithBackground - m_gridOffset;
        lineSideRotationY = QVector3D(0, -90, 90);
        lineSideRotationZ = QVector3D(0, -90, 0);
    }

    for (int i = gridLineCountY; i < segmentLineRepeaterY()->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterY()->objectAt(i));
        linePosY = valueAxis()->gridPositionAt(k) * scale - m_yScale;
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineSideRotationY);
        k++;
    }

    k = 0;
    for (int i = subGridLineCountY; i < subsegmentLineRepeaterY()->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterY()->objectAt(i));
        linePosY = valueAxis()->subGridPositionAt(k) * scale - m_yScale;
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineSideRotationY);
        k++;
    }
}

void QQuickDataVisBars::updateLabels()
{
    float labelAutoAngle = 0.0f;
    float rowPos = 0.0f;
    float colPos = 0.0f;

    // X Labels
    auto labels = m_barsController->m_axisX->labels();
    labelAutoAngle = m_barsController->m_axisX->labelAutoRotation();
    float labelAngleFraction = labelAutoAngle / 90.0f;
    float fractionCamX = m_barsController->scene()->activeCamera()->xRotation() * labelAngleFraction;
    float fractionCamY = m_barsController->scene()->activeCamera()->yRotation() * labelAngleFraction;

    QVector3D labelRotation;

    float xPos = 0.0f;
    float yPos = 0.0f;
    float zPos = 0.0f;

    const bool xFlipped = isXFlipped();
    const bool yFlipped = isYFlipped();
    const bool zFlipped = isZFlipped();

    if (labelAutoAngle == 0.0f) {
        labelRotation = QVector3D(-90.0f, 90.0f, 0.0f);
        if (xFlipped)
            labelRotation.setY(-90.0f);
        if (yFlipped) {
            if (xFlipped)
                labelRotation.setY(-90.0f);
            else
                labelRotation.setY(90.0f);
            labelRotation.setX(90.0f);
        }
    } else {
        if (xFlipped)
            labelRotation.setY(-90.0f);
        else
            labelRotation.setY(90.0f);
        if (yFlipped) {
            if (zFlipped) {
                if (xFlipped) {
                    labelRotation.setX(90.0f - (2.0f * labelAutoAngle - fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(90.0f - (2.0f * labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                }
            } else {
                if (xFlipped) {
                    labelRotation.setX(90.0f + fractionCamX
                                       * -(labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(90.0f - fractionCamX
                                       * (-labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                }
            }
        } else {
            if (zFlipped) {
                if (xFlipped) {
                    labelRotation.setX(-90.0f + (2.0f * labelAutoAngle - fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                }
            } else {
                if (xFlipped) {
                    labelRotation.setX(-90.0f - fractionCamX
                                       * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(-90.0f + fractionCamX
                                       * -(labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                }
            }
        }
    }

    if (!xFlipped) {
        if (!yFlipped)
            yPos = -m_scaleYWithBackground;
        else
            yPos = m_scaleYWithBackground;
    } else {
        if (!yFlipped)
            yPos = -m_scaleYWithBackground;
        else
            yPos = m_scaleYWithBackground;
    }
    auto totalRotation = Utils::calculateRotation(labelRotation);
    auto labelTrans = QVector3D(0.0f, yPos, zPos);
    float labelsMaxWidth = 0.0f;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(m_barsController->m_axisX->labels())
                                                / (m_scaleXWithBackground * m_scaleZWithBackground)));
    float rowPosValue = m_scaleZWithBackground + labelMargin();

    for (int i = 0; i < repeaterX()->count(); i++) {
        colPos = (i + 0.5f) * m_cachedBarSpacing.width();
        if (zFlipped)
            rowPos = -rowPosValue;
        else
            rowPos = rowPosValue;

        labelTrans.setX((colPos - m_rowWidth) / m_scaleFactor);
        labelTrans.setZ(rowPos);

        auto obj = static_cast<QQuick3DNode *>(repeaterX()->objectAt(i));
        obj->setPosition(labelTrans);
        obj->setRotation(totalRotation);
        obj->setProperty("labelText", labels[i]);
        obj->setProperty("labelWidth", labelsMaxWidth);
    }

    if (titleLabelX()->visible()) {
        labelTrans.setX(0.0f);
        updateXTitle(labelRotation, labelTrans, totalRotation, labelsMaxWidth);
    }

    // Y Labels
    labels = m_barsController->m_axisY->labels();
    labelAutoAngle = m_barsController->m_axisY->labelAutoRotation();
    labelAngleFraction = labelAutoAngle / 90.0f;
    fractionCamX = m_barsController->scene()->activeCamera()->xRotation() * labelAngleFraction;
    fractionCamY = m_barsController->scene()->activeCamera()->yRotation() * labelAngleFraction;
    float labelMarginXTrans = 0.05f;
    float labelMarginZTrans = 0.05f;
    float labelXTrans = m_scaleXWithBackground;
    float labelZTrans = m_scaleZWithBackground;

    QVector3D sideLabelRotation(0.0f, -90.0f, 0.0f);
    QVector3D backLabelRotation(0.0f, 0.0f, 0.0f);

    int rightSideCount = repeaterY()->count() / 2;

    if (labelAutoAngle == 0.0f) {
        if (!xFlipped)
            sideLabelRotation.setY(90.0f);
        if (zFlipped)
            backLabelRotation.setY(180.f);
    } else {
        // Orient side labels somewhat towards the camera
        if (xFlipped) {
            if (zFlipped)
                backLabelRotation.setY(180.0f + (2.0f * labelAutoAngle) - fractionCamX);
            else
                backLabelRotation.setY(-fractionCamX);
            sideLabelRotation.setY(-90.0f + labelAutoAngle - fractionCamX);
        } else {
            if (zFlipped)
                backLabelRotation.setY(180.0f - (2.0f * labelAutoAngle) - fractionCamX);
            else
                backLabelRotation.setY(-fractionCamX);
            sideLabelRotation.setY(90.0f - labelAutoAngle - fractionCamX);
        }
    }

    if (!xFlipped) {
        labelXTrans = -labelXTrans;
        labelMarginXTrans = -labelMarginXTrans;
    }
    if (zFlipped) {
        labelZTrans = -labelZTrans;
        labelMarginZTrans = -labelMarginXTrans;
    }

    backLabelRotation.setX(-fractionCamY);
    sideLabelRotation.setX(-fractionCamY);

    auto totalSideLabelRotation = Utils::calculateRotation(sideLabelRotation);
    QVector3D SideLabelTrans = QVector3D(-labelXTrans - labelMarginXTrans,
                                         0.0f, -labelZTrans - labelMarginZTrans);

    labelsMaxWidth = 0.0f;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(m_barsController->m_axisY->labels())
                                                / (m_scaleXWithBackground * m_scaleZWithBackground)));
    auto scale = m_yScale * m_scaleYWithBackground + m_yScale;

    for (int i = 0; i < rightSideCount; i++) {
        auto obj = static_cast<QQuick3DNode *>(repeaterY()->objectAt(i));
        SideLabelTrans.setY(m_helperAxisY.labelPositionAt(i) * scale - m_yScale);
        obj->setPosition(SideLabelTrans);
        obj->setRotation(totalSideLabelRotation);
        obj->setProperty("labelText", labels[i]);
        obj->setProperty("labelWidth", labelsMaxWidth);
    }

    int label = 0;
    auto totalBackLabelRotation = Utils::calculateRotation(backLabelRotation);
    QVector3D backLabelTrans = QVector3D(labelXTrans + labelMarginXTrans, 0.0f,
                                         labelZTrans + labelMarginZTrans);

    for (int i = rightSideCount; i < repeaterY()->count(); i++) {
        auto obj = static_cast<QQuick3DNode *>(repeaterY()->objectAt(i));
        backLabelTrans.setY(m_helperAxisY.labelPositionAt(label) * scale - m_yScale);
        obj->setPosition(backLabelTrans);
        obj->setRotation(totalBackLabelRotation);
        obj->setProperty("labelText", labels[label]);
        obj->setProperty("labelWidth", labelsMaxWidth);
        label++;
   }
   backLabelTrans.setY(0.0f);
   SideLabelTrans.setY(0.0f);

   if (titleLabelY()->visible()) {
       updateYTitle(sideLabelRotation, backLabelRotation,
                        SideLabelTrans, backLabelTrans,
                        totalSideLabelRotation, totalBackLabelRotation, 1);
   }

    // Z labels
    labels = m_barsController->m_axisZ->labels();
    labelAutoAngle = m_barsController->m_axisZ->labelAutoRotation();
    labelAngleFraction = labelAutoAngle / 90.0f;
    fractionCamX = m_barsController->scene()->activeCamera()->xRotation() * labelAngleFraction;
    fractionCamY = m_barsController->scene()->activeCamera()->yRotation() * labelAngleFraction;

    if (labelAutoAngle == 0.0f) {
        if (zFlipped)
            labelRotation.setY(180.0f);
        else
            labelRotation.setY(0.0f);
        if (yFlipped) {
            if (zFlipped)
                labelRotation.setY(180.0f);
            else
                labelRotation.setY(0.0f);
            labelRotation.setX(90.0f);
        } else {
            labelRotation.setX(-90.0f);
        }
    } else {
        if (zFlipped)
            labelRotation.setY(180.0f);
        else
            labelRotation.setY(0.0f);
        if (yFlipped) {
            if (zFlipped) {
                if (xFlipped) {
                    labelRotation.setX(90.0f - (labelAutoAngle - fractionCamX)
                                       * (-labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(90.0f + (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                }
            } else {
                if (xFlipped) {
                    labelRotation.setX(90.0f + (labelAutoAngle - fractionCamX)
                                       * -(labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(90.0f - (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                }
            }
        } else {
            if (zFlipped) {
                if (xFlipped) {
                    labelRotation.setX(-90.0f + (labelAutoAngle - fractionCamX)
                                       * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(-90.0f - (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                }
            } else {
                if (xFlipped) {
                    labelRotation.setX(-90.0f - (labelAutoAngle - fractionCamX)
                                       * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(-90.0f + (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                }
            }
        }
    }

    if (!zFlipped) {
        if (!yFlipped)
            yPos = -m_scaleYWithBackground;
        else
            yPos = m_scaleYWithBackground;
    } else {
        if (!yFlipped)
            yPos = -m_scaleYWithBackground;
        else
            yPos = m_scaleYWithBackground;
    }

    totalRotation = Utils::calculateRotation(labelRotation);
    labelTrans = QVector3D(xPos, yPos, 0);
    labelsMaxWidth = 0.0f;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(m_barsController->m_axisZ->labels())
                                                / (m_scaleXWithBackground * m_scaleZWithBackground)));
    float colPosValue = m_scaleXWithBackground + (labelMargin() / 2.0f);

    for (int i = 0; i < repeaterZ()->count(); i++) {
        rowPos = (i + 0.5f) * m_cachedBarSpacing.height();
        if (xFlipped)
            colPos = -colPosValue;
        else
            colPos = colPosValue;

        labelTrans.setX(colPos);
        labelTrans.setZ((m_columnDepth - rowPos) / m_scaleFactor);

        auto obj = static_cast<QQuick3DNode *>(repeaterZ()->objectAt(i));
        obj->setPosition(labelTrans);
        obj->setRotation(totalRotation);
        obj->setProperty("labelText", labels[i]);
        obj->setProperty("labelWidth", labelsMaxWidth);
    }

    if (titleLabelZ()->visible()) {
        labelTrans.setZ(0.0f);
        updateZTitle(labelRotation, labelTrans, totalRotation, labelsMaxWidth);
    }
}

void QQuickDataVisBars::updateGraph()
{
    QList<QBar3DSeries *> barSeriesList = m_barsController->barSeriesList();
    calculateSceneScalingFactors();

    for (auto *barSeries : std::as_const(barSeriesList)) {
        auto *visualizer = visualizerForSeries(barSeries);
        if (visualizer && barSeries->isVisible()) {
            if (visualizer)
                visualizer->generateBars(m_barsController->m_seriesList);
            if (visualizer->m_barsGenerated && m_barsController->m_isDataDirty)
                updateDataPoints(barSeries);
            if (visualizer->m_barsGenerated && m_barsController->m_isSeriesVisualsDirty)
                updateDataPointVisuals(barSeries);
//            if (visualizer->m_barsGenerated) {
//                updateGrid();
//                updateLabels();
//            }
        }
    }
}

void QQuickDataVisBars::calculateSceneScalingFactors()
{
    m_rowWidth = (m_cachedColumnCount * m_cachedBarSpacing.width()) / 2.0f;
    m_columnDepth = (m_cachedRowCount * m_cachedBarSpacing.height()) / 2.0f;
    m_maxDimension = qMax(m_rowWidth, m_columnDepth);
    m_scaleFactor = qMin((m_cachedColumnCount *(m_maxDimension / m_maxSceneSize)),
                         (m_cachedRowCount * (m_maxDimension / m_maxSceneSize)));

    // Single bar scaling
    m_xScale = m_cachedBarThickness.width() / m_scaleFactor;
    m_zScale = m_cachedBarThickness.height() / m_scaleFactor;

    // Adjust scaling according to margin
    m_xScale = m_xScale - m_xScale * m_cachedBarSeriesMargin.width();
    m_zScale = m_zScale - m_zScale * m_cachedBarSeriesMargin.height();

    // Whole graph scale factors
    m_xScaleFactor = m_rowWidth / m_scaleFactor;
    m_zScaleFactor = m_columnDepth / m_scaleFactor;

    if (m_requestedMargin < 0.0f) {
        m_hBackgroundMargin = 0.0f;
        m_vBackgroundMargin = 0.0f;
    } else {
        m_hBackgroundMargin = m_requestedMargin;
        m_vBackgroundMargin = m_requestedMargin;
    }

    m_scaleXWithBackground = m_xScaleFactor + m_hBackgroundMargin;
    m_scaleYWithBackground = 1.0f + m_vBackgroundMargin;
    m_scaleZWithBackground = m_zScaleFactor + m_hBackgroundMargin;

    setScaleWithBackground(QVector3D(m_scaleXWithBackground, m_scaleYWithBackground, m_scaleZWithBackground));

    m_helperAxisX.setScale(m_scaleXWithBackground * 2);
    m_helperAxisY.setScale(m_yScale);
    m_helperAxisZ.setScale(-m_scaleZWithBackground * 2);
    m_helperAxisX.setTranslate(-m_xScale);
    m_helperAxisY.setTranslate(0.0f);
}

void QQuickDataVisBars::calculateHeightAdjustment()
{
}

void QQuickDataVisBars::handleAxisXChanged(QAbstract3DAxis *axis)
{
    emit columnAxisChanged(static_cast<QCategory3DAxis *>(axis));
}

void QQuickDataVisBars::handleAxisYChanged(QAbstract3DAxis *axis)
{
    emit valueAxisChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisBars::handleAxisZChanged(QAbstract3DAxis *axis)
{
    emit rowAxisChanged(static_cast<QCategory3DAxis *>(axis));
}

QQmlComponent *QQuickDataVisBars::createRepeaterDelegate(QAbstract3DSeries::Mesh meshType)
{
    switch (meshType) {
    case QAbstract3DSeries::MeshSphere:
        return createRepeaterDelegateComponent(QStringLiteral(":/datapointModels/DatapointSphere"));
    case QAbstract3DSeries::MeshCube:
        return createRepeaterDelegateComponent(QStringLiteral(":/datapointModels/DatapointCube"));
    default:
        return createRepeaterDelegateComponent(QStringLiteral(":/datapointModels/DatapointSphere"));
    }
}

void QQuickDataVisBars::setVisualizerForSeries(QBar3DSeries *series, QQuickBarSeriesVisualizer *visualizer)
{
    m_seriesVisualizerMap.insert(series, visualizer);
}

QQuickBarSeriesVisualizer *QQuickDataVisBars::visualizerForSeries(QBar3DSeries *series)
{
    return m_seriesVisualizerMap.value(series, nullptr);
}

void QQuickDataVisBars::updateBarSpecs(float thicknessRatio, const QSizeF &spacing, bool relative)
{
    // Convert ratio to QSizeF, as we need it in that format for autoscaling calculations
    m_cachedBarThickness.setWidth(1.0);
    m_cachedBarThickness.setHeight(1.0f / thicknessRatio);

    if (relative) {
        m_cachedBarSpacing.setWidth((m_cachedBarThickness.width()*2)
                                    * (spacing.width() + 1.0f));
        m_cachedBarSpacing.setHeight((m_cachedBarThickness.height() * 2)
                                     * (spacing.height() + 1.0f));
    } else {
        m_cachedBarSpacing = m_cachedBarThickness * 2 + spacing * 2;
    }

    /* Will I need those?
     *
    // Slice mode doesn't update correctly without this
    if (m_cachedIsSlicingActivated)
        m_selectionDirty = true;
     *
    */

    // Calculate here and at setting sample space
    calculateSceneScalingFactors();
}

void QQuickDataVisBars::updateDataPoints(QBar3DSeries *series)
{
    auto visualizer = visualizerForSeries(series);
    visualizer->updateData(series->dataProxy());
}

void QQuickDataVisBars::updateDataPointVisuals(QBar3DSeries *series)
{
    Q_UNUSED(series);
    auto visualizer = visualizerForSeries(series);
    visualizer->updateItemVisuals(series);
}

