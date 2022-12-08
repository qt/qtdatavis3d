// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickdatavissurface_p.h"
#include <QtCore/QMutexLocker>

#include "declarativescene_p.h"
#include "surface3dcontroller_p.h"
#include "utils_p.h"

#include <QtQuick3D/private/qquick3drepeater_p.h>
#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>
#include <QtQuick3D/private/qquick3ddefaultmaterial_p.h>

QT_BEGIN_NAMESPACE

QQuickDataVisSurface::QQuickDataVisSurface(QQuickItem *parent)
    : QQuickDataVisItem(parent),
      m_surfaceController(0)
{
    setAcceptedMouseButtons(Qt::AllButtons);

    // Create the shared component on the main GUI thread.
    m_surfaceController = new Surface3DController(boundingRect().toRect(), new Declarative3DScene);
    setSharedController(m_surfaceController);

    QObject::connect(m_surfaceController, &Surface3DController::selectedSeriesChanged,
                     this, &QQuickDataVisSurface::selectedSeriesChanged);
    QObject::connect(m_surfaceController, &Surface3DController::flipHorizontalGridChanged,
                     this, &QQuickDataVisSurface::flipHorizontalGridChanged);
}

QQuickDataVisSurface::~QQuickDataVisSurface()
{
    QMutexLocker locker(m_nodeMutex.data());
    const QMutexLocker locker2(mutex());
    delete m_surfaceController;
    for (auto model : m_model)
        delete model;
}

QValue3DAxis *QQuickDataVisSurface::axisX() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisX());
}

void QQuickDataVisSurface::setAxisX(QValue3DAxis *axis)
{
    m_surfaceController->setAxisX(axis);
}

QValue3DAxis *QQuickDataVisSurface::axisY() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisY());
}

void QQuickDataVisSurface::setAxisY(QValue3DAxis *axis)
{
    m_surfaceController->setAxisY(axis);
}

QValue3DAxis *QQuickDataVisSurface::axisZ() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisZ());
}

void QQuickDataVisSurface::setAxisZ(QValue3DAxis *axis)
{
    m_surfaceController->setAxisZ(axis);
}

void QQuickDataVisSurface::handleFlatShadingEnabledChanged()
{
    auto series = static_cast<QSurface3DSeries *>(sender());
    for (auto model : m_model) {
        if (model->series == series) {
            updateModel(model);
            break;
        }
    }
}

QSurface3DSeries *QQuickDataVisSurface::selectedSeries() const
{
    return m_surfaceController->selectedSeries();
}

void QQuickDataVisSurface::setFlipHorizontalGrid(bool flip)
{
   m_surfaceController->setFlipHorizontalGrid(flip);
}

bool QQuickDataVisSurface::flipHorizontalGrid() const
{
    return m_surfaceController->flipHorizontalGrid();
}

QQmlListProperty<QSurface3DSeries> QQuickDataVisSurface::seriesList()
{
    return QQmlListProperty<QSurface3DSeries>(this, this,
                                              &QQuickDataVisSurface::appendSeriesFunc,
                                              &QQuickDataVisSurface::countSeriesFunc,
                                              &QQuickDataVisSurface::atSeriesFunc,
                                              &QQuickDataVisSurface::clearSeriesFunc);
}

void QQuickDataVisSurface::appendSeriesFunc(QQmlListProperty<QSurface3DSeries> *list,
                                          QSurface3DSeries *series)
{
    reinterpret_cast<QQuickDataVisSurface *>(list->data)->addSeries(series);
}

qsizetype QQuickDataVisSurface::countSeriesFunc(QQmlListProperty<QSurface3DSeries> *list)
{
    return reinterpret_cast<QQuickDataVisSurface *>(list->data)->m_surfaceController->surfaceSeriesList().size();
}

QSurface3DSeries *QQuickDataVisSurface::atSeriesFunc(QQmlListProperty<QSurface3DSeries> *list,
                                                   qsizetype index)
{
    return reinterpret_cast<QQuickDataVisSurface *>(list->data)->m_surfaceController->surfaceSeriesList().at(index);
}

void QQuickDataVisSurface::clearSeriesFunc(QQmlListProperty<QSurface3DSeries> *list)
{
    QQuickDataVisSurface *declSurface = reinterpret_cast<QQuickDataVisSurface *>(list->data);
    QList<QSurface3DSeries *> realList = declSurface->m_surfaceController->surfaceSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declSurface->removeSeries(realList.at(i));
}

void QQuickDataVisSurface::addSeries(QSurface3DSeries *series)
{
    m_surfaceController->addSeries(series);
}

void QQuickDataVisSurface::removeSeries(QSurface3DSeries *series)
{
    m_surfaceController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

void QQuickDataVisSurface::handleAxisXChanged(QAbstract3DAxis *axis)
{
    emit axisXChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisSurface::handleAxisYChanged(QAbstract3DAxis *axis)
{
    emit axisYChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisSurface::handleAxisZChanged(QAbstract3DAxis *axis)
{
    emit axisZChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisSurface::componentComplete()
{
    QQuickDataVisItem::componentComplete();

    auto scene = QQuick3DViewport::scene();

    for (auto series : m_surfaceController->surfaceSeriesList()) {
        bool visible = series->isVisible();

        auto model = new QQuick3DModel();
        model->setParent(scene);
        model->setParentItem(scene);
        model->setObjectName(QStringLiteral("SurfaceModel"));
        model->setVisible(visible);
        if (m_surfaceController->selectionMode().testFlag(QAbstract3DGraph::SelectionNone))
            model->setPickable(false);
        else
            model->setPickable(true);

        auto geometry = new QQuick3DGeometry();
        geometry->setParent(this);
        geometry->setStride(sizeof(SurfaceVertex));
        geometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
        geometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                               0,
                               QQuick3DGeometry::Attribute::F32Type);
        geometry->addAttribute(QQuick3DGeometry::Attribute::TexCoord0Semantic,
                               sizeof(QVector3D) * 2,
                               QQuick3DGeometry::Attribute::F32Type);
        geometry->addAttribute(QQuick3DGeometry::Attribute::NormalSemantic,
                               sizeof(QVector3D),
                               QQuick3DGeometry::Attribute::F32Type);
        geometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                               0,
                               QQuick3DGeometry::Attribute::U32Type);
        model->setGeometry(geometry);

        QQmlListReference materialRef(model, "materials");
        auto material = new QQuick3DDefaultMaterial();
        material->setParent(this);
        QQuick3DTexture *texture = new QQuick3DTexture();
        texture->setParent(this);
        QQuick3DTextureData *textureData = new QQuick3DTextureData();
        textureData->setParent(this);
        texture->setTextureData(textureData);
        material->setDiffuseMap(texture);
        material->setSpecularAmount(7.0f);
        material->setSpecularRoughness(0.025f);
        material->setCullMode(QQuick3DMaterial::NoCulling);
        materialRef.append(material);

        auto gridModel = new QQuick3DModel();
        gridModel->setParent(scene);
        gridModel->setParentItem(scene);
        gridModel->setVisible(visible);
        gridModel->setDepthBias(1.0f);
        auto gridGeometry = new QQuick3DGeometry();
        gridGeometry->setParent(this);
        gridGeometry->setStride(sizeof(SurfaceVertex));
        gridGeometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);
        gridGeometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                                   0,
                                   QQuick3DGeometry::Attribute::F32Type);
        gridGeometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                                   0,
                                   QQuick3DGeometry::Attribute::U32Type);
        gridModel->setGeometry(gridGeometry);
        QQmlListReference gridMaterialRef(gridModel, "materials");
        auto gridMaterial = new QQuick3DPrincipledMaterial();
        gridMaterial->setParent(this);
        gridMaterial->setLighting(QQuick3DPrincipledMaterial::NoLighting);
        gridMaterial->setParent(this);
        gridMaterialRef.append(gridMaterial);

        SurfaceModel *surfaceModel = new SurfaceModel();
        surfaceModel->model = model;
        surfaceModel->gridModel = gridModel;
        surfaceModel->series = series;

        m_model.push_back(surfaceModel);

        connect(series, &QSurface3DSeries::flatShadingEnabledChanged, this, &QQuickDataVisSurface::handleFlatShadingEnabledChanged);
    }

    setScaleWithBackground({2.0f, 1.0f, 2.0f});
    setBackgroundScaleMargin({0.1f, 0.1f, 0.1f});
    setScale({2.0f, 1.0f, 2.0f});
}

void QQuickDataVisSurface::synchData()
{
    QQuickDataVisItem::synchData();

    if (m_surfaceController->isSelectedPointChanged()) {
        updateSelectedPoint();
        m_surfaceController->setSelectedPointChanged(false);
    }
}

inline static float getDataValue(const QSurfaceDataArray &array, bool searchRow, int index)
{
    if (searchRow)
        return array.at(0)->at(index).x();
    else
        return array.at(index)->at(0).z();
}

inline static int binarySearchArray(const QSurfaceDataArray &array, int maxIndex, float limitValue, bool searchRow, bool lowBound, bool ascending)
{
    int min = 0;
    int max = maxIndex;
    int mid = 0;
    int retVal;

    while (max >= min) {
        mid = (min + max) / 2;
        float arrayValue = getDataValue(array, searchRow, mid);
        if (arrayValue == limitValue)
            return mid;
        if (ascending) {
            if (arrayValue < limitValue)
                min = mid + 1;
            else
                max = mid -1;
        } else {
            if (arrayValue > limitValue)
                min = mid + 1;
            else
                max = mid - 1;
        }
    }

    if (lowBound == ascending) {
        if (mid > max)
            retVal = mid;
        else
            retVal = min;
    } else {
        if (mid > max)
            retVal = max;
        else
            retVal = mid;
    }

    if (retVal < 0 || retVal > maxIndex) {
        retVal = -1;
    } else if (lowBound) {
        if (getDataValue(array, searchRow, retVal) < limitValue)
            retVal = -1;
    } else {
        if (getDataValue(array, searchRow, retVal) > limitValue)
            retVal = -1;
    }
    return retVal;
}

void QQuickDataVisSurface::updateGraph()
{
    if (m_surfaceController->hasChangedSeriesList())
        handleChangedSeries();

    if (m_surfaceController->isSeriesVisibilityDirty()) {
        for (auto model : m_model) {
            bool visible = model->series->isVisible();
            if (!visible) {
                model->model->setVisible(visible);
                model->gridModel->setVisible(visible);
                continue;
            }
            model->gridModel->setVisible(model->series->drawMode().testFlag(QSurface3DSeries::DrawWireframe));
            model->model->setVisible(model->series->drawMode().testFlag(QSurface3DSeries::DrawSurface));

            SurfaceVertex selectedVertex = model->selectedVertex;
            visible = visible && !selectedVertex.position.isNull();
            if (visible) {
                selectionPointer()->setPosition(selectedVertex.position);
                const QSurfaceDataArray &array = *(model->series->dataProxy())->array();
                const QSurfaceDataRow &rowArray = *array.at(selectedVertex.coord.y());
                QVector3D value = rowArray.at(selectedVertex.coord.x()).position();
                QString label = QString::number(value.x()) + QStringLiteral(", ") +
                        QString::number(value.y()) + QStringLiteral(", ") +
                        QString::number(value.z());
                itemLabel()->setPosition(selectedVertex.position);
                itemLabel()->setProperty("labelText", label);
            }
            selectionPointer()->setVisible(visible);
            itemLabel()->setVisible(visible);
        }
    }

    if (m_surfaceController->isDataDirty()) {
        for (auto model : m_model) {
            bool visible = model->series->isVisible();
            if (visible)
                updateModel(model);
        }
    }
}

void QQuickDataVisSurface::handleChangedSeries()
{
    auto changedSeries = m_surfaceController->changedSeriesList();

    for (auto series : changedSeries) {
        for (auto model : m_model) {
            if (model->series == series) {
                updateModel(model);
            }
        }
    }
}

void QQuickDataVisSurface::updateModel(SurfaceModel *model)
{
    const QSurfaceDataArray &array = *(model->series->dataProxy())->array();

    // calculateSampleRect
    QRect sampleSpace;
    if (array.size() > 0) {
        if (array.size() >= 2 && array.at(0)->size() >= 2) {
            const int maxRow = array.size() - 1;
            const int maxColumn = array.at(0)->size() - 1;

            const bool ascendingX = array.at(0)->at(0).x() < array.at(0)->at(maxColumn).x();
            const bool ascendingZ = array.at(0)->at(0).z() < array.at(maxRow)->at(0).z();

            int idx = binarySearchArray(array, maxColumn, m_surfaceController->axisX()->min(), true, true, ascendingX);
            if (idx != -1) {
                if (ascendingX)
                    sampleSpace.setLeft(idx);
                else
                    sampleSpace.setRight(idx);
            } else {
                sampleSpace.setWidth(-1);
            }

            idx = binarySearchArray(array, maxColumn, m_surfaceController->axisX()->max(), true, false, ascendingX);
            if (idx != -1) {
                if (ascendingX)
                    sampleSpace.setRight(idx);
                else
                    sampleSpace.setLeft(idx);
            } else {
                sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
            }

            idx = binarySearchArray(array, maxRow, m_surfaceController->axisZ()->min(), false, true, ascendingZ);
            if (idx != -1) {
                if (ascendingZ)
                    sampleSpace.setTop(idx);
                else
                    sampleSpace.setBottom(idx);
            } else {
                sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
            }

            idx = binarySearchArray(array, maxRow, m_surfaceController->axisZ()->max(), false, false, ascendingZ);
            if (idx != -1) {
                if (ascendingZ)
                    sampleSpace.setBottom(idx);
                else
                    sampleSpace.setTop(idx);
            } else {
                sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
            }
        }

        int rowCount = sampleSpace.height();
        int columnCount = sampleSpace.width();
        model->rowCount = rowCount;
        model->columnCount = columnCount;

        int totalSize = rowCount * columnCount * 2;
        float uvX = 1.0f / float(columnCount - 1);
        float uvY = 1.0f / float(rowCount - 1);

        // checkDirection
        int dataDimensions = Surface3DController::BothAscending;
        if (array.at(0)->at(0).x() > array.at(0)->at(array.at(0)->size() - 1).x())
            dataDimensions |= Surface3DController::XDescending;
        if (static_cast<QValue3DAxis *>(m_surfaceController->axisX())->reversed())
            dataDimensions ^= Surface3DController::XDescending;

        if (array.at(0)->at(0).z() > array.at(array.size() - 1)-> at(0).z())
            dataDimensions |= Surface3DController::ZDescending;
        if (static_cast<QValue3DAxis *>(m_surfaceController->axisZ())->reversed())
            dataDimensions ^= Surface3DController::ZDescending;

        m_surfaceController->setDataDimensions(static_cast<Surface3DController::DataDimensions>(dataDimensions));

        model->vertices.reserve(totalSize);

        bool isFlatShadingEnabled = model->series->isFlatShadingEnabled();

        QVector3D boundsMin(0.0f, 0.0f, 0.0f);
        QVector3D boundsMax(0.0f, 0.0f, 0.0f);

        model->vertices.clear();
        model->height.clear();
        for (int i = 0 ; i < rowCount ; i++) {
            const QSurfaceDataRow &row = *array.at(i);
            for (int j = 0 ; j < columnCount ; j++) {
                // getNormalizedVertex
                SurfaceVertex vertex;
                QVector3D pos = getNormalizedVertex(model, row.at(j), false, false);
                vertex.position = pos;
                vertex.normal = QVector3D(0, 0, 0);
                vertex.uv = QVector2D(j * uvX, i * uvY);
                vertex.coord = QPoint(i, j);
                model->vertices.push_back(vertex);
                if (boundsMin.isNull())
                    boundsMin = pos;
                else
                    boundsMin = QVector3D(qMin(boundsMin.x(), pos.x()), qMin(boundsMin.y(), pos.y()), qMin(boundsMin.z(), pos.z()));
                if (boundsMax.isNull())
                    boundsMax = pos;
                else
                    boundsMax = QVector3D(qMax(boundsMax.x(), pos.x()), qMax(boundsMax.y(), pos.y()), qMax(boundsMax.z(), pos.z()));
            }
        }

        //create normals
        int rowLimit = rowCount - 1;
        int colLimit = columnCount - 1;

        int totalIndex = 0;

        model->indices.clear();

        if (isFlatShadingEnabled) {
            model->coarceVertices.clear();

            createCoarseVertices(model, 0, 0, colLimit, rowLimit);
        } else {
            if (dataDimensions == Surface3DController::BothAscending || dataDimensions == Surface3DController::XDescending) {
                for (int row = 0 ; row < rowLimit ; row++)
                    createSmoothNormalBodyLine(model, totalIndex, row * columnCount);
                createSmoothNormalUpperLine(model, totalIndex);
            }
            else {
                createSmoothNormalUpperLine(model, totalIndex);
                for (int row = 1 ; row < rowCount ; row++)
                    createSmoothNormalBodyLine(model, totalIndex, row * columnCount);
            }

            createSmoothIndices(model, 0, 0, colLimit, rowLimit);
        }

        auto geometry = model->model->geometry();
        QByteArray vertexBuffer;
        if (isFlatShadingEnabled)
            vertexBuffer.setRawData(reinterpret_cast<char *>(model->coarceVertices.data()), model->coarceVertices.size() * sizeof(SurfaceVertex));
        else
            vertexBuffer.setRawData(reinterpret_cast<char *>(model->vertices.data()), model->vertices.size() * sizeof(SurfaceVertex));
        geometry->setVertexData(vertexBuffer);
        QByteArray indexBuffer(reinterpret_cast<char *>(model->indices.data()), model->indices.size() * sizeof(quint32));
        geometry->setIndexData(indexBuffer);
        geometry->setBounds(boundsMin, boundsMax);
        geometry->update();

        auto axisY = m_surfaceController->axisY();
        float maxY = axisY->max();
        float minY = axisY->min();
        QQmlListReference materialRef(model->model, "materials");
        auto material = static_cast<QQuick3DDefaultMaterial *>(materialRef.at(0));
        auto textureData = material->diffuseMap()->textureData();
        textureData->setSize(QSize(rowCount, columnCount));
        textureData->setFormat(QQuick3DTextureData::RGBA8);
        QByteArray imageData;
        imageData.resize(model->height.size() * 4);
        QLinearGradient gradient = model->series->baseGradient();
        auto stops = gradient.stops();
        for (int i = 0; i < model->height.size(); i++) {
            float height = model->height.at(i);
            float normalizedHeight = (height - minY) / (maxY - minY);
            for (int j = 0; j < stops.size(); j++) {
                if (normalizedHeight < stops.at(j).first ||
                        (normalizedHeight >= (float)stops.at(j).first && j == stops.size() - 1)) {
                    QColor color;
                    if (j == 0 || normalizedHeight >= (float)stops.at(j).first) {
                        color = stops.at(j).second;
                    } else {
                        float normalLowerBound = stops.at(j - 1).first;
                        float normalUpperBound = stops.at(j).first;
                        normalizedHeight = (normalizedHeight - normalLowerBound) / (normalUpperBound - normalLowerBound);
                        QColor start = stops.at(j - 1).second;
                        QColor end = stops.at(j).second;
                        float red = start.redF() + ((end.redF() - start.redF()) * normalizedHeight);
                        float green = start.greenF() + ((end.greenF() - start.greenF()) * normalizedHeight);
                        float blue = start.blueF() + ((end.blueF() - start.blueF()) * normalizedHeight);
                        color.setRedF(red);
                        color.setGreenF(green);
                        color.setBlueF(blue);
                    }
                    imageData.data()[i * 4 + 0] = char(color.red());
                    imageData.data()[i * 4 + 1] = char(color.green());
                    imageData.data()[i * 4 + 2] = char(color.blue());
                    imageData.data()[i * 4 + 3] = char(color.alpha());
                    break;
                }
            }
        }
        textureData->setTextureData(imageData);

        createGridlineIndices(model, 0, 0, colLimit, rowLimit);

        auto gridGeometry = model->gridModel->geometry();

        if (isFlatShadingEnabled)
            vertexBuffer.setRawData(reinterpret_cast<char *>(model->vertices.data()), model->vertices.size() * sizeof(SurfaceVertex));
        gridGeometry->setVertexData(vertexBuffer);
        QByteArray gridIndexBuffer(reinterpret_cast<char *>(model->gridIndices.data()), model->gridIndices.size() * sizeof(quint32));
        gridGeometry->setIndexData(gridIndexBuffer);
        gridGeometry->setBounds(boundsMin, boundsMax);
        gridGeometry->update();

        QQmlListReference gridMaterialRef(model->gridModel, "materials");
        auto gridMaterial = static_cast<QQuick3DPrincipledMaterial *>(gridMaterialRef.at(0));
        QColor gridColor = model->series->wireframeColor();
        gridMaterial->setBaseColor(gridColor);
    }
}

QVector3D QQuickDataVisSurface::getNormalizedVertex(SurfaceModel *model, const QSurfaceDataItem &data, bool polar, bool flipXZ)
{
    Q_UNUSED(polar);
    Q_UNUSED(flipXZ);

    float normalizedX;
    float normalizedY;
    float normalizedZ;
    QValue3DAxis* axisX = static_cast<QValue3DAxis *>(m_surfaceController->axisX());
    QValue3DAxis* axisY = static_cast<QValue3DAxis *>(m_surfaceController->axisY());
    QValue3DAxis* axisZ = static_cast<QValue3DAxis *>(m_surfaceController->axisZ());
    // TODO : Need to handle polar, flipXZ
    float scale, translate;
    scale = translate = this->scale().x();
    normalizedX = axisX->positionAt(data.x()) * scale * 2.0f - translate;
    scale = translate = this->scale().y();
    model->height.push_back(data.y());
    normalizedY = axisY->positionAt(data.y()) * scale * 2.0f - translate;
    scale = translate = this->scale().z();
    normalizedZ = axisZ->positionAt(data.z()) * -scale * 2.0f + translate;
    return QVector3D(normalizedX, normalizedY, normalizedZ);
}

inline static QVector3D normal(const QVector3D &a, const QVector3D &b, const QVector3D &c)
{
    QVector3D v1 = b - a;
    QVector3D v2 = c - a;
    QVector3D normal = QVector3D::crossProduct(v1, v2);

    return normal;
}

void QQuickDataVisSurface::createSmoothNormalBodyLine(SurfaceModel *model, int &totalIndex, int column)
{
    int columnCount = model->columnCount;
    int colLimit = columnCount - 1;
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();
    if (dataDimensions == Surface3DController::BothAscending) {
        int end = colLimit + column;
        for (int j = column ; j < end ; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j + 1).position,
                                   model->vertices.at(j + columnCount).position);
            model->vertices.replace(totalIndex++, vertex);
        }
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(end).position,
                               model->vertices.at(end + columnCount).position,
                               model->vertices.at(end - 1).position);
        model->vertices.replace(totalIndex++, vertex);
    } else if (dataDimensions == Surface3DController::XDescending) {
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(column).position,
                               model->vertices.at(column + columnCount).position,
                               model->vertices.at(column + 1).position);
        model->vertices.replace(totalIndex++, vertex);
        int end = column + columnCount;
        for (int j = column + 1 ; j < end ; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j - 1).position,
                                   model->vertices.at(j + columnCount).position);
            model->vertices.replace(totalIndex++, vertex);
        }
    } else if (dataDimensions == Surface3DController::ZDescending) {
        int end = colLimit + column;
        for (int j = column; j < end ; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j + 1).position,
                                   model->vertices.at(j - columnCount).position);
            model->vertices.replace(totalIndex++, vertex);
        }
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(end).position,
                               model->vertices.at(end - columnCount).position,
                               model->vertices.at(end - 1).position);
        model->vertices.replace(totalIndex++, vertex);
    } else {
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(column).position,
                               model->vertices.at(column - columnCount).position,
                               model->vertices.at(column + 1).position);
        model->vertices.replace(totalIndex++, vertex);
        int end = column + columnCount;
        for (int j = 0 ; j < end ; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j-1).position,
                                   model->vertices.at(j - columnCount).position);
            model->vertices.replace(totalIndex++, vertex);
        }
    }
}

void QQuickDataVisSurface::createSmoothNormalUpperLine(SurfaceModel *model, int &totalIndex)
{
    int columnCount = model->columnCount;
    int rowCount = model->rowCount;
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();

    if (dataDimensions == Surface3DController::BothAscending) {
        int lineEnd = rowCount * columnCount - 1;
        for (int j = (rowCount - 1) * columnCount; j < lineEnd; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j - columnCount).position,
                                   model->vertices.at(j + 1).position);
            model->vertices.replace(totalIndex++, vertex);
        }
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(lineEnd).position,
                               model->vertices.at(lineEnd - 1).position,
                               model->vertices.at(lineEnd - columnCount).position);
        model->vertices.replace(totalIndex++, vertex);
    } else if (dataDimensions == Surface3DController::XDescending) {
        int lineStart = (rowCount - 1) * columnCount;
        int lineEnd = rowCount * columnCount;
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(lineStart).position,
                               model->vertices.at(lineStart + 1).position,
                               model->vertices.at(lineStart - columnCount).position);
        model->vertices.replace(totalIndex++, vertex);
        for (int j = lineStart + 1; j < lineEnd; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j - columnCount).position,
                                   model->vertices.at(j - 1).position);
            model->vertices.replace(totalIndex++, vertex);
        }
    } else if (dataDimensions == Surface3DController::ZDescending) {
        int colLimit = columnCount - 1;
        for (int j = 0; j < colLimit; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j + columnCount).position,
                                   model->vertices.at(j + 1).position);
            model->vertices.replace(totalIndex++, vertex);
        }
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(colLimit).position,
                               model->vertices.at(colLimit - 1).position,
                               model->vertices.at(colLimit + columnCount).position);
        model->vertices.replace(totalIndex++, vertex);
    } else { // BothDescending
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(0).position,
                               model->vertices.at(1).position,
                               model->vertices.at(columnCount).position);
        model->vertices.replace(totalIndex++, vertex);
        for (int j = 1; j < columnCount; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j + columnCount).position,
                                   model->vertices.at(j - 1).position);
            model->vertices.replace(totalIndex++, vertex);
        }
    }
}

void QQuickDataVisSurface::createSmoothIndices(SurfaceModel *model, int x, int y, int endX, int endY)
{
    int columnCount = model->columnCount;
    int rowCount = model->rowCount;
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();

    if (endX >= columnCount)
        endX = columnCount - 1;
    if (endY >= rowCount)
        endY = rowCount - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    int indexCount = 6 * (endX - x) * (endY - y);
    model->indices.clear();
    model->indices.resize(indexCount);

    int rowEnd = endY * columnCount;
    for (int row = y * columnCount ; row < rowEnd ; row += columnCount) {
        for (int j = x ; j < endX ; j++) {
            if (dataDimensions == Surface3DController::BothAscending
                    || dataDimensions == Surface3DController::BothDescending) {
                model->indices.push_back(row + j + 1);
                model->indices.push_back(row + columnCount + j);
                model->indices.push_back(row + j);

                model->indices.push_back(row + columnCount + j + 1);
                model->indices.push_back(row + columnCount + j);
                model->indices.push_back(row + j + 1);
            } else if (dataDimensions == Surface3DController::XDescending) {
                model->indices.push_back(row + columnCount + j);
                model->indices.push_back(row + columnCount + j + 1);
                model->indices.push_back(row + j);

                model->indices.push_back(row + j);
                model->indices.push_back(row + columnCount + j + 1);
                model->indices.push_back(row + j + 1);
            } else {
                model->indices.push_back(row + columnCount + j);
                model->indices.push_back(row + columnCount + j + 1);
                model->indices.push_back(row + j + 1);

                model->indices.push_back(row + j);
                model->indices.push_back(row + columnCount + j + 1);
                model->indices.push_back(row + j + 1);
            }
        }
    }
}

void QQuickDataVisSurface::createCoarseVertices(SurfaceModel *model, int x, int y, int endX, int endY)
{
    int columnCount = model->columnCount;
    int rowCount = model->rowCount;
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();

    if (endX >= columnCount)
        endX = columnCount - 1;
    if (endY >= rowCount)
        endY = rowCount - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    int indexCount = 6 * (endX - x) * (endY - y);
    model->indices.clear();
    model->indices.resize(indexCount);

    int index = 0;
    int rowEnd = endY * columnCount;

    int i1, i2, i3;
    SurfaceVertex v1, v2, v3;
    QVector3D normalVector;

    for (int row = y * columnCount; row < rowEnd; row += columnCount) {
        for (int j = x; j < endX; j++) {
            if (dataDimensions == Surface3DController::BothAscending
                    || dataDimensions == Surface3DController::BothDescending) {
                i1 = row + j + 1, i2 = row + columnCount + j, i3 = row + j;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);

                i1 = row + columnCount + j + 1, i2 = row + columnCount + j, i3 = row + j + 1;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
            } else if (dataDimensions == Surface3DController::XDescending) {
                i1 = row + columnCount + j, i2 = row + columnCount + j + 1, i3 = row + j;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);

                i1 = row + j, i2 = row + columnCount + j + 1, i3 = row + j + 1;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
            } else {
                i1 = row + columnCount + j, i2 = row + columnCount + j + 1, i3 = row + j + 1;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);

                i1 = row + j, i2 = row + columnCount + j + 1, i3 = row + j + 1;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
            }
        }
    }
}

void QQuickDataVisSurface::createGridlineIndices(SurfaceModel *model, int x, int y, int endX, int endY)
{
    int columnCount = model->columnCount;
    int rowCount = model->rowCount;

    if (endX >= columnCount)
        endX = columnCount - 1;
    if (endY >= rowCount)
        endY = rowCount - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    int nColumns = endX - x + 1;
    int nRows = endY - y + 1;

    int gridIndexCount = 2 * nColumns * (nRows - 1) + 2 * nRows * (nColumns - 1);
    model->gridIndices.resize(gridIndexCount);
    model->gridIndices.clear();

    for (int i = y, row = columnCount * y ; i <= endY ; i++, row += columnCount) {
        for (int j = x ; j < endX ; j++) {
            model->gridIndices.push_back(row + j);
            model->gridIndices.push_back(row + j + 1);
        }
    }
    for (int i = y, row = columnCount * y ; i < endY ; i++, row += columnCount) {
        for (int j = x ; j <= endX ; j++) {
            model->gridIndices.push_back(row + j);
            model->gridIndices.push_back(row + j + columnCount);
        }
    }
}

void QQuickDataVisSurface::handleMousePressedEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button()) {
        auto mousePos = event->pos();
        auto pickResult = pickAll(mousePos.x(), mousePos.y());
        QVector3D pickedPos(0.0f, 0.0f, 0.0f);

        auto selectionMode = m_surfaceController->selectionMode();
        if (!selectionMode.testFlag(QAbstract3DGraph::SelectionNone)) {
            for (auto picked : pickResult) {
                if (picked.objectHit()->objectName().contains(QStringLiteral("SurfaceModel"))) {
                    pickedPos = picked.position();
                    break;
                }
            }

            if (!pickedPos.isNull()) {
                float min = -1.0f;
                for (auto model : m_model) {
                    if (!model->series->isVisible())
                        continue;

                    SurfaceVertex selectedVertex;
                    for (auto vertex : model->vertices) {
                        QVector3D pos = vertex.position;
                        float dist = pickedPos.distanceToPoint(pos);
                        if (selectedVertex.position.isNull() || dist < min) {
                            min = dist;
                            selectedVertex = vertex;
                        }
                    }

                    if (selectionMode == QAbstract3DGraph::SelectionItem) {
                        model->selectedVertex = selectedVertex;

                        if (!selectedVertex.position.isNull())
                            model->series->setSelectedPoint(selectedVertex.coord);
                        else
                            model->series->setSelectedPoint(m_surfaceController->invalidSelectionPosition());
                    }
                }
            }
        }
    }
}

void QQuickDataVisSurface::updateSelectedPoint()
{
    for (auto model : m_model) {
        SurfaceVertex selectedVertex = model->selectedVertex;
        if (model->series->isVisible() &&
                !selectedVertex.position.isNull()) {
            selectionPointer()->setPosition(selectedVertex.position);
            const QSurfaceDataArray &array = *(model->series->dataProxy())->array();
            const QSurfaceDataRow &rowArray = *array.at(selectedVertex.coord.y());
            QVector3D value = rowArray.at(selectedVertex.coord.x()).position();
            QString label = QString::number(value.x()) + QStringLiteral(", ") +
                    QString::number(value.y()) + QStringLiteral(", ") +
                    QString::number(value.z());
            itemLabel()->setPosition(selectedVertex.position);
            itemLabel()->setProperty("labelText", label);
            itemLabel()->setEulerRotation(QVector3D(
                                              -m_surfaceController->scene()->activeCamera()->yRotation(),
                                              -m_surfaceController->scene()->activeCamera()->xRotation(),
                                              0));
        }
    }
}
QT_END_NAMESPACE
