// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickbarseriesvisualizer_p.h"
#include "bars3dcontroller_p.h"
#include "qquickdatavisitem_p.h"
#include "datavisquick3dtexturedata_p.h"
#include "q3dtheme.h"
#include "qabstract3dseries_p.h"

#include <QtCore/QRandomGenerator>
#include <QtQuick3D/private/qquick3dtexture_p.h>
#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>
#include <QtQuick3D/private/qquick3dcustommaterial_p.h>
#include <QtQuick3D/private/qquick3dshaderutils_p.h>


QQuickBarSeriesVisualizer::QQuickBarSeriesVisualizer(QObject *parent)
    : QObject(parent),
      //m_sliceCache(0),
      m_barsGenerated(false),
      m_sliceTitleItem(0),
      m_seriesScaleX(0.0f),
      m_seriesScaleZ(0.0f),
      m_visibleSeriesCount(0),
      m_seriesStep(0.0f),
      m_seriesStart(0.0f),
      m_zeroPosition(0.0f),
      m_keepSeriesUniform(false),
      m_haveUniformColorSeries(false),
      m_haveGradientSeries(false)
{

}

QQuickBarSeriesVisualizer::~QQuickBarSeriesVisualizer()
{
    if (m_texture)
        m_texture->deleteLater();

    removeDataItems();

    removeDummyDataItems();
    m_dummyItemList.clear();

    if (m_instancing)
        delete m_instancing;
    if (m_seriesRootItem)
        delete m_seriesRootItem;
}

void QQuickBarSeriesVisualizer::handleSeriesMeshChanged(QAbstract3DSeries::Mesh mesh)
{
    m_meshType = mesh;
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        removeDataItems();
//        generateBars(m_seriesList);
    } else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationStatic) {
        removeDummyDataItems();
        resetSelection();
        m_instancingRootItem->setSource(QUrl(getMeshFileName()));
        m_selectionIndicator->setSource(QUrl(getMeshFileName()));
        m_controller->markDataDirty();
        m_controller->markSeriesVisualsDirty();
//        generateBars(m_seriesList);
    }
}

void QQuickBarSeriesVisualizer::handleOptimizationHintsChanged(QAbstract3DGraph::OptimizationHints hints)
{
    Q_UNUSED(hints);
    setup();
}

void QQuickBarSeriesVisualizer::handleMeshSmoothChanged(bool enable)
{
    m_smooth = enable;

    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        removeDataItems();
//        generateBars(m_seriesList);
    } else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationStatic) {
        removeDummyDataItems();
        resetSelection();
        m_instancingRootItem->setSource(QUrl(getMeshFileName()));
        m_selectionIndicator->setSource(QUrl(getMeshFileName()));
        m_controller->markDataDirty();
        m_controller->markSeriesVisualsDirty();
//        generateBars(m_seriesList);
    }
}

void QQuickBarSeriesVisualizer::handleRowCountChanged()
{
    QCategory3DAxis *categoryAxisZ = static_cast<QCategory3DAxis *>(m_controller->axisZ());
    m_dataVisBars->segmentLineRepeaterZ()->setModel(categoryAxisZ->labels().size());
    m_dataVisBars->repeaterZ()->setModel(categoryAxisZ->labels().size());
    m_controller->handleAxisLabelsChangedBySender(m_controller->axisZ());
    m_dataVisBars->updateGrid();
    m_dataVisBars->updateLabels();
}

void QQuickBarSeriesVisualizer::handleColCountChanged()
{
    QCategory3DAxis *categoryAxisX = static_cast<QCategory3DAxis *>(m_controller->axisX());
    m_dataVisBars->segmentLineRepeaterX()->setModel(categoryAxisX->labels().size());
    m_dataVisBars->repeaterX()->setModel(categoryAxisX->labels().size());
    m_controller->handleAxisLabelsChangedBySender(m_controller->axisX());
    m_dataVisBars->updateGrid();
    m_dataVisBars->updateLabels();
}

void QQuickBarSeriesVisualizer::createParent()
{
    m_visualizerRoot.reset(new QObject());
}

void QQuickBarSeriesVisualizer::handleSeriesChanged(QBar3DSeries *series)
{
    int size = m_seriesList.size();
    m_seriesList.insert(size, series);
}

void QQuickBarSeriesVisualizer::setup()
{
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationHint::OptimizationDefault) {
        removeDummyDataItems();
        if (m_instancing)
            delete m_instancing;
        if (m_instancingRootItem)
            delete m_instancingRootItem;
    }
    else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationHint::OptimizationStatic) {
        removeDataItems();
        m_instancing = new DatavisQuick3DInstancing();
        createInstancingRootItem();
        createSelectionIndicator();
    }
}

void QQuickBarSeriesVisualizer::handleSeriesConnected()
{
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {

    } else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationStatic) {
        createInstancingRootItem();
        createSelectionIndicator();
    }
}

void QQuickBarSeriesVisualizer::connectSeries(QBar3DSeries *series)
{
    m_meshType = series->mesh();
    m_smooth = series->isMeshSmooth();

    m_seriesRootItem = createSeriesRoot();
    m_seriesRootItem->setParent(series);
    handleSeriesChanged(series);
    QObject::connect(series, &QBar3DSeries::meshChanged, this, &QQuickBarSeriesVisualizer::handleSeriesMeshChanged);
    QObject::connect(series, &QBar3DSeries::meshSmoothChanged, this, &QQuickBarSeriesVisualizer::handleMeshSmoothChanged);
    QObject::connect(m_controller, &Abstract3DController::optimizationHintsChanged, this, &QQuickBarSeriesVisualizer::handleOptimizationHintsChanged);
    QObject::connect(series->dataProxy(), &QBarDataProxy::rowCountChanged, this, &QQuickBarSeriesVisualizer::handleRowCountChanged);
    QObject::connect(series->dataProxy(), &QBarDataProxy::colCountChanged, this, &QQuickBarSeriesVisualizer::handleColCountChanged);
    handleSeriesConnected();
}

void QQuickBarSeriesVisualizer::disconnectSeries(QBar3DSeries *series)
{
    QObject::disconnect(series, 0, this, 0);
}

void QQuickBarSeriesVisualizer::generateBars(const QList<QAbstract3DSeries *> &seriesList)
{
    if (!m_visualizerRoot)
        createParent();

    int minRow = m_controller->m_axisZ->min();
    int dataRowCount = 0;
    int dataColCount = 0;

    m_visibleSeriesCount = 0;
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        int seriesCount = seriesList.size();
        for (int i = 0; i < seriesCount; i++) {
            QAbstract3DSeries *series = seriesList.at(i);
            QBar3DSeries *currentSeries = static_cast<QBar3DSeries *>(series);
            const QBarDataArray *array = currentSeries->dataProxy()->array();
            QBarDataProxy *dataProxy = currentSeries->dataProxy();
            dataRowCount = dataProxy->rowCount();
            if (dataRowCount == 0)
                continue;
            dataColCount = dataProxy->colCount();
            int dataRowIndex = minRow;

            while (dataRowIndex < dataRowCount) {
                const QBarDataRow *dataRow = array->at(dataRowIndex);
                Q_ASSERT(dataRow->size() == dataColCount);
                for (int i = 0; i < dataColCount; i++) {
                    QBarDataItem *dataItem = const_cast <QBarDataItem *> (&(dataRow->at(i)));
                    QQuick3DModel *model = m_modelList.key(dataItem);
                    if (!model) {
                        model = createDataItem();
                        model->setPickable(true);
                        model->setParentItem(m_seriesRootItem);
                    }
                    m_modelList.insert(model,dataItem);
                    m_controller->markDataDirty();
                    m_controller->markSeriesVisualsDirty();

                    m_barsGenerated = true;
                }
                 m_visibleSeriesCount++;
                ++dataRowIndex;
            }
        }
    }
}

QPoint QQuickBarSeriesVisualizer::getItemIndex(QQuick3DModel *item)
{
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault){
        int row = item->position().z();
        int col = item->position().x();

        return QPoint(row, col);
    }
    return QPoint(-1, -1);
}

void QQuickBarSeriesVisualizer::setSelected(QPoint index)
{
    m_selectedRowIndex = index.x();
    m_selectedColIndex = index.y();
}

void QQuickBarSeriesVisualizer::clearSelection()
{
    resetSelection();
    m_selectedRowIndex = -1;
    m_selectedColIndex = -1;
}

void QQuickBarSeriesVisualizer::resetSelection()
{
    if (m_selectedRowIndex != -1 || m_selectedColIndex != -1) {
        if (m_controller->optimizationHints()==QAbstract3DGraph::OptimizationStatic) {
            auto positions = m_instancing->dataArray();
            DataItemHolder dih;
            dih.position = m_selectionIndicator->position();
            dih.rotation = m_selectionIndicator->rotation();
            dih.scale = m_selectionIndicator->scale();
            positions.insert(m_selectedRowIndex, m_selectedColIndex, dih);
            m_selectionActive = false;
            m_selectionIndicator->setVisible(false);
            m_instancing->setDataArray(positions);
        }
    }
}

void QQuickBarSeriesVisualizer::updateData(QBarDataProxy *dataProxy)
{
    m_seriesScaleX = 1.0f / float(m_visibleSeriesCount);
    m_seriesStep = 1.0f / float(m_visibleSeriesCount);
    m_seriesStart = -((float(m_visibleSeriesCount) - 1.0f) / 2.0f)
            * (m_seriesStep - (m_seriesStep * m_dataVisBars->m_cachedBarSeriesMargin.width()));

    if (m_keepSeriesUniform)
        m_seriesScaleZ = m_seriesScaleX;
    else
        m_seriesScaleZ = 1.0f;

    m_dataVisBars->calculateSceneScalingFactors();
    m_meshRotation = dataProxy->series()->meshRotation();

    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        int rowCount = dataProxy->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            auto *dataRow = dataProxy->rowAt(i);
            int colCount = dataRow->size();
            for (int j = 0; j < colCount; ++j) {
                QBarDataItem *item = const_cast<QBarDataItem *>(&(dataRow->at(j)));
                QQuick3DModel *model = m_modelList.key(item);
                float value = item->value();
                float heightValue = m_helperAxisY->itemPositionAt(value);
                float angle = item->rotation();

                float rowPos = i * (m_dataVisBars->m_cachedBarSpacing.height());
                float colPos = j * (m_dataVisBars->m_cachedBarSpacing.width());
                float xPos = (colPos - ((float)m_dataVisBars->m_rowWidth)) / (float)m_dataVisBars->m_rowWidth;
                float zPos = (((float)m_dataVisBars->m_columnDepth) - rowPos) / ((float)m_dataVisBars->m_columnDepth);

                if (angle) {
                    model->setRotation(
                                QQuaternion::fromAxisAndAngle(
                                    upVector, angle));
                } else {
                    model->setRotation(identityQuaternion);
                }
                model->setPosition(QVector3D((xPos + 0.075) * m_dataVisBars->m_xScaleFactor, heightValue - m_dataVisBars->m_yScale, (zPos - 0.15) * m_dataVisBars->m_zScaleFactor));
                model->setScale(QVector3D(m_dataVisBars->m_xScale, heightValue , m_dataVisBars->m_zScale));
            }
        }
    }
}

void QQuickBarSeriesVisualizer::updateItemVisuals(QBar3DSeries *series)
{
    bool useGradient = series->d_ptr->isUsingGradient();
    QBarDataProxy *dataProxy = series->dataProxy();
    auto rowCount = dataProxy->rowCount();
    int colCount;

    if (rowCount == 0 )
        colCount = 0;
    else
        colCount = dataProxy->colCount();

    if (useGradient) {
        if (!m_hasTexture) {
            m_texture = createTexture();
            m_hasTexture = true;
        }
        auto gradient = series->baseGradient();
        auto textureData = static_cast<DatavisQuick3DTextureData *>(m_texture->textureData());
        textureData->createGradient(gradient);

        if (!m_hasHighlightTexture) {
            m_highlightTexture = createTexture();
            m_hasHighlightTexture = true;
        }
        auto highlightGradient = series->singleHighlightGradient();
        auto highlightTextureData = static_cast<DatavisQuick3DTextureData *>(m_highlightTexture->textureData());
        highlightTextureData->createGradient(highlightGradient);
    } else {
        if (m_hasTexture) {
            m_texture->deleteLater();
            m_hasTexture = false;
        }
        if (m_hasHighlightTexture) {
            m_highlightTexture->deleteLater();
            m_hasHighlightTexture = false;
        }
    }

    bool rangeGradient = (useGradient && series->d_ptr->m_colorStyle == Q3DTheme::ColorStyleRangeGradient) ? true : false;

    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        if (!rangeGradient) {
            for (int i = 0; i < rowCount; i++) {
                for (int j = 0; j < colCount; j++) {
                    QBarDataItem *item = const_cast<QBarDataItem *>(dataProxy->itemAt(i, j));
                    auto obj = m_modelList.key(item);
                    updateItemMaterial(obj, useGradient, rangeGradient);
                    updatePrincipledMaterial(obj, series->baseColor(), useGradient);
                }
            }
            if (m_selectedRowIndex != -1 || m_selectedColIndex != -1) {
                QBarDataItem *item = const_cast<QBarDataItem *>(dataProxy->itemAt(m_selectedRowIndex, m_selectedColIndex));
                auto selectedItem = m_modelList.key(item);
                updatePrincipledMaterial(selectedItem, series->singleHighlightColor(), useGradient, true);
            }
        } else {
            for (int i = 0; i < rowCount; i++) {
                for (int j = 0; j < colCount; j++) {
                    QBarDataItem *item = const_cast<QBarDataItem *>(dataProxy->itemAt(i, j));
                    auto obj = m_modelList.key(item);
                    updateItemMaterial(obj, useGradient, rangeGradient);
                    updateCustomMaterial(obj);
                }
            }
            if (m_selectedRowIndex != -1 && m_selectedColIndex != -1) {
                QBarDataItem *item = const_cast<QBarDataItem *>(dataProxy->itemAt(m_selectedRowIndex, m_selectedColIndex));
                auto selectedItem = m_modelList.key(item);
                auto itemLabel = series->itemLabel();
                updateCustomMaterial(selectedItem, true);
            }
        }
    }
}

void QQuickBarSeriesVisualizer::updateItemMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient)
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

void QQuickBarSeriesVisualizer::updateItemInstancedMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient)
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

void QQuickBarSeriesVisualizer::updateInstancedCustomMaterial(QQuick3DModel *model, bool isHighlight)
{
    QQmlListReference materialsRef(model, "materials");

    // Rangegradient
    auto customMaterial = static_cast<QQuick3DCustomMaterial *>(materialsRef.at(0));

    QVariant textureInputAsVariant = customMaterial->property("custex");
    QQuick3DShaderUtilsTextureInput *textureInput = textureInputAsVariant.value<QQuick3DShaderUtilsTextureInput *>();

    if (isHighlight) {
        textureInput->setTexture(m_highlightTexture);

        //STATIC
        if ((m_selectedRowIndex != -1 || m_selectedColIndex != -1) && !m_selectionActive)
            m_selectedGradientPos = m_instancing->customData().takeAt(m_selectedRowIndex);

        customMaterial->setProperty("gradientPos", m_selectedGradientPos);
    } else {
        textureInput->setTexture(m_texture);
    }
}

void QQuickBarSeriesVisualizer::updateSelectionIndicatorMaterial(bool useGradient, bool rangeGradient)
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

void QQuickBarSeriesVisualizer::updateCustomMaterial(QQuick3DModel *item, bool isHighlight)
{
    QQmlListReference materialsRef(item, "materials");
    auto customMaterial = static_cast<QQuick3DCustomMaterial *>(materialsRef.at(0));
    QVariant textureInputAsVariant = customMaterial->property("custex");
    QQuick3DShaderUtilsTextureInput *textureInput = textureInputAsVariant.value<QQuick3DShaderUtilsTextureInput *>();

    if (!isHighlight)
        textureInput->setTexture(m_texture);
    else
        textureInput->setTexture(m_highlightTexture);

    float rangeGradientYScaler = 0.5f / m_dataVisBars->m_yScale;
    float value = (item->y() + m_dataVisBars->m_yScale) * rangeGradientYScaler;
    customMaterial->setProperty("gradientPos", value);
}

void QQuickBarSeriesVisualizer::updatePrincipledMaterial(QQuick3DModel *model, const QColor &color, bool useGradient, bool isHighlight)
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

void QQuickBarSeriesVisualizer::createDummyDataItems(int count)
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
            dummyItem->setScale(m_instancingRootItem->scale()*1.05f);
            dummyItem->setPickable(true);

            m_dummyItemList[i] = dummyItem;
        }
        dummiesCreated = true;
    }
}

QQuick3DTexture *QQuickBarSeriesVisualizer::createTexture()
{
    QQuick3DTexture *texture = new QQuick3DTexture();
    texture->setRotationUV(-90.0f);
    texture->setHorizontalTiling(QQuick3DTexture::ClampToEdge);
    texture->setVerticalTiling(QQuick3DTexture::ClampToEdge);
    DatavisQuick3DTextureData *textureData = new DatavisQuick3DTextureData();
    textureData->setParent(texture);
    textureData->setParentItem(texture);
    texture->setTextureData(textureData);

    return texture;
}

QQuick3DModel *QQuickBarSeriesVisualizer::createDataItemModel(QAbstract3DSeries::Mesh meshType)
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

QQmlComponent *QQuickBarSeriesVisualizer::createRepeaterDelegate(QAbstract3DSeries::Mesh meshType)
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

QQuick3DNode *QQuickBarSeriesVisualizer::createSeriesRoot()
{
    auto model = new QQuick3DNode();

    model->setParentItem(m_qml->rootNode());
    return model;
}

QQuick3DModel *QQuickBarSeriesVisualizer::createDataItem()
{
    auto model = new QQuick3DModel();
    model->setParent(m_visualizerRoot.get());
    model->setParentItem(m_seriesRootItem);
    QString fileName;
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
    fixMeshFileName(fileName, m_meshType);
    model->setSource(QUrl(fileName));
    return model;
}

void QQuickBarSeriesVisualizer::createInstancingRootItem()
{
    m_instancingRootItem = createDataItem();
    m_instancingRootItem->setInstancing(m_instancing);
}

void QQuickBarSeriesVisualizer::createSelectionIndicator()
{
    m_selectionIndicator = createDataItem();

    m_selectionIndicator->setVisible(false);
}

void QQuickBarSeriesVisualizer::removeDataItems()
{
    for (auto it = m_modelList.begin(); it != m_modelList.end(); it++) {
        m_modelList.erase(it);
    }
}

void QQuickBarSeriesVisualizer::removeDummyDataItems()
{
    for (const auto &item : std::as_const(m_dummyItemList)) {
        delete item;
    }
    dummiesCreated = false;
}

void QQuickBarSeriesVisualizer::createItemLabel()
{
    QQmlComponent component(qmlEngine(m_qml), QStringLiteral(":/axis/ItemLabel"));
    QQuick3DNode *labelNode = qobject_cast<QQuick3DNode *>(component.create());
    labelNode->setParent(m_qml->rootNode());
    labelNode->setParentItem(m_qml->rootNode());
    m_itemLabel = labelNode;
}

void QQuickBarSeriesVisualizer::updateItemLabelVisuals(const Q3DTheme *activeTheme)
{
    Q_UNUSED(activeTheme);
}

QVector3D QQuickBarSeriesVisualizer::selectedItemPosition()
{
    if (m_selectedRowIndex != -1 || m_selectedColIndex != -1)
        return QVector3D();

    QVector3D position;
    if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
//        position = m_itemList[m_selectedRowIndex][m_selectedColIndex]->position();
    } else if (m_controller->optimizationHints() == QAbstract3DGraph::OptimizationStatic) {
        position = m_selectionIndicator->position();
    }

    return position;
}

void QQuickBarSeriesVisualizer::fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh meshType)
{
    if (meshType != QAbstract3DSeries::MeshSphere && meshType != QAbstract3DSeries::MeshArrow
            && meshType != QAbstract3DSeries::MeshMinimal && meshType != QAbstract3DSeries::MeshPoint)
        fileName.append(QStringLiteral("Full"));
}

QString QQuickBarSeriesVisualizer::getMeshFileName()
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
