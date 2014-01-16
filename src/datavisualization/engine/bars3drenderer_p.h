/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
**
** Licensees holding valid Qt Enterprise licenses may use this file in
** accordance with the Qt Enterprise License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q3DBARSRENDERER_P_H
#define Q3DBARSRENDERER_P_H

#include "datavisualizationglobal_p.h"
#include "bars3dcontroller_p.h"
#include "abstract3drenderer_p.h"
#include "qbardataproxy.h"
#include "barrenderitem_p.h"

class QPoint;
class QSizeF;
class QOpenGLShaderProgram;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class ShaderHelper;
class ObjectHelper;
class LabelItem;
class Q3DScene;

class QT_DATAVISUALIZATION_EXPORT Bars3DRenderer : public Abstract3DRenderer
{
    Q_OBJECT

private:
    // Cached state based on emitted signals from the controller
    QSizeF m_cachedBarThickness;
    QSizeF m_cachedBarSpacing;
    bool m_cachedIsSlicingActivated;
    int m_cachedRowCount;
    int m_cachedColumnCount;

    // Internal state
    BarRenderItem *m_selectedBar; // points to renderitem array
    QVector<BarRenderItem *> m_sliceSelection;
    AxisRenderCache *m_sliceCache; // not owned
    const LabelItem *m_sliceTitleItem; // not owned
    bool m_xFlipped;
    bool m_zFlipped;
    bool m_yFlipped;
    bool m_updateLabels;
    ShaderHelper *m_barShader;
    ShaderHelper *m_barGradientShader;
    ShaderHelper *m_depthShader;
    ShaderHelper *m_selectionShader;
    ShaderHelper *m_backgroundShader;
    ShaderHelper *m_labelShader;
    ObjectHelper *m_backgroundObj;
    ObjectHelper *m_gridLineObj;
    ObjectHelper *m_labelObj;
    GLuint m_bgrTexture;
    GLuint m_depthTexture;
    GLuint m_selectionTexture;
    GLuint m_depthFrameBuffer;
    GLuint m_selectionFrameBuffer;
    GLuint m_selectionDepthBuffer;
    GLfloat m_shadowQualityToShader;
    GLint m_shadowQualityMultiplier;
    GLfloat m_heightNormalizer;
    GLfloat m_gradientFraction;
    GLfloat m_negativeBackgroundAdjustment;
    GLfloat m_rowWidth;
    GLfloat m_columnDepth;
    GLfloat m_maxDimension;
    GLfloat m_scaleX;
    GLfloat m_scaleZ;
    GLfloat m_scaleFactor;
    GLfloat m_maxSceneSize;
    QPoint m_visualSelectedBarPos;
    int m_visualSelectedBarSeriesIndex;
    bool m_hasHeightAdjustmentChanged;
    QPoint m_selectedBarPos;
    const QBar3DSeries *m_selectedBarSeries;
    BarRenderItem m_dummyBarRenderItem;
    QVector<BarRenderItemArray> m_renderingArrays;
    bool m_noZeroInRange;
    float m_seriesScale;
    float m_seriesStep;
    float m_seriesStart;
    QPoint m_clickedPosition;

public:
    explicit Bars3DRenderer(Bars3DController *controller);
    ~Bars3DRenderer();

    void updateData();
    void updateScene(Q3DScene *scene);
    void render(GLuint defaultFboHandle = 0);

protected:
    virtual void initializeOpenGL();

public slots:
    void updateBarSpecs(GLfloat thicknessRatio = 1.0f,
                        const QSizeF &spacing = QSizeF(1.0, 1.0),
                        bool relative = true);
    void updateSlicingActive(bool isSlicing);
    void updateSelectedBar(const QPoint &position, const QBar3DSeries *series);
    inline QPoint clickedPosition() const { return m_clickedPosition; }
    void resetClickedStatus();

    // Overloaded from abstract renderer
    virtual void updateAxisRange(QAbstract3DAxis::AxisOrientation orientation, float min, float max);

private:
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader);
    virtual void initGradientShaders(const QString &vertexShader, const QString &fragmentShader);
    virtual void updateShadowQuality(QAbstract3DGraph::ShadowQuality quality);
    virtual void updateTextures();
    virtual void fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh mesh);

    void drawSlicedScene();
    void drawScene(GLuint defaultFboHandle);

    void loadBackgroundMesh();
    void loadGridLineMesh();
    void loadLabelMesh();
    void initSelectionShader();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initLabelShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionBuffer();
#if !defined(QT_OPENGL_ES_2)
    void initDepthShader();
    void updateDepthBuffer();
#endif
    void calculateSceneScalingFactors();
    void calculateHeightAdjustment();
    Abstract3DController::SelectionType isSelected(int row, int bar, int seriesIndex);
    QPoint selectionColorToArrayPosition(const QVector3D &selectionColor);
    QBar3DSeries *selectionColorToSeries(const QVector3D &selectionColor);

    Q_DISABLE_COPY(Bars3DRenderer)

    friend class BarRenderItem;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
