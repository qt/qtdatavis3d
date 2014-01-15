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

#include "bars3dcontroller_p.h"
#include "bars3drenderer_p.h"
#include "camerahelper_p.h"
#include "qabstract3daxis_p.h"
#include "qvalue3daxis_p.h"
#include "qcategory3daxis_p.h"
#include "qbardataproxy_p.h"
#include "qbar3dseries_p.h"
#include "thememanager_p.h"
#include "q3dtheme_p.h"

#include <QMatrix4x4>
#include <qmath.h>

namespace QtDataVisualization {

Bars3DController::Bars3DController(QRect boundRect, Q3DScene *scene)
    : Abstract3DController(boundRect, scene),
      m_selectedBar(invalidSelectionPosition()),
      m_selectedBarSeries(0),
      m_isBarSpecRelative(true),
      m_barThicknessRatio(1.0f),
      m_barSpacing(QSizeF(1.0, 1.0)),
      m_renderer(0)
{
    // Setting a null axis creates a new default axis according to orientation and graph type.
    // Note: these cannot be set in the Abstract3DController constructor, as they will call virtual
    //       functions implemented by subclasses.
    setAxisX(0);
    setAxisY(0);
    setAxisZ(0);
}

Bars3DController::~Bars3DController()
{
}

void Bars3DController::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (isInitialized())
        return;

    m_renderer = new Bars3DRenderer(this);

    setRenderer(m_renderer);
    synchDataToRenderer();

    QObject::connect(m_renderer, &Bars3DRenderer::barClicked, this,
                     &Bars3DController::handleBarClicked, Qt::QueuedConnection);
    emitNeedRender();
}

void Bars3DController::synchDataToRenderer()
{
    // Background change requires reloading the meshes in bar graphs, so dirty the series visuals
    if (m_themeManager->activeTheme()->d_ptr->m_dirtyBits.backgroundEnabledDirty) {
        m_isSeriesVisualsDirty = true;
        foreach (QAbstract3DSeries *series, m_seriesList)
            series->d_ptr->m_changeTracker.meshChanged = true;
    }

    Abstract3DController::synchDataToRenderer();

    if (!isInitialized())
        return;

    // Notify changes to renderer
    if (m_changeTracker.barSpecsChanged) {
        m_renderer->updateBarSpecs(m_barThicknessRatio, m_barSpacing, m_isBarSpecRelative);
        m_changeTracker.barSpecsChanged = false;
    }

    // Needs to be done after data is set, as it needs to know the visual array.
    if (m_changeTracker.selectedBarChanged) {
        m_renderer->updateSelectedBar(m_selectedBar, m_selectedBarSeries);
        m_changeTracker.selectedBarChanged = false;
    }
}

void Bars3DController::handleArrayReset()
{
    QBar3DSeries *series = static_cast<QBarDataProxy *>(sender())->series();
    if (series->isVisible()) {
        adjustAxisRanges();
        m_isDataDirty = true;
    }
    // Clear selection unless still valid
    setSelectedBar(m_selectedBar, m_selectedBarSeries);
    emitNeedRender();
}

void Bars3DController::handleRowsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    QBar3DSeries *series = static_cast<QBarDataProxy *>(sender())->series();
    if (series->isVisible()) {
        adjustAxisRanges();
        m_isDataDirty = true;
    }
    emitNeedRender();
}

void Bars3DController::handleRowsChanged(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    QBar3DSeries *series = static_cast<QBarDataProxy *>(sender())->series();
    if (series->isVisible()) {
        adjustAxisRanges();
        m_isDataDirty = true;
    }
    emitNeedRender();
}

void Bars3DController::handleRowsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    QBar3DSeries *series = static_cast<QBarDataProxy *>(sender())->series();
    if (series->isVisible()) {
        adjustAxisRanges();
        m_isDataDirty = true;
    }

    // Clear selection unless still valid
    setSelectedBar(m_selectedBar, m_selectedBarSeries);

    emitNeedRender();
}

void Bars3DController::handleRowsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    QBar3DSeries *series = static_cast<QBarDataProxy *>(sender())->series();
    if (series->isVisible()) {
        adjustAxisRanges();
        m_isDataDirty = true;
    }
    emitNeedRender();
}

void Bars3DController::handleItemChanged(int rowIndex, int columnIndex)
{
    Q_UNUSED(rowIndex)
    Q_UNUSED(columnIndex)
    QBar3DSeries *series = static_cast<QBarDataProxy *>(sender())->series();
    if (series->isVisible()) {
        adjustAxisRanges();
        m_isDataDirty = true;
    }
    emitNeedRender();
}

void Bars3DController::handleDataRowLabelsChanged()
{
    QBar3DSeries *firstSeries = 0;
    if (m_seriesList.size())
        firstSeries = static_cast<QBar3DSeries *>(m_seriesList.at(0));
    if (m_axisZ && firstSeries && firstSeries->dataProxy()) {
        // Grab a sublist equal to data window (no need to have more labels in axis)
        int min = int(m_axisZ->min());
        int count = int(m_axisZ->max()) - min + 1;
        QStringList subList = firstSeries->dataProxy()->rowLabels().mid(min, count);
        static_cast<QCategory3DAxis *>(m_axisZ)->dptr()->setDataLabels(subList);
    }
}

void Bars3DController::handleDataColumnLabelsChanged()
{
    QBar3DSeries *firstSeries = 0;
    if (m_seriesList.size())
        firstSeries = static_cast<QBar3DSeries *>(m_seriesList.at(0));
    if (m_axisX && firstSeries && firstSeries->dataProxy()) {
        // Grab a sublist equal to data window (no need to have more labels in axis)
        int min = int(m_axisX->min());
        int count = int(m_axisX->max()) - min + 1;
        QStringList subList = static_cast<QBarDataProxy *>(firstSeries->dataProxy())
                ->columnLabels().mid(min, count);
        static_cast<QCategory3DAxis *>(m_axisX)->dptr()->setDataLabels(subList);
    }
}

void Bars3DController::handleBarClicked(const QPoint &position, QBar3DSeries *series)
{
    setSelectedBar(position, series);

    // TODO: pass clicked to parent. (QTRD-2517)
    // TODO: Also hover needed? (QTRD-2131)
}

void Bars3DController::handleAxisAutoAdjustRangeChangedInOrientation(
        QAbstract3DAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)
    adjustAxisRanges();
}

void Bars3DController::handleSeriesVisibilityChangedBySender(QObject *sender)
{
    Abstract3DController::handleSeriesVisibilityChangedBySender(sender);

    adjustAxisRanges();

    // Visibility changes may require disabling/enabling slicing,
    // so just reset selection to ensure everything is still valid.
    setSelectedBar(m_selectedBar, m_selectedBarSeries);
}

QPoint Bars3DController::invalidSelectionPosition()
{
    static QPoint invalidSelectionPos(-1, -1);
    return invalidSelectionPos;
}

void Bars3DController::setAxisX(QAbstract3DAxis *axis)
{
    Abstract3DController::setAxisX(axis);
    handleDataColumnLabelsChanged();
}

void Bars3DController::setAxisZ(QAbstract3DAxis *axis)
{
    Abstract3DController::setAxisZ(axis);
    handleDataRowLabelsChanged();
}

void Bars3DController::addSeries(QAbstract3DSeries *series)
{
    Q_ASSERT(series && series->type() == QAbstract3DSeries::SeriesTypeBar);

    bool firstAdded = !m_seriesList.size();

    Abstract3DController::addSeries(series);

    if (series->isVisible())
        adjustAxisRanges();

    if (firstAdded) {
        handleDataRowLabelsChanged();
        handleDataColumnLabelsChanged();
    }

    QBar3DSeries *barSeries =  static_cast<QBar3DSeries *>(series);
    if (barSeries->selectedBar() != invalidSelectionPosition())
        setSelectedBar(barSeries->selectedBar(), barSeries);
}

void Bars3DController::removeSeries(QAbstract3DSeries *series)
{
    bool firstRemoved = (m_seriesList.size() && m_seriesList.at(0) == series);

    bool wasVisible = (series && series->d_ptr->m_controller == this && series->isVisible());

    Abstract3DController::removeSeries(series);

    if (m_selectedBarSeries == series)
        setSelectedBar(invalidSelectionPosition(), 0);

    if (wasVisible)
        adjustAxisRanges();

    if (firstRemoved) {
        handleDataRowLabelsChanged();
        handleDataColumnLabelsChanged();
    }
}

QList<QBar3DSeries *> Bars3DController::barSeriesList()
{
    QList<QAbstract3DSeries *> abstractSeriesList = seriesList();
    QList<QBar3DSeries *> barSeriesList;
    foreach (QAbstract3DSeries *abstractSeries, abstractSeriesList) {
        QBar3DSeries *barSeries = qobject_cast<QBar3DSeries *>(abstractSeries);
        if (barSeries)
            barSeriesList.append(barSeries);
    }

    return barSeriesList;
}

void Bars3DController::handleAxisRangeChangedBySender(QObject *sender)
{
    // Data window changed
    if (sender == m_axisX || sender == m_axisZ) {
        if (sender == m_axisX)
            handleDataColumnLabelsChanged();
        if (sender == m_axisZ)
            handleDataRowLabelsChanged();
    }

    Abstract3DController::handleAxisRangeChangedBySender(sender);

    // Update selected bar - may be moved offscreen
    setSelectedBar(m_selectedBar, m_selectedBarSeries);
}

void Bars3DController::setBarSpecs(GLfloat thicknessRatio, const QSizeF &spacing, bool relative)
{
    m_barThicknessRatio = thicknessRatio;
    m_barSpacing        = spacing;
    m_isBarSpecRelative = relative;

    m_changeTracker.barSpecsChanged = true;
    emitNeedRender();
}

GLfloat Bars3DController::barThickness()
{
    return m_barThicknessRatio;
}

QSizeF Bars3DController::barSpacing()
{
    return m_barSpacing;
}

bool Bars3DController::isBarSpecRelative()
{
    return m_isBarSpecRelative;
}

void Bars3DController::setSelectionMode(QAbstract3DGraph::SelectionFlags mode)
{
    if (mode.testFlag(QAbstract3DGraph::SelectionSlice)
            && (mode.testFlag(QAbstract3DGraph::SelectionRow) == mode.testFlag(QAbstract3DGraph::SelectionColumn))) {
        qWarning("Must specify one of either row or column selection mode in conjunction with slicing mode.");
    } else {
        QAbstract3DGraph::SelectionFlags oldMode = selectionMode();

        Abstract3DController::setSelectionMode(mode);

        if (mode != oldMode) {
            // Refresh selection upon mode change to ensure slicing is correctly updated
            // according to series the visibility.
            setSelectedBar(m_selectedBar, m_selectedBarSeries);

            // Special case: Always deactivate slicing when changing away from slice
            // automanagement, as this can't be handled in setSelectedBar.
            if (!mode.testFlag(QAbstract3DGraph::SelectionSlice)
                    && oldMode.testFlag(QAbstract3DGraph::SelectionSlice)) {
                scene()->setSlicingActive(false);
            }
        }
    }
}

void Bars3DController::setSelectedBar(const QPoint &position, QBar3DSeries *series)
{
    // If the selection targets non-existent bar, clear selection instead.
    QPoint pos = position;

    // Series may already have been removed, so check it before setting the selection.
    if (!m_seriesList.contains(series))
        series = 0;

    adjustSelectionPosition(pos, series);

    if (selectionMode().testFlag(QAbstract3DGraph::SelectionSlice)) {
        // If the selected bar is outside data window, or there is no visible selected bar, disable slicing
        if (pos.x() < m_axisZ->min() || pos.x() > m_axisZ->max()
                || pos.y() < m_axisX->min() || pos.y() > m_axisX->max()
                || !series->isVisible()) {
            scene()->setSlicingActive(false);
        } else {
            scene()->setSlicingActive(true);
        }
        emitNeedRender();
    }

    if (pos != m_selectedBar || series != m_selectedBarSeries) {
        m_selectedBar = pos;
        m_selectedBarSeries = series;
        m_changeTracker.selectedBarChanged = true;

        // Clear selection from other series and finally set new selection to the specified series
        foreach (QAbstract3DSeries *otherSeries, m_seriesList) {
            QBar3DSeries *barSeries = static_cast<QBar3DSeries *>(otherSeries);
            if (barSeries != m_selectedBarSeries)
                barSeries->dptr()->setSelectedBar(invalidSelectionPosition());
        }
        if (m_selectedBarSeries)
            m_selectedBarSeries->dptr()->setSelectedBar(m_selectedBar);

        emitNeedRender();
    }
}

void Bars3DController::adjustAxisRanges()
{
    QCategory3DAxis *categoryAxisZ = static_cast<QCategory3DAxis *>(m_axisZ);
    QCategory3DAxis *categoryAxisX = static_cast<QCategory3DAxis *>(m_axisX);
    QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(m_axisY);

    bool adjustZ = (categoryAxisZ && categoryAxisZ->isAutoAdjustRange());
    bool adjustX = (categoryAxisX && categoryAxisX->isAutoAdjustRange());
    bool adjustY = (valueAxis && categoryAxisX && categoryAxisZ && valueAxis->isAutoAdjustRange());

    if (adjustZ || adjustX || adjustY) {
        int maxRowCount = 1;
        int maxColumnCount = 1;
        float minValue = 0.0f;
        float maxValue = 0.0f;

        // First figure out row and column counts
        int seriesCount = m_seriesList.size();
        if (adjustZ || adjustX) {
            for (int series = 0; series < seriesCount; series++) {
                const QBar3DSeries *barSeries = static_cast<QBar3DSeries *>(m_seriesList.at(series));
                if (barSeries->isVisible()) {
                    const QBarDataProxy *proxy = barSeries->dataProxy();

                    if (adjustZ && proxy) {
                        int rowCount = proxy->rowCount();
                        if (rowCount)
                            rowCount--;

                        maxRowCount = qMax(maxRowCount, rowCount);
                    }

                    if (adjustX && proxy) {
                        const QBarDataArray *array = proxy->array();
                        int columnCount = 0;
                        for (int i = 0; i < array->size(); i++) {
                            if (columnCount < array->at(i)->size())
                                columnCount = array->at(i)->size();
                        }
                        if (columnCount)
                            columnCount--;

                        maxColumnCount = qMax(maxColumnCount, columnCount);
                    }
                }
            }
            // Call private implementations of setRange to avoid unsetting auto adjust flag
            if (adjustZ)
                categoryAxisZ->dptr()->setRange(0.0f, float(maxRowCount));
            if (adjustX)
                categoryAxisX->dptr()->setRange(0.0f, float(maxColumnCount));
        }

        // Now that we know the row and column ranges, figure out the value axis range
        if (adjustY) {
            for (int series = 0; series < seriesCount; series++) {
                const QBar3DSeries *barSeries = static_cast<QBar3DSeries *>(m_seriesList.at(series));
                if (barSeries->isVisible()) {
                    const QBarDataProxy *proxy = barSeries->dataProxy();
                    if (adjustY && proxy) {
                        QPair<GLfloat, GLfloat> limits = proxy->dptrc()->limitValues(categoryAxisZ->min(),
                                                                                     categoryAxisZ->max(),
                                                                                     categoryAxisX->min(),
                                                                                     categoryAxisX->max());
                        if (!series) {
                            // First series initializes the values
                            minValue = limits.first;
                            maxValue = limits.second;
                        } else {
                            minValue = qMin(minValue, limits.first);
                            maxValue = qMax(maxValue, limits.second);
                        }
                    }
                }
            }

            if (maxValue < 0.0f)
                maxValue = 0.0f;
            if (minValue > 0.0f)
                minValue = 0.0f;
            if (minValue == 0.0f && maxValue == 0.0f) {
                // Only zero value values in data set, set range to something.
                minValue = 0.0f;
                maxValue = 1.0f;
            }
            valueAxis->dptr()->setRange(minValue, maxValue);
        }
    }
}

// Invalidate selection position if outside data for the series
void Bars3DController::adjustSelectionPosition(QPoint &pos, const QBar3DSeries *series)
{
    const QBarDataProxy *proxy = 0;
    if (series)
        proxy = series->dataProxy();

    if (!proxy)
        pos = invalidSelectionPosition();

    if (pos != invalidSelectionPosition()) {
        int maxRow = proxy->rowCount() - 1;
        int maxCol = (pos.x() <= maxRow && pos.x() >= 0 && proxy->rowAt(pos.x()))
                ? proxy->rowAt(pos.x())->size() - 1 : -1;

        if (pos.x() < 0 || pos.x() > maxRow || pos.y() < 0 || pos.y() > maxCol)
            pos = invalidSelectionPosition();
    }
}

QAbstract3DAxis *Bars3DController::createDefaultAxis(QAbstract3DAxis::AxisOrientation orientation)
{
    QAbstract3DAxis *defaultAxis = 0;

    if (orientation == QAbstract3DAxis::AxisOrientationY)
        defaultAxis = createDefaultValueAxis();
    else
        defaultAxis = createDefaultCategoryAxis();

    return defaultAxis;
}

}
