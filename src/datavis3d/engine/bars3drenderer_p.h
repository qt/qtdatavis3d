/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q3DBARSRENDERER_p_H
#define Q3DBARSRENDERER_p_H

#include "datavis3dglobal_p.h"
#include "bars3dcontroller_p.h"
#include "abstract3drenderer_p.h"
#include "qbardataproxy.h"
#include "barrenderitem_p.h"

class QPoint;
class QSizeF;
class QOpenGLShaderProgram;

QT_DATAVIS3D_BEGIN_NAMESPACE

class ShaderHelper;
class ObjectHelper;
class LabelItem;
class CameraHelper;

class QT_DATAVIS3D_EXPORT Bars3dRenderer : public Abstract3DRenderer
{
    Q_OBJECT

private:
    // TODO: Filter to the set of attributes to be moved to the model object.
    Bars3dController *m_controller;

    // Cached state based on emitted signals from the controller
    QSizeF m_cachedBarThickness;
    QSizeF m_cachedBarSpacing;
    bool m_cachedIsSlicingActivated;
    int m_cachedRowCount;
    int m_cachedColumnCount;

    // Internal state
    BarRenderItem *m_selectedBar; // points to renderitem array
    QList<BarRenderItem *> *m_sliceSelection;
    AxisRenderCache *m_sliceCache; // not owned
    const LabelItem *m_sliceTitleItem; // not owned
    bool m_xFlipped;
    bool m_zFlipped;
    bool m_yFlipped;
    QRect m_mainViewPort;
    QRect m_sliceViewPort;
    bool m_updateLabels;
    ShaderHelper *m_barShader;
    ShaderHelper *m_depthShader;
    ShaderHelper *m_selectionShader;
    ShaderHelper *m_backgroundShader;
    ShaderHelper *m_labelShader;
    ObjectHelper *m_barObj;
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
    GLfloat m_heightNormalizer;
    GLfloat m_yAdjustment;
    GLfloat m_rowWidth;
    GLfloat m_columnDepth;
    GLfloat m_maxDimension;
    GLfloat m_scaleX;
    GLfloat m_scaleZ;
    GLfloat m_scaleFactor;
    GLfloat m_maxSceneSize;
    QVector3D m_selection;
    QVector3D m_previousSelection;

    bool m_hasHeightAdjustmentChanged;
    BarRenderItem m_dummyBarRenderItem;

    BarRenderItemArray m_renderItemArray;

public:
    explicit Bars3dRenderer(Bars3dController *controller);
    ~Bars3dRenderer();

    void updateDataModel(QBarDataProxy *dataProxy);
    void render(CameraHelper *camera, const GLuint defaultFboHandle = 0);

    QRect mainViewPort();

protected:
    virtual void initializeOpenGL();
    virtual void loadMeshFile();

public slots:
    void updateBarSpecs(GLfloat thicknessRatio = 1.0f,
                        QSizeF spacing = QSizeF(1.0, 1.0),
                        bool relative = true);
    void updateSelectionMode(QDataVis::SelectionMode newMode);
    void updateSlicingActive(bool isSlicing);
    void updateSampleSpace(int rowCount, int columnCount);
    void updateBackgroundEnabled(bool enable);
    void updateSelectedBarPos(QPoint position);

    // Overloaded from abstract renderer
    virtual void updateAxisRange(QAbstractAxis::AxisOrientation orientation, qreal min, qreal max);

signals:
    void selectedBarPosChanged(QPoint position);

private:
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader);
    virtual void updateShadowQuality(QDataVis::ShadowQuality quality);
    virtual void updateTextures();

    void drawSlicedScene(CameraHelper *camera,
                         const LabelItem &xLabel, const LabelItem &yLabel, const LabelItem &zLabel);
    void drawScene(CameraHelper *camera, const GLuint defaultFboHandle);
    void handleResize();

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
    Abstract3DController::SelectionType isSelected(GLint row, GLint bar);

    Q_DISABLE_COPY(Bars3dRenderer)

    friend class BarRenderItem;
};


QT_DATAVIS3D_END_NAMESPACE

#endif
