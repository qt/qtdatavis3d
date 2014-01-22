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

#include "datavisualizationglobal_p.h"
#include "surface3dcontroller_p.h"
#include "abstract3drenderer_p.h"
#include "scatterrenderitem_p.h"
#include "qsurfacedataproxy.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class ShaderHelper;
class ObjectHelper;
class SurfaceObject;
class Q3DScene;
class SelectionPointer;

class QT_DATAVISUALIZATION_EXPORT Surface3DRenderer : public Abstract3DRenderer
{
    Q_OBJECT

public:
    // Visual parameters
    QRect m_boundingRect;
    bool m_labelBackground;
    QFont m_font;
    bool m_isGridEnabled;

private:
    bool m_cachedIsSlicingActivated;

    // Internal attributes purely related to how the scene is drawn with GL.
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
    GLfloat m_minVisibleColumnValue;
    GLfloat m_maxVisibleColumnValue;
    GLfloat m_minVisibleRowValue;
    GLfloat m_maxVisibleRowValue;
    GLfloat m_visibleColumnRange;
    GLfloat m_visibleRowRange;
    ObjectHelper *m_backgroundObj;
    ObjectHelper *m_gridLineObj;
    ObjectHelper *m_labelObj;
    SurfaceObject *m_surfaceObj;
    SurfaceObject *m_sliceSurfaceObj;
    GLuint m_depthTexture;
    GLuint m_depthModelTexture;
    GLuint m_depthFrameBuffer;
    GLuint m_selectionFrameBuffer;
    GLuint m_selectionDepthBuffer;
    GLuint m_selectionTexture;
    GLuint m_selectionResultTexture;
    GLfloat m_shadowQualityToShader;
    bool m_cachedFlatShading;
    bool m_flatSupported;
    bool m_cachedSurfaceVisible;
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
    uint m_clickedPointId;
    bool m_hasHeightAdjustmentChanged;
    QPoint m_selectedPoint;
    const QSurface3DSeries *m_selectedSeries;
    GLuint m_uniformGradientTexture;
    QVector3D m_uniformGradientTextureColor;
    QPoint m_clickedPosition;

public:
    explicit Surface3DRenderer(Surface3DController *controller);
    ~Surface3DRenderer();

    void updateData();
    void updateSeries(const QList<QAbstract3DSeries *> &seriesList, bool updateVisibility);
    void updateRows(const QVector<int> &rows);
    void updateItem(const QVector<QPoint> &points);
    void updateScene(Q3DScene *scene);
    bool updateFlatStatus(bool enable);
    void updateSurfaceGridStatus(bool enable);
    void updateSlicingActive(bool isSlicing);
    void updateSelectedPoint(const QPoint &position, const QSurface3DSeries *series);
    inline QPoint clickedPosition() const { return m_clickedPosition; }
    void resetClickedStatus();

    void drawSlicedScene();
    void render(GLuint defaultFboHandle = 0);

protected:
    void initializeOpenGL();

signals:
    void flatShadingSupportedChanged(bool supported);

private:
    void updateSliceDataModel(const QPoint &point);
    void updateShadowQuality(QAbstract3DGraph::ShadowQuality quality);
    void updateTextures();
    void initShaders(const QString &vertexShader, const QString &fragmentShader);
    QRect calculateSampleRect(const QSurfaceDataArray &array);
    void loadBackgroundMesh();
    void loadGridLineMesh();
    void loadLabelMesh();
    void loadSurfaceObj();
    void loadSliceSurfaceObj();
    void drawScene(GLuint defaultFboHandle);
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
    void surfacePointSelected(const QPoint &point);
    QPoint selectionIdToSurfacePoint(uint id);
    QString createSelectionLabel(float value, int column, int row);
#if !defined(QT_OPENGL_ES_2)
    void updateDepthBuffer();
#endif
    void emitSelectedPointChanged(QPoint position);
    void generateUniformGradient(const QVector3D newColor);

    Q_DISABLE_COPY(Surface3DRenderer)
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
