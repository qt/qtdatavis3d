// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QQUICKDATAVISITEM_H
#define QQUICKDATAVISITEM_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtGraphs API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include "qabstract3dgraph.h"

#include <QtQuick3D/private/qquick3dviewport_p.h>

QT_BEGIN_NAMESPACE

class Abstract3DController;
class Declarative3DScene;
class Q3DTheme;
class QAbstract3DAxis;
class QAbstract3DInputHandler;
class QAbstract3DSeries;
class QCustom3DItem;
class QQuick3DCustomMaterial;
class QQuick3DDirectionalLight;
class QQuick3DPrincipledMaterial;
class QQuick3DRepeater;

class QQuickDataVisItem : public QQuick3DViewport
{
    Q_OBJECT
    Q_PROPERTY(SelectionFlags selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(bool shadowsSupported READ shadowsSupported NOTIFY shadowsSupportedChanged)
    Q_PROPERTY(int msaaSamples READ msaaSamples WRITE setMsaaSamples NOTIFY msaaSamplesChanged)
    Q_PROPERTY(Declarative3DScene *scene READ scene NOTIFY sceneChanged)
    Q_PROPERTY(QAbstract3DInputHandler *inputHandler READ inputHandler WRITE setInputHandler NOTIFY inputHandlerChanged)
    Q_PROPERTY(Q3DTheme *theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(RenderingMode renderingMode READ renderingMode WRITE setRenderingMode NOTIFY renderingModeChanged)
    Q_PROPERTY(bool measureFps READ measureFps WRITE setMeasureFps NOTIFY measureFpsChanged)
    Q_PROPERTY(int currentFps READ currentFps NOTIFY currentFpsChanged)
    Q_PROPERTY(QQmlListProperty<QCustom3DItem> customItemList READ customItemList)
    Q_PROPERTY(bool orthoProjection READ isOrthoProjection WRITE setOrthoProjection NOTIFY orthoProjectionChanged)
    Q_PROPERTY(ElementType selectedElement READ selectedElement NOTIFY selectedElementChanged)
    Q_PROPERTY(qreal aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(OptimizationHints optimizationHints READ optimizationHints WRITE setOptimizationHints NOTIFY optimizationHintsChanged)
    Q_PROPERTY(bool polar READ isPolar WRITE setPolar NOTIFY polarChanged)
    Q_PROPERTY(float radialLabelOffset READ radialLabelOffset WRITE setRadialLabelOffset NOTIFY radialLabelOffsetChanged)
    Q_PROPERTY(qreal horizontalAspectRatio READ horizontalAspectRatio WRITE setHorizontalAspectRatio NOTIFY horizontalAspectRatioChanged)
    Q_PROPERTY(bool reflection READ isReflection WRITE setReflection NOTIFY reflectionChanged)
    Q_PROPERTY(qreal reflectivity READ reflectivity WRITE setReflectivity NOTIFY reflectivityChanged)
    Q_PROPERTY(QLocale locale READ locale WRITE setLocale NOTIFY localeChanged)
    Q_PROPERTY(QVector3D queriedGraphPosition READ queriedGraphPosition NOTIFY queriedGraphPositionChanged)
    Q_PROPERTY(qreal margin READ margin WRITE setMargin NOTIFY marginChanged)

    QML_NAMED_ELEMENT(AbstractGraph3DNG)
    QML_ADDED_IN_VERSION(6, 6)
    QML_UNCREATABLE("Trying to create uncreatable: AbstractGraph3D.")

public:
    enum SelectionFlag {
        SelectionNone              = 0,
        SelectionItem              = 1,
        SelectionRow               = 2,
        SelectionItemAndRow        = SelectionItem | SelectionRow,
        SelectionColumn            = 4,
        SelectionItemAndColumn     = SelectionItem | SelectionColumn,
        SelectionRowAndColumn      = SelectionRow | SelectionColumn,
        SelectionItemRowAndColumn  = SelectionItem | SelectionRow | SelectionColumn,
        SelectionSlice             = 8,
        SelectionMultiSeries       = 16
    };
    Q_DECLARE_FLAGS(SelectionFlags, SelectionFlag)

    enum ShadowQuality {
        ShadowQualityNone = 0,
        ShadowQualityLow,
        ShadowQualityMedium,
        ShadowQualityHigh,
        ShadowQualitySoftLow,
        ShadowQualitySoftMedium,
        ShadowQualitySoftHigh
    };

    enum ElementType {
        ElementNone = 0,
        ElementSeries,
        ElementAxisXLabel,
        ElementAxisYLabel,
        ElementAxisZLabel,
        ElementCustomItem
    };

    enum RenderingMode {
        RenderDirectToBackground = 0,
        RenderDirectToBackground_NoClear,
        RenderIndirect
    };

    enum OptimizationHint {
        OptimizationDefault = 0,
        OptimizationStatic  = 1
    };
    Q_DECLARE_FLAGS(OptimizationHints, OptimizationHint)

    Q_ENUM(ShadowQuality)
    Q_ENUM(RenderingMode)
    Q_ENUM(ElementType)
    Q_FLAGS(SelectionFlag SelectionFlags)
    Q_FLAGS(OptimizationHint OptimizationHints)

public:
    explicit QQuickDataVisItem(QQuickItem *parent = 0);
    virtual ~QQuickDataVisItem();

    virtual void setRenderingMode(RenderingMode mode);
    virtual QQuickDataVisItem::RenderingMode renderingMode() const;

    virtual void setSelectionMode(SelectionFlags mode);
    virtual QQuickDataVisItem::SelectionFlags selectionMode() const;

    virtual void setShadowQuality(ShadowQuality quality);
    virtual QQuickDataVisItem::ShadowQuality shadowQuality() const;

    virtual QQuickDataVisItem::ElementType selectedElement() const;

    virtual bool shadowsSupported() const;

    virtual void setMsaaSamples(int samples);
    virtual int msaaSamples() const;

    virtual Declarative3DScene *scene() const;

    virtual QAbstract3DInputHandler *inputHandler() const;
    virtual void setInputHandler(QAbstract3DInputHandler *inputHandler);

    virtual void setTheme(Q3DTheme *theme);
    virtual Q3DTheme *theme() const;

    Q_INVOKABLE virtual void clearSelection();

    Q_INVOKABLE virtual bool hasSeries(QAbstract3DSeries *series);

    Q_INVOKABLE virtual int addCustomItem(QCustom3DItem *item);
    Q_INVOKABLE virtual void removeCustomItems();
    Q_INVOKABLE virtual void removeCustomItem(QCustom3DItem *item);
    Q_INVOKABLE virtual void removeCustomItemAt(const QVector3D &position);
    Q_INVOKABLE virtual void releaseCustomItem(QCustom3DItem *item);

    Q_INVOKABLE virtual int selectedLabelIndex() const;
    Q_INVOKABLE virtual QAbstract3DAxis *selectedAxis() const;

    Q_INVOKABLE virtual int selectedCustomItemIndex() const;
    Q_INVOKABLE virtual QCustom3DItem *selectedCustomItem() const;

    QQmlListProperty<QCustom3DItem> customItemList();
    static void appendCustomItemFunc(QQmlListProperty<QCustom3DItem> *list,
                                     QCustom3DItem *item);
    static qsizetype countCustomItemFunc(QQmlListProperty<QCustom3DItem> *list);
    static QCustom3DItem *atCustomItemFunc(QQmlListProperty<QCustom3DItem> *list, qsizetype index);
    static void clearCustomItemFunc(QQmlListProperty<QCustom3DItem> *list);

    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    void setSharedController(Abstract3DController *controller);

    void checkWindowList(QQuickWindow *window);

    void setMeasureFps(bool enable);
    bool measureFps() const;
    int currentFps() const;

    void setOrthoProjection(bool enable);
    bool isOrthoProjection() const;

    void setAspectRatio(qreal ratio);
    qreal aspectRatio() const;

    void setOptimizationHints(OptimizationHints hints);
    OptimizationHints optimizationHints() const;

    void setPolar(bool enable);
    bool isPolar() const;

    void setRadialLabelOffset(float offset);
    float radialLabelOffset() const;

    void setHorizontalAspectRatio(qreal ratio);
    qreal horizontalAspectRatio() const;

    void setReflection(bool enable);
    bool isReflection() const;

    void setReflectivity(qreal reflectivity);
    qreal reflectivity() const;

    void setLocale(const QLocale &locale);
    QLocale locale() const;

    QVector3D queriedGraphPosition() const;

    void setMargin(qreal margin);
    qreal margin() const;

    QMutex *mutex() { return &m_mutex; }

    bool isReady() { return isComponentComplete(); }
    QQuick3DNode *rootNode() const;

    QQuick3DNode *cameraTarget() { return m_cameraTarget; }
    void setCameraTarget(QQuick3DNode *target) { m_cameraTarget = target; }

    QQuick3DModel *background() const { return m_background; }
    void setBackground(QQuick3DModel *newBackground) { m_background = newBackground; }
    inline QQuick3DModel *backgroundBB() const { return m_backgroundBB; }

    QQuick3DDirectionalLight *light() const;
    QQuick3DCustomMaterial *createQmlCustomMaterial(const QString &fileName);
    QQuick3DPrincipledMaterial *createPrincipledMaterial();

    QQuick3DNode *itemLabel() { return m_itemLabel; }

    QQuick3DModel *m_targetVisualizer;

    QQuick3DRepeater *repeaterX() const { return m_repeaterX; }
    QQuick3DRepeater *repeaterY() const { return m_repeaterY; }
    QQuick3DRepeater *repeaterZ() const { return m_repeaterZ; }

    QQuick3DNode *titleLabelX() const { return m_titleLabelX; }
    QQuick3DNode *titleLabelY() const { return m_titleLabelY; }
    QQuick3DNode *titleLabelZ() const { return m_titleLabelZ; }

    QQuick3DRepeater *segmentLineRepeaterX() const { return m_segmentLineRepeaterX; }
    QQuick3DRepeater *segmentLineRepeaterY() const { return m_segmentLineRepeaterY; }
    QQuick3DRepeater *segmentLineRepeaterZ() const { return m_segmentLineRepeaterZ; }

    QQuick3DRepeater *subsegmentLineRepeaterX() const { return m_subsegmentLineRepeaterX; }
    QQuick3DRepeater *subsegmentLineRepeaterY() const { return m_subsegmentLineRepeaterY; }
    QQuick3DRepeater *subsegmentLineRepeaterZ() const { return m_subsegmentLineRepeaterZ; }

    bool isXFlipped() const { return m_xFlipped; }
    void setXFlipped(bool xFlipped) { m_xFlipped = xFlipped; }
    bool isYFlipped() const { return m_yFlipped; }
    void setYFlipped(bool yFlipped) { m_yFlipped = yFlipped; }
    bool isZFlipped() const { return m_zFlipped; }
    void setZFlipped(bool zFlipped) { m_zFlipped = zFlipped; }
    QVector3D scaleWithBackground() const { return m_scaleWithBackground; }
    void setScaleWithBackground(const QVector3D &scale) { m_scaleWithBackground = scale; }
    void setBackgroundScaleMargin(const QVector3D &margin) { m_backgroundScaleMargin = margin; }
    QVector3D rotation() const { return m_rot; }
    void setRotation(const QVector3D &rotation) { m_rot = rotation; }
    QVector3D scale() const { return m_scale; }
    void setScale(const QVector3D &scale) { m_scale = scale; }
    QVector3D translate() const { return m_translate; }
    void setTranslate(const QVector3D &translate) { m_translate = translate;}

    float lineLengthScaleFactor() const { return m_lineLengthScaleFactor; }
    void setLineLengthScaleFactor(float scaleFactor) { m_lineLengthScaleFactor = scaleFactor; }
    float lineWidthScaleFactor() const { return m_lineWidthScaleFactor; }
    void setLineWidthScaleFactor(float scaleFactor) { m_lineWidthScaleFactor = scaleFactor; }
    float gridOffset() const { return m_gridOffset; }
    void setLabelMargin(float margin) { m_labelMargin = margin; }
    float labelMargin() const { return m_labelMargin; }

    void changeLabelBackgroundColor(QQuick3DRepeater *repeater, const QColor &color);
    void changeLabelBackgroundEnabled(QQuick3DRepeater *repeater, const bool &enabled);
    void changeLabelBorderEnabled(QQuick3DRepeater *repeater, const bool &enabled);
    void changeLabelTextColor(QQuick3DRepeater *repeater, const QColor &color);
    void changeLabelFont(QQuick3DRepeater *repeater, const QFont &font);
    void changeGridLineColor(QQuick3DRepeater *repeater, const QColor &color);
    void updateTitleLabels();

    QQuick3DNode *itemSelectionLabel() const;
    void setItemSelectionLabel(QQuick3DNode *newItemSelectionLabel);

public Q_SLOTS:
    virtual void handleAxisXChanged(QAbstract3DAxis *axis) = 0;
    virtual void handleAxisYChanged(QAbstract3DAxis *axis) = 0;
    virtual void handleAxisZChanged(QAbstract3DAxis *axis) = 0;
    void handleFpsChanged();
    void windowDestroyed(QObject *obj);

Q_SIGNALS:
    void selectionModeChanged(QQuickDataVisItem::SelectionFlags mode);
    void shadowQualityChanged(QQuickDataVisItem::ShadowQuality quality);
    void shadowsSupportedChanged(bool supported);
    void msaaSamplesChanged(int samples);
    void inputHandlerChanged(QAbstract3DInputHandler *inputHandler);
    void themeChanged(Q3DTheme *theme);
    void renderingModeChanged(QQuickDataVisItem::RenderingMode mode);
    void measureFpsChanged(bool enabled);
    void currentFpsChanged(int fps);
    void selectedElementChanged(QQuickDataVisItem::ElementType type);
    void orthoProjectionChanged(bool enabled);
    void aspectRatioChanged(qreal ratio);
    void optimizationHintsChanged(QQuickDataVisItem::OptimizationHints hints);
    void polarChanged(bool enabled);
    void radialLabelOffsetChanged(float offset);
    void horizontalAspectRatioChanged(qreal ratio);
    void reflectionChanged(bool enabled);
    void reflectivityChanged(qreal reflectivity);
    void localeChanged(const QLocale &locale);
    void queriedGraphPositionChanged(const QVector3D &data);
    void marginChanged(qreal margin);

protected:
    bool event(QEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void touchEvent(QTouchEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
    virtual void handleWindowChanged(/*QQuickWindow *win*/);
    void itemChange(ItemChange change, const ItemChangeData &value) override;
    virtual void updateWindowParameters();
    virtual void handleSelectionModeChange(QAbstract3DGraph::SelectionFlags mode);
    virtual void handleShadowQualityChange(QAbstract3DGraph::ShadowQuality quality);
    virtual void handleSelectedElementChange(QAbstract3DGraph::ElementType type);
    virtual void handleOptimizationHintChange(QAbstract3DGraph::OptimizationHints hints);
    virtual void keyPressEvent(QKeyEvent *ev) override;
    virtual void handleMousePressedEvent(QMouseEvent *event);

    void componentComplete() override;

    void createSliceView();

    QQuick3DNode *graphNode() { return m_graphNode; }
    QQuick3DViewport *sliceView() { return m_sliceView; }

    QQmlComponent *createRepeaterDelegateComponent(const QString &fileName);
    QQuick3DRepeater *createRepeater();
    QQuick3DNode *createTitleLabel();

    void updateXTitle(const QVector3D &labelRotation, const QVector3D &labelTrans,
                      const QQuaternion &totalRotation, float labelsMaxWidth, float labelHeight, const QVector3D &scale);
    void updateYTitle(const QVector3D &sideLabelRotation, const QVector3D &backLabelRotation,
                      const QVector3D &sideLabelTrans, const QVector3D &backLabelTrans,
                      const QQuaternion &totalSideRotation, const QQuaternion &totalBackRotation,
                      float labelsMaxWidth, float labelHeight, const QVector3D &scale);
    void updateZTitle(const QVector3D &labelRotation, const QVector3D &labelTrans,
                      const QQuaternion &totalRotation, float labelsMaxWidth, float labelHeight, const QVector3D &scale);

    void positionAndScaleLine(QQuick3DNode *lineNode, QVector3D scale, QVector3D position);
    int findLabelsMaxWidth(const QStringList &labels);
    virtual QVector3D calculateCategoryLabelPosition(QAbstract3DAxis *axis, QVector3D labelPosition, int index);
    virtual float calculateCategoryGridLinePosition(QAbstract3DAxis *axis, int index);
    void setFloorGridInRange(bool inRange) { m_isFloorGridInRange = inRange; }
    void setVerticalSegmentLine(bool hasVerticalLine) { m_hasVerticalSegmentLine = hasVerticalLine; }
    void updateGrid();
    void updateLabels();
    void updateSliceGrid();
    void updateSliceLabels();

    virtual void synchData();
    virtual void updateGraph() {}

    bool isSliceEnabled() const { return m_sliceEnabled; }
    void setSliceEnabled(bool enabled) { m_sliceEnabled = enabled; }
    void setSliceActivatedChanged(bool changed) { m_sliceActivatedChanged = changed; }
    virtual void updateSliceGraph();

    virtual void updateShadowQuality(QQuickDataVisItem::ShadowQuality quality);
    virtual void updateAxisRange(float min, float max);
    virtual void updateAxisReversed(bool enable);
    virtual void updateSingleHighlightColor() {}

    QSharedPointer<QMutex> m_nodeMutex;

private:
    QQuick3DNode *m_graphNode = nullptr;
    QQuick3DModel *m_background = nullptr;
    QQuick3DModel *m_backgroundBB = nullptr;
    QQuick3DNode *m_backgroundScale = nullptr;
    QQuick3DNode *m_backgroundRotation = nullptr;

    QQuick3DRepeater *m_repeaterX = nullptr;
    QQuick3DRepeater *m_repeaterY = nullptr;
    QQuick3DRepeater *m_repeaterZ = nullptr;

    QQuick3DNode *m_titleLabelX = nullptr;
    QQuick3DNode *m_titleLabelY = nullptr;
    QQuick3DNode *m_titleLabelZ = nullptr;

    QQuick3DNode *m_itemLabel = nullptr;
    QQuick3DNode *m_itemSelectionLabel = nullptr;

    QQuick3DRepeater *m_segmentLineRepeaterX = nullptr;
    QQuick3DRepeater *m_subsegmentLineRepeaterX = nullptr;
    QQuick3DRepeater *m_segmentLineRepeaterY = nullptr;
    QQuick3DRepeater *m_subsegmentLineRepeaterY = nullptr;
    QQuick3DRepeater *m_segmentLineRepeaterZ = nullptr;
    QQuick3DRepeater *m_subsegmentLineRepeaterZ = nullptr;

    QQuick3DViewport *m_sliceView = nullptr;
    QQuick3DRepeater *m_sliceHorizontalGridRepeater = nullptr;
    QQuick3DRepeater *m_sliceVerticalGridRepeater = nullptr;
    QQuick3DRepeater *m_sliceHorizontalLabelRepeater = nullptr;
    QQuick3DRepeater *m_sliceVerticalLabelRepeater = nullptr;

    QQuick3DNode *m_sliceHorizontalTitleLabel = nullptr;
    QQuick3DNode *m_sliceVerticalTitleLabel = nullptr;

    QPointer<Abstract3DController> m_controller;
    QQuick3DNode *m_cameraTarget = nullptr;
    QQuick3DDirectionalLight *m_light = nullptr;
    QRectF m_cachedGeometry;
    QQuickDataVisItem::RenderingMode m_renderMode;
    int m_samples;
    int m_windowSamples;
    QSize m_initialisedSize;
    bool m_runningInDesigner;
    QMutex m_mutex;

    bool m_xFlipped = false;
    bool m_yFlipped = false;
    bool m_zFlipped = false;

    bool m_flipScales;

    bool m_isFloorGridInRange = false;
    bool m_hasVerticalSegmentLine = true;

    QVector3D m_scaleWithBackground = QVector3D(1.0f, 1.0f, 1.0f);
    QVector3D m_backgroundScaleMargin = QVector3D(0.0f, 0.0f, 0.0f);

    QVector3D m_rot = QVector3D(1.0f, 1.0f, 1.0f);

    QVector3D m_scale = QVector3D(1.0f, 1.0f, 1.0f);

    QVector3D m_translate = QVector3D(1.0f, 1.0f, 1.0f);

    QVector3D m_labelScale = QVector3D(0.01f, 0.01f, 0.0f);

    float m_gridOffset = 0.002f;
    float m_lineWidthScaleFactor = 0.0001f;
    float m_lineLengthScaleFactor = 0.02f;

    float m_labelFontScaleFactor = 4.0f;
    float m_fontScaleFactor = .3f;

    float m_labelMargin = 0.1f;

    bool m_sliceEnabled = false;
    bool m_sliceActivatedChanged = false;

    void setUpCamera();
    void setUpLight();
    void graphPositionAt(const QPoint& point);
    void updateCamera();
    QVector3D calculateLabelRotation(float labelAutoAngle);

    friend class Scatter3DController;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QQuickDataVisItem::SelectionFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QQuickDataVisItem::OptimizationHints)

QT_END_NAMESPACE

#endif
