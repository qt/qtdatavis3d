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

#ifndef Q3DSCATTERRENDERER_P_H
#define Q3DSCATTERRENDERER_P_H

#include "datavis3dglobal_p.h"
#include "scatter3dcontroller_p.h"
#include "abstract3drenderer_p.h"
#include "qscatterdataproxy.h"
#include "scatterrenderitem_p.h"

class QPoint;
class QSizeF;
class QOpenGLShaderProgram;

QT_DATAVIS3D_BEGIN_NAMESPACE

class ShaderHelper;
class ObjectHelper;
class LabelItem;
class CameraHelper;
class QAbstractAxisPrivate;

class QT_DATAVIS3D_EXPORT Scatter3DRenderer : public Abstract3DRenderer
{
    Q_OBJECT

private:
    // TODO: Filter to the set of attributes to be moved to the model object.
    // * All GL rendering only related attribs should be moved out of this public set.
    // * All attribs that are modifiable from QML need to e in this set.

    Scatter3DController *m_controller;

    // Internal state
    ScatterRenderItem *m_selectedItem; // points to renderitem array
    ScatterRenderItem *m_previouslySelectedItem; // points to renderitem array
    bool m_xFlipped;
    bool m_zFlipped;
    bool m_yFlipped;
    QRect m_mainViewPort;
    bool m_updateLabels;
    ShaderHelper *m_dotShader;
    ShaderHelper *m_depthShader;
    ShaderHelper *m_selectionShader;
    ShaderHelper *m_backgroundShader;
    ShaderHelper *m_labelShader;
    ObjectHelper *m_dotObj;
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
    GLfloat m_scaleFactor;
    QVector3D m_selection;
    QSizeF m_areaSize;
    GLfloat m_dotSizeScale;

    bool m_hasHeightAdjustmentChanged;
    ScatterRenderItem m_dummyRenderItem;

    ScatterRenderItemArray m_renderItemArray;

public:
    explicit Scatter3DRenderer(Scatter3DController *controller);
    ~Scatter3DRenderer();

    void updateDataModel(QScatterDataProxy *dataProxy);
    void render(CameraHelper *camera, const GLuint defaultFboHandle);

    QRect mainViewPort();

protected:
    virtual void initializeOpenGL();
    virtual void loadMeshFile();

private:
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader);
    virtual void updateShadowQuality(QDataVis::ShadowQuality quality);
    virtual void updateTextures();

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
    void calculateTranslation(ScatterRenderItem &item);
    void calculateSceneScalingFactors();
    Scatter3DController::SelectionType isSelected(GLint dot, const QVector3D &selection);

    Q_DISABLE_COPY(Scatter3DRenderer)

    friend class ScatterRenderItem;

public slots:
    void updateBackgroundEnabled(bool enable);

    // Overloaded from abstract renderer
    virtual void updateAxisRange(QAbstractAxis::AxisOrientation orientation, qreal min, qreal max);
};


QT_DATAVIS3D_END_NAMESPACE

#endif
