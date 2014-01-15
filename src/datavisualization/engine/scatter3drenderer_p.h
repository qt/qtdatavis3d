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

#ifndef Q3DSCATTERRENDERER_P_H
#define Q3DSCATTERRENDERER_P_H

#include "datavisualizationglobal_p.h"
#include "scatter3dcontroller_p.h"
#include "abstract3drenderer_p.h"
#include "qscatterdataproxy.h"
#include "scatterrenderitem_p.h"

class QPoint;
class QSizeF;
class QOpenGLShaderProgram;

namespace QtDataVisualization {

class ShaderHelper;
class ObjectHelper;
class LabelItem;
class Q3DScene;
class QAbstractAxisPrivate;

class QT_DATAVISUALIZATION_EXPORT Scatter3DRenderer : public Abstract3DRenderer
{
    Q_OBJECT

private:
    // Internal state
    ScatterRenderItem *m_selectedItem; // points to renderitem array
    bool m_xFlipped;
    bool m_zFlipped;
    bool m_yFlipped;
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
    GLfloat m_scaleFactor;
    int m_selectedItemIndex;
    int m_selectedItemTotalIndex;
    int m_selectedItemSeriesIndex;
    const QScatter3DSeries *m_selectedSeries;
    QSizeF m_areaSize;
    GLfloat m_dotSizeScale;
    QVector3D m_translationOffset;
    bool m_hasHeightAdjustmentChanged;
    ScatterRenderItem m_dummyRenderItem;
    QVector<ScatterRenderItemArray> m_renderingArrays;
    GLfloat m_backgroundMargin;
    GLfloat m_maxItemSize;
    QVector<float> m_cachedItemSize;

public:
    explicit Scatter3DRenderer(Scatter3DController *controller);
    ~Scatter3DRenderer();

    void updateSeries(const QList<QAbstract3DSeries *> &seriesList, bool updateVisibility);
    void updateData();
    void updateScene(Q3DScene *scene);

    void render(GLuint defaultFboHandle);

protected:
    virtual void initializeOpenGL();

private:
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader);
    virtual void initGradientShaders(const QString &vertexShader, const QString &fragmentShader);
    virtual void updateShadowQuality(QAbstract3DGraph::ShadowQuality quality);
    virtual void updateTextures();
    virtual void fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh mesh);

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
#else
    void initPointShader();
#endif
    void calculateTranslation(ScatterRenderItem &item);
    void calculateSceneScalingFactors();

    Q_DISABLE_COPY(Scatter3DRenderer)

    friend class ScatterRenderItem;

public slots:
    // Overloaded from abstract renderer
    virtual void updateAxisRange(QAbstract3DAxis::AxisOrientation orientation, float min, float max);

    void updateSelectedItem(int index, const QScatter3DSeries *series);

signals:
    void itemClicked(int index, QScatter3DSeries *series);

private:
    QVector3D indexToSelectionColor(GLint index);
    void selectionColorToSeriesAndIndex(const QVector3D &color, int &index, QScatter3DSeries *&series);
};

}

#endif
