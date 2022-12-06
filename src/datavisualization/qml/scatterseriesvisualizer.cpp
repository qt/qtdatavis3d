// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "scatter3dcontroller_p.h"
#include "scatterseriesvisualizer_p.h"
#include "qquickdatavisitem_p.h"
#include "qquickdatavisscatter_p.h"
#include "qvalue3daxis_p.h"
#include "axishelper_p.h"
#include "datavisquick3dtexturedata_p.h"
#include "q3dtheme.h"
#include <QtCore/QRandomGenerator>
#include <QtQuick3D/private/qquick3dtexture_p.h>
#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>
#include <QtQuick3D/private/qquick3dcustommaterial_p.h>
#include <QtQuick3D/private/qquick3dshaderutils_p.h>

const float itemScaler = 3.0f;
ScatterSeriesVisualizer::ScatterSeriesVisualizer(QObject *parent)
    : QObject(parent)
{

}

ScatterSeriesVisualizer::~ScatterSeriesVisualizer()
{
    removeDataItems();
    m_itemList.clear();
    removeDummyDataItems();
    m_dummyItemList.clear();
    m_itemCount = 0;

    if (m_instancing)
        delete m_instancing;
}

void ScatterSeriesVisualizer::handleSeriesMeshChanged(QAbstract3DSeries::Mesh mesh)
{
    m_meshType = mesh;
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        removeDataItems();
        generatePoints(m_itemCount);
    } else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationStatic) {
        removeDummyDataItems();
        resetSelection();
        m_instancingRootItem->setSource(QUrl(getMeshFileName()));
        m_selectionIndicator->setSource(QUrl(getMeshFileName()));
        m_controller->markDataDirty();
        m_controller->markSeriesVisualsDirty();
        generatePoints(m_itemCount);
    }
}

void ScatterSeriesVisualizer::handleOptimizationHintsChanged(QAbstract3DGraph::OptimizationHints hints)
{
    Q_UNUSED(hints);
    setup();
}

void ScatterSeriesVisualizer::handleMeshSmoothChanged(bool enable)
{
    m_smooth = enable;

    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        removeDataItems();
        generatePoints(m_itemCount);
    } else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationStatic) {
        removeDummyDataItems();
        resetSelection();
        m_instancingRootItem->setSource(QUrl(getMeshFileName()));
        m_selectionIndicator->setSource(QUrl(getMeshFileName()));
        m_controller->markDataDirty();
        m_controller->markSeriesVisualsDirty();
        generatePoints(m_itemCount);
    }
}

void ScatterSeriesVisualizer::handleItemCountChanged(int count)
{
    m_itemCount = count;
    removeDataItems();
    generatePoints(count);
}

void ScatterSeriesVisualizer::setScaleZ(float newScaleZ)
{
    m_scaleZ = newScaleZ;
}

void ScatterSeriesVisualizer::setScaleY(float newScaleY)
{
    m_scaleY = newScaleY;
}

void ScatterSeriesVisualizer::setScaleX(float newScaleX)
{
    m_scaleX = newScaleX;
}

void ScatterSeriesVisualizer::setDotSizedScale(float newDotSizedScale)
{
    m_dotSizedScale = newDotSizedScale;
}

void ScatterSeriesVisualizer::setQml(QQuickDataVisScatter *newQml)
{
    m_qml = newQml;
}

void ScatterSeriesVisualizer::setController(Scatter3DController *newController)
{
    m_controller = newController;
}

void ScatterSeriesVisualizer::createParent()
{
    m_visualizerRoot.reset(new QObject());
}

bool ScatterSeriesVisualizer::pointsGenerated() const
{
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault)
        return m_itemCount == m_itemList.size();
    else
        return m_instancingRootItem ? true : false;
}

void ScatterSeriesVisualizer::setup()
{
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationHint::OptimizationDefault) {
        removeDummyDataItems();
        if (m_instancing)
            delete m_instancing;
        if (m_instancingRootItem)
            delete m_instancingRootItem;
    } else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationHint::OptimizationStatic) {
        removeDataItems();
        m_instancing = new DatavisQuick3DInstancing();
        createInstancingRootItem();
        createSelectionIndicator();
    }
}

void ScatterSeriesVisualizer::connectSeries(QScatter3DSeries *series)
{
    m_meshType = series->mesh();
    m_smooth = series->isMeshSmooth();

    m_seriesRootItem = createSeriesRoot();
    m_seriesRootItem->setParent(series);
    QObject::connect(series, &QScatter3DSeries::meshChanged, this, &ScatterSeriesVisualizer::handleSeriesMeshChanged);
    QObject::connect(series, &QScatter3DSeries::meshSmoothChanged, this, &ScatterSeriesVisualizer::handleMeshSmoothChanged);
    QObject::connect(m_controller, &Abstract3DController::optimizationHintsChanged, this, &ScatterSeriesVisualizer::handleOptimizationHintsChanged);
    QObject::connect(series->dataProxy(), &QScatterDataProxy::itemCountChanged, this, &ScatterSeriesVisualizer::handleItemCountChanged);
}

void ScatterSeriesVisualizer::disconnectSeries(QScatter3DSeries *series)
{
    QObject::disconnect(series, 0, this, 0);
}

void ScatterSeriesVisualizer::generatePoints(int count)
{
    if (!m_visualizerRoot)
        createParent();
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        if (count > 0)
            m_itemList.resize(count);

        for (int i = 0; i < count; i++) {
            auto item = createDataItem();
            item->setPickable(true);
            item->setParentItem(m_seriesRootItem);
            m_itemList[i] = item;
        }
        m_controller->markDataDirty();
    } else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationStatic) {
        if (m_controller->selectionMode() != QAbstract3DGraph::SelectionNone)
            m_instancingRootItem->setPickable(true);
    }
    m_controller->markSeriesVisualsDirty();
}

qsizetype ScatterSeriesVisualizer::getItemIndex(QQuick3DModel *item)
{
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault)
        return m_itemList.indexOf(item);

    return -1;
}

void ScatterSeriesVisualizer::setSelected(qsizetype index)
{
    m_selectedIndex = index;
}

void ScatterSeriesVisualizer::clearSelection()
{
    resetSelection();
    m_selectedIndex = -1;
}

void ScatterSeriesVisualizer::resetSelection()
{
    if (m_selectedIndex != -1) {
        if (m_controller->optimizationHints()==QAbstract3DGraph::OptimizationStatic) {
            m_selectionActive = false;
            m_selectionIndicator->setVisible(false);
        }
    }
}

void ScatterSeriesVisualizer::updateItemPositions(QScatterDataProxy *dataProxy)
{

    float itemSize = dataProxy->series()->itemSize() / itemScaler;
    m_meshRotation = dataProxy->series()->meshRotation();

    if (itemSize == 0.0f)
        itemSize = m_dotSizedScale;

    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        if (dataProxy->itemCount() != m_itemList.size())
            qWarning()<<__func__<<"Item count differs from itemList count";
        for (int i = 0; i < dataProxy->itemCount(); ++i) {
            auto item = dataProxy->itemAt(i);
            auto dataPoint = m_itemList.at(i);

            auto dotPos = item->position();
            auto dotRot = item->rotation();
            auto posX = m_qml->axisX()->positionAt(dotPos.x()) * m_qml->scale().x() + m_qml->translate().x();
            auto posY = m_qml->axisY()->positionAt(dotPos.y()) * m_qml->scale().y() + m_qml->translate().y();
            auto posZ = m_qml->axisZ()->positionAt(dotPos.z()) * m_qml->scale().z() + m_qml->translate().z();

            dataPoint->setPosition(QVector3D(posX, posY, posZ));

            dataPoint->setRotation(dotRot * m_meshRotation);
            dataPoint->setScale(QVector3D(itemSize, itemSize, itemSize));
        }
    } else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationStatic)  {
        int count = dataProxy->itemCount();
        QList<DataItemHolder> positions;
        positions.resize(count);

        for (int i = 0; i < count; i++) {
            auto item = dataProxy->itemAt(i);
            auto dotPos = item->position();

            auto posX = m_qml->axisX()->positionAt(dotPos.x()) * m_qml->scale().x() + m_qml->translate().x();
            auto posY = m_qml->axisY()->positionAt(dotPos.y()) * m_qml->scale().y() + m_qml->translate().y();
            auto posZ = m_qml->axisZ()->positionAt(dotPos.z()) * m_qml->scale().z() + m_qml->translate().z();

            DataItemHolder dih;
            dih.position = {posX,posY,posZ};
            dih.rotation = item->rotation() * m_meshRotation;
            dih.scale = {itemSize, itemSize, itemSize};

            if (m_dummyItemList.size() > 0) {
                // Update dummy item positions as well
                auto dummyItem = m_dummyItemList.at(i);
                dummyItem->setPosition({posX,posY,posZ});
                dummyItem->setRotation(dih.rotation);
                dummyItem->setScale(dih.scale * 1.05f);
            }
            positions[i] = dih;
        }
        m_instancing->setDataArray(positions);
    }
}

void ScatterSeriesVisualizer::updateItemVisuals(QScatter3DSeries *series)
{
    bool useGradient = series->d_ptr->isUsingGradient();
    auto itemCount = series->dataProxy()->itemCount();

    if (useGradient) {
        if (!m_hasTexture) {
            m_texture = createTexture();
            m_texture->setParent(this);
            m_hasTexture = true;
        }
        auto gradient = series->baseGradient();
        auto textureData = static_cast<DatavisQuick3DTextureData *>(m_texture->textureData());
        textureData->createGradient(gradient);

        if (!m_hasHighLightTexture) {
            m_highlightTexture = createTexture();
            m_highlightTexture->setParent(this);
            m_hasHighLightTexture = true;
        }
        auto highlightGradient = series->singleHighlightGradient();
        auto highlightTextureData = static_cast<DatavisQuick3DTextureData *>(m_highlightTexture->textureData());
        highlightTextureData->createGradient(highlightGradient);
    } else {
        if (m_hasTexture) {
            m_texture->deleteLater();
            m_hasTexture = false;
        }
        if (m_hasHighLightTexture) {
            m_highlightTexture->deleteLater();
            m_hasHighLightTexture = false;
        }
    }

    bool rangeGradient = (useGradient && series->d_ptr->m_colorStyle == Q3DTheme::ColorStyleRangeGradient) ? true : false;

    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        if (itemCount != m_itemList.size())
            qWarning()<<__func__<<"Item count differs from itemList count";

        if (!rangeGradient) {
            for (int i = 0; i < itemCount; i++) {
                auto obj = static_cast<QQuick3DModel *>(m_itemList.at(i));

                updateItemMaterial(obj, useGradient, rangeGradient);

                updatePrincipledMaterial(obj, series->baseColor(), useGradient);
            }
            if (m_selectedIndex != -1) {
                auto selectedItem = m_itemList.at(m_selectedIndex);
                updatePrincipledMaterial(selectedItem, series->singleHighlightColor(), useGradient, true);
            }
        } else {
            for (int i = 0; i < itemCount; i++) {
                auto obj = static_cast<QQuick3DModel *>(m_itemList.at(i));
                updateItemMaterial(obj, useGradient, rangeGradient);
                updateCustomMaterial(obj);
            }

            if (m_selectedIndex != -1) {
                auto obj = m_itemList.at(m_selectedIndex);
                auto itemLabel = series->itemLabel();

                updateCustomMaterial(obj, true);
            }
        }
    } else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationStatic) {
        updateItemInstancedMaterial(m_instancingRootItem, useGradient, rangeGradient);
        m_instancing->setRangeGradient(rangeGradient);
        if (!rangeGradient) {
            updatePrincipledMaterial(m_instancingRootItem, series->baseColor(), useGradient, rangeGradient);
        } else {
            float rangeGradientYScaler = 0.5f / m_scaleY;

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
            updatePrincipledMaterial(m_selectionIndicator, series->singleHighlightColor(), useGradient, true);
        } else {
            // Rangegradient
            updateInstancedCustomMaterial(m_selectionIndicator, true);
        }

        if (m_selectedIndex != -1 && !m_selectionActive) {
            auto dih = m_instancing->dataArray().at(m_selectedIndex);
            m_selectionIndicator->setPosition(dih.position);
            m_selectionIndicator->setRotation(dih.rotation);
            m_selectionIndicator->setScale(dih.scale*1.1f);
            m_selectionIndicator->setVisible(true);
            m_qml->m_itemLabel->setPosition(m_selectionIndicator->position());
            m_selectionActive = true;
            m_instancing->markDataDirty();
        }
    }
}

void ScatterSeriesVisualizer::updateItemMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient)
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
                auto customMaterial = m_qml->createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterial"));
                auto oldPrincipledMaterial = materialsRef.at(0);
                materialsRef.replace(0, customMaterial);
                delete oldPrincipledMaterial;
            }
        } else {
            auto customMaterial = m_qml->createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterial"));
            materialsRef.append(customMaterial);
        }
    }
}

void ScatterSeriesVisualizer::updateItemInstancedMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient)
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
                auto customMaterial = m_qml->createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterialInstancing"));
                auto oldPrincipledMaterial = materialsRef.at(0);
                materialsRef.replace(0, customMaterial);
                delete oldPrincipledMaterial;
            }
        } else {
            auto customMaterial = m_qml->createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterialInstancing"));
            materialsRef.append(customMaterial);
        }
    }
}

void ScatterSeriesVisualizer::updateSelectionIndicatorMaterial(bool useGradient, bool rangeGradient)
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
                auto customMaterial = m_qml->createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterial"));
                materialsRef.replace(0, customMaterial);
                delete old;
            }
        } else {
            auto customMaterial = m_qml->createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterial"));
            materialsRef.append(customMaterial);
        }
    }
}

void ScatterSeriesVisualizer::updateInstancedCustomMaterial(QQuick3DModel *model, bool isHighlight)
{
    QQmlListReference materialsRef(model, "materials");

    // Rangegradient
    auto customMaterial = static_cast<QQuick3DCustomMaterial *>(materialsRef.at(0));

    QVariant textureInputAsVariant = customMaterial->property("custex");
    QQuick3DShaderUtilsTextureInput *textureInput = textureInputAsVariant.value<QQuick3DShaderUtilsTextureInput *>();

    if (isHighlight) {
        textureInput->setTexture(m_highlightTexture);

        if (m_selectedIndex != -1 && !m_selectionActive)
            m_selectedGradientPos = m_instancing->customData().takeAt(m_selectedIndex);

        customMaterial->setProperty("gradientPos", m_selectedGradientPos);
    } else {
        textureInput->setTexture(m_texture);
    }
}

void ScatterSeriesVisualizer::updateCustomMaterial(QQuick3DModel *item, bool isHighlight)
{
    QQmlListReference materialsRef(item, "materials");
    auto customMaterial = static_cast<QQuick3DCustomMaterial *>(materialsRef.at(0));
    QVariant textureInputAsVariant = customMaterial->property("custex");
    QQuick3DShaderUtilsTextureInput *textureInput = textureInputAsVariant.value<QQuick3DShaderUtilsTextureInput *>();

    if (!isHighlight)
        textureInput->setTexture(m_texture);
    else
        textureInput->setTexture(m_highlightTexture);

    float rangeGradientYScaler = 0.5f / m_scaleY;
    float value = (item->y() + m_scaleY) * rangeGradientYScaler;
    customMaterial->setProperty("gradientPos", value);
}

void ScatterSeriesVisualizer::updatePrincipledMaterial(QQuick3DModel *model, const QColor &color, bool useGradient, bool isHighlight)
{
    QQmlListReference materialsRef(model, "materials");
    auto principledMaterial = static_cast<QQuick3DPrincipledMaterial *>(materialsRef.at(0));

    if (useGradient) {
        principledMaterial->setBaseColor(QColor(Qt::white));
        if (!isHighlight)
            principledMaterial->setBaseColorMap(m_texture);
        else
            principledMaterial->setBaseColorMap(m_highlightTexture);
    } else {
        principledMaterial->setBaseColor(color);
    }
}

void ScatterSeriesVisualizer::createDummyDataItems(int count)
{
    if (!dummiesCreated && count > 0) {
        if (!m_instancingRootItem)
            qWarning("No instancing root item");
        m_dummyItemList.resize(count);

        for (int i = 0; i < count; i++) {
            auto dummyItem = new QQuick3DModel();
            dummyItem->setParent(m_seriesRootItem);
            dummyItem->setParentItem(m_seriesRootItem);
            dummyItem->setSource(m_instancingRootItem->source());
            dummyItem->setScale(m_instancingRootItem->scale() * 1.05f);
            dummyItem->setPickable(true);
            dummyItem->setVisible(false);

            m_dummyItemList[i] = dummyItem;
        }
        dummiesCreated = true;
    }
}

QQuick3DTexture *ScatterSeriesVisualizer::createTexture()
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

QQuick3DModel *ScatterSeriesVisualizer::createDataItemModel(QAbstract3DSeries::Mesh meshType)
{
    QQmlComponent component(qmlEngine(m_qml));
    switch (meshType) {
    case QAbstract3DSeries::MeshSphere:
        component.loadUrl(QUrl::fromLocalFile((QStringLiteral(":/datapointModels/DatapointSphere"))));
        break;
    case QAbstract3DSeries::MeshCube:
        component.loadUrl(QUrl::fromLocalFile((QStringLiteral(":/datapointModels/DatapointCube"))));
        break;
    default:
        component.loadUrl(QUrl::fromLocalFile((QStringLiteral(":/datapointModels/DatapointSphere"))));
    }
    auto item = static_cast<QQuick3DModel *>(component.create());
    item->setParent(m_seriesRootItem);
    item->setParentItem(m_seriesRootItem);
    return item;
}

QQmlComponent *ScatterSeriesVisualizer::createRepeaterDelegate(QAbstract3DSeries::Mesh meshType)
{
    QQmlComponent component(qmlEngine(m_qml));
    switch (meshType) {
    case QAbstract3DSeries::MeshSphere:
        component.loadUrl(QUrl::fromLocalFile((QStringLiteral(":/datapointModels/DatapointSphere"))));
        break;
    case QAbstract3DSeries::MeshCube:
        component.loadUrl(QUrl::fromLocalFile((QStringLiteral(":/datapointModels/DatapointCube"))));
        break;
    default:
        component.loadUrl(QUrl::fromLocalFile((QStringLiteral(":/datapointModels/DatapointSphere"))));
    }

    return static_cast<QQmlComponent *>(component.create());
}

QQuick3DNode *ScatterSeriesVisualizer::createSeriesRoot()
{
    auto model = new QQuick3DNode();

    model->setParentItem(m_qml->rootNode());
    return model;
}

QQuick3DModel *ScatterSeriesVisualizer::createDataItem()
{
    auto model = new QQuick3DModel();
    model->setParent(m_visualizerRoot.get());
    model->setParentItem(m_seriesRootItem);
    QString fileName = getMeshFileName();

    model->setSource(QUrl(fileName));
    return model;
}

void ScatterSeriesVisualizer::createInstancingRootItem()
{
    m_instancingRootItem = createDataItem();
    m_instancingRootItem->setInstancing(m_instancing);
}

void ScatterSeriesVisualizer::createSelectionIndicator()
{
    m_selectionIndicator = createDataItem();

    m_selectionIndicator->setVisible(false);
}

void ScatterSeriesVisualizer::removeDataItems()
{
    for (const auto &item : std::as_const(m_itemList)) {
        QQmlListReference materialsRef(item, "materials");
        if (materialsRef.size()) {
            auto material = materialsRef.at(0);
            delete material;
        }
        delete item;
    }
}

void ScatterSeriesVisualizer::removeDummyDataItems()
{
    for (const auto &item : std::as_const(m_dummyItemList))
        delete item;
    dummiesCreated = false;
}

void ScatterSeriesVisualizer::createItemLabel()
{
    QQmlComponent component(qmlEngine(m_qml), QStringLiteral(":/axis/ItemLabel"));
    QQuick3DNode *labelNode = qobject_cast<QQuick3DNode *>(component.create());
    labelNode->setParent(m_qml->rootNode());
    labelNode->setParentItem(m_qml->rootNode());
    m_itemLabel = labelNode;
}

QVector3D ScatterSeriesVisualizer::selectedItemPosition()
{
    if (m_selectedIndex == -1)
        return QVector3D();

    QVector3D position;
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault)
        position = m_itemList.at(m_selectedIndex)->position();
    else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationStatic)
        position = m_selectionIndicator->position();

    return position;
}

void ScatterSeriesVisualizer::fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh meshType)
{
    if (meshType != QAbstract3DSeries::MeshSphere && meshType != QAbstract3DSeries::MeshArrow
            && meshType != QAbstract3DSeries::MeshMinimal && meshType != QAbstract3DSeries::MeshPoint) {
        fileName.append(QStringLiteral("Full"));
    }
}

QString ScatterSeriesVisualizer::getMeshFileName()
{
    QString fileName;
    QString smoothString = QStringLiteral("Smooth");
    switch (m_meshType) {
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
    if (m_smooth && m_meshType != QAbstract3DSeries::MeshPoint)
        fileName += smoothString;

    fixMeshFileName(fileName, m_meshType);

    return fileName;
}
