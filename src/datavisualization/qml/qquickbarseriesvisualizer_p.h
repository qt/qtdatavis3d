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

#ifndef QQUICKBARSERIESVISUALIZER_H
#define QQUICKBARSERIESVISUALIZER_H

#include "bars3dcontroller_p.h"
#include "qquickdatavisbars_p.h"
#include "qabstract3dgraph.h"
#include "datavisquick3dinstancing_p.h"

#include <QtQuick3D/private/qquick3dmodel_p.h>
#include <QtQuick3D/private/qquick3drepeater_p.h>
#include <QtQuick3D/private/qquick3dinstancing_p.h>

QT_BEGIN_NAMESPACE
class LabelItem;
class QQuickDataVisItem;
class Abstract3DController;
class QBar3DSeries;
class QBarDataProxy;
class QBarDataItem;

class AxisHelper;
class QQuick3DTexture;
class Q3DTheme;

class QQuickBarSeriesVisualizer : public QObject
{
    Q_OBJECT
public:
    explicit QQuickBarSeriesVisualizer (QObject *parent = nullptr);
    ~QQuickBarSeriesVisualizer ();

    QQuickDataVisBars *m_dataVisBars;
    Bars3DController *m_controller = nullptr;
    QAbstract3DSeries::Mesh m_meshType = QAbstract3DSeries::MeshSphere;
    bool m_smooth = false;
    DatavisQuick3DInstancing *m_instancing = nullptr;
    QQuick3DModel *m_instancingRootItem = nullptr;
    QQuick3DNode *m_seriesRootItem = nullptr;
    QQuick3DTexture *m_texture = nullptr;
    QQuick3DTexture *m_highlightTexture = nullptr;
    bool m_hasTexture = false;
    bool m_hasHighlightTexture = false;

    QQuick3DNode *m_itemLabel = nullptr;
    AxisHelper *m_helperAxisX = nullptr;
    AxisHelper *m_helperAxisY = nullptr;
    AxisHelper *m_helperAxisZ = nullptr;
    QQuaternion m_meshRotation;

    QHash<QQuick3DModel *, QBarDataItem *> m_modelList;
    qsizetype m_selectedRowIndex = -1;
    qsizetype m_selectedColIndex = -1;
    QQuick3DModel *m_selectionIndicator = nullptr;
    bool m_selectionActive = false;
    float m_selectedGradientPos = 0.0f;
    QQuickDataVisItem *m_qml;
    bool dummiesCreated = false;
    bool m_barsGenerated;

    QScopedPointer<QObject> m_visualizerRoot;

    void createParent();
    void setup();
    void connectSeries(QBar3DSeries *series);
    void disconnectSeries(QBar3DSeries *series);
    void generateBars(QBar3DSeries *series);
    QPoint getItemIndex(QQuick3DModel *item);
    void setSelected(QPoint index);
    void clearSelection();
    void resetSelection();

    void updateData(QBar3DSeries *series);
    void updateItemVisuals(QBar3DSeries *series);
    void updateItemMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient);
    void updateItemInstancedMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient);
    void updateInstancedCustomMaterial(QQuick3DModel *model, bool isHighlight = false);
    void updateSelectionIndicatorMaterial(bool useGradient, bool rangeGradient);
    void updateCustomMaterial(QQuick3DModel *item, bool isHighlight = false);
    void updatePrincipledMaterial(QQuick3DModel *model, const QColor &color, bool useGradient, bool isHighlight = false);

    QQuick3DTexture *createTexture();
    QQuick3DModel *createDataItemModel(QAbstract3DSeries::Mesh meshType);
    QQmlComponent *createRepeaterDelegate(QAbstract3DSeries::Mesh meshType);
    QQuick3DNode *createSeriesRoot();
    QQuick3DModel *createDataItem();
    void createInstancingRootItem();
    void createSelectionIndicator();
    void removeDataItems();
    void createItemLabel();
    QVector3D selectedItemPosition();
    void fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh meshType);
    QString getMeshFileName();
    void setVisualIndex(int index) {m_visualIndex = index;}

public Q_SLOTS:
    void handleSeriesMeshChanged(QAbstract3DSeries::Mesh mesh);
    void handleOptimizationHintsChanged(QAbstract3DGraph::OptimizationHints hints);
    void handleMeshSmoothChanged(bool enable);
    void handleRowCountChanged();
    void handleColCountChanged();

private:
    //AxisRenderCache *m_sliceCache; // not owned
    const LabelItem *m_sliceTitleItem;
    float m_seriesScaleX;
    float m_seriesScaleZ;
    int m_visibleSeriesCount;
    float m_seriesStep;
    float m_seriesStart;
    float m_zeroPosition;
    bool m_keepSeriesUniform;
    bool m_haveUniformColorSeries;
    bool m_haveGradientSeries;
    int m_visualIndex;
};

QT_END_NAMESPACE
#endif // QQUICKBARSERIESVISUALIZER_H
