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
#include <QMouseEvent>

#include <QDebug>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Surface3DController::Surface3DController(QRect rect)
    : Abstract3DController(rect),
      m_renderer(0),
      m_isSmoothSurfaceEnabled(false),
      m_isSurfaceGridEnabled(true)
{
    setActiveDataProxy(0);

    // Setting a null axis creates a new default axis according to orientation and chart type.
    // Note: These cannot be set in Abstract3DController constructor, as they will call virtual
    //       functions implemented by subclasses.
    setAxisX(0);
    setAxisY(0);
    setAxisZ(0);
    QObject::connect(m_activeInputHandler, &QAbstract3DInputHandler::selectionAtPoint,
                     this, &Surface3DController::handleSelectionAtPoint);

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
        m_isSmoothSurfaceEnabled = m_renderer->updateSmoothStatus(m_isSmoothSurfaceEnabled);
        m_changeTracker.smoothStatusChanged = false;
    }

    if (m_changeTracker.surfaceGridChanged) {
        m_renderer->updateSurfaceGridStatus(m_isSurfaceGridEnabled);
        m_changeTracker.surfaceGridChanged = false;
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

void Surface3DController::setSmoothSurface(bool enable)
{
    m_isSmoothSurfaceEnabled = enable;
    m_changeTracker.smoothStatusChanged = true;
    emitNeedRender();
}

bool Surface3DController::smoothSurface()
{
    return m_isSmoothSurfaceEnabled;
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
    m_userDefinedGradient.setStart(1, 1000);
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
}

void Surface3DController::handleArrayReset()
{
    adjustValueAxisRange();
    m_isDataDirty = true;
    emitNeedRender();
}


void Surface3DController::handleSelectionAtPoint(const QPoint &point)
{
    emit leftMousePressed(point);
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
