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

#ifndef SURFACE3DRENDERER_P_H
#define SURFACE3DRENDERER_P_H

#include "datavisualizationglobal_p.h"
#include "surface3dcontroller_p.h"
#include "abstract3drenderer_p.h"
#include "surfaceseriesrendercache_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class ShaderHelper;
class Q3DScene;

class QT_DATAVISUALIZATION_EXPORT Surface3DRenderer : public Abstract3DRenderer
{
    Q_OBJECT

private:
    bool m_cachedIsSlicingActivated;

    // Internal attributes purely related to how the scene is drawn with GL.
    ShaderHelper *m_depthShader;
    ShaderHelper *m_backgroundShader;
    ShaderHelper *m_surfaceFlatShader;
    ShaderHelper *m_surfaceSmoothShader;
    ShaderHelper *m_surfaceGridShader;
    ShaderHelper *m_surfaceSliceFlatShader;
    ShaderHelper *m_surfaceSliceSmoothShader;
    ShaderHelper *m_selectionShader;
    ShaderHelper *m_labelShader;
    GLfloat m_heightNormalizer;
    GLfloat m_scaleFactor;
    GLfloat m_scaleX;
    GLfloat m_scaleZ;
    GLfloat m_scaleXWithBackground;
    GLfloat m_scaleZWithBackground;
    GLuint m_depthTexture;
    GLuint m_depthModelTexture;
    GLuint m_depthFrameBuffer;
    GLuint m_selectionFrameBuffer;
    GLuint m_selectionDepthBuffer;
    GLuint m_selectionResultTexture;
    GLfloat m_shadowQualityToShader;
    bool m_flatSupported;
    bool m_selectionActive;
    AbstractRenderItem m_dummyRenderItem;
    GLint m_shadowQualityMultiplier;
    QSizeF m_areaSize;
    bool m_hasHeightAdjustmentChanged;
    QPoint m_selectedPoint;
    QSurface3DSeries *m_selectedSeries;
    QPoint m_clickedPosition;
    bool m_selectionTexturesDirty;
    GLuint m_noShadowTexture;

public:
    explicit Surface3DRenderer(Surface3DController *controller);
    ~Surface3DRenderer();

    void updateData();
    void updateSeries(const QList<QAbstract3DSeries *> &seriesList);
    SeriesRenderCache *createNewCache(QAbstract3DSeries *series);
    void cleanCache(SeriesRenderCache *cache);
    void updateSelectionMode(QAbstract3DGraph::SelectionFlags mode);
    void updateRows(const QVector<Surface3DController::ChangeRow> &rows);
    void updateItems(const QVector<Surface3DController::ChangeItem> &points);
    void updateScene(Q3DScene *scene);
    void updateSlicingActive(bool isSlicing);
    void updateSelectedPoint(const QPoint &position, QSurface3DSeries *series);
    inline QPoint clickedPosition() const { return m_clickedPosition; }
    void resetClickedStatus();
    QVector3D convertPositionToTranslation(const QVector3D &position, bool isAbsolute);

    void render(GLuint defaultFboHandle = 0);

protected:
    void initializeOpenGL();

signals:
    void flatShadingSupportedChanged(bool supported);

private:
    void checkFlatSupport(SurfaceSeriesRenderCache *cache);
    void updateObjects(SurfaceSeriesRenderCache *cache, bool dimensionChanged);
    void updateSliceDataModel(const QPoint &point);
    QPoint mapCoordsToSampleSpace(SurfaceSeriesRenderCache *cache, const QPointF &coords);
    void findMatchingRow(float z, int &sample, int direction, QSurfaceDataArray &dataArray);
    void findMatchingColumn(float x, int &sample, int direction, QSurfaceDataArray &dataArray);
    void updateSliceObject(SurfaceSeriesRenderCache *cache, const QPoint &point);
    void updateShadowQuality(QAbstract3DGraph::ShadowQuality quality);
    void updateTextures();
    void initShaders(const QString &vertexShader, const QString &fragmentShader);
    QRect calculateSampleRect(const QSurfaceDataArray &array);
    void loadBackgroundMesh();

    void drawSlicedScene();
    void drawScene(GLuint defaultFboHandle);
    void drawLabels(bool drawSelection, const Q3DCamera *activeCamera,
                    const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix);

    void calculateSceneScalingFactors();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initLabelShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionShaders();
    void initSurfaceShaders();
    void initSelectionBuffer();
    void initDepthShader();
    void updateSelectionTextures();
    void createSelectionTexture(SurfaceSeriesRenderCache *cache, uint &lastSelectionId);
    void idToRGBA(uint id, uchar *r, uchar *g, uchar *b, uchar *a);
    void fillIdCorner(uchar *p, uchar r, uchar g, uchar b, uchar a, int stride);
    void surfacePointSelected(const QPoint &point);
    void updateSelectionPoint(SurfaceSeriesRenderCache *cache, const QPoint &point, bool label);
    QPoint selectionIdToSurfacePoint(uint id);
#if !defined(QT_OPENGL_ES_2)
    void updateDepthBuffer();
#endif
    void emitSelectedPointChanged(QPoint position);

    Q_DISABLE_COPY(Surface3DRenderer)

    friend class SurfaceObject;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
