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

#include <private/datavisualizationglobal_p.h>
#include <private/qqmldelegatemodel_p.h>

#include <axishelper_p.h>

QT_BEGIN_NAMESPACE

class ScatterSeriesVisualizer;

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
    QHash<QScatter3DSeries *, ScatterSeriesVisualizer *> m_seriesVisualizerMap;
    const float m_defaultMinSize = 0.01f;
    const float m_defaultMaxSize = 0.1f;
    const float m_itemScaler = 3.0f;
    float m_pointScale = 0;

    // Testing sketching
    AxisHelper m_helperAxisX;
    AxisHelper m_helperAxisY;
    AxisHelper m_helperAxisZ;

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

    bool m_xFlipped = false;
    bool m_yFlipped = false;
    bool m_zFlipped = false;
    int m_selectedItem;
    QScatter3DSeries *m_selectedItemSeries; // Points to the series for which the bar is selected
                                            // in single series selection cases.
    QQuick3DModel *m_selected = nullptr;
    QQuick3DModel *m_previousSelected = nullptr;
    QColor m_selectedSeriesColor;

    float m_labelMargin = 0.15f;

    float m_gridOffset = 0.002f;
    float m_lineWidthScaleFactor = 0.0001f;
    float m_lineLengthScaleFactor = 0.011f;

    QQmlComponent *createRepeaterDelegate(QAbstract3DSeries::Mesh MeshType);
    void calculateSceneScalingFactors();
    float calculatePointScaleSize();
    void updatePointScaleSize();
    void updateDataPoints(QScatter3DSeries *series);
    void updateDataPointVisuals(QScatter3DSeries *series);

    QQuick3DModel *selected() const;
    void setSelected(QQuick3DModel *newSelected);
    void setSelected(QQuick3DModel *root, qsizetype index);
    void clearSelectionModel();

    void setVisualizerForSeries(QScatter3DSeries *series, ScatterSeriesVisualizer * visualizer);
    ScatterSeriesVisualizer *visualizerForSeries(QScatter3DSeries * series);
    int findLabelsMaxWidth(const QStringList &labels);
    void updateLabels() override;
    void updateGrid() override;
    void updateGraph() override;
    void synchData() override;

    // Change selection mode
    void setSelectionModeNG(QAbstract3DGraph::SelectionFlags mode);

    friend class Scatter3DController;
};

QT_END_NAMESPACE
#endif // QQUICKDATAVISSCATTER_P_H
