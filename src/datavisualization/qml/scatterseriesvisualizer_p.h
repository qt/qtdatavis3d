// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef SCATTERSERIESVISUALIZER_H
#define SCATTERSERIESVISUALIZER_H
#include "qscatter3dseries_p.h"
#include "qabstract3dgraph.h"
#include "datavisquick3dinstancing_p.h"

#include <QtQuick3D/private/qquick3dmodel_p.h>
#include <QtQuick3D/private/qquick3drepeater_p.h>
#include <QtQuick3D/private/qquick3dinstancing_p.h>

QT_BEGIN_NAMESPACE
class AbstractDeclarative;
class Scatter3DController;
class AxisHelper;
class QQuick3DTexture;
class Q3DTheme;
class QQuickDataVisScatter;

class ScatterSeriesVisualizer : public QObject
{
    Q_OBJECT
public:
    explicit ScatterSeriesVisualizer(QObject *parent = nullptr);
    ~ScatterSeriesVisualizer();

    void setup();
    void connectSeries(QScatter3DSeries *series);
    void disconnectSeries(QScatter3DSeries *series);
    void generatePoints(int count);
    qsizetype getItemIndex(QQuick3DModel *item);
    void setSelected(qsizetype index);
    void clearSelection();
    void updateItemPositions(QScatterDataProxy *dataProxy);
    void updateItemVisuals(QScatter3DSeries *series);
    void createItemLabel();
    QVector3D selectedItemPosition();

    bool pointsGenerated() const;

    void setController(Scatter3DController *newController);
    void setQml(QQuickDataVisScatter *newQml);
    void setHelperAxisX(AxisHelper *newHelperAxisX);
    void setHelperAxisY(AxisHelper *newHelperAxisY);
    void setHelperAxisZ(AxisHelper *newHelperAxisZ);
    void setDotSizedScale(float newDotSizedScale);
    void setScaleX(float newScaleX);
    void setScaleY(float newScaleY);
    void setScaleZ(float newScaleZ);

public Q_SLOTS:
    void handleSeriesMeshChanged(QAbstract3DSeries::Mesh mesh);
    void handleOptimizationHintsChanged(QAbstract3DGraph::OptimizationHints hints);
    void handleMeshSmoothChanged(bool enable);
    void handleItemCountChanged(int count);

private:
    QAbstract3DGraph::OptimizationHint optimizationHint = QAbstract3DGraph::OptimizationHint::OptimizationDefault;
    Scatter3DController *m_controller = nullptr;
    QAbstract3DSeries::Mesh m_meshType = QAbstract3DSeries::MeshSphere;
    bool m_smooth = false;
    DatavisQuick3DInstancing *m_instancing = nullptr;
    QQuick3DModel *m_instancingRootItem = nullptr;
    QQuick3DRepeater *m_repeater = nullptr;
    QQuick3DNode *m_seriesRootItem = nullptr;
    QQuick3DMaterial *m_seriesMaterial = nullptr;
    QQuick3DTexture *m_texture = nullptr;
    QQuick3DTexture *m_highlightTexture = nullptr;
    bool m_hasTexture = false;
    bool m_hasHighLightTexture = false;

    QQuick3DNode *m_itemLabel = nullptr;
    AxisHelper *m_helperAxisX = nullptr;
    AxisHelper *m_helperAxisY = nullptr;
    AxisHelper *m_helperAxisZ = nullptr;
    float m_scaleX = 1.0f;
    float m_scaleY = 1.0f;
    float m_scaleZ = 1.0f;
    float m_dotSizedScale = 1.0f;
    QQuaternion m_meshRotation;
    QList<QQuick3DModel *> m_itemList;
    QList<QQuick3DModel *> m_dummyItemList;
    qsizetype m_selectedIndex = -1;
    QQuick3DModel *m_selectionIndicator = nullptr;
    bool m_selectionActive = false;
    float m_selectedGradientPos = 0.0f;
    QQuickDataVisScatter *m_qml;
    bool dummiesCreated = false;
    QScatter3DSeries *m_series;
    int m_itemCount = 0;

    QScopedPointer<QObject> m_visualizerRoot;

    void createParent();
    void resetSelection();
    void updateItemMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient);
    void updateItemInstancedMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient);
    void updateInstancedCustomMaterial(QQuick3DModel *model, bool isHighlight = false);
    void updateSelectionIndicatorMaterial(bool useGradient, bool rangeGradient);
    void updateCustomMaterial(QQuick3DModel *item, bool isHighlight = false);
    void updatePrincipledMaterial(QQuick3DModel *model, const QColor &color, bool useGradient, bool isHighlight = false);
    void createDummyDataItems(int count);
    QQuick3DTexture *createTexture();
    QQuick3DModel *createDataItemModel(QAbstract3DSeries::Mesh meshType);
    QQmlComponent *createRepeaterDelegate(QAbstract3DSeries::Mesh meshType);
    QQuick3DNode *createSeriesRoot();
    QQuick3DModel *createDataItem();
    void createInstancingRootItem();
    void createSelectionIndicator();
    void removeDataItems();
    void removeDummyDataItems();
    void fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh meshType);
    QString getMeshFileName();
};

QT_END_NAMESPACE

#endif // SCATTERSERIESVISUALIZER_H
