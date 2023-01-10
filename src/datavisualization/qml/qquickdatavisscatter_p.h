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
#ifndef QQUICKDATAVISSCATTER_P_H
#define QQUICKDATAVISSCATTER_P_H

#include "qscatter3dseries.h"
#include "qquickdatavisitem_p.h"
#include "scatter3dcontroller_p.h"
#include "scatterinstancing_p.h"

#include <private/datavisualizationglobal_p.h>
#include <private/qqmldelegatemodel_p.h>

QT_BEGIN_NAMESPACE

class QQuickDataVisScatter : public QQuickDataVisItem
{
    Q_OBJECT
    Q_PROPERTY(QValue3DAxis *axisX READ axisX WRITE setAxisX NOTIFY axisXChanged)
    Q_PROPERTY(QValue3DAxis *axisY READ axisY WRITE setAxisY NOTIFY axisYChanged)
    Q_PROPERTY(QValue3DAxis *axisZ READ axisZ WRITE setAxisZ NOTIFY axisZChanged)
    Q_PROPERTY(QScatter3DSeries *selectedSeries READ selectedSeries NOTIFY selectedSeriesChanged)
    Q_PROPERTY(QQmlListProperty<QScatter3DSeries> seriesList READ seriesList)
    Q_CLASSINFO("DefaultProperty", "seriesList")

    QML_NAMED_ELEMENT(Scatter3DNG)
    QML_ADDED_IN_VERSION(1, 0)

public:
    explicit QQuickDataVisScatter(QQuickItem *parent = 0);
    ~QQuickDataVisScatter();

    QValue3DAxis *axisX() const;
    void setAxisX(QValue3DAxis *axis);
    QValue3DAxis *axisY() const;
    void setAxisY(QValue3DAxis *axis);
    QValue3DAxis *axisZ() const;
    void setAxisZ(QValue3DAxis *axis);

    QQmlListProperty<QScatter3DSeries> seriesList();
    static void appendSeriesFunc(QQmlListProperty<QScatter3DSeries> *list, QScatter3DSeries *series);
    static qsizetype countSeriesFunc(QQmlListProperty<QScatter3DSeries> *list);
    static QScatter3DSeries *atSeriesFunc(QQmlListProperty<QScatter3DSeries> *list, qsizetype index);
    static void clearSeriesFunc(QQmlListProperty<QScatter3DSeries> *list);
    Q_INVOKABLE void addSeries(QScatter3DSeries *series);
    Q_INVOKABLE void removeSeries(QScatter3DSeries *series);

    QScatter3DSeries *selectedSeries() const;
    void setSelectedItem(int index, QScatter3DSeries *series);
    static inline int invalidSelectionIndex() { return -1; }

public Q_SLOTS:
    void handleAxisXChanged(QAbstract3DAxis *axis) override;
    void handleAxisYChanged(QAbstract3DAxis *axis) override;
    void handleAxisZChanged(QAbstract3DAxis *axis) override;

Q_SIGNALS:
    void axisXChanged(QValue3DAxis *axis);
    void axisYChanged(QValue3DAxis *axis);
    void axisZChanged(QValue3DAxis *axis);
    void selectedSeriesChanged(QScatter3DSeries *series);

private:
    Scatter3DController *m_scatterController;
    float m_maxItemSize = 0.0f;

    // For Quick3D renderer integration
    const float m_defaultMinSize = 0.01f;
    const float m_defaultMaxSize = 0.1f;
    const float m_itemScaler = 3.0f;
    float m_pointScale = 0;

    // These were in renderer
    float m_scaleX = 1.0f;
    float m_scaleY = 1.0f;
    float m_scaleZ = 1.0f;

    float m_requestedMargin = -1.0f;
    float m_vBackgroundMargin = 0.1f;
    float m_hBackgroundMargin = 0.1f;

    float m_scaleXWithBackground = 0.0f;
    float m_scaleYWithBackground = 0.0f;
    float m_scaleZWithBackground = 0.0f;

    bool m_polarGraph = false;

    float m_graphHorizontalAspectRatio = 0.0f;
    float m_graphAspectRatio = 2.0f;

    int m_selectedItem;
    QScatter3DSeries *m_selectedItemSeries; // Points to the series for which the bar is selected
                                            // in single series selection cases.
    QQuick3DModel *m_selected = nullptr;
    QQuick3DModel *m_previousSelected = nullptr;

    QHash<QScatter3DSeries *, QList<QQuick3DModel *>> m_seriesModelMap;

    // From seriesvisualizer
    void setup(QScatter3DSeries *series);
    void connectSeries(QScatter3DSeries *series);
    void disconnectSeries(QScatter3DSeries *series);
    void generatePointsForSeries(QScatter3DSeries *series);
    qsizetype getItemIndex(QQuick3DModel *item);
    void setSelected(qsizetype index);
//    void clearSelection();
    void updateItemPositions(QScatter3DSeries *series);
    void updateItemVisuals(QScatter3DSeries *series);
    void createItemLabel();
    QVector3D selectedItemPosition();

    bool pointsGenerated() const;

    ScatterInstancing *m_instancing = nullptr;
    QQuick3DModel *m_instancingRootItem = nullptr;
    QQuick3DNode *m_seriesRootItem = nullptr;
    QQuick3DMaterial *m_seriesMaterial = nullptr;
    QQuick3DTexture *m_texture = nullptr;
    QQuick3DTexture *m_highlightTexture = nullptr;
    bool m_hasTexture = false;
    bool m_hasHighLightTexture = false;
    bool m_smooth = false;

    float m_dotSizedScale = 1.0f;
    QQuaternion m_meshRotation;
    QList<QQuick3DModel *> m_itemList;
    qsizetype m_selectedIndex = -1;
    QQuick3DModel *m_selectionIndicator = nullptr;
    bool m_selectionActive = false;
    float m_selectedGradientPos = 0.0f;
    QQuickDataVisScatter *m_qml;
    QScatter3DSeries *m_series;
    int m_itemCount = 0;

    void resetSelection();
    void updateItemMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient);
    void updateItemInstancedMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient);
    void updateInstancedCustomMaterial(QQuick3DModel *model, bool isHighlight = false);
    void updateSelectionIndicatorMaterial(bool useGradient, bool rangeGradient);
    void updateCustomMaterial(QQuick3DModel *item, bool isHighlight = false);
    void updatePrincipledMaterial(QQuick3DModel *model, const QColor &color, bool useGradient, bool isHighlight = false);

    QQuick3DTexture *createTexture();
    QQuick3DModel *createDataItemModel(QAbstract3DSeries::Mesh meshType);
    QQuick3DNode *createSeriesRoot();
    QQuick3DModel *createDataItem(const QAbstract3DSeries::Mesh meshType);
    void createInstancingRootItem(QAbstract3DSeries::Mesh meshType);
    void createSelectionIndicator(QAbstract3DSeries::Mesh meshType);
    void removeDataItems(QList<QQuick3DModel *> &items);
    void removeDummyDataItems();
    void fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh meshType);
    QString getMeshFileName(QAbstract3DSeries::Mesh meshType);
    // ----------------------------------------

    QQuick3DNode *m_itemLabel = nullptr;

    QColor m_selectedSeriesColor;

    float m_labelMargin = 0.25f;

    float m_gridOffset = 0.002f;
    float m_lineWidthScaleFactor = 0.0001f;
    float m_lineLengthScaleFactor = 0.011f;

    QQmlComponent *createRepeaterDelegate(QAbstract3DSeries::Mesh MeshType);
    void calculateSceneScalingFactors();
    float calculatePointScaleSize();
    void updatePointScaleSize();
    void updateDataPoints(QScatter3DSeries *series);
    void updateDataPointVisuals(QScatter3DSeries *series);
    void updateShadowQuality(QQuickDataVisItem::ShadowQuality quality) override;

    QQuick3DModel *selected() const;
    void setSelected(QQuick3DModel *newSelected);
    void setSelected(QQuick3DModel *root, qsizetype index);
    void clearSelectionModel();

    void updateGraph() override;
    void synchData() override;

    // Change selection mode
    void setSelectionModeNG(QAbstract3DGraph::SelectionFlags mode);

    friend class Scatter3DController;
};

QT_END_NAMESPACE
#endif // QQUICKDATAVISSCATTER_P_H
