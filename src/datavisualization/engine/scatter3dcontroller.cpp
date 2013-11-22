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
    // Default object type; specific to scatter
    setObjectType(QDataVis::MeshStyleSpheres, false);

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

void Scatter3DController::setObjectType(QDataVis::MeshStyle style, bool smooth)
{
    QString objFile;
    if (style == QDataVis::MeshStyleSpheres) {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/sphereSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/sphere");
    } else if (style == QDataVis::MeshStyleDots) {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/dotSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/dot");
    } else {
        objFile = QString();
#if defined(QT_OPENGL_ES_2)
        qWarning("MeshStylePoints is not fully supported on OpenGL ES2");
#endif
    }
    Abstract3DController::setMeshFileName(objFile);
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
    QVector3D minLimits;
    QVector3D maxLimits;
    if (m_seriesList.size()) {
        const QScatterDataProxy *proxy = static_cast<QScatter3DSeries *>(m_seriesList.at(0))->dataProxy();
        proxy->dptrc()->limitValues(minLimits, maxLimits);
        Q3DValueAxis *valueAxis = static_cast<Q3DValueAxis *>(m_axisX);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (minLimits.x() != maxLimits.x())
                valueAxis->dptr()->setRange(minLimits.x(), maxLimits.x());
            else
                valueAxis->dptr()->setRange(minLimits.x() - 1.0f, minLimits.x() + 1.0f); // Default to a valid range
        }

        valueAxis = static_cast<Q3DValueAxis *>(m_axisY);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (minLimits.y() != maxLimits.y())
                valueAxis->dptr()->setRange(minLimits.y(), maxLimits.y());
            else
                valueAxis->dptr()->setRange(minLimits.y() - 1.0f, minLimits.y() + 1.0f); // Default to a valid range
        }

        valueAxis = static_cast<Q3DValueAxis *>(m_axisZ);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (minLimits.z() != maxLimits.z())
                valueAxis->dptr()->setRange(minLimits.z(), maxLimits.z());
            else
                valueAxis->dptr()->setRange(minLimits.z() - 1.0f, minLimits.z() + 1.0f); // Default to a valid range
        }
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
