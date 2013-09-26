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

#ifndef SURFACE3DRENDERER_P_H
#define SURFACE3DRENDERER_P_H

#include <QtCore/QSize>
#include <QtCore/QObject>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QFont>
#include <QLinearGradient>
#include <QWindow>

#include "datavisualizationglobal_p.h"
#include "surface3dcontroller_p.h"
#include "abstract3drenderer_p.h"
#include "scatterrenderitem_p.h"
#include "qsurfacedataproxy.h"

class QOpenGLShaderProgram;

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class ShaderHelper;
class ObjectHelper;
class SurfaceObject;
class TextureHelper;
class Theme;
class Drawer;
class Q3DScene;
class SelectionPointer;

class QT_DATAVISUALIZATION_EXPORT Surface3DRenderer : public Abstract3DRenderer
{
    Q_OBJECT

public:
    Surface3DController *m_controller;

    // Visual parameters
    QRect m_boundingRect;
    QDataVis::LabelTransparency m_labelTransparency;
    QFont m_font;
    bool m_isGridEnabled;

private:
    bool m_cachedIsSlicingActivated;

    // Internal attributes purely related to how the scene is drawn with GL.
    QRect m_mainViewPort;
    QRect m_sliceViewPort;
    ShaderHelper *m_shader;
    ShaderHelper *m_depthShader;
    ShaderHelper *m_backgroundShader;
    ShaderHelper *m_surfaceShader;
    ShaderHelper *m_surfaceGridShader;
    ShaderHelper *m_selectionShader;
    ShaderHelper *m_labelShader;
    GLfloat m_heightNormalizer;
    GLfloat m_scaleFactor;
    GLfloat m_scaleX;
    GLfloat m_scaleZ;
    GLfloat m_scaleXWithBackground;
    GLfloat m_scaleZWithBackground;
    GLfloat m_surfaceScaleX;
    GLfloat m_surfaceScaleZ;
    GLfloat m_surfaceOffsetX;
    GLfloat m_surfaceOffsetZ;
    qreal m_minVisibleColumnValue;
    qreal m_maxVisibleColumnValue;
    qreal m_minVisibleRowValue;
    qreal m_maxVisibleRowValue;
    qreal m_dataStepX;
    qreal m_dataStepZ;
    ObjectHelper *m_backgroundObj;
    ObjectHelper *m_gridLineObj;
    ObjectHelper *m_labelObj;
    SurfaceObject *m_surfaceObj;
    SurfaceObject *m_sliceSurfaceObj;
    GLuint m_depthTexture;
    GLuint m_depthFrameBuffer;
    GLuint m_selectionFrameBuffer;
    GLuint m_selectionDepthBuffer;
    GLuint m_gradientTexture;
    GLuint m_selectionTexture;
    GLuint m_selectionResultTexture;
    GLfloat m_shadowQualityToShader;
    bool m_cachedSmoothSurface;
    bool m_flatSupported;
    bool m_cachedSurfaceGridOn;
    SelectionPointer *m_selectionPointer;
    bool m_selectionActive;
    bool m_xFlipped;
    bool m_zFlipped;
    bool m_yFlipped;
    AbstractRenderItem m_dummyRenderItem;
    QSurfaceDataArray m_dataArray;
    QSurfaceDataArray m_sliceDataArray;
    QRect m_sampleSpace;
    GLint m_shadowQualityMultiplier;
    QSizeF m_areaSize;

    bool m_hasHeightAdjustmentChanged;

public:
    explicit Surface3DRenderer(Surface3DController *controller);
    ~Surface3DRenderer();

    void updateDataModel(QSurfaceDataProxy *dataProxy);
    void updateScene(Q3DScene *scene);
    void drawSlicedScene();
    void render(GLuint defaultFboHandle = 0);

protected:
    void initializeOpenGL();
    virtual void loadMeshFile();

public slots:
    bool updateSmoothStatus(bool enable);
    void updateSurfaceGridStatus(bool enable);
    void updateSurfaceGradient(const QLinearGradient &gradient);
    void updateSlicingActive(bool isSlicing);

private:
    void updateSliceDataModel(int selectionId);
    virtual void updateShadowQuality(QDataVis::ShadowQuality quality);
    virtual void updateTextures();
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader);
    QRect calculateSampleRect(QSurfaceDataProxy *dataProxy);
    void loadBackgroundMesh();
    void loadGridLineMesh();
    void loadLabelMesh();
    void loadSurfaceObj();
    void loadSliceSurfaceObj();
    void drawScene(GLuint defaultFboHandle);
    void handleResize();
    void calculateSceneScalingFactors();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initLabelShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionShaders();
    void initSurfaceShaders();
    void initSelectionBuffer();
    void initDepthShader();
    void updateSelectionTexture();
    void idToRGBA(uint id, uchar *r, uchar *g, uchar *b, uchar *a);
    void fillIdCorner(uchar *p, uchar r, uchar g, uchar b, uchar a, int stride);
    void surfacePointSelected(int id);
    QString createSelectionLabel(qreal value, int column, int row);
    qreal columnInRange(int column);
    qreal rowInRange(int row);
    QVector3D normalize(float x, float y, float z);
#if !defined(QT_OPENGL_ES_2)
    void updateDepthBuffer();
#endif

    Q_DISABLE_COPY(Surface3DRenderer)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // SURFACE3DRENDERER_P_H
