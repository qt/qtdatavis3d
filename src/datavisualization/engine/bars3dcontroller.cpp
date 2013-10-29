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
#include "q3dabstractaxis_p.h"
#include "q3dvalueaxis_p.h"
#include "q3dcategoryaxis_p.h"
#include "qbardataproxy_p.h"

#include <QMatrix4x4>
#include <qmath.h>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Bars3DController::Bars3DController(QRect boundRect)
    : Abstract3DController(boundRect),
      m_selectedBar(noSelectionPoint()),
      m_isBarSpecRelative(true),
      m_barThicknessRatio(1.0f),
      m_barSpacing(QSizeF(1.0, 1.0)),
      m_renderer(0)
{
    // Default bar type; specific to bars
    setBarType(QDataVis::MeshStyleBevelBars, false);

    setActiveDataProxy(0);

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
    Abstract3DController::synchDataToRenderer();

    if (!isInitialized())
        return;

    // Notify changes to renderer
    if (m_changeTracker.barSpecsChanged) {
        m_renderer->updateBarSpecs(m_barThicknessRatio, m_barSpacing, m_isBarSpecRelative);
        m_changeTracker.barSpecsChanged = false;
    }

    if (m_changeTracker.selectedBarChanged) {
        m_renderer->updateSelectedBar(m_selectedBar);
        m_changeTracker.selectedBarChanged = false;
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
    QObject::connect(barDataProxy, &QBarDataProxy::rowLabelsChanged, this,
                     &Bars3DController::handleDataRowLabelsChanged);
    QObject::connect(barDataProxy, &QBarDataProxy::columnLabelsChanged, this,
                     &Bars3DController::handleDataColumnLabelsChanged);

    adjustAxisRanges();

    // Always clear selection on proxy change
    setSelectedBar(noSelectionPoint());

    handleDataRowLabelsChanged();
    handleDataColumnLabelsChanged();
    m_isDataDirty = true;
    emitNeedRender();
}

void Bars3DController::handleArrayReset()
{
    adjustAxisRanges();
    m_isDataDirty = true;
    // Clear selection unless still valid
    setSelectedBar(m_selectedBar);
    emitNeedRender();
}

void Bars3DController::handleRowsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    adjustAxisRanges();
    m_isDataDirty = true;
    emitNeedRender();
}

void Bars3DController::handleRowsChanged(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    adjustAxisRanges();
    m_isDataDirty = true;
    emitNeedRender();
}

void Bars3DController::handleRowsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    adjustAxisRanges();
    m_isDataDirty = true;

    // Clear selection unless still valid
    setSelectedBar(m_selectedBar);

    emitNeedRender();
}

void Bars3DController::handleRowsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    adjustAxisRanges();
    m_isDataDirty = true;
    emitNeedRender();
}

void Bars3DController::handleItemChanged(int rowIndex, int columnIndex)
{
    Q_UNUSED(rowIndex)
    Q_UNUSED(columnIndex)
    adjustAxisRanges();
    m_isDataDirty = true;
    emitNeedRender();
}

void Bars3DController::handleDataRowLabelsChanged()
{
    if (m_axisX && m_data) {
        // Grab a sublist equal to data window (no need to have more labels in axis)
        int min = int(m_axisX->min());
        int count = int(m_axisX->max()) - min + 1;
        QStringList subList = static_cast<QBarDataProxy *>(m_data)->rowLabels().mid(min, count);
        static_cast<Q3DCategoryAxis *>(m_axisX)->dptr()->setDataLabels(subList);
    }
}

void Bars3DController::handleDataColumnLabelsChanged()
{
    if (m_axisZ && m_data) {
        // Grab a sublist equal to data window (no need to have more labels in axis)
        int min = int(m_axisZ->min());
        int count = int(m_axisZ->max()) - min + 1;
        QStringList subList = static_cast<QBarDataProxy *>(m_data)->columnLabels().mid(min, count);
        static_cast<Q3DCategoryAxis *>(m_axisZ)->dptr()->setDataLabels(subList);
    }
}

void Bars3DController::handleBarClicked(const QPoint &position)
{
    setSelectedBar(position);
    // TODO: pass clicked to parent. (QTRD-2517)
    // TODO: Also hover needed? (QTRD-2131)
}

void Bars3DController::handleAxisAutoAdjustRangeChangedInOrientation(
        Q3DAbstractAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)
    adjustAxisRanges();
}

QPoint Bars3DController::noSelectionPoint()
{
    static QPoint noSelectionPos(-1, -1);
    return noSelectionPos;
}

void Bars3DController::setAxisX(Q3DAbstractAxis *axis)
{
    Abstract3DController::setAxisX(axis);
    handleDataRowLabelsChanged();
}

void Bars3DController::setAxisZ(Q3DAbstractAxis *axis)
{
    Abstract3DController::setAxisZ(axis);
    handleDataColumnLabelsChanged();
}

void Bars3DController::handleAxisRangeChangedBySender(QObject *sender)
{
    // Data window changed
    if (sender == m_axisX || sender == m_axisZ) {
        if (sender == m_axisX)
            handleDataRowLabelsChanged();
        if (sender == m_axisZ)
            handleDataColumnLabelsChanged();
    }

    Abstract3DController::handleAxisRangeChangedBySender(sender);

    // Update selected bar - may be moved offscreen
    setSelectedBar(m_selectedBar);
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
    if (style == QDataVis::MeshStyleBars)
        objFile = QStringLiteral(":/defaultMeshes/bar");
    else if (style == QDataVis::MeshStylePyramids)
        objFile = QStringLiteral(":/defaultMeshes/pyramid");
    else if (style == QDataVis::MeshStyleCones)
        objFile = QStringLiteral(":/defaultMeshes/cone");
    else if (style == QDataVis::MeshStyleCylinders)
        objFile = QStringLiteral(":/defaultMeshes/cylinder");
    else if (style == QDataVis::MeshStyleBevelBars)
        objFile = QStringLiteral(":/defaultMeshes/bevelbar");

    if (smooth)
        objFile += QStringLiteral("Smooth");

    Abstract3DController::setMeshFileName(objFile);
}

void Bars3DController::setSelectionMode(QDataVis::SelectionFlags mode)
{
    if (mode.testFlag(QDataVis::SelectionSlice)
            && (mode.testFlag(QDataVis::SelectionRow) == mode.testFlag(QDataVis::SelectionColumn))) {
        qWarning("Must specify one of either row or column selection mode in conjunction with slicing mode.");
    } else {
        // When setting selection mode to a new slicing mode, activate slicing
        if (mode != selectionMode()) {
            bool isSlicing = mode.testFlag(QDataVis::SelectionSlice);
            if (isSlicing && m_selectedBar != noSelectionPoint())
                scene()->setSlicingActive(true);
            else
                scene()->setSlicingActive(false);
        }

        Abstract3DController::setSelectionMode(mode);
    }
}

void Bars3DController::setSelectedBar(const QPoint &position)
{
    // If the selection targets non-existent bar, clear selection instead.
    QPoint pos = position;

    if (pos != noSelectionPoint()) {
        const QBarDataProxy *proxy = static_cast<const QBarDataProxy *>(m_data);
        int maxRow = proxy->rowCount() - 1;
        int maxCol = (pos.x() <= maxRow && pos.x() >= 0 && proxy->rowAt(pos.x()))
                ? proxy->rowAt(pos.x())->size() - 1 : -1;

        if (pos.x() < 0 || pos.x() > maxRow || pos.y() < 0 || pos.y() > maxCol)
            pos = noSelectionPoint();
    }

    if (selectionMode().testFlag(QDataVis::SelectionSlice)) {
        // If the selected bar is outside data window, or there is no selected bar, disable slicing
        if (pos.x() < m_axisX->min() || pos.x() > m_axisX->max()
                || pos.y() < m_axisZ->min() || pos.y() > m_axisZ->max()) {
            scene()->setSlicingActive(false);
        } else {
            scene()->setSlicingActive(true);
        }
        emitNeedRender();
    }

    if (pos != m_selectedBar) {
        m_selectedBar = pos;
        m_changeTracker.selectedBarChanged = true;
        emit selectedBarChanged(pos);
        emitNeedRender();
    }
}

QPoint Bars3DController::selectedBar() const
{
    return m_selectedBar;
}

void Bars3DController::adjustAxisRanges()
{
    const QBarDataProxy *proxy = static_cast<QBarDataProxy *>(m_data);
    const QBarDataArray *array = proxy->array();

    Q3DCategoryAxis *categoryAxisX = static_cast<Q3DCategoryAxis *>(m_axisX);
    if (categoryAxisX && categoryAxisX->isAutoAdjustRange() && proxy) {
        int rowCount = proxy->rowCount();
        if (rowCount)
            rowCount--;
        categoryAxisX->dptr()->setRange(0.0, qreal(rowCount));
    }

    Q3DCategoryAxis *categoryAxisZ = static_cast<Q3DCategoryAxis *>(m_axisZ);
    if (categoryAxisZ && categoryAxisZ->isAutoAdjustRange() && proxy) {
        int columnCount = 0;
        for (int i = 0; i < array->size(); i++) {
            if (columnCount < array->at(i)->size())
                columnCount = array->at(i)->size();
        }
        if (columnCount)
            columnCount--;
        categoryAxisZ->dptr()->setRange(0.0, qreal(columnCount));
    }

    Q3DValueAxis *valueAxis = static_cast<Q3DValueAxis *>(m_axisY);
    if (valueAxis && categoryAxisX && categoryAxisZ && valueAxis->isAutoAdjustRange() && proxy) {
        QPair<GLfloat, GLfloat> limits = proxy->dptrc()->limitValues(categoryAxisX->min(),
                                                                    categoryAxisX->max(),
                                                                    categoryAxisZ->min(),
                                                                    categoryAxisZ->max());
        if (limits.first < 0) {
            // TODO: Currently we only support symmetric y-axis for bar graph if there are negative values
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

QT_DATAVISUALIZATION_END_NAMESPACE
