/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
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
    SelectionMode m_selectionMode;

    // Visual parameters
    QRect m_boundingRect;
    Theme *m_theme;
    LabelTransparency m_labelTransparency;
    QFont m_font;
    bool m_isGridEnabled;
    bool m_isBackgroundEnabled;
    ShadowQuality m_shadowQuality;
    bool m_hasNegativeValues;

private:
    // Data parameters
    QList<qreal> m_series; // TODO: TEMP
    GLint m_tickYCount;
    GLfloat m_tickYStep;
    GLint m_tickXCount;
    GLint m_tickZCount;

    // Internal attributes purely related to how the scene is drawn with GL.
    QRect m_mainViewPort;
    QRect m_sliceViewPort;
    ShaderHelper *m_backgroundShader;
    ShaderHelper *m_surfaceShader;
    ShaderHelper *m_surfaceGridShader;
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
    GLfloat m_shadowQualityToShader;
    bool m_cachedSmoothSurface;
    bool m_cachedSurfaceGridOn;

    Drawer *m_drawer;

public:
    explicit Surface3dRenderer(Surface3dController *controller);
    ~Surface3dRenderer();

    void initializeOpenGL();
    void render(CameraHelper *camera, const GLuint defaultFboHandle = 0);

    // TODO: Not thread-safe, needs rethinking how axes create labels
    Drawer *drawer() { return m_drawer; }

public slots:
    void updateSmoothStatus(bool enable);
    void updateSurfaceGridStatus(bool enable);
    void updateTickCount(GLint tickCount, GLfloat step, GLfloat minimum = 0.0f);

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

#if defined(Q_OS_ANDROID)
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
#endif
    void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    void wheelEvent(QWheelEvent *event);
    void handleResize();

#if !defined(QT_OPENGL_ES_2)
    void updateDepthBuffer();
#endif
    void loadBackgroundMesh();
    void loadGridLineMesh();
    void loadSurfaceObj();

    // TODO: temp
    void setXZStuff(GLint tickXCount, GLint tickZCount);
    void setSeries(QList<qreal> series);

private:
    void drawScene(CameraHelper *camera, const GLuint defaultFboHandle);
    void calculateSceneScalingFactors();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSurfaceShaders();

    Q_DISABLE_COPY(Surface3dRenderer)
};

QT_DATAVIS3D_END_NAMESPACE

#endif // SURFACE3DRENDERER_P_H
