/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
#include "q3dabstractaxis_p.h"
#include "q3dvalueaxis_p.h"
#include "q3dcategoryaxis.h"
#include "qbardataproxy_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <qmath.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

Bars3DController::Bars3DController(QRect boundRect)
    : Abstract3DController(boundRect),
      m_rowCount(10),
      m_columnCount(10),
      m_selectedBarPos(noSelectionPoint()),
      m_isBarSpecRelative(true),
      m_barThicknessRatio(1.0f),
      m_barSpacing(QSizeF(1.0, 1.0)),
      m_renderer(0)
{
    // Default bar type; specific to bars
    setBarType(QDataVis::BevelBars, false);

    setActiveDataProxy(0);

    // Setting a null axis creates a new default axis according to orientation and chart type.
    // Note: These cannot be set in Abstract3DController constructor, as they will call virtual
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

    QObject::connect(m_renderer, &Bars3DRenderer::selectedBarPosChanged, this,
                     &Bars3DController::handleSelectedBarPosChanged, Qt::QueuedConnection);
    emitNeedRender();
}

void Bars3DController::synchDataToRenderer()
{
    Abstract3DController::synchDataToRenderer();

    if (!isInitialized())
        return;

    // Notify changes to renderer
    if (m_changeTracker.sampleSpaceChanged) {
        m_renderer->updateSampleSpace(m_rowCount, m_columnCount);
        m_changeTracker.sampleSpaceChanged = false;
    }

    if (m_changeTracker.barSpecsChanged) {
        m_renderer->updateBarSpecs(m_barThicknessRatio, m_barSpacing, m_isBarSpecRelative);
        m_changeTracker.barSpecsChanged = false;
    }

    if (m_changeTracker.selectedBarPosChanged) {
        m_renderer->updateSelectedBarPos(m_selectedBarPos);
        m_changeTracker.selectedBarPosChanged = false;
    }

    if (m_isDataDirty) {
        m_renderer->updateDataModel(static_cast<QBarDataProxy *>(m_data));
        m_isDataDirty = false;
    }
}

void Bars3DController::setActiveDataProxy(QAbstractDataProxy *proxy)
{
    // Setting null proxy indicates default proxy
    if (!proxy) {
        proxy = new QBarDataProxy;
        proxy->d_ptr->setDefaultProxy(true);
    }

    Q_ASSERT(proxy->type() == QAbstractDataProxy::DataTypeBar);

    Abstract3DController::setActiveDataProxy(proxy);

    QBarDataProxy *barDataProxy = static_cast<QBarDataProxy *>(m_data);

    QObject::connect(barDataProxy, &QBarDataProxy::arrayReset, this,
                     &Bars3DController::handleArrayReset);
    QObject::connect(barDataProxy, &QBarDataProxy::rowsAdded, this,
                     &Bars3DController::handleRowsAdded);
    QObject::connect(barDataProxy, &QBarDataProxy::rowsChanged, this,
                     &Bars3DController::handleRowsChanged);
    QObject::connect(barDataProxy, &QBarDataProxy::rowsRemoved, this,
                     &Bars3DController::handleRowsRemoved);
    QObject::connect(barDataProxy, &QBarDataProxy::rowsInserted, this,
                     &Bars3DController::handleRowsInserted);
    QObject::connect(barDataProxy, &QBarDataProxy::itemChanged, this,
                     &Bars3DController::handleItemChanged);

    adjustValueAxisRange();

    // Always clear selection on proxy change
    setSelectedBarPos(noSelectionPoint());
}

void Bars3DController::handleArrayReset()
{
    scene()->setSlicingActivated(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
    // Clear selection unless still valid
    setSelectedBarPos(m_selectedBarPos);
    emitNeedRender();
}

void Bars3DController::handleRowsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    scene()->setSlicingActivated(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
    emitNeedRender();
}

void Bars3DController::handleRowsChanged(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    scene()->setSlicingActivated(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
    emitNeedRender();
}

void Bars3DController::handleRowsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    scene()->setSlicingActivated(false);
    adjustValueAxisRange();
    m_isDataDirty = true;

    // Clear selection unless still valid
    setSelectedBarPos(m_selectedBarPos);

    emitNeedRender();
}

void Bars3DController::handleRowsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    scene()->setSlicingActivated(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
    emitNeedRender();
}

void Bars3DController::handleItemChanged(int rowIndex, int columnIndex)
{
    Q_UNUSED(rowIndex)
    Q_UNUSED(columnIndex)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    scene()->setSlicingActivated(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
    emitNeedRender();
}

void Bars3DController::handleSelectedBarPosChanged(const QPoint &position)
{
    QPoint pos = position;
    if (pos == QPoint(255, 255))
        pos = noSelectionPoint();
    if (pos != m_selectedBarPos) {
        m_selectedBarPos = pos;
        emit selectedBarPosChanged(pos);
        emitNeedRender();
    }
}

void Bars3DController::handleAxisAutoAdjustRangeChangedInOrientation(
        Q3DAbstractAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)
    adjustValueAxisRange();
}

QPoint Bars3DController::noSelectionPoint()
{
    static QPoint noSelectionPos(-1, -1);
    return noSelectionPos;
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

void Bars3DController::setBarType(QDataVis::MeshStyle style, bool smooth)
{
    QString objFile;
    if (style == QDataVis::Bars)
        objFile = QStringLiteral(":/defaultMeshes/bar");
    else if (style == QDataVis::Pyramids)
        objFile = QStringLiteral(":/defaultMeshes/pyramid");
    else if (style == QDataVis::Cones)
        objFile = QStringLiteral(":/defaultMeshes/cone");
    else if (style == QDataVis::Cylinders)
        objFile = QStringLiteral(":/defaultMeshes/cylinder");
    else if (style == QDataVis::BevelBars)
        objFile = QStringLiteral(":/defaultMeshes/bevelbar");

    if (smooth)
        objFile += QStringLiteral("Smooth");

    Abstract3DController::setMeshFileName(objFile);
}

// TODO: This sets data window. Needs more parameters, now assumes window always starts at 0,0.
void Bars3DController::setDataWindow(int rowCount, int columnCount)
{
    // Disable zoom mode if we're in it (causes crash if not, as zoom selection is deleted)
    scene()->setSlicingActivated(false);

    m_rowCount = rowCount;
    m_columnCount = columnCount;

    adjustValueAxisRange();

    // Clear selection unless still valid
    setSelectedBarPos(m_selectedBarPos);

    m_changeTracker.sampleSpaceChanged = true;
    m_isDataDirty = true; // Render item array is recreated in renderer
    emitNeedRender();
}

void Bars3DController::setSelectionMode(QDataVis::SelectionMode mode)
{
    // Disable zoom if selection mode changes
    scene()->setSlicingActivated(false);
    Abstract3DController::setSelectionMode(mode);
}

void Bars3DController::setSelectedBarPos(const QPoint &position)
{
    // If the selection is outside data window or targets non-existent
    // bar, clear selection instead.
    // TODO this will break once data window offset is implemented
    QPoint pos = position;
    if (pos.x() < 0 || pos.y() < 0
            || pos.x() >= static_cast<QBarDataProxy *>(m_data)->rowCount()
            || pos.y() >= static_cast<QBarDataProxy *>(m_data)->rowAt(pos.x())->size()
            || pos.x() >= m_rowCount || pos.y() >= m_columnCount) {
        pos = noSelectionPoint();
    }

    if (pos != m_selectedBarPos) {
        m_selectedBarPos = pos;
        m_changeTracker.selectedBarPosChanged = true;
        emit selectedBarPosChanged(pos);
        emitNeedRender();
    }
}

QPoint Bars3DController::selectedBarPos() const
{
    return m_selectedBarPos;
}

int Bars3DController::columnCount()
{
    return m_columnCount;
}

int Bars3DController::rowCount()
{
    return m_rowCount;
}

void Bars3DController::adjustValueAxisRange()
{
    Q3DValueAxis *valueAxis = static_cast<Q3DValueAxis *>(m_axisY);
    if (valueAxis && valueAxis->isAutoAdjustRange() && m_data) {
        QPair<GLfloat, GLfloat> limits =
                static_cast<QBarDataProxy *>(m_data)->dptr()->limitValues(0, m_rowCount,
                                                                          0, m_columnCount);
        if (limits.first < 0) {
            // TODO: Currently we only support symmetric y-axis for bar chart if there are negative values
            qreal maxAbs = qMax(qFabs(limits.first), qFabs(limits.second));
            // Call private implementation to avoid unsetting auto adjust flag
            valueAxis->dptr()->setRange(-maxAbs, maxAbs);
        } else if (limits.second == 0.0) {
            valueAxis->dptr()->setRange(0.0, 1.0); // Only zero value values in data set, set range to something.
        } else {
            valueAxis->dptr()->setRange(0.0, limits.second);
        }
    }
}

Q3DAbstractAxis *Bars3DController::createDefaultAxis(Q3DAbstractAxis::AxisOrientation orientation)
{
    Q3DAbstractAxis *defaultAxis = 0;

    if (orientation == Q3DAbstractAxis::AxisOrientationY)
        defaultAxis = createDefaultValueAxis();
    else
        defaultAxis = createDefaultCategoryAxis();

    return defaultAxis;
}

QT_DATAVIS3D_END_NAMESPACE
