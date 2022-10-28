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

    setScaleWithBackground({m_scaleX + m_hBackgroundMargin, m_scaleY + m_vBackgroundMargin,
                            m_scaleZ + m_hBackgroundMargin});
    setScale({m_scaleX * 2.0f, m_scaleY * 2.0f, -m_scaleZ * 2.0f});
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

void QQuickDataVisScatter::updateLabels()
{
    float labelAutoAngle = 0.0f;

    auto xFlipped = isXFlipped();
    auto yFlipped = isYFlipped();
    auto zFlipped = isZFlipped();
    // X labels
    {
        auto xLabelPositions = axisX()->formatter()->labelPositions();
        auto labels = axisX()->labels();
        labelAutoAngle = axisX()->labelAutoRotation();
        float labelAngleFraction = labelAutoAngle / 90.0f;
        float fractionCamX = scene()->activeCamera()->xRotation() * labelAngleFraction;
        float fractionCamY = scene()->activeCamera()->yRotation() * labelAngleFraction;
        QVector3D labelRotation;

        float yPos = 0.0f;
        float zPos = 0.0f;

        if (labelAutoAngle == 0.0f) {
            labelRotation = QVector3D(-90.0f, 90.0f, 0.0f);
            if (xFlipped)
                labelRotation.setY(-90.0f);
            if (yFlipped) {
                if (xFlipped)
                    labelRotation.setY(-90.0f);
                else
                    labelRotation.setY(90.0f);
                labelRotation.setX(90.0f);
            }
        } else {
            if (xFlipped)
                labelRotation.setY(-90.0f);
            else
                labelRotation.setY(90.0f);
            if (yFlipped) {
                if (zFlipped) {
                    if (xFlipped) {
                        labelRotation.setX(90.0f - (2.0f * labelAutoAngle - fractionCamX)
                                           * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(-labelAutoAngle - fractionCamY);
                    } else {
                        labelRotation.setX(90.0f - (2.0f * labelAutoAngle + fractionCamX)
                                           * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(labelAutoAngle + fractionCamY);
                    }
                } else {
                    if (xFlipped) {
                        labelRotation.setX(90.0f + fractionCamX
                                           * -(labelAutoAngle + fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(labelAutoAngle + fractionCamY);
                    } else {
                        labelRotation.setX(90.0f - fractionCamX
                                           * (-labelAutoAngle - fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(-labelAutoAngle - fractionCamY);
                    }
                }
            } else {
                if (zFlipped) {
                    if (xFlipped) {
                        labelRotation.setX(-90.0f + (2.0f * labelAutoAngle - fractionCamX)
                                           * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(labelAutoAngle - fractionCamY);
                    } else {
                        labelRotation.setX(-90.0f + (2.0f * labelAutoAngle + fractionCamX)
                                           * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(-labelAutoAngle + fractionCamY);
                    }
                } else {
                    if (xFlipped) {
                        labelRotation.setX(-90.0f - fractionCamX
                                           * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(-labelAutoAngle + fractionCamY);
                    } else {
                        labelRotation.setX(-90.0f + fractionCamX
                                           * -(labelAutoAngle - fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(labelAutoAngle - fractionCamY);
                    }
                }
            }
        }
        if (!xFlipped) {
            if (!yFlipped)
                yPos = -scaleWithBackground().y();
            else
                yPos = scaleWithBackground().y();
        } else {
            if (!yFlipped)
                yPos = -scaleWithBackground().y();
            else
                yPos = scaleWithBackground().y();
        }

        if (zFlipped)
            zPos = -scaleWithBackground().z() - m_labelMargin;
        else
            zPos = scaleWithBackground().z() + m_labelMargin;

        auto totalRotation = Utils::calculateRotation(labelRotation);
        auto labelTrans = QVector3D(0.0f, yPos, zPos);
        for (int i = 0; i < repeaterX()->count(); i++) {
            auto obj = static_cast<QQuick3DNode *>(repeaterX()->objectAt(i));
            auto posX = axisX()->labelPositionAt(i) * scale().x() + translate().x();
            labelTrans.setX(posX);
            obj->setPosition(labelTrans);
            obj->setRotation(totalRotation);
            obj->setProperty("labelText", labels[i]);
        }

        float labelsMaxWidth = 0.0f;
        labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(axisX()->labels())));

        if (titleLabelX()->visible()) {
            labelTrans.setX(0.0f);
            updateXTitle(labelRotation, labelTrans, totalRotation,labelsMaxWidth);
        }
    }

    // Y labels
    {
        auto yLabelPositions = axisY()->formatter()->labelPositions();
        auto labels = axisY()->labels();
        float labelAutoAngle = axisY()->labelAutoRotation();
        float labelAngleFraction = labelAutoAngle / 90.0f;
        float fractionCamX = scene()->activeCamera()->xRotation() * labelAngleFraction;
        float fractionCamY = scene()->activeCamera()->yRotation() * labelAngleFraction;

        QVector3D sideLabelRotation(0.0f, -90.0f, 0.0f);
        QVector3D backLabelRotation(0.0f, 0.0f, 0.0f);
        // Right side
        float xPos = 0.0f;
        float zPos = 0.0f;

        int rightSideCount = repeaterY()->count() / 2;

        if (labelAutoAngle == 0.0f) {
            if (!xFlipped)
                sideLabelRotation.setY(90.0f);
            if (zFlipped)
                backLabelRotation.setY(180.f);
        } else {
            // Orient side labels somewhat towards the camera
            if (xFlipped) {
                if (zFlipped)
                    backLabelRotation.setY(180.0f + (2.0f * labelAutoAngle) - fractionCamX);
                else
                    backLabelRotation.setY(-fractionCamX);
                sideLabelRotation.setY(-90.0f + labelAutoAngle - fractionCamX);
            } else {
                if (zFlipped)
                    backLabelRotation.setY(180.0f - (2.0f * labelAutoAngle) - fractionCamX);
                else
                    backLabelRotation.setY(-fractionCamX);
                sideLabelRotation.setY(90.0f - labelAutoAngle - fractionCamX);
            }
        }

        if (xFlipped)
            xPos = -scaleWithBackground().x() - m_labelMargin;
        else
            xPos = scaleWithBackground().x() + m_labelMargin;

        if (zFlipped)
            zPos = scaleWithBackground().z() + m_labelMargin;
        else
            zPos = -scaleWithBackground().z() - m_labelMargin;

        backLabelRotation.setX(-fractionCamY);
        sideLabelRotation.setX(-fractionCamY);

        auto totalSideLabelRotation = Utils::calculateRotation(sideLabelRotation);
        auto SideLabelTrans = QVector3D(xPos, 0, zPos);

        for (int i = 0; i < rightSideCount; i++) {
            auto obj = static_cast<QQuick3DNode *>(repeaterY()->objectAt(i));
            auto posY = axisY()->labelPositionAt(i) * scale().y() + translate().y();
            SideLabelTrans.setY(posY);
            obj->setPosition(SideLabelTrans);
            obj->setRotation(totalSideLabelRotation);
            obj->setProperty("labelText", labels[i]);
        }

        int label = 0;
        //Left side
        if (!xFlipped)
            xPos = -scaleWithBackground().x() - m_labelMargin;
        else
            xPos = scaleWithBackground().x() + m_labelMargin;

        if (!zFlipped)
            zPos = scaleWithBackground().z() + m_labelMargin;
        else
            zPos = -scaleWithBackground().z() - m_labelMargin;

        auto totalBackLabelRotation = Utils::calculateRotation(backLabelRotation);
        auto backLabelTrans = QVector3D(xPos, 0, zPos);

        for (int i = rightSideCount; i < repeaterY()->count(); i++) {
            auto obj = static_cast<QQuick3DNode *>(repeaterY()->objectAt(i));
            auto posY = axisY()->labelPositionAt(label) * scale().y() + translate().y();
            backLabelTrans.setY(posY);
            obj->setPosition(backLabelTrans);
            obj->setRotation(totalBackLabelRotation);
            obj->setProperty("labelText", labels[label]);
            label++;
        }
        backLabelTrans.setY(0.0f);
        SideLabelTrans.setY(0.0f);

        float labelsMaxWidth = 0.0f;
        labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(axisX()->labels())));

        updateYTitle(sideLabelRotation, backLabelRotation,
                     SideLabelTrans,    backLabelTrans,
                     totalSideLabelRotation, totalBackLabelRotation,1);
    }

    // Z labels
    {
        auto zLabelPositions = axisZ()->formatter()->labelPositions();
        auto labels = axisZ()->labels();

        float labelAutoAngle = axisZ()->labelAutoRotation();
        float labelAngleFraction = labelAutoAngle / 90.0f;
        float fractionCamX = scene()->activeCamera()->xRotation() * labelAngleFraction;
        float fractionCamY = scene()->activeCamera()->yRotation() * labelAngleFraction;

        float xPos;
        float yPos;
        QVector3D labelRotation;

        if (labelAutoAngle == 0.0f) {
            if (zFlipped)
                labelRotation.setY(180.0f);
            if (yFlipped) {
                if (zFlipped)
                    labelRotation.setY(180.0f);
                else
                    labelRotation.setY(0.0f);
                labelRotation.setX(90.0f);
            } else {
                labelRotation.setX(-90.0f);
            }
        } else {
            if (zFlipped)
                labelRotation.setY(180.0f);
            if (yFlipped) {
                if (zFlipped) {
                    if (xFlipped) {
                        labelRotation.setX(90.0f - (labelAutoAngle - fractionCamX)
                                           * (-labelAutoAngle - fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(labelAutoAngle + fractionCamY);
                    } else {
                        labelRotation.setX(90.0f + (labelAutoAngle + fractionCamX)
                                           * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(-labelAutoAngle - fractionCamY);
                    }
                } else {
                    if (xFlipped) {
                        labelRotation.setX(90.0f + (labelAutoAngle - fractionCamX)
                                           * -(labelAutoAngle + fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(-labelAutoAngle - fractionCamY);
                    } else {
                        labelRotation.setX(90.0f - (labelAutoAngle + fractionCamX)
                                           * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(labelAutoAngle + fractionCamY);
                    }
                }
            } else {
                if (zFlipped) {
                    if (xFlipped) {
                        labelRotation.setX(-90.0f + (labelAutoAngle - fractionCamX)
                                           * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(-labelAutoAngle + fractionCamY);
                    } else {
                        labelRotation.setX(-90.0f - (labelAutoAngle + fractionCamX)
                                           * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(labelAutoAngle - fractionCamY);
                    }
                } else {
                    if (xFlipped) {
                        labelRotation.setX(-90.0f - (labelAutoAngle - fractionCamX)
                                           * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(labelAutoAngle - fractionCamY);
                    } else {
                        labelRotation.setX(-90.0f + (labelAutoAngle + fractionCamX)
                                           * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                        labelRotation.setZ(-labelAutoAngle + fractionCamY);
                    }
                }
            }
        }

        if (xFlipped)
            xPos = -scaleWithBackground().x() - m_labelMargin;
        else
            xPos = scaleWithBackground().x() + m_labelMargin;

        if (!zFlipped) {
            if (!yFlipped)
                yPos = -scaleWithBackground().y();
            else
                yPos = scaleWithBackground().y();
        }
        else {
            if (!yFlipped)
                yPos = -scaleWithBackground().y();
            else
                yPos = scaleWithBackground().y();
        }
        auto totalRotation = Utils::calculateRotation(labelRotation);
        auto labelTrans = QVector3D(xPos, yPos, 0);

        for (int i = 0; i < repeaterZ()->count(); i++) {
            auto obj = static_cast<QQuick3DNode *>(repeaterZ()->objectAt(i));
            auto posZ = axisZ()->labelPositionAt(i) * scale().z() + translate().z();
            labelTrans.setZ(posZ);
            obj->setPosition(labelTrans);
            obj->setRotation(totalRotation);
            obj->setProperty("labelText", labels[i]);
        }

        labelTrans.setZ(0);
        float labelsMaxWidth = 0.0f;
        labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(axisX()->labels())));

        updateZTitle(labelRotation, labelTrans, totalRotation,labelsMaxWidth);
    }
}

int QQuickDataVisScatter::findLabelsMaxWidth(const QStringList &labels)
{
    int labelWidth = 0;
    QFontMetrics labelFM(m_scatterController->activeTheme()->font());

    for (const auto &label : std::as_const(labels)) {
        auto width = labelFM.horizontalAdvance(label);
        if (labelWidth < width)
            labelWidth = width;
    }
    return labelWidth;
}

void QQuickDataVisScatter::updateGrid()
{
    calculateSceneScalingFactors();
    updatePointScaleSize();
    int gridLineCountX = segmentLineRepeaterX()->count() / 2;
    int subGridLineCountX = subsegmentLineRepeaterX()->count() / 2;
    int gridLineCountY = segmentLineRepeaterY()->count() / 2;
    int subGridLineCountY = subsegmentLineRepeaterY()->count() / 2;
    int gridLineCountZ = segmentLineRepeaterZ()->count() / 2;
    int subGridLineCountZ = subsegmentLineRepeaterZ()->count() / 2;

    auto lineLengthScaleFactor = this->lineLengthScaleFactor();
    auto lineWidthScaleFactor = this->lineWidthScaleFactor();
    auto gridOffset = this->gridOffset();

    auto scaleXWithBackground = scaleWithBackground().x();
    auto scaleYWithBackground = scaleWithBackground().y();
    auto scaleZWithBackground = scaleWithBackground().z();

    auto axisX = this->axisX();
    auto axisY = this->axisY();
    auto axisZ = this->axisZ();

    QVector3D scaleX(scale().x() * lineLengthScaleFactor, lineWidthScaleFactor, lineWidthScaleFactor);
    QVector3D scaleY(lineWidthScaleFactor, scale().y() * lineLengthScaleFactor, lineWidthScaleFactor);
    QVector3D scaleZ(lineWidthScaleFactor, -scale().z() * lineLengthScaleFactor, 1);

    QVector3D lineBackRotationX(0, 0, 0);
    QVector3D lineBackRotationY(0, 0, 0);

    QVector3D lineSideRotationY(0, 90, -90);
    QVector3D lineSideRotationZ(0, 90, 0);

    QVector3D lineFloorRotationX(-90,0,0);
    QVector3D lineFloorRotationZ(-90,0,0);
    float linePosX = 0;
    float linePosY = 0;
    float linePosZ = 0;
    // Wall lines

    // Back

    // X = Column
    linePosY = 0;
    if (!isZFlipped()) {
        linePosZ = -scaleZWithBackground + gridOffset;
    } else {
        linePosZ = scaleZWithBackground - gridOffset;
        lineBackRotationX = QVector3D(0, 180, 0);
        lineBackRotationY = QVector3D(0, 180, 0);
    }

    for (int i  = 0; i < gridLineCountX; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterX()->objectAt(i));
        linePosX = axisX->gridPositionAt(i) * scale().x() + translate().x();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineBackRotationX);
    }

    for (int i = 0; i <subGridLineCountX; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterX()->objectAt(i));
        linePosX = axisX->subGridPositionAt(i) * scale().x() + translate().x();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineBackRotationX);
    }

    // Y = Row
    linePosX = 0;
    for (int i  = 0; i < gridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterY()->objectAt(i));
        linePosY = axisY->gridPositionAt(i) * scale().y() + translate().y();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineBackRotationY);
    }

    for (int i = 0; i <subGridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterY()->objectAt(i));
        linePosY = axisY->subGridPositionAt(i) * scale().y() + translate().y();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineBackRotationY);
    }

    // Side
    // Y = Row

    linePosZ = 0;
    int k = 0;
    if (!isXFlipped()) {
        linePosX = -scaleXWithBackground + gridOffset;
    } else {
        linePosX = scaleXWithBackground - gridOffset;
        lineSideRotationY = QVector3D(0, -90, 90);
        lineSideRotationZ = QVector3D(0, -90, 0);
    }

    for (int i = gridLineCountY; i < segmentLineRepeaterY()->count(); i++)
    {
        auto lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterY()->objectAt(i));
        linePosY = axisY->gridPositionAt(k) * scale().y() + translate().y();
        positionAndScaleLine(lineNode,scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineSideRotationY);
        k++;
    }

    k = 0;
    for (int i = subGridLineCountY; i < subsegmentLineRepeaterY()->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterY()->objectAt(i));
        linePosY = axisY->subGridPositionAt(k) * scale().y() + translate().y();
        positionAndScaleLine(lineNode,scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineSideRotationY);
        k++;
    }

    // Z = Column
    linePosY = 0;
    for (int i = 0; i < gridLineCountZ; i++) {
        auto lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterZ()->objectAt(i));
        linePosZ = axisZ->gridPositionAt(i) * scale().z() + translate().z();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineSideRotationZ);
    }

    for (int i = 0; i < subGridLineCountZ; i++) {
        auto lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterZ()->objectAt(i));
        linePosZ = axisZ->subGridPositionAt(i) * scale().z() + translate().z();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineSideRotationZ);
    }

    // Floor lines
    // X = Column
    linePosZ = 0;
    k = 0;
    if (!isYFlipped()) {
        linePosY = -scaleYWithBackground + gridOffset;
    } else {
        linePosY = scaleYWithBackground - gridOffset;
        lineFloorRotationX = QVector3D(90, 0, 0);
        lineFloorRotationZ = QVector3D(90, 0, 0);
    }
    for (int i  = gridLineCountX; i < segmentLineRepeaterX()->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterX()->objectAt(i));
        linePosX = axisX->gridPositionAt(k) * scale().x() + translate().x();
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineFloorRotationX);
        k++;
    }

    k = 0;
    for (int i = subGridLineCountX; i < subsegmentLineRepeaterX()->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterX()->objectAt(i));
        linePosX = axisX->subGridPositionAt(k) * scale().x() + translate().x();
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineFloorRotationX);
        k++;
    }

    // Z = Row
    linePosX = 0;
    k = 0;
    for (int i = gridLineCountZ; i < segmentLineRepeaterZ()->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterZ()->objectAt(i));
        linePosZ = axisZ->gridPositionAt(k) * scale().z() + translate().z();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineFloorRotationZ);
        k++;
    }

    k = 0;
    for (int i = subGridLineCountZ; i < subsegmentLineRepeaterZ()->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterZ()->objectAt(i));
        linePosZ = axisZ->subGridPositionAt(k) * scale().z() + translate().z();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineFloorRotationZ);
        k++;
    }
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
    QQuickDataVisItem::synchData();
    scene()->activeCamera()->d_ptr->setMinYRotation(-90.0f);

    auto *aX = axisX();
    auto *aY = axisY();
    auto *aZ = axisZ();

    aX->formatter()->d_ptr->recalculate();
    aY->formatter()->d_ptr->recalculate();
    aZ->formatter()->d_ptr->recalculate();

    m_pointScale = calculatePointScaleSize();

    updateLabels();

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
