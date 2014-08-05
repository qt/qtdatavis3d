/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#ifndef Q3DSCATTERRENDERER_P_H
#define Q3DSCATTERRENDERER_P_H

#include "datavisualizationglobal_p.h"
#include "scatter3dcontroller_p.h"
#include "abstract3drenderer_p.h"
#include "scatterrenderitem_p.h"

class QSizeF;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class ShaderHelper;
class Q3DScene;
class ScatterSeriesRenderCache;
class QScatterDataItem;

class QT_DATAVISUALIZATION_EXPORT Scatter3DRenderer : public Abstract3DRenderer
{
    Q_OBJECT

private:
    // Internal state
    ScatterRenderItem *m_selectedItem; // points to renderitem array
    bool m_updateLabels;
    ShaderHelper *m_dotShader;
    ShaderHelper *m_dotGradientShader;
#if defined(QT_OPENGL_ES_2)
    ShaderHelper *m_pointShader;
#endif
    ShaderHelper *m_depthShader;
    ShaderHelper *m_selectionShader;
    ShaderHelper *m_backgroundShader;
    ShaderHelper *m_labelShader;
    GLuint m_bgrTexture;
    GLuint m_selectionTexture;
    GLuint m_depthFrameBuffer;
    GLuint m_selectionFrameBuffer;
    GLuint m_selectionDepthBuffer;
    GLfloat m_shadowQualityToShader;
    GLint m_shadowQualityMultiplier;
    float m_scaleX;
    float m_scaleY;
    float m_scaleZ;
    float m_scaleXWithBackground;
    float m_scaleYWithBackground;
    float m_scaleZWithBackground;
    int m_selectedItemIndex;
    ScatterSeriesRenderCache *m_selectedSeriesCache;
    ScatterSeriesRenderCache *m_oldSelectedSeriesCache;
    GLfloat m_dotSizeScale;
    ScatterRenderItem m_dummyRenderItem;
    GLfloat m_maxItemSize;
    int m_clickedIndex;
    bool m_havePointSeries;
    bool m_haveMeshSeries;
    bool m_haveUniformColorMeshSeries;
    bool m_haveGradientMeshSeries;

public:
    explicit Scatter3DRenderer(Scatter3DController *controller);
    ~Scatter3DRenderer();

    void updateData();
    void updateSeries(const QList<QAbstract3DSeries *> &seriesList);
    SeriesRenderCache *createNewCache(QAbstract3DSeries *series);
    void updateItems(const QVector<Scatter3DController::ChangeItem> &items);
    void updateScene(Q3DScene *scene);
    void updateAxisLabels(QAbstract3DAxis::AxisOrientation orientation,
                          const QStringList &labels);
    void updateAxisTitleVisibility(QAbstract3DAxis::AxisOrientation orientation,
                                   bool visible);

    QVector3D convertPositionToTranslation(const QVector3D &position, bool isAbsolute);

    inline int clickedIndex() const { return m_clickedIndex; }
    void resetClickedStatus();

    void render(GLuint defaultFboHandle);

public slots:
    void updateSelectedItem(int index, QScatter3DSeries *series);

protected:
    virtual void initializeOpenGL();
    virtual void fixCameraTarget(QVector3D &target);

private:
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader);
    virtual void initGradientShaders(const QString &vertexShader, const QString &fragmentShader);
    virtual void updateShadowQuality(QAbstract3DGraph::ShadowQuality quality);
    virtual void updateTextures();
    virtual void fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh mesh);

    void drawScene(GLuint defaultFboHandle);
    void drawLabels(bool drawSelection, const Q3DCamera *activeCamera,
                    const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix);

    void loadBackgroundMesh();
    void initSelectionShader();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initLabelShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionBuffer();
#if !defined(QT_OPENGL_ES_2)
    void initDepthShader();
    void updateDepthBuffer();
#else
    void initPointShader();
#endif
    void calculateTranslation(ScatterRenderItem &item);
    void calculateSceneScalingFactors();

    void selectionColorToSeriesAndIndex(const QVector4D &color, int &index,
                                        QAbstract3DSeries *&series);
    inline void updateRenderItem(const QScatterDataItem &dataItem, ScatterRenderItem &renderItem);

    Q_DISABLE_COPY(Scatter3DRenderer)
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
