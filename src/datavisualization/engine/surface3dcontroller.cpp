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
#include "q3dabstractaxis_p.h"
#include "q3dvalueaxis_p.h"
#include "q3dcategoryaxis.h"
#include "qsurfacedataproxy_p.h"

#include <QMatrix4x4>

#include <QDebug>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Surface3DController::Surface3DController(QRect rect)
    : Abstract3DController(rect),
      m_renderer(0),
      m_isSmoothSurfaceEnabled(false),
      m_isSurfaceEnabled(true),
      m_isSurfaceGridEnabled(true),
      m_selectedPoint(noSelectionPoint())
{
    setActiveDataProxy(0);

    // Setting a null axis creates a new default axis according to orientation and graph type.
    // Note: these cannot be set in the Abstract3DController constructor, as they will call virtual
    //       functions implemented by subclasses.
    setAxisX(0);
    setAxisY(0);
    setAxisZ(0);

    // Set the default from the theme
    m_userDefinedGradient = theme().m_surfaceGradient;
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
    QObject::connect(m_renderer, &Surface3DRenderer::requestSmoothSurface, this,
                     &Surface3DController::handleRequestSmoothSurface, Qt::QueuedConnection);
    emitNeedRender();
}

void Surface3DController::synchDataToRenderer()
{
    Abstract3DController::synchDataToRenderer();

    if (!isInitialized())
        return;

    // Notify changes to renderer
    if (m_changeTracker.gradientColorChanged) {
        m_renderer->updateSurfaceGradient(m_userDefinedGradient);
        m_changeTracker.gradientColorChanged = false;
    }

    if (m_changeTracker.smoothStatusChanged) {
        bool oldSmoothStatus = m_isSmoothSurfaceEnabled;
        m_isSmoothSurfaceEnabled = m_renderer->updateSmoothStatus(m_isSmoothSurfaceEnabled);
        m_changeTracker.smoothStatusChanged = false;
        if (oldSmoothStatus != m_isSmoothSurfaceEnabled)
            emit smoothSurfaceEnabledChanged(m_isSmoothSurfaceEnabled);
    }

    if (m_changeTracker.surfaceVisibilityChanged) {
        m_renderer->updateSurfaceVisibilityStatus(m_isSurfaceEnabled);
        m_changeTracker.surfaceVisibilityChanged = false;
    }

    if (m_changeTracker.surfaceGridChanged) {
        m_renderer->updateSurfaceGridStatus(m_isSurfaceGridEnabled);
        m_changeTracker.surfaceGridChanged = false;
    }

    if (m_changeTracker.selectedPointChanged) {
        m_renderer->updateSelectedPoint(m_selectedPoint);
        m_changeTracker.selectedPointChanged = false;
    }

    if (m_isDataDirty) {
        m_renderer->updateDataModel(static_cast<QSurfaceDataProxy *>(m_data));
        m_isDataDirty = false;
    }
}

void Surface3DController::handleAxisAutoAdjustRangeChangedInOrientation(Q3DAbstractAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)

    adjustValueAxisRange();
}

void Surface3DController::handleAxisRangeChangedBySender(QObject *sender)
{
    Abstract3DController::handleAxisRangeChangedBySender(sender);

    // Update selected point - may be moved offscreen
    setSelectedPoint(m_selectedPoint);
}

QPoint Surface3DController::noSelectionPoint()
{
    static QPoint noSelectionPoint(-1, -1);
    return noSelectionPoint;
}

void Surface3DController::setSmoothSurface(bool enable)
{
    bool changed = m_isSmoothSurfaceEnabled != enable;
    m_isSmoothSurfaceEnabled = enable;
    m_changeTracker.smoothStatusChanged = true;
    emitNeedRender();

    if (changed)
        emit smoothSurfaceEnabledChanged(m_isSmoothSurfaceEnabled);
}

bool Surface3DController::smoothSurface()
{
    return m_isSmoothSurfaceEnabled;
}

void Surface3DController::setSurfaceVisible(bool visible)
{
    m_isSurfaceEnabled = visible;
    m_changeTracker.surfaceVisibilityChanged = true;
    emitNeedRender();
}

bool Surface3DController::surfaceVisible() const
{
    return m_isSurfaceEnabled;
}

void Surface3DController::setSurfaceGrid(bool enable)
{
    m_isSurfaceGridEnabled = enable;
    m_changeTracker.surfaceGridChanged = true;
    emitNeedRender();
}

bool Surface3DController::surfaceGrid()
{
    return m_isSurfaceGridEnabled;
}

void Surface3DController::setGradient(const QLinearGradient &gradient)
{
    m_userDefinedGradient = gradient;
    m_userDefinedGradient.setStart(2, 1024);
    m_userDefinedGradient.setFinalStop(0, 0);
    m_changeTracker.gradientColorChanged = true;
    emitNeedRender();
}

QLinearGradient Surface3DController::gradient() const
{
    return m_userDefinedGradient;
}

void Surface3DController::setGradientColorAt(qreal pos, const QColor &color)
{
    m_userDefinedGradient.setColorAt(pos, color);
    m_changeTracker.gradientColorChanged = true;
    emitNeedRender();
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
        // When setting selection mode to a new slicing mode, activate slicing
        if (mode != selectionMode()) {
            bool isSlicing = mode.testFlag(QDataVis::SelectionSlice);
            if (isSlicing && m_selectedPoint != noSelectionPoint())
                scene()->setSlicingActive(true);
            else
                scene()->setSlicingActive(false);
        }

        Abstract3DController::setSelectionMode(mode);
    }
}

void Surface3DController::setSelectedPoint(const QPoint &position)
{
    // If the selection targets non-existent point, clear selection instead.
    QPoint pos = position;

    const QSurfaceDataProxy *proxy = static_cast<const QSurfaceDataProxy *>(m_data);
    if (pos != noSelectionPoint()) {
        int maxRow = proxy->rowCount() - 1;
        int maxCol = proxy->columnCount() - 1;

        if (pos.x() < 0 || pos.x() > maxRow || pos.y() < 0 || pos.y() > maxCol)
            pos = noSelectionPoint();
    }

    if (selectionMode().testFlag(QDataVis::SelectionSlice)) {
        if (pos == noSelectionPoint()) {
            scene()->setSlicingActive(false);
        } else {
            // If the selected point is outside data window, or there is no selected point, disable slicing
            // TODO: (QTRD-2351) This logic doesn't match the renderer logic for non straight surfaces,
            // but that logic needs to change anyway, so this is good for now.
            float axisMinX = float(m_axisX->min());
            float axisMaxX = float(m_axisX->max());
            float axisMinZ = float(m_axisZ->min());
            float axisMaxZ = float(m_axisZ->max());

            // Comparisons between float and double are not accurate, so fudge our comparison values
            //a little to get all rows and columns into view that need to be visible.
            const float fudgeFactor = 0.00001f;
            float fudgedAxisXRange = (axisMaxX - axisMinX) * fudgeFactor;
            float fudgedAxisZRange = (axisMaxZ - axisMinZ) * fudgeFactor;
            axisMinX -= fudgedAxisXRange;
            axisMinZ -= fudgedAxisZRange;
            axisMaxX += fudgedAxisXRange;
            axisMaxZ += fudgedAxisZRange;
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
        m_changeTracker.selectedPointChanged = true;
        emit selectedPointChanged(pos);
        emitNeedRender();
    }
}

QPoint Surface3DController::selectedPoint() const
{
    return m_selectedPoint;
}

void Surface3DController::setActiveDataProxy(QAbstractDataProxy *proxy)
{
    // Setting null proxy indicates default proxy
    if (!proxy) {
        proxy = new QSurfaceDataProxy;
        proxy->d_ptr->setDefaultProxy(true);
    }

    Q_ASSERT(proxy->type() == QAbstractDataProxy::DataTypeSurface);

    Abstract3DController::setActiveDataProxy(proxy);

    QSurfaceDataProxy *surfaceDataProxy = static_cast<QSurfaceDataProxy *>(m_data);

    QObject::connect(surfaceDataProxy, &QSurfaceDataProxy::arrayReset,
                     this, &Surface3DController::handleArrayReset);

    adjustValueAxisRange();

    // Always clear selection on proxy change
    setSelectedPoint(noSelectionPoint());

    m_isDataDirty = true;
    emitNeedRender();
}

void Surface3DController::handleArrayReset()
{
    adjustValueAxisRange();
    m_isDataDirty = true;
    // Clear selection unless still valid
    setSelectedPoint(m_selectedPoint);
    emitNeedRender();
}

void Surface3DController::handlePointClicked(const QPoint &position)
{
    setSelectedPoint(position);
    // TODO: pass clicked to parent. (QTRD-2517)
    // TODO: Also hover needed? (QTRD-2131)
}

void Surface3DController::handleRequestSmoothSurface(bool enable)
{
    setSmoothSurface(enable);
    emitNeedRender();
}

void Surface3DController::adjustValueAxisRange()
{
    if (m_data) {
        QVector3D minLimits;
        QVector3D maxLimits;
        static_cast<QSurfaceDataProxy *>(m_data)->dptr()->limitValues(minLimits, maxLimits);
        Q3DValueAxis *valueAxis = static_cast<Q3DValueAxis *>(m_axisX);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (minLimits.x() != maxLimits.x())
                valueAxis->dptr()->setRange(minLimits.x(), maxLimits.x());
            else
                valueAxis->dptr()->setRange(minLimits.x() - 1.0f, minLimits.x() + 1.0f); // Default to some valid range
        }

        valueAxis = static_cast<Q3DValueAxis *>(m_axisY);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (minLimits.y() != maxLimits.y())
                valueAxis->dptr()->setRange(minLimits.y(), maxLimits.y());
            else
                valueAxis->dptr()->setRange(minLimits.y() - 1.0f, minLimits.y() + 1.0f); // Default to some valid range
        }

        valueAxis = static_cast<Q3DValueAxis *>(m_axisZ);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (minLimits.z() != maxLimits.z())
                valueAxis->dptr()->setRange(minLimits.z(), maxLimits.z());
            else
                valueAxis->dptr()->setRange(minLimits.z() - 1.0f, minLimits.z() + 1.0f); // Default to some valid range
        }
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
