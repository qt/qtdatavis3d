// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "q3dcamera_p.h"
#include "qquickdatavisscatter_p.h"
#include "qscatterdataproxy_p.h"
#include "declarativescene_p.h"
#include "axishelper_p.h"
#include "qvalue3daxis_p.h"
#include "scatterseriesvisualizer_p.h"
#include <QtCore/QMutexLocker>
#include <QColor>
#include <QtQuick3D/private/qquick3drepeater_p.h>
#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>
#include <QtQuick3D/private/qquick3dperspectivecamera_p.h>
#include <private/qquick3ddirectionallight_p.h>
#include <private/qquick3dpointlight_p.h>

QT_BEGIN_NAMESPACE

QQuickDataVisScatter::QQuickDataVisScatter(QQuickItem *parent)
    : QQuickDataVisItem(parent),
      m_scatterController(0)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemHasContents);
    // Create the shared component on the main GUI thread.
    m_scatterController = new Scatter3DController(boundingRect().toRect(), new Declarative3DScene);

    setSharedController(m_scatterController);

    QQuick3DSceneEnvironment *scene = environment();
    scene->setBackgroundMode(QQuick3DSceneEnvironment::QQuick3DEnvironmentBackgroundTypes::Color);
    scene->setClearColor(Qt::blue);

    QObject::connect(m_scatterController, &Scatter3DController::selectedSeriesChanged,
                     this, &QQuickDataVisScatter::selectedSeriesChanged);
}

QQuickDataVisScatter::~QQuickDataVisScatter()
{
    QMutexLocker locker(m_nodeMutex.data());
    const QMutexLocker locker2(mutex());
    delete m_scatterController;
}

QValue3DAxis *QQuickDataVisScatter::axisX() const
{
    return static_cast<QValue3DAxis *>(m_scatterController->axisX());
}

void QQuickDataVisScatter::setAxisX(QValue3DAxis *axis)
{
    m_scatterController->setAxisX(axis);
}

QValue3DAxis *QQuickDataVisScatter::axisY() const
{
    return static_cast<QValue3DAxis *>(m_scatterController->axisY());
}

void QQuickDataVisScatter::setAxisY(QValue3DAxis *axis)
{
    m_scatterController->setAxisY(axis);
}

QValue3DAxis *QQuickDataVisScatter::axisZ() const
{
    return static_cast<QValue3DAxis *>(m_scatterController->axisZ());
}

void QQuickDataVisScatter::setAxisZ(QValue3DAxis *axis)
{
    m_scatterController->setAxisZ(axis);
}

QScatter3DSeries *QQuickDataVisScatter::selectedSeries() const
{
    return m_scatterController->selectedSeries();
}

void QQuickDataVisScatter::setSelectedItem(int index, QScatter3DSeries *series)
{
    const QScatterDataProxy *proxy = 0;

    // Series may already have been removed, so check it before setting the selection.
    if (!m_scatterController->m_seriesList.contains(series))
        series = 0;

    if (series)
        proxy = series->dataProxy();

    if (!proxy || index < 0 || index >= proxy->itemCount())
        index = invalidSelectionIndex();

    if (index != m_selectedItem || series != m_selectedItemSeries) {
        bool seriesChanged = (series != m_selectedItemSeries);
        m_selectedItem = index;
        m_selectedItemSeries = series;
        m_scatterController->m_changeTracker.selectedItemChanged = true;

        // Clear selection from other series and finally set new selection to the specified series
        foreach (QAbstract3DSeries *otherSeries, m_scatterController->m_seriesList) {
            QScatter3DSeries *scatterSeries = static_cast<QScatter3DSeries *>(otherSeries);
            if (scatterSeries != m_selectedItemSeries) {
                auto visualizer = visualizerForSeries(scatterSeries);
                visualizer->clearSelection();
                scatterSeries->dptr()->setSelectedItem(invalidSelectionIndex());
            }
        }
        if (m_selectedItemSeries)
            m_selectedItemSeries->dptr()->setSelectedItem(m_selectedItem);

        if (seriesChanged)
            emit selectedSeriesChanged(m_selectedItemSeries);

    }
    if (index != invalidSelectionIndex())
        itemSelectionLabel()->setVisible(true);
}

QQmlListProperty<QScatter3DSeries> QQuickDataVisScatter::seriesList()
{
    return QQmlListProperty<QScatter3DSeries>(this, this,
                                              &QQuickDataVisScatter::appendSeriesFunc,
                                              &QQuickDataVisScatter::countSeriesFunc,
                                              &QQuickDataVisScatter::atSeriesFunc,
                                              &QQuickDataVisScatter::clearSeriesFunc);
}

void QQuickDataVisScatter::appendSeriesFunc(QQmlListProperty<QScatter3DSeries> *list,
                                            QScatter3DSeries *series)
{
    reinterpret_cast<QQuickDataVisScatter *>(list->data)->addSeries(series);
}

qsizetype QQuickDataVisScatter::countSeriesFunc(QQmlListProperty<QScatter3DSeries> *list)
{
    return reinterpret_cast<QQuickDataVisScatter *>(list->data)->m_scatterController->scatterSeriesList().size();
}

QScatter3DSeries *QQuickDataVisScatter::atSeriesFunc(QQmlListProperty<QScatter3DSeries> *list,
                                                     qsizetype index)
{
    return reinterpret_cast<QQuickDataVisScatter *>(list->data)->m_scatterController->scatterSeriesList().at(index);
}

void QQuickDataVisScatter::clearSeriesFunc(QQmlListProperty<QScatter3DSeries> *list)
{
    QQuickDataVisScatter *declScatter = reinterpret_cast<QQuickDataVisScatter *>(list->data);
    QList<QScatter3DSeries *> realList = declScatter->m_scatterController->scatterSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declScatter->removeSeries(realList.at(i));
}

void QQuickDataVisScatter::addSeries(QScatter3DSeries *series)
{
    m_scatterController->addSeries(series);
    auto visualizer = new ScatterSeriesVisualizer();
    visualizer->setController(m_scatterController);
    visualizer->setQml(this);
    visualizer->setup();
    visualizer->connectSeries(series);
    setVisualizerForSeries(series, visualizer);

    if (series->dataProxy()->itemCount() > 0)
        visualizer->generatePoints(series->dataProxy()->itemCount());

    if (series->selectedItem() != invalidSelectionIndex())
        setSelectedItem(series->selectedItem(), series);
}

void QQuickDataVisScatter::removeSeries(QScatter3DSeries *series)
{
    m_scatterController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

void QQuickDataVisScatter::handleAxisXChanged(QAbstract3DAxis *axis)
{
    emit axisXChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisScatter::handleAxisYChanged(QAbstract3DAxis *axis)
{
    emit axisYChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisScatter::handleAxisZChanged(QAbstract3DAxis *axis)
{
    emit axisZChanged(static_cast<QValue3DAxis *>(axis));
}

QQmlComponent *QQuickDataVisScatter::createRepeaterDelegate(QAbstract3DSeries::Mesh meshType)
{
    switch (meshType) {
    case QAbstract3DSeries::MeshSphere:
        return createRepeaterDelegateComponent(QStringLiteral(":/datapointModels/DatapointSphere"));
    case QAbstract3DSeries::MeshCube:
        return createRepeaterDelegateComponent(QStringLiteral(":/datapointModels/DatapointCube"));
    default:
        return createRepeaterDelegateComponent(QStringLiteral(":/datapointModels/DatapointSphere"));
    }
}

void QQuickDataVisScatter::calculateSceneScalingFactors()
{
    if (m_requestedMargin < 0.0f) {
        if (m_maxItemSize > m_defaultMaxSize)
            m_hBackgroundMargin = m_maxItemSize / m_itemScaler;
        else
            m_hBackgroundMargin = m_defaultMaxSize;
        m_vBackgroundMargin = m_hBackgroundMargin;
    } else {
        m_hBackgroundMargin = m_requestedMargin;
        m_vBackgroundMargin = m_requestedMargin;
    }

    float horizontalAspectRatio;
    horizontalAspectRatio = m_graphHorizontalAspectRatio;

    QSizeF areaSize;
    auto *axisX = static_cast<QValue3DAxis *>(m_scatterController->axisX());
    auto *axisZ = static_cast<QValue3DAxis *>(m_scatterController->axisZ());

    if (horizontalAspectRatio == 0.0f) {
        areaSize.setHeight(axisZ->max() - axisZ->min());
        areaSize.setWidth(axisX->max() - axisX->min());
    } else {
        areaSize.setHeight(1.0f);
        areaSize.setWidth(horizontalAspectRatio);
    }

    float horizontalMaxDimension;

    if (m_graphAspectRatio > 2.0f) {
        horizontalMaxDimension = 2.0f;
        m_scaleY = 2.0f / m_graphAspectRatio;
    } else {
        horizontalMaxDimension = m_graphAspectRatio;
        m_scaleY = 1.0f;
    }
    float scaleFactor = qMax(areaSize.width(), areaSize.height());
    m_scaleX = horizontalMaxDimension * areaSize.width() / scaleFactor;
    m_scaleZ = horizontalMaxDimension * areaSize.height() / scaleFactor;

    setBackgroundScaleMargin({m_hBackgroundMargin, m_vBackgroundMargin, m_hBackgroundMargin});

    setLineLengthScaleFactor(0.02f);
    setScaleWithBackground({m_scaleX, m_scaleY, m_scaleZ});
    setScale({m_scaleX * 2.0f, m_scaleY * 2.0f, m_scaleZ * -2.0f});
    setTranslate({-m_scaleX, -m_scaleY, m_scaleZ});
}

float QQuickDataVisScatter::calculatePointScaleSize()
{
    auto series = m_scatterController->scatterSeriesList();
    int totalDataSize = 0;
    for (const auto &scatterSeries : std::as_const(series)) {
        if (scatterSeries->isVisible())
            totalDataSize += scatterSeries->dataProxy()->array()->size();
    }
    return qBound(m_defaultMinSize, 2.0f / float(qSqrt(qreal(totalDataSize))), m_defaultMaxSize);
}

void QQuickDataVisScatter::updatePointScaleSize()
{
    m_pointScale = calculatePointScaleSize();
}

void QQuickDataVisScatter::updateDataPoints(QScatter3DSeries *series)
{
    auto visualizer = visualizerForSeries(series);
    visualizer->setDotSizedScale(m_pointScale);
    visualizer->updateItemPositions(series->dataProxy());
}

void QQuickDataVisScatter::updateDataPointVisuals(QScatter3DSeries *series)
{
    auto visualizer = visualizerForSeries(series);
    visualizer->setScaleX(m_scaleX);
    visualizer->setScaleY(m_scaleY);
    visualizer->setScaleZ(m_scaleZ);
    visualizer->updateItemVisuals(series);
}

void QQuickDataVisScatter::updateShadowQuality(ShadowQuality quality)
{
    QQuickDataVisItem::updateShadowQuality(quality);
}

QQuick3DModel *QQuickDataVisScatter::selected() const
{
    return m_selected;
}

void QQuickDataVisScatter::setSelected(QQuick3DModel *newSelected)
{
    if (newSelected != m_selected) {
        m_previousSelected = m_selected;
        m_selected = newSelected;

        auto seriesRoot = m_selected->parentItem();
        auto series = static_cast<QScatter3DSeries *>(seriesRoot->parent());
        ScatterSeriesVisualizer *visualizer = visualizerForSeries(series);
        qsizetype seriesItemIndex = visualizer->getItemIndex(m_selected);
        visualizer->clearSelection();
        visualizer->setSelected(seriesItemIndex);
        m_scatterController->m_isSeriesVisualsDirty = true;
        setSelectedItem(seriesItemIndex, series);
        m_scatterController->m_changeTracker.selectedItemChanged = true;
    }
}

void QQuickDataVisScatter::setSelected(QQuick3DModel *root, qsizetype index)
{
    if (index != m_selectedItem) {
        auto seriesRoot = root->parentItem();
        auto series = static_cast<QScatter3DSeries *>(seriesRoot->parent());
        ScatterSeriesVisualizer *visualizer = visualizerForSeries(series);
        visualizer->clearSelection();
        visualizer->setSelected(index);
        m_scatterController->m_isSeriesVisualsDirty = true;
        setSelectedItem(index, series);
        m_scatterController->m_changeTracker.selectedItemChanged = true;
    }
}

void QQuickDataVisScatter::clearSelectionModel()
{
    setSelectedItem(invalidSelectionIndex(), 0);
    if (m_selectedItemSeries)  {
        auto visualizer = visualizerForSeries(m_selectedItemSeries);
        visualizer->clearSelection();
    }
    itemSelectionLabel()->setVisible(false);
    m_scatterController->m_isSeriesVisualsDirty = true;
    m_selected = nullptr;
    m_previousSelected = nullptr;
}

void QQuickDataVisScatter::setVisualizerForSeries(QScatter3DSeries *series, ScatterSeriesVisualizer *visualizer)
{
    m_seriesVisualizerMap.insert(series, visualizer);
}

ScatterSeriesVisualizer *QQuickDataVisScatter::visualizerForSeries(QScatter3DSeries *series)
{
    return m_seriesVisualizerMap.value(series, nullptr);
}

void QQuickDataVisScatter::updateGraph()
{
    //for each series one visualizer
    auto scatterSeriesList = m_scatterController->scatterSeriesList();

    for (auto *scatterSeries : std::as_const(scatterSeriesList)) {
        auto *visualizer = visualizerForSeries(scatterSeries);
        if (visualizer && scatterSeries->isVisible()) {
            if (m_scatterController->m_isDataDirty)
                updateDataPoints(scatterSeries);

            if (m_scatterController->m_isSeriesVisualsDirty)
                updateDataPointVisuals(scatterSeries);
        }
    }
}

void QQuickDataVisScatter::synchData()
{
    QList<QScatter3DSeries *> seriesList = m_scatterController->scatterSeriesList();

    float maxItemSize = 0.0f;

    for (const auto &series : std::as_const(seriesList)) {
        if (series->isVisible()) {
            float itemSize = series->itemSize();
            if (itemSize > maxItemSize)
                maxItemSize = itemSize;
        }
    }

    m_maxItemSize = maxItemSize;

    calculateSceneScalingFactors();
    updatePointScaleSize();
    QQuickDataVisItem::synchData();
    scene()->activeCamera()->d_ptr->setMinYRotation(-90.0f);

    auto *aX = axisX();
    auto *aY = axisY();
    auto *aZ = axisZ();

    aX->formatter()->d_ptr->recalculate();
    aY->formatter()->d_ptr->recalculate();
    aZ->formatter()->d_ptr->recalculate();

    m_pointScale = calculatePointScaleSize();

    // Notify changes to renderer
    if (m_scatterController->m_changeTracker.itemChanged) {
        m_scatterController->m_changeTracker.itemChanged = false;
        m_scatterController->m_changedItems.clear();
    }

    if (m_scatterController->m_changeTracker.selectedItemChanged) {
        if (m_scatterController->m_selectedItem != m_scatterController->invalidSelectionIndex()) {
            auto visualizer = visualizerForSeries(m_scatterController->m_selectedItemSeries);
            auto selectedPosition = visualizer->selectedItemPosition();
            itemSelectionLabel()->setPosition(selectedPosition);
            QString itemLabelText = m_scatterController->m_selectedItemSeries->itemLabel();
            itemSelectionLabel()->setProperty("labelText", itemLabelText);
        }
        m_scatterController->m_changeTracker.selectedItemChanged = false;
    }
}

void QQuickDataVisScatter::setSelectionModeNG(QAbstract3DGraph::SelectionFlags mode)
{
    // We only support single item selection mode and no selection mode
    if (mode != QAbstract3DGraph::SelectionItem && mode != QAbstract3DGraph::SelectionNone) {
        qWarning("Unsupported selection mode - only none and item selection modes are supported.");
        return;
    }

    if (mode == QAbstract3DGraph::SelectionItem && isReady()) {
        for (const auto &visualizer : std::as_const(m_seriesVisualizerMap))
            visualizer->createItemLabel();
    }
}
QT_END_NAMESPACE
