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
      m_smoothSurface(false),
      m_surfaceGrid(true)
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
    if (m_isDataDirty) {
        m_renderer->updateDataModel(static_cast<QSurfaceDataProxy *>(m_data));
        m_isDataDirty = false;
    }
}

void Surface3DController::handleAxisAutoAdjustRangeChangedInOrientation(Q3DAbstractAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)

    // TODO: Implement!
}

void Surface3DController::setSmoothSurface(bool enable)
{
    m_smoothSurface = enable;
    emit smoothStatusChanged(m_smoothSurface);
    emitNeedRender();
}

bool Surface3DController::smoothSurface()
{
    return m_smoothSurface;
}

void Surface3DController::setSurfaceGrid(bool enable)
{
    m_surfaceGrid = enable;
    emit surfaceGridChanged(m_surfaceGrid);
    emitNeedRender();
}

bool Surface3DController::surfaceGrid()
{
    return m_surfaceGrid;
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

    // TODO connections and handler for proxy changes
    QObject::connect(surfaceDataProxy, &QSurfaceDataProxy::arrayReset,
                     this, &Surface3DController::handleArrayReset);
}

void Surface3DController::handleArrayReset()
{
    m_isDataDirty = true;
    emitNeedRender();
}

void Surface3DController::setSegmentCount(GLint segmentCount, GLfloat step, GLfloat minimum)
{
    m_segmentCount   = segmentCount;
    m_segmentStep    = step;
    m_segmentMinimum = minimum;

    emit segmentCountChanged(m_segmentCount, m_segmentStep, m_segmentMinimum);
    emitNeedRender();
}

void Surface3DController::setGradientColorAt(qreal pos, const QColor &color)
{
    Theme t = theme();
    t.m_surfaceGradient.setColorAt(pos, color);
    emitNeedRender();
}

void Surface3DController::handleSelectionAtPoint(const QPoint &point)
{
    emit leftMousePressed(point);
}

QT_DATAVISUALIZATION_END_NAMESPACE
