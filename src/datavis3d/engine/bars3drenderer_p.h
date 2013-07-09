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

#ifndef Q3DBARSRENDERER_p_H
#define Q3DBARSRENDERER_p_H

#include <QtCore/QSize>
#include <QtCore/QObject>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QFont>
#include <QTime>
#include <QWindow>
#include <QMutex>

#include "datavis3dglobal_p.h"
#include "bars3dcontroller_p.h"
#include "qbardataproxy.h"
#include "barrenderitem_p.h"

//#define DISPLAY_RENDER_SPEED

class QPoint;
class QSizeF;
class QOpenGLShaderProgram;

QT_DATAVIS3D_BEGIN_NAMESPACE

class ShaderHelper;
class ObjectHelper;
class TextureHelper;
class Theme;
class Drawer;
class LabelItem;
class CameraHelper;
class QAbstractAxisPrivate;

class QT_DATAVIS3D_EXPORT Bars3dRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    // TODO: Filter to the set of attributes to be moved to the model object.
    // * All GL rendering only related attribs should be moved out of this public set.
    // * All attribs that are modifiable from QML need to e in this set.

    Bars3dController *m_controller;

    // Mutex for sharing resources between render and main threads.
    QMutex m_mutex;

    // Cached state based on emitted signals from the controller
    QSizeF m_cachedBarThickness;
    QSizeF m_cachedBarSpacing;
    QString m_cachedObjFile;
    bool m_cachedIsSlicingActivated;
    SelectionMode m_cachedSelectionMode;
    int m_cachedZoomLevel;
    int m_cachedRowCount;
    int m_cachedColumnCount;
    QRect m_cachedBoundingRect;
    Theme m_cachedTheme;
    LabelTransparency m_cachedLabelTransparency;
    QFont m_cachedFont;
    bool m_cachedIsGridEnabled;
    bool m_cachedIsBackgroundEnabled;
    ShadowQuality m_cachedShadowQuality;

    // Internal state
    bool m_hasNegativeValues;
    BarRenderItem *m_selectedBar; // TODO: Does this need to be member variable?
    BarRenderItem *m_previouslySelectedBar;
    QList<BarRenderItem *> *m_sliceSelection;
    QAbstractAxisPrivate *m_sliceAxisP; // not owned
    const LabelItem *m_sliceTitleItem; // not owned
    GLint m_tickCount;
    GLfloat m_tickStep;
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
    TextureHelper *m_textureHelper;
    Drawer *m_drawer;
    GLuint m_bgrTexture;
    GLuint m_depthTexture;
    GLuint m_selectionTexture;
    GLuint m_depthFrameBuffer;
    GLuint m_selectionFrameBuffer;
    GLuint m_selectionDepthBuffer;
    GLfloat m_shadowQualityToShader;
    GLfloat m_autoScaleAdjustment;
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

    QPoint m_selectionPointRequest;
    bool m_isSelectionPointRequestActive;

    bool m_hasHeightAdjustmentChanged;
    QPair<GLfloat,GLfloat> m_limits;
    BarRenderItem m_dummyBarRenderItem;
    QBarDataProxy *m_dataProxy; // Only valid during render

    BarRenderItemArray m_renderItemArray;
    bool m_valueUpdateNeeded;

#ifdef DISPLAY_RENDER_SPEED
    bool m_isFirstFrame;
    QTime m_lastFrameTime;
    GLint m_numFrames;
#endif

public:
    explicit Bars3dRenderer(Bars3dController *controller);
    ~Bars3dRenderer();

    void render(QBarDataProxy *dataProxy, bool valuesDirty, CameraHelper *camera,
                const LabelItem &xLabel, const LabelItem &yLabel, const LabelItem &zLabel,
                const GLuint defaultFboHandle = 0);

    QRect mainViewPort();
    // TODO: Not thread-safe, needs rethinking how axes create labels
    Drawer *drawer() { return m_drawer; }

public slots:
    void updateBarSpecs(QSizeF thickness = QSizeF(1.0f, 1.0f),
                        QSizeF spacing = QSizeF(1.0f, 1.0f),
                        bool relative = true);
    void updateTheme(Theme theme);
    void updateSelectionMode(SelectionMode newMode);
    void updateSlicingActive(bool isSlicing);
    void updateLimits(QPair<GLfloat, GLfloat> newLimits);
    void updateSampleSpace(int rowCount, int columnCount);
    void updateZoomLevel(int newZoomLevel);
    void updateFont(const QFont &font);
    void updateLabelTransparency(LabelTransparency transparency);
    void updateGridEnabled(bool enable);
    void updateBackgroundEnabled(bool enable);
    void updateShadowQuality(ShadowQuality quality);
    void updateTickCount(GLint tickCount, GLfloat step, GLfloat minimum = 0.0f);
    void updateMeshFileName(const QString &objFileName);
    void updateBoundingRect(const QRect boundingRect);
    void updatePosition(const QRect boundingRect);

    // Requests that upon next render pass the column and row under the given point is inspected for selection.
    // Only one request can be queued per render pass at this point. New request will override any pending requests.
    // After inspection the selectionUpdated signal is emitted.
    void requestSelectionAtPoint(const QPoint &point);

signals:
    void selectionUpdated(QVector3D selection);

private:
    void initializeOpenGL();
    void drawSlicedScene(CameraHelper *camera,
                         const LabelItem &xLabel, const LabelItem &yLabel, const LabelItem &zLabel);
    void drawScene(CameraHelper *camera, const GLuint defaultFboHandle);
    void handleResize();

    void loadBarMesh();
    void loadBackgroundMesh();
    void loadGridLineMesh();
    void loadLabelMesh();
    void initShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionShader();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initLabelShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionBuffer();
#if !defined(QT_OPENGL_ES_2)
    void initDepthShader();
    void updateDepthBuffer();
#endif
    void updateTextures();
    void calculateSceneScalingFactors();
    void calculateHeightAdjustment(const QPair<GLfloat, GLfloat> &limits);
    Bars3dController::SelectionType isSelected(GLint row, GLint bar);

    Q_DISABLE_COPY(Bars3dRenderer)

    friend class BarRenderItem;
};


QT_DATAVIS3D_END_NAMESPACE

#endif
