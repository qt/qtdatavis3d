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

#ifndef Q3DSCATTERRENDERER_P_H
#define Q3DSCATTERRENDERER_P_H

#include <QtCore/QSize>
#include <QtCore/QObject>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QFont>
#include <QTime>
#include <QWindow>
#include <QMutex>

#include "datavis3dglobal_p.h"
#include "scatter3dcontroller_p.h"
#include "abstract3drenderer_p.h"
#include "qscatterdataproxy.h"
#include "scatterrenderitem_p.h"

//#define DISPLAY_RENDER_SPEED

class QPoint;
class QSizeF;
class QOpenGLShaderProgram;

QT_DATAVIS3D_BEGIN_NAMESPACE

class ShaderHelper;
class ObjectHelper;
class TextureHelper;
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

    // Mutex for sharing resources between render and main threads.
    QMutex m_mutex;

    // Cached state based on emitted signals from the controller
    QString m_cachedObjFile;
    SelectionMode m_cachedSelectionMode;
    int m_cachedZoomLevel;
    bool m_cachedIsGridEnabled;
    bool m_cachedIsBackgroundEnabled;

    // Internal state
    ScatterRenderItem *m_selectedItem; // points to renderitem array
    ScatterRenderItem *m_previouslySelectedItem; // points to renderitem array
    GLint m_tickCount;
    GLfloat m_tickStep;
    bool m_xFlipped;
    bool m_zFlipped;
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
    TextureHelper *m_textureHelper;
    GLuint m_bgrTexture;
    GLuint m_depthTexture;
    GLuint m_selectionTexture;
    GLuint m_depthFrameBuffer;
    GLuint m_selectionFrameBuffer;
    GLuint m_selectionDepthBuffer;
    GLfloat m_shadowQualityToShader;
    GLfloat m_heightNormalizer;
    GLfloat m_yAdjustment;
    GLfloat m_scaleFactor;
    QVector3D m_selection;
    QSizeF m_areaSize;

    QPoint m_selectionPointRequest;
    bool m_isSelectionPointRequestActive;

    bool m_hasHeightAdjustmentChanged;
    QPair<GLfloat, GLfloat> m_limits;
    ScatterRenderItem m_dummyRenderItem;
    QScatterDataProxy *m_dataProxy; // Only valid during render

    ScatterRenderItemArray m_renderItemArray;
    bool m_valueUpdateNeeded;

#ifdef DISPLAY_RENDER_SPEED
    bool m_isFirstFrame;
    QTime m_lastFrameTime;
    GLint m_numFrames;
#endif

public:
    explicit Scatter3DRenderer(Scatter3DController *controller);
    ~Scatter3DRenderer();

    void render(QScatterDataProxy *dataProxy, bool valuesDirty, CameraHelper *camera,
                const GLuint defaultFboHandle = 0);

    QRect mainViewPort();

public slots:
    void updateSelectionMode(SelectionMode newMode);
    //void updateLimits(QPair<GLfloat, GLfloat> newLimits);
    void updateZoomLevel(int newZoomLevel);
    void updateGridEnabled(bool enable);
    void updateBackgroundEnabled(bool enable);
    void updateTickCount(GLint tickCount, GLfloat step, GLfloat minimum = 0.0f);
    void updateMeshFileName(const QString &objFileName);

    // Requests that upon next render pass the column and row under the given point is inspected for selection.
    // Only one request can be queued per render pass at this point. New request will override any pending requests.
    // After inspection the selectionUpdated signal is emitted.
    virtual void requestSelectionAtPoint(const QPoint &point);

signals:
    void selectionUpdated(QVector3D selection);

private:
    virtual void initializePreOpenGL();
    virtual void initializeOpenGL();
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader);
    virtual void updateShadowQuality(ShadowQuality quality);
    virtual void updateTextures();

    void drawScene(CameraHelper *camera, const GLuint defaultFboHandle);
    void handleResize();

    void loadBarMesh();
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
    void calculateSceneScalingFactors(const QRect &areaRect = QRect(0, 0, 1, 1));
    void calculateTranslation(ScatterRenderItem &item);
    void calculateHeightAdjustment(const QPair<GLfloat, GLfloat> &limits);
    Scatter3DController::SelectionType isSelected(GLint bar, const QVector3D &selection);

    Q_DISABLE_COPY(Scatter3DRenderer)

    friend class ScatterRenderItem;
};


QT_DATAVIS3D_END_NAMESPACE

#endif
