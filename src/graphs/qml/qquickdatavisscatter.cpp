// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qml/qquickdatavisscatter_p.h"
#include "qml/declarativescene_p.h"
#include "data/qscatter3dseries_p.h"
#include "qvalue3daxis_p.h"
#include "qcategory3daxis_p.h"
#include "axis/qvalue3daxisformatter_p.h"
#include "engine/q3dcamera_p.h"
#include "engine/datavisquick3dtexturedata_p.h"

#include <QtCore/QMutexLocker>
#include <QColor>
#include <QtQuick3D/private/qquick3drepeater_p.h>
#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>
#include <QtQuick3D/private/qquick3dperspectivecamera_p.h>
#include <QtQuick3D/private/qquick3dmodel_p.h>
#include <QtQuick3D/private/qquick3dcustommaterial_p.h>
#include <QtQuick3D/private/qquick3ddirectionallight_p.h>
#include <QtQuick3D/private/qquick3dpointlight_p.h>

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

    if (m_instancing)
        delete m_instancing;
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

// From seriesvisualizer
void QQuickDataVisScatter::disconnectSeries(QScatter3DSeries *series)
{
    QObject::disconnect(series, 0, this, 0);
}

void QQuickDataVisScatter::generatePointsForScatterModel(ScatterModel *graphModel)
{
    QList<QQuick3DModel *> itemList;
    if (m_scatterController->optimizationHints() == QAbstract3DGraphNG::OptimizationDefault) {
        int itemCount = graphModel->series->dataProxy()->itemCount();
        if (graphModel->series->dataProxy()->itemCount() > 0)
            itemList.resize(itemCount);

        QAbstract3DSeries::Mesh meshType = graphModel->series->mesh();
        for (int i = 0; i < itemCount; i++) {
            QQuick3DModel *item = createDataItem(meshType);
            item->setPickable(true);
            item->setParent(graphModel->series);
            itemList[i] = item;
        }
        graphModel->dataItems = itemList;
        m_scatterController->markDataDirty();
    } else if (m_scatterController->optimizationHints() == QAbstract3DGraphNG::OptimizationStatic) {
        if (m_scatterController->selectionMode() != QAbstract3DGraphNG::SelectionNone)
            m_instancingRootItem->setPickable(true);
    }
    m_scatterController->markSeriesVisualsDirty();
}

qsizetype QQuickDataVisScatter::getItemIndex(QQuick3DModel *item)
{
    Q_UNUSED(item);
    if (m_scatterController->optimizationHints() == QAbstract3DGraphNG::OptimizationDefault)
        return 0;

    return -1;
}

void QQuickDataVisScatter::resetSelection()
{
    if (m_selectedIndex != -1) {
        if (m_scatterController->optimizationHints() == QAbstract3DGraphNG::OptimizationStatic) {
            m_selectionActive = false;
            m_selectionIndicator->setVisible(false);
        }
    }
}

void QQuickDataVisScatter::updateScatterGraphItemPositions(ScatterModel *graphModel)
{
    float itemSize = graphModel->series->itemSize() / m_itemScaler;
    QQuaternion meshRotation = graphModel->series->meshRotation();
    QScatterDataProxy *dataProxy = graphModel->series->dataProxy();
    QList<QQuick3DModel *> itemList = graphModel->dataItems;

    if (itemSize == 0.0f)
        itemSize = m_pointScale;

    if (m_scatterController->optimizationHints() == QAbstract3DGraphNG::OptimizationDefault) {
        if (dataProxy->itemCount() != itemList.size())
            qWarning() << __func__ << "Item count differs from itemList count";

        for (int i = 0; i < dataProxy->itemCount(); ++i) {
            const QScatterDataItem *item = dataProxy->itemAt(i);
            QQuick3DModel *dataPoint = itemList.at(i);

            QVector3D dotPos = item->position();
            QQuaternion dotRot = item->rotation();
            float posX = axisX()->positionAt(dotPos.x()) * scale().x() + translate().x();
            float posY = axisY()->positionAt(dotPos.y()) * scale().y() + translate().y();
            float posZ = axisZ()->positionAt(dotPos.z()) * scale().z() + translate().z();

            dataPoint->setPosition(QVector3D(posX, posY, posZ));

            dataPoint->setRotation(dotRot * meshRotation);
            dataPoint->setScale(QVector3D(itemSize, itemSize, itemSize));
        }
    } else if (m_scatterController->optimizationHints() == QAbstract3DGraphNG::OptimizationStatic)  {
        int count = dataProxy->itemCount();
        QList<DataItemHolder> positions;
        positions.resize(count);

        for (int i = 0; i < count; i++) {
            auto item = dataProxy->itemAt(i);
            auto dotPos = item->position();

            auto posX = axisX()->positionAt(dotPos.x()) * scale().x() + translate().x();
            auto posY = axisY()->positionAt(dotPos.y()) * scale().y() + translate().y();
            auto posZ = axisZ()->positionAt(dotPos.z()) * scale().z() + translate().z();

            DataItemHolder dih;
            dih.position = {posX, posY, posZ};
            dih.rotation = item->rotation() * meshRotation;
            dih.scale = {itemSize, itemSize, itemSize};

            positions[i] = dih;
        }
        m_instancing->setDataArray(positions);
    }
}

void QQuickDataVisScatter::updateScatterGraphItemVisuals(ScatterModel *graphModel)
{
    bool useGradient = graphModel->series->d_ptr->isUsingGradient();
    int itemCount = graphModel->series->dataProxy()->itemCount();

    if (useGradient) {
        if (!graphModel->seriesTexture) {
            graphModel->seriesTexture = createTexture();
            graphModel->seriesTexture->setParent(graphModel->series);
        }

        QLinearGradient gradient = graphModel->series->baseGradient();
        auto textureData = static_cast<DatavisQuick3DTextureData *>(
                    graphModel->seriesTexture->textureData());
        textureData->createGradient(gradient);

        if (!graphModel->highlightTexture) {
            graphModel->highlightTexture = createTexture();
            graphModel->highlightTexture->setParent(graphModel->series);
        }

        QLinearGradient highlightGradient = graphModel->series->singleHighlightGradient();
        auto highlightTextureData = static_cast<DatavisQuick3DTextureData *>(
                    graphModel->highlightTexture->textureData());
        highlightTextureData->createGradient(highlightGradient);
    } else {
        if (graphModel->seriesTexture) {
            graphModel->seriesTexture->deleteLater();
            graphModel->seriesTexture = nullptr;
        }

        if (graphModel->highlightTexture) {
            graphModel->highlightTexture->deleteLater();
            graphModel->highlightTexture = nullptr;
        }
    }

    bool rangeGradient = (useGradient && graphModel->series->d_ptr->m_colorStyle
                          == Q3DTheme::ColorStyleRangeGradient) ? true : false;

    if (m_scatterController->optimizationHints() == QAbstract3DGraphNG::OptimizationDefault) {
        QList<QQuick3DModel *> itemList = graphModel->dataItems;

        if (itemCount != itemList.size())
            qWarning() << __func__ << "Item count differs from itemList count";

        if (!rangeGradient) {
            for (int i = 0; i < itemCount; i++) {
                auto obj = static_cast<QQuick3DModel *>(itemList.at(i));

                updateItemMaterial(obj, useGradient, rangeGradient);

                updatePrincipledMaterial(obj, graphModel->series->baseColor(),
                                         useGradient, graphModel->seriesTexture);
            }
            if (m_scatterController->m_selectedItem != invalidSelectionIndex()
                    && graphModel->series == m_scatterController->m_selectedItemSeries) {
                QQuick3DModel *selectedItem = itemList.at(m_scatterController->m_selectedItem);
                updatePrincipledMaterial(selectedItem, graphModel->series->singleHighlightColor(),
                                         useGradient, graphModel->highlightTexture);
            }
        } else {
            for (int i = 0; i < itemCount; i++) {
                auto obj = static_cast<QQuick3DModel *>(itemList.at(i));
                updateItemMaterial(obj, useGradient, rangeGradient);
                updateCustomMaterial(obj, graphModel->seriesTexture);
            }

            if (m_selectedIndex != -1) {
                QQuick3DModel *obj = itemList.at(m_selectedIndex);

                updateCustomMaterial(obj, graphModel->highlightTexture);
            }
        }
    } else if (m_scatterController->optimizationHints() == QAbstract3DGraphNG::OptimizationStatic) {
        updateItemInstancedMaterial(m_instancingRootItem, useGradient, rangeGradient);
        m_instancing->setRangeGradient(rangeGradient);
        if (!rangeGradient) {
            updatePrincipledMaterial(m_instancingRootItem, graphModel->series->baseColor(), useGradient);
        } else {
            float rangeGradientYScaler = m_rangeGradientYHelper / m_scaleY;

            updateInstancedCustomMaterial(m_instancingRootItem);

            QList<float> customData;
            customData.resize(itemCount);

            QList<DataItemHolder> instancingData = m_instancing->dataArray();
            for (int i = 0; i < instancingData.size(); i++) {
                auto dih = instancingData.at(i);
                float value = (dih.position.y() + m_scaleY) * rangeGradientYScaler;
                customData[i] = value;
            }
            m_instancing->setCustomData(customData);
        }

        updateSelectionIndicatorMaterial(useGradient, rangeGradient);

        if (!rangeGradient) {
            updatePrincipledMaterial(m_selectionIndicator, graphModel->series->singleHighlightColor(),
                                     useGradient, graphModel->highlightTexture);
        } else {
            // Rangegradient
            updateInstancedCustomMaterial(m_selectionIndicator, true);
        }

        if (m_selectedIndex != -1 && !m_selectionActive) {
            auto dih = m_instancing->dataArray().at(m_selectedIndex);
            m_selectionIndicator->setPosition(dih.position);
            m_selectionIndicator->setRotation(dih.rotation);
            m_selectionIndicator->setScale(dih.scale * m_indicatorScaleAdjustment);
            m_selectionIndicator->setVisible(true);
            itemLabel()->setPosition(m_selectionIndicator->position());
            m_selectionActive = true;
            m_instancing->markDataDirty();
        }
    }
}

void QQuickDataVisScatter::updateItemMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient)
{
    Q_UNUSED(useGradient);
    QQmlListReference materialsRef(item, "materials");
    if (!rangeGradient) {
        if (materialsRef.size()) {
            if (!qobject_cast<QQuick3DPrincipledMaterial *>(materialsRef.at(0))) {
                auto principledMaterial = new QQuick3DPrincipledMaterial();
                QObject *oldCustomMaterial = materialsRef.at(0);
                materialsRef.replace(0, principledMaterial);
                delete oldCustomMaterial;
            }
        } else {
            auto principledMaterial = new QQuick3DPrincipledMaterial();
            materialsRef.append(principledMaterial);
        }
    } else {
        if (materialsRef.size()) {
            if (!qobject_cast<QQuick3DCustomMaterial *>(materialsRef.at(0))) {
                QQuick3DCustomMaterial *customMaterial = createQmlCustomMaterial(
                            QStringLiteral(":/materials/RangeGradientMaterial"));
                QObject *oldPrincipledMaterial = materialsRef.at(0);
                materialsRef.replace(0, customMaterial);
                delete oldPrincipledMaterial;
            }
        } else {
            QQuick3DCustomMaterial *customMaterial = createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterial"));
            materialsRef.append(customMaterial);
        }
    }
}

void QQuickDataVisScatter::updateItemInstancedMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient)
{
    Q_UNUSED(useGradient);
    QQmlListReference materialsRef(item, "materials");
    if (!rangeGradient) {
        if (materialsRef.size()) {
            if (!qobject_cast<QQuick3DPrincipledMaterial *>(materialsRef.at(0))) {
                auto principledMaterial = new QQuick3DPrincipledMaterial();
                auto oldCustomMaterial = materialsRef.at(0);
                materialsRef.replace(0, principledMaterial);
                delete oldCustomMaterial;
            }
        } else {
            auto principledMaterial = new QQuick3DPrincipledMaterial();
            materialsRef.append(principledMaterial);
        }
    } else {
        if (materialsRef.size()) {
            if (!qobject_cast<QQuick3DCustomMaterial *>(materialsRef.at(0))) {
                auto customMaterial = createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterialInstancing"));
                auto oldPrincipledMaterial = materialsRef.at(0);
                materialsRef.replace(0, customMaterial);
                delete oldPrincipledMaterial;
            }
        } else {
            auto customMaterial = createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterialInstancing"));
            materialsRef.append(customMaterial);
        }
    }
}

void QQuickDataVisScatter::updateSelectionIndicatorMaterial(bool useGradient, bool rangeGradient)
{
    Q_UNUSED(useGradient);
    QQmlListReference materialsRef(m_selectionIndicator, "materials");
    if (!rangeGradient) {
        if (materialsRef.size()) {
            if (!qobject_cast<QQuick3DPrincipledMaterial *>(materialsRef.at(0))) {
                auto principledMaterial = new QQuick3DPrincipledMaterial();
                auto old = qobject_cast<QQuick3DCustomMaterial *>(materialsRef.at(0));
                materialsRef.replace(0, principledMaterial);
                delete old;
            }
        } else {
            auto material = new QQuick3DPrincipledMaterial();
            materialsRef.append(material);
        }
    } else {
        // Rangegradient
        if (materialsRef.size()) {
            if (!qobject_cast<QQuick3DCustomMaterial *>(materialsRef.at(0))) {
                auto old = materialsRef.at(0);
                auto customMaterial = createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterial"));
                materialsRef.replace(0, customMaterial);
                delete old;
            }
        } else {
            auto customMaterial = createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterial"));
            materialsRef.append(customMaterial);
        }
    }
}

void QQuickDataVisScatter::updateInstancedCustomMaterial(QQuick3DModel *model, bool isHighlight,
                                                         QQuick3DTexture *seriesTexture,
                                                         QQuick3DTexture *highlightTexture)
{
    QQmlListReference materialsRef(model, "materials");

    // Rangegradient
    auto customMaterial = static_cast<QQuick3DCustomMaterial *>(materialsRef.at(0));

    QVariant textureInputAsVariant = customMaterial->property("custex");
    QQuick3DShaderUtilsTextureInput *textureInput = textureInputAsVariant.value<QQuick3DShaderUtilsTextureInput *>();

    if (isHighlight) {
        textureInput->setTexture(highlightTexture);

        if (m_selectedIndex != -1 && !m_selectionActive)
            m_selectedGradientPos = m_instancing->customData().takeAt(m_selectedIndex);

        customMaterial->setProperty("gradientPos", m_selectedGradientPos);
    } else {
        textureInput->setTexture(seriesTexture);
    }
}

void QQuickDataVisScatter::updateCustomMaterial(QQuick3DModel *item, QQuick3DTexture *texture)
{
    QQmlListReference materialsRef(item, "materials");
    auto customMaterial = static_cast<QQuick3DCustomMaterial *>(materialsRef.at(0));
    QVariant textureInputAsVariant = customMaterial->property("custex");
    QQuick3DShaderUtilsTextureInput *textureInput = textureInputAsVariant.value<QQuick3DShaderUtilsTextureInput *>();

    textureInput->setTexture(texture);

    float rangeGradientYScaler = m_rangeGradientYHelper / m_scaleY;
    float value = (item->y() + m_scaleY) * rangeGradientYScaler;
    customMaterial->setProperty("gradientPos", value);
}

void QQuickDataVisScatter::updatePrincipledMaterial(QQuick3DModel *model, const QColor &color,
                                                    bool useGradient, QQuick3DTexture *texture)
{
    QQmlListReference materialsRef(model, "materials");
    auto principledMaterial = static_cast<QQuick3DPrincipledMaterial *>(materialsRef.at(0));

    if (useGradient) {
        principledMaterial->setBaseColor(QColor(Qt::white));
        principledMaterial->setBaseColorMap(texture);
    } else {
        principledMaterial->setBaseColor(color);
    }
}

QQuick3DTexture *QQuickDataVisScatter::createTexture()
{
    QQuick3DTexture *texture = new QQuick3DTexture();
    texture->setParent(this);
    texture->setRotationUV(-90.0f);
    texture->setHorizontalTiling(QQuick3DTexture::ClampToEdge);
    texture->setVerticalTiling(QQuick3DTexture::ClampToEdge);
    DatavisQuick3DTextureData *textureData = new DatavisQuick3DTextureData();
    textureData->setParent(texture);
    textureData->setParentItem(texture);
    texture->setTextureData(textureData);

    return texture;
}

QQuick3DNode *QQuickDataVisScatter::createSeriesRoot()
{
    auto model = new QQuick3DNode();

    model->setParentItem(QQuick3DViewport::scene());
    return model;
}

QQuick3DModel *QQuickDataVisScatter::createDataItem(const QAbstract3DSeries::Mesh meshType)
{
    auto model = new QQuick3DModel();
    model->setParent(this);
    model->setParentItem(QQuick3DViewport::scene());
    QString fileName = getMeshFileName(meshType);

    model->setSource(QUrl(fileName));
    return model;
}

void QQuickDataVisScatter::createInstancingRootItem(QAbstract3DSeries::Mesh meshType)
{
    m_instancingRootItem = createDataItem(meshType);
    m_instancingRootItem->setInstancing(m_instancing);
}

void QQuickDataVisScatter::createSelectionIndicator(QAbstract3DSeries::Mesh meshType)
{
    m_selectionIndicator = createDataItem(meshType);

    m_selectionIndicator->setVisible(false);
}

void QQuickDataVisScatter::removeDataItems(QList<QQuick3DModel *> &items)
{
    removeDataItems(items, items.count());
}

void QQuickDataVisScatter::removeDataItems(QList<QQuick3DModel *> &items, qsizetype count)
{
    for (int i = 0; i < count; ++i) {
        QQuick3DModel *item = items.takeLast();
        QQmlListReference materialsRef(item, "materials");
        if (materialsRef.size()) {
            QObject *material = materialsRef.at(0);
            delete material;
        }
        delete item;
    }
}

void QQuickDataVisScatter::addPointsToScatterModel(ScatterModel *graphModel, qsizetype count)
{
    QAbstract3DSeries::Mesh meshType = graphModel->series->mesh();
    for (int i = 0; i < count; i++) {
        QQuick3DModel *item = createDataItem(meshType);
        item->setPickable(true);
        item->setParent(graphModel->series);
        graphModel->dataItems.push_back(item);
    }
    m_scatterController->setSeriesVisualsDirty();
}

int QQuickDataVisScatter::sizeDifference(qsizetype size1, qsizetype size2)
{
    return size2 - size1;
}

QVector3D QQuickDataVisScatter::selectedItemPosition()
{
    if (m_selectedIndex == -1)
        return QVector3D();

    QVector3D position;
    if (m_scatterController->optimizationHints() == QAbstract3DGraphNG::OptimizationDefault)
        position = {0.0f, 0.0f, 0.0f};
    else if (m_scatterController->optimizationHints() == QAbstract3DGraphNG::OptimizationStatic)
        position = {0.0f, 0.0f, 0.0f};

    return position;
}

void QQuickDataVisScatter::fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh meshType)
{
    if (meshType != QAbstract3DSeries::MeshSphere && meshType != QAbstract3DSeries::MeshArrow
            && meshType != QAbstract3DSeries::MeshMinimal && meshType != QAbstract3DSeries::MeshPoint) {
        fileName.append(QStringLiteral("Full"));
    }
}

QString QQuickDataVisScatter::getMeshFileName(QAbstract3DSeries::Mesh meshType)
{
    QString fileName;
    QString smoothString = QStringLiteral("Smooth");
    switch (meshType) {
        case QAbstract3DSeries::MeshSphere:
        fileName = QStringLiteral("defaultMeshes/sphereMesh");
        break;
    case QAbstract3DSeries::MeshBar:
    case QAbstract3DSeries::MeshCube:
        fileName = QStringLiteral("defaultMeshes/barMesh");
        break;
    case QAbstract3DSeries::MeshPyramid:
        fileName = QStringLiteral("defaultMeshes/pyramidMesh");
        break;
    case QAbstract3DSeries::MeshCone:
        fileName = QStringLiteral("defaultMeshes/coneMesh");
        break;
    case QAbstract3DSeries::MeshCylinder:
        fileName = QStringLiteral("defaultMeshes/cylinderMesh");
        break;
    case QAbstract3DSeries::MeshBevelBar:
    case QAbstract3DSeries::MeshBevelCube:
        fileName = QStringLiteral("defaultMeshes/bevelBarMesh");
        break;
    case QAbstract3DSeries::MeshMinimal:
        fileName = QStringLiteral("defaultMeshes/minimalMesh");
        break;
    case QAbstract3DSeries::MeshArrow:
        fileName = QStringLiteral("defaultMeshes/arrowMesh");
        break;
    case QAbstract3DSeries::MeshPoint:
        qWarning("Points not supported yet");
        break;
    default:
        fileName = QStringLiteral("defaultMeshes/sphereMesh");
    }
    if (m_smooth && meshType != QAbstract3DSeries::MeshPoint)
        fileName += smoothString;

    fixMeshFileName(fileName, meshType);

    return fileName;
}

void QQuickDataVisScatter::handleSeriesChanged(QList<QAbstract3DSeries *> changedSeries)
{
    Q_UNUSED(changedSeries)
    // TODO: generate items and remove old items
}

// ------------------------------------------------------------------------------
QScatter3DSeries *QQuickDataVisScatter::selectedSeries() const
{
    return m_scatterController->selectedSeries();
}

void QQuickDataVisScatter::setSelectedItem(int index, QScatter3DSeries *series)
{
    m_scatterController->setSelectedItem(index, series);

    if (index != invalidSelectionIndex())
        itemLabel()->setVisible(true);
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

    auto graphModel = new ScatterModel;
    graphModel->series = series;
    graphModel->seriesTexture = nullptr;
    graphModel->highlightTexture = nullptr;
    m_scatterGraphs.push_back(graphModel);

    if (series->selectedItem() != invalidSelectionIndex())
        setSelectedItem(series->selectedItem(), series);
}

void QQuickDataVisScatter::removeSeries(QScatter3DSeries *series)
{
    m_scatterController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless

    // Find scattergraph model
    for (QList<ScatterModel *>::Iterator it = m_scatterGraphs.begin(); it != m_scatterGraphs.end(); ++it) {
        if ((*it)->series == series) {
            removeDataItems((*it)->dataItems);

            if ((*it)->seriesTexture)
                delete (*it)->seriesTexture;
            if ((*it)->highlightTexture)
                delete (*it)->highlightTexture;

            delete *it;
            m_scatterGraphs.erase(it);
        }
    }
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

void QQuickDataVisScatter::handleMousePressedEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button()) {
        if (selectionMode() == QAbstract3DGraphNG::SelectionItem) {
            const auto clickPosition = event->pos();
            QList<QQuick3DPickResult> results = pickAll(clickPosition.x(), clickPosition.y());
            if (!results.empty()) {
                for (const auto &result : std::as_const(results)) {
                    if (const auto &hit = result.objectHit()) {
                        if (hit == backgroundBB() || hit == background()) {
                            clearSelectionModel();
                            continue;
                        }
                        if (optimizationHints() == QAbstract3DGraphNG::OptimizationDefault) {
                            setSelected(hit);
                            break;
                        } else if (optimizationHints() == QAbstract3DGraphNG::OptimizationStatic) {
                            setSelected(hit, result.instanceIndex());
                            break;
                        }
                    }
                }
            } else {
                clearSelectionModel();
            }
        }
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

    float hAspectRatio = horizontalAspectRatio();

    QSizeF areaSize;
    auto *axisX = static_cast<QValue3DAxis *>(m_scatterController->axisX());
    auto *axisZ = static_cast<QValue3DAxis *>(m_scatterController->axisZ());

    if (hAspectRatio == 0.0f) {
        areaSize.setHeight(axisZ->max() - axisZ->min());
        areaSize.setWidth(axisX->max() - axisX->min());
    } else {
        areaSize.setHeight(1.0f);
        areaSize.setWidth(hAspectRatio);
    }

    float horizontalMaxDimension;
    float graphAspectRatio = aspectRatio();

    if (graphAspectRatio > 2.0f) {
        horizontalMaxDimension = 2.0f;
        m_scaleY = 2.0f / graphAspectRatio;
    } else {
        horizontalMaxDimension = graphAspectRatio;
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
    QList<QScatter3DSeries *> series = m_scatterController->scatterSeriesList();
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

        auto series = static_cast<QScatter3DSeries *>(m_selected->parent());

        // Find scattermodel
        ScatterModel *graphModel = nullptr;

        for (const auto &model : std::as_const(m_scatterGraphs)) {
            if (model->series == series) {
                graphModel = model;
                break;
            }
        }

        qsizetype index = graphModel->dataItems.indexOf(m_selected);
        setSelectedItem(index, series);

        m_scatterController->setSeriesVisualsDirty();
        m_scatterController->setSelectedItemChanged(true);
    }
}

void QQuickDataVisScatter::setSelected(QQuick3DModel *root, qsizetype index)
{
    if (index != m_scatterController->m_selectedItem) {
        QQuick3DObject *seriesRoot = root->parentItem();
        auto series = static_cast<QScatter3DSeries *>(seriesRoot->parent());

        m_scatterController->setSeriesVisualsDirty();
        setSelectedItem(index, series);
        m_scatterController->setSelectedItemChanged(true);
    }
}

void QQuickDataVisScatter::setSelected(qsizetype index)
{
    m_selectedIndex = index;
}

void QQuickDataVisScatter::clearSelectionModel()
{
    setSelectedItem(invalidSelectionIndex(), nullptr);

    itemLabel()->setVisible(false);
    m_scatterController->setSeriesVisualsDirty();
    m_selected = nullptr;
    m_previousSelected = nullptr;
}

void QQuickDataVisScatter::updateGraph()
{
    updatePointScaleSize();
    for (auto graphModel : std::as_const(m_scatterGraphs)) {
        if (m_scatterController->isDataDirty()) {
            if (graphModel->dataItems.count() != graphModel->series->dataProxy()->itemCount()) {
                int sizeDiff = sizeDifference(graphModel->dataItems.count(),
                                              graphModel->series->dataProxy()->itemCount());

                if (sizeDiff > 0)
                    addPointsToScatterModel(graphModel, sizeDiff);
                else
                    removeDataItems(graphModel->dataItems, qAbs(sizeDiff));
            }

            updateScatterGraphItemPositions(graphModel);
        }

        if (m_scatterController->isSeriesVisualsDirty())
            updateScatterGraphItemVisuals(graphModel);

        if (m_scatterController->m_selectedItemSeries == graphModel->series
                && m_scatterController->m_selectedItem != invalidSelectionIndex()) {
            QQuick3DModel *selectedModel = graphModel->dataItems.at(m_scatterController->m_selectedItem);
            itemLabel()->setPosition(selectedModel->position());
            QString label = m_scatterController->m_selectedItemSeries->itemLabel();
            itemLabel()->setProperty("labelText", label);
        }
    }

    if (m_scatterController->m_selectedItem == invalidSelectionIndex()) {
        itemLabel()->setVisible(false);
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

    QValue3DAxis *aX = axisX();
    QValue3DAxis *aY = axisY();
    QValue3DAxis *aZ = axisZ();

    aX->formatter()->d_ptr->recalculate();
    aY->formatter()->d_ptr->recalculate();
    aZ->formatter()->d_ptr->recalculate();

    m_pointScale = calculatePointScaleSize();

    // Notify changes to renderer
    if (m_scatterController->hasItemChanged()) {
        m_scatterController->setItemChanged(false);
        m_scatterController->clearChangedItems();
    }

    if (m_scatterController->hasSelectedItemChanged()) {
        if (m_scatterController->m_selectedItem != m_scatterController->invalidSelectionIndex()) {
            QString itemLabelText = m_scatterController->m_selectedItemSeries->itemLabel();
            itemLabel()->setProperty("labelText", itemLabelText);
        }
        m_scatterController->setSelectedItemChanged(false);
    }
}
QT_END_NAMESPACE
