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

#include "scatter3dcontroller_p.h"
#include "scatter3drenderer_p.h"
#include "camerahelper_p.h"
#include "q3dabstractaxis_p.h"
#include "q3dvalueaxis_p.h"
#include "qscatterdataproxy_p.h"
#include "qscatter3dseries_p.h"

#include <QMatrix4x4>
#include <qmath.h>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Scatter3DController::Scatter3DController(QRect boundRect)
    : Abstract3DController(boundRect),
      m_renderer(0),
      m_selectedItem(invalidSelectionIndex()),
      m_selectedItemSeries(0)
{
    // Setting a null axis creates a new default axis according to orientation and graph type.
    // Note: These cannot be set in Abstract3DController constructor, as they will call virtual
    //       functions implemented by subclasses.
    setAxisX(0);
    setAxisY(0);
    setAxisZ(0);
}

Scatter3DController::~Scatter3DController()
{
}

void Scatter3DController::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (isInitialized())
        return;

    m_renderer = new Scatter3DRenderer(this);
    setRenderer(m_renderer);
    synchDataToRenderer();

    QObject::connect(m_renderer, &Scatter3DRenderer::itemClicked, this,
                     &Scatter3DController::handleItemClicked, Qt::QueuedConnection);
    emitNeedRender();
}

void Scatter3DController::synchDataToRenderer()
{
    Abstract3DController::synchDataToRenderer();

    if (!isInitialized())
        return;

    // Notify changes to renderer
    if (m_changeTracker.selectedItemChanged) {
        m_renderer->updateSelectedItem(m_selectedItem, m_selectedItemSeries);
        m_changeTracker.selectedItemChanged = false;
    }
}

void Scatter3DController::addSeries(QAbstract3DSeries *series)
{
    Q_ASSERT(series && series->type() == QAbstract3DSeries::SeriesTypeScatter);

    bool firstAdded = !m_seriesList.size();

    Abstract3DController::addSeries(series);

    if (firstAdded)
        adjustValueAxisRange();

    QScatter3DSeries *scatterSeries =  static_cast<QScatter3DSeries *>(series);
    if (scatterSeries->selectedItem() != invalidSelectionIndex())
        setSelectedItem(scatterSeries->selectedItem(), scatterSeries);
}

void Scatter3DController::removeSeries(QAbstract3DSeries *series)
{
    bool firstRemoved = (m_seriesList.size() && m_seriesList.at(0) == series);

    Abstract3DController::removeSeries(series);

    if (m_selectedItemSeries == series)
        setSelectedItem(invalidSelectionIndex(), 0);

    if (firstRemoved)
        adjustValueAxisRange();
}

QList<QScatter3DSeries *> Scatter3DController::scatterSeriesList()
{
    QList<QAbstract3DSeries *> abstractSeriesList = seriesList();
    QList<QScatter3DSeries *> scatterSeriesList;
    foreach (QAbstract3DSeries *abstractSeries, abstractSeriesList) {
        QScatter3DSeries *scatterSeries = qobject_cast<QScatter3DSeries *>(abstractSeries);
        if (scatterSeries)
            scatterSeriesList.append(scatterSeries);
    }

    return scatterSeriesList;
}

void Scatter3DController::handleArrayReset()
{
    adjustValueAxisRange();
    m_isDataDirty = true;
    setSelectedItem(m_selectedItem, m_selectedItemSeries);
    emitNeedRender();
}

void Scatter3DController::handleItemsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_isDataDirty = true;
    emitNeedRender();
}

void Scatter3DController::handleItemsChanged(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_isDataDirty = true;
    emitNeedRender();
}

void Scatter3DController::handleItemsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_isDataDirty = true;

    // Clear selection unless it is still valid
    setSelectedItem(m_selectedItem, m_selectedItemSeries);

    emitNeedRender();
}

void Scatter3DController::handleItemsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_isDataDirty = true;
    emitNeedRender();
}

void Scatter3DController::handleItemClicked(int index, QScatter3DSeries *series)
{
    setSelectedItem(index, series);

    // TODO: pass clicked to parent. (QTRD-2517)
    // TODO: Also hover needed? (QTRD-2131)
}

void Scatter3DController::handleAxisAutoAdjustRangeChangedInOrientation(
        Q3DAbstractAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)
    adjustValueAxisRange();
}

void Scatter3DController::handleAxisRangeChangedBySender(QObject *sender)
{
    Abstract3DController::handleAxisRangeChangedBySender(sender);

    // Update selected index - may be moved offscreen
    setSelectedItem(m_selectedItem, m_selectedItemSeries);
}

void Scatter3DController::setSelectionMode(QDataVis::SelectionFlags mode)
{
    // We only support single item selection mode and no selection mode
    if (mode != QDataVis::SelectionItem && mode != QDataVis::SelectionNone) {
        qWarning("Unsupported selection mode - only none and item selection modes are supported.");
        return;
    }

    Abstract3DController::setSelectionMode(mode);
}

void Scatter3DController::setSelectedItem(int index, QScatter3DSeries *series)
{
    const QScatterDataProxy *proxy = 0;

    // Series may already have been removed, so check it before setting the selection.
    if (!m_seriesList.contains(series))
        series = 0;

    if (series)
        proxy = series->dataProxy();

    if (!proxy || index < 0 || index >= proxy->itemCount())
        index = invalidSelectionIndex();

    if (index != m_selectedItem || series != m_selectedItemSeries) {
        m_selectedItem = index;
        m_selectedItemSeries = series;
        m_changeTracker.selectedItemChanged = true;

        // Clear selection from other series and finally set new selection to the specified series
        foreach (QAbstract3DSeries *otherSeries, m_seriesList) {
            QScatter3DSeries *scatterSeries = static_cast<QScatter3DSeries *>(otherSeries);
            if (scatterSeries != m_selectedItemSeries)
                scatterSeries->dptr()->setSelectedItem(invalidSelectionIndex());
        }
        if (m_selectedItemSeries)
            m_selectedItemSeries->dptr()->setSelectedItem(m_selectedItem);

        emitNeedRender();
    }
}

void Scatter3DController::adjustValueAxisRange()
{
    Q3DValueAxis *valueAxisX = static_cast<Q3DValueAxis *>(m_axisX);
    Q3DValueAxis *valueAxisY = static_cast<Q3DValueAxis *>(m_axisY);
    Q3DValueAxis *valueAxisZ = static_cast<Q3DValueAxis *>(m_axisZ);
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
            const QScatter3DSeries *scatterSeries =
                    static_cast<QScatter3DSeries *>(m_seriesList.at(series));
            const QScatterDataProxy *proxy = scatterSeries->dataProxy();
            if (scatterSeries->isVisible() && proxy) {
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
