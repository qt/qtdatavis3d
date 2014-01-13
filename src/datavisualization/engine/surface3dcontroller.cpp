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

#include "surface3dcontroller_p.h"
#include "surface3drenderer_p.h"
#include "camerahelper_p.h"
#include "qabstract3daxis_p.h"
#include "qvalue3daxis_p.h"
#include "qcategory3daxis.h"
#include "qsurfacedataproxy_p.h"
#include "qsurface3dseries_p.h"
#include "shaderhelper_p.h"

#include <QMatrix4x4>

#include <QDebug>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Surface3DController::Surface3DController(QRect rect, Q3DScene *scene)
    : Abstract3DController(rect, scene),
      m_renderer(0),
      m_selectedPoint(invalidSelectionPosition()),
      m_selectedSeries(0),
      m_flatShadingSupported(true)
{
    // Setting a null axis creates a new default axis according to orientation and graph type.
    // Note: these cannot be set in the Abstract3DController constructor, as they will call virtual
    //       functions implemented by subclasses.
    setAxisX(0);
    setAxisY(0);
    setAxisZ(0);
}

Surface3DController::~Surface3DController()
{
}

void Surface3DController::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (isInitialized())
        return;

    m_renderer = new Surface3DRenderer(this);
    setRenderer(m_renderer);
    synchDataToRenderer();
    QObject::connect(m_renderer, &Surface3DRenderer::pointClicked, this,
                     &Surface3DController::handlePointClicked, Qt::QueuedConnection);
    emitNeedRender();
}

void Surface3DController::synchDataToRenderer()
{
    if (!isInitialized())
        return;

    Abstract3DController::synchDataToRenderer();

    // Notify changes to renderer
    if (m_changeTracker.rowsChanged) {
        m_renderer->updateRows(m_changedRows);
        m_changeTracker.rowsChanged = false;
        m_changedRows.clear();
    }

    if (m_changeTracker.itemChanged) {
        m_renderer->updateItem(m_changedItems);
        m_changeTracker.itemChanged = false;
        m_changedItems.clear();
    }

    if (m_changeTracker.selectedPointChanged) {
        m_renderer->updateSelectedPoint(m_selectedPoint, m_selectedSeries);
        m_changeTracker.selectedPointChanged = false;
    }
}

void Surface3DController::handleAxisAutoAdjustRangeChangedInOrientation(QAbstract3DAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)

    adjustValueAxisRange();
}

void Surface3DController::handleAxisRangeChangedBySender(QObject *sender)
{
    Abstract3DController::handleAxisRangeChangedBySender(sender);

    // Update selected point - may be moved offscreen
    setSelectedPoint(m_selectedPoint, m_selectedSeries);
}

void Surface3DController::handleSeriesVisibilityChangedBySender(QObject *sender)
{
    Abstract3DController::handleSeriesVisibilityChangedBySender(sender);

    adjustValueAxisRange();

    // Visibility changes may require disabling/enabling slicing,
    // so just reset selection to ensure everything is still valid.
    setSelectedPoint(m_selectedPoint, m_selectedSeries);
}

QPoint Surface3DController::invalidSelectionPosition()
{
    static QPoint invalidSelectionPoint(-1, -1);
    return invalidSelectionPoint;
}

bool Surface3DController::isFlatShadingSupported()
{
    return m_flatShadingSupported;
}

void Surface3DController::addSeries(QAbstract3DSeries *series)
{
    Q_ASSERT(series && series->type() == QAbstract3DSeries::SeriesTypeSurface);

    if (!m_seriesList.size()) {
        Abstract3DController::addSeries(series);

        if (series->isVisible())
            adjustValueAxisRange();
    } else {
        qWarning("Surface graph only supports a single series.");
    }

    QSurface3DSeries *surfaceSeries = static_cast<QSurface3DSeries *>(series);
    if (surfaceSeries->selectedPoint() != invalidSelectionPosition())
        setSelectedPoint(surfaceSeries->selectedPoint(), surfaceSeries);
}

void Surface3DController::removeSeries(QAbstract3DSeries *series)
{
    bool wasVisible = (series && series->d_ptr->m_controller == this && series->isVisible());

    Abstract3DController::removeSeries(series);

    if (m_selectedSeries == series)
        setSelectedPoint(invalidSelectionPosition(), 0);

    if (wasVisible)
        adjustValueAxisRange();
}

QList<QSurface3DSeries *> Surface3DController::surfaceSeriesList()
{
    QList<QAbstract3DSeries *> abstractSeriesList = seriesList();
    QList<QSurface3DSeries *> surfaceSeriesList;
    foreach (QAbstract3DSeries *abstractSeries, abstractSeriesList) {
        QSurface3DSeries *surfaceSeries = qobject_cast<QSurface3DSeries *>(abstractSeries);
        if (surfaceSeries)
            surfaceSeriesList.append(surfaceSeries);
    }

    return surfaceSeriesList;
}

void Surface3DController::setSelectionMode(QDataVis::SelectionFlags mode)
{
    // Currently surface only supports row and column modes when also slicing
    if ((mode.testFlag(QDataVis::SelectionRow) || mode.testFlag(QDataVis::SelectionColumn))
            && !mode.testFlag(QDataVis::SelectionSlice)) {
        qWarning("Unsupported selection mode.");
        return;
    } else if (mode.testFlag(QDataVis::SelectionSlice)
               && (mode.testFlag(QDataVis::SelectionRow) == mode.testFlag(QDataVis::SelectionColumn))) {
        qWarning("Must specify one of either row or column selection mode in conjunction with slicing mode.");
    } else {
        QDataVis::SelectionFlags oldMode = selectionMode();

        Abstract3DController::setSelectionMode(mode);

        if (mode != oldMode) {
            // Refresh selection upon mode change to ensure slicing is correctly updated
            // according to series the visibility.
            setSelectedPoint(m_selectedPoint, m_selectedSeries);

            // Special case: Always deactivate slicing when changing away from slice
            // automanagement, as this can't be handled in setSelectedBar.
            if (!mode.testFlag(QDataVis::SelectionSlice)
                    && oldMode.testFlag(QDataVis::SelectionSlice)) {
                scene()->setSlicingActive(false);
            }
        }
    }
}

void Surface3DController::setSelectedPoint(const QPoint &position, QSurface3DSeries *series)
{
    // If the selection targets non-existent point, clear selection instead.
    QPoint pos = position;

    // Series may already have been removed, so check it before setting the selection.
    if (!m_seriesList.contains(series))
        series = 0;

    const QSurfaceDataProxy *proxy = 0;
    if (series)
        proxy = series->dataProxy();

    if (!proxy)
        pos = invalidSelectionPosition();

    if (pos != invalidSelectionPosition()) {
        int maxRow = proxy->rowCount() - 1;
        int maxCol = proxy->columnCount() - 1;

        if (pos.x() < 0 || pos.x() > maxRow || pos.y() < 0 || pos.y() > maxCol)
            pos = invalidSelectionPosition();
    }

    if (selectionMode().testFlag(QDataVis::SelectionSlice)) {
        if (pos == invalidSelectionPosition() || !series->isVisible()) {
            scene()->setSlicingActive(false);
        } else {
            // If the selected point is outside data window, or there is no selected point, disable slicing
            // TODO: (QTRD-2351) This logic doesn't match the renderer logic for non straight surfaces,
            // but that logic needs to change anyway, so this is good for now.
            float axisMinX = m_axisX->min();
            float axisMaxX = m_axisX->max();
            float axisMinZ = m_axisZ->min();
            float axisMaxZ = m_axisZ->max();

            QSurfaceDataItem item = proxy->array()->at(pos.x())->at(pos.y());
            if (item.x() < axisMinX || item.x() > axisMaxX
                    || item.z() < axisMinZ || item.z() > axisMaxZ) {
                scene()->setSlicingActive(false);
            } else {
                scene()->setSlicingActive(true);
            }
        }
        emitNeedRender();
    }

    if (pos != m_selectedPoint) {
        m_selectedPoint = pos;
        m_selectedSeries = series;
        m_changeTracker.selectedPointChanged = true;

        // Clear selection from other series and finally set new selection to the specified series
        foreach (QAbstract3DSeries *otherSeries, m_seriesList) {
            QSurface3DSeries *surfaceSeries = static_cast<QSurface3DSeries *>(otherSeries);
            if (surfaceSeries != m_selectedSeries)
                surfaceSeries->dptr()->setSelectedPoint(invalidSelectionPosition());
        }
        if (m_selectedSeries)
            m_selectedSeries->dptr()->setSelectedPoint(m_selectedPoint);

        emitNeedRender();
    }
}

void Surface3DController::handleArrayReset()
{
    QSurface3DSeries *series = static_cast<QSurfaceDataProxy *>(sender())->series();
    if (series->isVisible()) {
        adjustValueAxisRange();
        m_isDataDirty = true;
    }
    // Clear selection unless still valid
    setSelectedPoint(m_selectedPoint, m_selectedSeries);
    emitNeedRender();
}

void Surface3DController::handlePointClicked(const QPoint &position, QSurface3DSeries *series)
{
    setSelectedPoint(position, series);
    // TODO: pass clicked to parent. (QTRD-2517)
    // TODO: Also hover needed? (QTRD-2131)
}

void Surface3DController::handleFlatShadingSupportedChange(bool supported)
{
    // Handle renderer flat surface support indicator signal. This happens exactly once per renderer.
    if (m_flatShadingSupported != supported) {
        m_flatShadingSupported = supported;
        // Emit the change for all added surfaces
        foreach (QAbstract3DSeries *series, m_seriesList) {
            QSurface3DSeries *surfaceSeries = static_cast<QSurface3DSeries *>(series);
            emit surfaceSeries->flatShadingSupportedChanged(m_flatShadingSupported);
        }
    }
}

void Surface3DController::handleRowsChanged(int startIndex, int count)
{
    QSurfaceDataProxy *sender = static_cast<QSurfaceDataProxy *>(QObject::sender());
    if (m_changedRows.size() == 0)
        m_changedRows.reserve(sender->rowCount());

    QSurface3DSeries *series = sender->series();
    if (series->isVisible()) {
        // Change is for the visible series, put the change to queue
        int oldChangeCount = m_changedRows.size();
        for (int i = 0; i < count; i++) {
            bool newItem = true;
            int candidate = startIndex + i;
            for (int i = 0; i < oldChangeCount; i++) {
                if (m_changedRows.at(i) == candidate) {
                    newItem = false;
                    break;
                }
            }
            if (newItem)
                m_changedRows.append(candidate);
        }
        if (m_changedRows.size()) {
            m_changeTracker.rowsChanged = true;

            adjustValueAxisRange();
            // Clear selection unless still valid
            setSelectedPoint(m_selectedPoint, m_selectedSeries);
            emitNeedRender();
        }
    }
}

void Surface3DController::handleItemChanged(int rowIndex, int columnIndex)
{
    QSurfaceDataProxy *sender = static_cast<QSurfaceDataProxy *>(QObject::sender());
    QSurface3DSeries *series = sender->series();
    if (series->isVisible()) {
        // Change is for the visible series, put the change to queue
        bool newItem = true;
        QPoint candidate(columnIndex, rowIndex);
        foreach (QPoint item, m_changedItems) {
            if (item == candidate) {
                newItem = false;
                break;
            }
        }
        if (newItem) {
            m_changedItems.append(candidate);
            m_changeTracker.itemChanged = true;

            adjustValueAxisRange();
            // Clear selection unless still valid
            setSelectedPoint(m_selectedPoint, m_selectedSeries);
            emitNeedRender();
        }
    }
}

void Surface3DController::handleRowsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    QSurface3DSeries *series = static_cast<QSurfaceDataProxy *>(sender())->series();
    if (series->isVisible()) {
        adjustValueAxisRange();
        m_isDataDirty = true;
    }
    emitNeedRender();
}

void Surface3DController::handleRowsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    QSurface3DSeries *series = static_cast<QSurfaceDataProxy *>(sender())->series();
    if (series->isVisible()) {
        adjustValueAxisRange();
        m_isDataDirty = true;
    }
    emitNeedRender();
}

void Surface3DController::handleRowsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    QSurface3DSeries *series = static_cast<QSurfaceDataProxy *>(sender())->series();
    if (series->isVisible()) {
        adjustValueAxisRange();
        m_isDataDirty = true;
    }

    // Clear selection unless still valid
    setSelectedPoint(m_selectedPoint, m_selectedSeries);

    emitNeedRender();
}

void Surface3DController::adjustValueAxisRange()
{
    QValue3DAxis *valueAxisX = static_cast<QValue3DAxis *>(m_axisX);
    QValue3DAxis *valueAxisY = static_cast<QValue3DAxis *>(m_axisY);
    QValue3DAxis *valueAxisZ = static_cast<QValue3DAxis *>(m_axisZ);
    bool adjustX = (valueAxisX && valueAxisX->isAutoAdjustRange());
    bool adjustY = (valueAxisY && valueAxisY->isAutoAdjustRange());
    bool adjustZ = (valueAxisZ && valueAxisZ->isAutoAdjustRange());

    if (adjustX || adjustY || adjustZ) {
        float minValueX = 0.0f;
        float maxValueX = 0.0f;
        float minValueY = 0.0f;
        float maxValueY = 0.0f;
        float minValueZ = 0.0f;
        float maxValueZ = 0.0f;
        int seriesCount = m_seriesList.size();
        for (int series = 0; series < seriesCount; series++) {
            const QSurface3DSeries *surfaceSeries =
                    static_cast<QSurface3DSeries *>(m_seriesList.at(series));
            const QSurfaceDataProxy *proxy = surfaceSeries->dataProxy();
            if (surfaceSeries->isVisible() && proxy) {
                QVector3D minLimits;
                QVector3D maxLimits;
                proxy->dptrc()->limitValues(minLimits, maxLimits);
                if (adjustX) {
                    if (!series) {
                        // First series initializes the values
                        minValueX = minLimits.x();
                        maxValueX = maxLimits.x();
                    } else {
                        minValueX = qMin(minValueX, minLimits.x());
                        maxValueX = qMax(maxValueX, maxLimits.x());
                    }
                }
                if (adjustY) {
                    if (!series) {
                        // First series initializes the values
                        minValueY = minLimits.y();
                        maxValueY = maxLimits.y();
                    } else {
                        minValueY = qMin(minValueY, minLimits.y());
                        maxValueY = qMax(maxValueY, maxLimits.y());
                    }
                }
                if (adjustZ) {
                    if (!series) {
                        // First series initializes the values
                        minValueZ = minLimits.z();
                        maxValueZ = maxLimits.z();
                    } else {
                        minValueZ = qMin(minValueZ, minLimits.z());
                        maxValueZ = qMax(maxValueZ, maxLimits.z());
                    }
                }
            }
        }

        static const float adjustmentRatio = 20.0f;
        static const float defaultAdjustment = 1.0f;

        if (adjustX) {
            // If all points at same coordinate, need to default to some valid range
            float adjustment = 0.0f;
            if (minValueX == maxValueX) {
                if (adjustZ) {
                    // X and Z are linked to have similar unit size, so choose the valid range based on it
                    if (minValueZ == maxValueZ)
                        adjustment = defaultAdjustment;
                    else
                        adjustment = qAbs(maxValueZ - minValueZ) / adjustmentRatio;
                } else {
                    if (valueAxisZ)
                        adjustment = qAbs(valueAxisZ->max() - valueAxisZ->min()) / adjustmentRatio;
                    else
                        adjustment = defaultAdjustment;
                }
            }
            valueAxisX->dptr()->setRange(minValueX - adjustment, maxValueX + adjustment);
        }
        if (adjustY) {
            // If all points at same coordinate, need to default to some valid range
            // Y-axis unit is not dependent on other axes, so simply adjust +-1.0f
            float adjustment = 0.0f;
            if (minValueY == maxValueY)
                adjustment = defaultAdjustment;
            valueAxisY->dptr()->setRange(minValueY - adjustment, maxValueY + adjustment);
        }
        if (adjustZ) {
            // If all points at same coordinate, need to default to some valid range
            float adjustment = 0.0f;
            if (minValueZ == maxValueZ) {
                if (adjustX) {
                    // X and Z are linked to have similar unit size, so choose the valid range based on it
                    if (minValueX == maxValueX)
                        adjustment = defaultAdjustment;
                    else
                        adjustment = qAbs(maxValueX - minValueX) / adjustmentRatio;
                } else {
                    if (valueAxisX)
                        adjustment = qAbs(valueAxisX->max() - valueAxisX->min()) / adjustmentRatio;
                    else
                        adjustment = defaultAdjustment;
                }
            }
            valueAxisZ->dptr()->setRange(minValueZ - adjustment, maxValueZ + adjustment);
        }
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
