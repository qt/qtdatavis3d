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

#include <QMatrix4x4>
#include <QMouseEvent>
#include <qmath.h>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Scatter3DController::Scatter3DController(QRect boundRect)
    : Abstract3DController(boundRect),
      m_renderer(0),
      m_selectedItemIndex(noSelectionIndex())
{
    // Default object type; specific to scatter
    setObjectType(QDataVis::MeshStyleSpheres, false);

    setActiveDataProxy(new QScatterDataProxy);

    // Setting a null axis creates a new default axis according to orientation and chart type.
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

    QObject::connect(m_renderer, &Scatter3DRenderer::selectedItemIndexChanged, this,
                     &Scatter3DController::handleSelectedItemIndexChanged, Qt::QueuedConnection);
    emitNeedRender();
}

void Scatter3DController::synchDataToRenderer()
{
    Abstract3DController::synchDataToRenderer();

    if (!isInitialized())
        return;

    // Notify changes to renderer
    if (m_changeTracker.slicingActiveChanged) {
        // TODO: Add notification.
        m_changeTracker.slicingActiveChanged = false;
    }

    if (m_changeTracker.selectedItemIndexChanged) {
        m_renderer->updateSelectedItemIndex(m_selectedItemIndex);
        m_changeTracker.selectedItemIndexChanged = false;
    }

    if (m_isDataDirty) {
        m_renderer->updateDataModel(static_cast<QScatterDataProxy *>(m_data));
        m_isDataDirty = false;
    }
}


void Scatter3DController::setActiveDataProxy(QAbstractDataProxy *proxy)
{
    // Setting null proxy indicates default proxy
    if (!proxy) {
        proxy = new QScatterDataProxy;
        proxy->d_ptr->setDefaultProxy(true);
    }

    Q_ASSERT(proxy->type() == QAbstractDataProxy::DataTypeScatter);

    Abstract3DController::setActiveDataProxy(proxy);

    QScatterDataProxy *scatterDataProxy = static_cast<QScatterDataProxy *>(m_data);

    QObject::connect(scatterDataProxy, &QScatterDataProxy::arrayReset,
                     this, &Scatter3DController::handleArrayReset);
    QObject::connect(scatterDataProxy, &QScatterDataProxy::itemsAdded,
                     this, &Scatter3DController::handleItemsAdded);
    QObject::connect(scatterDataProxy, &QScatterDataProxy::itemsChanged,
                     this, &Scatter3DController::handleItemsChanged);
    QObject::connect(scatterDataProxy, &QScatterDataProxy::itemsRemoved,
                     this, &Scatter3DController::handleItemsRemoved);
    QObject::connect(scatterDataProxy, &QScatterDataProxy::itemsInserted,
                     this, &Scatter3DController::handleItemsInserted);

    adjustValueAxisRange();
    setSelectedItemIndex(noSelectionIndex());
}

void Scatter3DController::handleArrayReset()
{
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
    setSelectedItemIndex(noSelectionIndex());
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
    if (startIndex >= static_cast<QScatterDataProxy *>(m_data)->itemCount())
        setSelectedItemIndex(noSelectionIndex());
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

void Scatter3DController::handleSelectedItemIndexChanged(int index)
{
    if (index != m_selectedItemIndex) {
        m_selectedItemIndex = index;
        emit selectedItemIndexChanged(index);
        emitNeedRender();
    }
}

void Scatter3DController::handleAxisAutoAdjustRangeChangedInOrientation(
        Q3DAbstractAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)
    adjustValueAxisRange();
}

void Scatter3DController::setObjectType(QDataVis::MeshStyle style, bool smooth)
{
    QString objFile;
    if (style == QDataVis::MeshStyleSpheres) {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/sphereSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/sphere");
    } else {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/dotSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/dot");
    }
    Abstract3DController::setMeshFileName(objFile);
}

void Scatter3DController::setSelectionMode(QDataVis::SelectionMode mode)
{
    if (mode > QDataVis::SelectionModeItem) {
        qWarning("Unsupported selection mode.");
        return;
    }
    // Disable zoom if selection mode changes
    setSlicingActive(false);
    Abstract3DController::setSelectionMode(mode);
}

void Scatter3DController::setSelectedItemIndex(int index)
{
    // TODO If items not within axis ranges are culled from drawing, should they be
    // TODO unselectable as well?
    if (index < 0 || index >= static_cast<QScatterDataProxy *>(m_data)->itemCount())
        index = noSelectionIndex();

    if (index != m_selectedItemIndex) {
        m_selectedItemIndex = index;
        m_changeTracker.selectedItemIndexChanged = true;
        emit selectedItemIndexChanged(index);
        emitNeedRender();
    }
}

int Scatter3DController::selectedItemIndex() const
{
    return m_selectedItemIndex;
}

void Scatter3DController::adjustValueAxisRange()
{
    if (m_data) {
        QVector3D limits = static_cast<QScatterDataProxy *>(m_data)->dptr()->limitValues();
        Q3DValueAxis *valueAxis = static_cast<Q3DValueAxis *>(m_axisX);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (limits.x() > 0)
                valueAxis->dptr()->setRange(-limits.x(), limits.x());
            else
                valueAxis->dptr()->setRange(-1.0, 1.0); // Only zero value values in data set, set range to default.
        }

        valueAxis = static_cast<Q3DValueAxis *>(m_axisY);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (limits.y() > 0)
                valueAxis->dptr()->setRange(-limits.y(), limits.y());
            else
                valueAxis->dptr()->setRange(-1.0, 1.0); // Only zero value values in data set, set range to default.
        }

        valueAxis = static_cast<Q3DValueAxis *>(m_axisZ);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (limits.z() > 0)
                valueAxis->dptr()->setRange(-limits.z(), limits.z());
            else
                valueAxis->dptr()->setRange(-1.0, 1.0); // Only zero value values in data set, set range to default.
        }
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
