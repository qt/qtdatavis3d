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

#ifndef SURFACE3DRENDERER_P_H
#define SURFACE3DRENDERER_P_H

#include <QtCore/QSize>
#include <QtCore/QObject>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QFont>
#include <QLinearGradient>
#include <QWindow>

#include "datavis3dglobal_p.h"
#include "surface3dcontroller_p.h"

class QOpenGLShaderProgram;

QT_DATAVIS3D_BEGIN_NAMESPACE

class ShaderHelper;
class ObjectHelper;
class SurfaceObject;
class TextureHelper;
class Theme;
class Drawer;
class CameraHelper;
class SelectionPointer;

class QT_DATAVIS3D_EXPORT Surface3dRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    enum MousePressType {
        MouseNone = 0,
        MouseOnScene,
        MouseOnOverview,
        MouseOnZoom,
        MouseRotating,
        MouseOnPinch
    };

    Surface3dController *m_controller;

    // Interaction related parameters // TODO: Moved to controller
    MousePressType m_mousePressed;
    QPoint m_mousePos;
    QDataVis::SelectionMode m_selectionMode;

    // Visual parameters
    QRect m_boundingRect;
    Theme m_cachedTheme;
    QDataVis::LabelTransparency m_labelTransparency;
    QFont m_font;
    bool m_isGridEnabled;
    bool m_isBackgroundEnabled;
    QDataVis::ShadowQuality m_shadowQuality;
    bool m_hasNegativeValues;

private:
    // Data parameters
    QList<qreal> m_series; // TODO: TEMP
    GLint m_segmentYCount;
    GLfloat m_segmentYStep;
    GLint m_segmentXCount;
    GLint m_segmentZCount;

    // Internal attributes purely related to how the scene is drawn with GL.
    QRect m_mainViewPort;
    QRect m_sliceViewPort;
    ShaderHelper *m_backgroundShader;
    ShaderHelper *m_surfaceShader;
    ShaderHelper *m_surfaceGridShader;
    ShaderHelper *m_selectionShader;
    TextureHelper *m_textureHelper;
    bool m_isInitialized;
    GLfloat m_yRange; // m_heightNormalizer
    GLfloat m_yAdjustment;
    GLfloat m_xLength;
    GLfloat m_zLength;
    GLfloat m_maxDimension;
    GLfloat m_scaleFactor;
    GLfloat m_scaleX;
    GLfloat m_scaleZ;
    GLfloat m_maxSceneSize;
    ObjectHelper *m_backgroundObj;
    ObjectHelper *m_gridLineObj;
    SurfaceObject *m_surfaceObj;
    GLuint m_depthTexture;
    GLuint m_depthFrameBuffer;
    GLuint m_selectionFrameBuffer;
    GLuint m_selectionDepthBuffer;
    GLuint m_gradientTexture;
    GLuint m_selectionTexture;
    GLuint m_selectionResultTexture;
    GLfloat m_shadowQualityToShader;
    bool m_querySelection;
    bool m_cachedSmoothSurface;
    bool m_cachedSurfaceGridOn;
    SelectionPointer *m_selectionPointer;
    bool m_selectionActive;

    Drawer *m_drawer;

public:
    explicit Surface3dRenderer(Surface3dController *controller);
    ~Surface3dRenderer();

    void initializeOpenGL();
    void render(CameraHelper *camera, const GLuint defaultFboHandle = 0);

    // TODO: Not thread-safe, needs rethinking how axes create labels
    Drawer *drawer() { return m_drawer; }

public slots:
    void updateTheme(Theme theme);
    void updateSmoothStatus(bool enable);
    void updateSurfaceGridStatus(bool enable);
    void updateSurfaceGradient();
    void updateSegmentCount(GLint segmentCount, GLfloat step, GLfloat minimum = 0.0f);

    void getSelection();

public:
    // Size
    const QSize size();
    const QRect boundingRect();
    void setBoundingRect(const QRect boundingRect);
    void setWidth(const int width);
    int width();
    void setHeight(const int height);
    int height();
    void setX(const int x);
    int x();
    void setY(const int y);
    int y();

    void handleResize();

#if !defined(QT_OPENGL_ES_2)
    void updateDepthBuffer();
#endif
    void loadBackgroundMesh();
    void loadGridLineMesh();
    void loadSurfaceObj();

    // TODO: temp
    void setXZStuff(GLint segmentXCount, GLint segmentZCount);
    void setSeries(QList<qreal> series);

private:
    void drawScene(CameraHelper *camera, const GLuint defaultFboHandle);
    void calculateSceneScalingFactors();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionShaders();
    void initSurfaceShaders();
    void updateSelectionTexture();
    void idToRGBA(uint id, uchar *r, uchar *g, uchar *b, uchar *a);
    void fillIdCorner(uchar *p, uchar r, uchar g, uchar b, uchar a, int stride);
    void surfacePointSelected(qreal value, int column, int row);
    void surfacePointCleared();
    QVector3D normalize(float x, float y, float z);

    Q_DISABLE_COPY(Surface3dRenderer)
};

QT_DATAVIS3D_END_NAMESPACE

#endif // SURFACE3DRENDERER_P_H
