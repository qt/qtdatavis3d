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

//#define DISPLAY_RENDER_SPEED

class QPoint;
class QSizeF;
class QOpenGLShaderProgram;

QT_DATAVIS3D_BEGIN_NAMESPACE

class QDataItem;
class QDataRow;
class QDataSetPrivate;
class ShaderHelper;
class ObjectHelper;
class TextureHelper;
class Theme;
class Drawer;
class LabelItem;
class CameraHelper;

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
    // Visual parameters
    QRect m_boundingRect;
    QString m_objFile;
    Theme *m_theme;
    LabelTransparency m_labelTransparency;
    QFont m_font;
    bool m_isGridEnabled;
    bool m_isBackgroundEnabled;
    ShadowQuality m_shadowQuality;
    bool m_hasNegativeValues;

    // Data parameters
    QDataItem *m_selectedBar;
    QDataRow *m_sliceSelection;

    GLint m_tickCount;
    GLfloat m_tickStep;

    // Internal attributes purely related to how the scene is drawn with GL.
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
    QSizeF m_barThickness;
    QSizeF m_barSpacing;

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

    // Cached state based on emitted signals from the controller
    SelectionMode m_selectionMode;
    int m_zoomLevel;
    bool m_isSlicingActivated;
    QPair<GLfloat,GLfloat> m_limits;
    int m_rowCount;
    int m_columnCount;

#ifdef DISPLAY_RENDER_SPEED
    bool m_isFirstFrame;
    QTime m_lastFrameTime;
    GLint m_numFrames;
#endif

public:
    explicit Bars3dRenderer(Bars3dController *controller);
    ~Bars3dRenderer();

    void render(QDataSetPrivate *dataSet, CameraHelper *camera, const LabelItem &xLabel, const LabelItem &yLabel, const LabelItem &zLabel, const GLuint defaultFboHandle = 0);

    // bar thickness, spacing between bars, and is spacing relative to thickness or absolute
    // y -component sets the thickness/spacing of z -direction
    // With relative 0.0f means side-to-side, 1.0f = one thickness in between
    void setBarSpecs(QSizeF thickness = QSizeF(1.0f, 1.0f),
                     QSizeF spacing = QSizeF(1.0f, 1.0f),
                     bool relative = true);

    // bar type; bars (=cubes), pyramids, cones, cylinders, etc.
    void setBarType(BarStyle style, bool smooth = false);

    // override bar type with own mesh
    void setMeshFileName(const QString &objFileName);


    // Select preset camera placement
    void setCameraPreset(CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) (or (-90...90) if there are negative values) angles and distance in
    // percentage (10...500))
    void setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance = 100);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text
    // colors are affected)
    void setColorTheme(ColorTheme colorTheme);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height)
    // color to change from bottom to top
    void setBarColor(QColor baseColor, QColor heightColor, QColor depthColor,
                     bool uniform = true);

    // Set tick count and step. Note; tickCount * step should be the maximum possible value of data
    // set. Minimum is the absolute minimum possible value a bar can have. This is especially
    // important to set if values can be negative.
    void setTickCount(GLint tickCount, GLfloat step, GLfloat minimum = 0.0f);

    // TODO: light placement API

    // Size
    void setSize(const int width, const int height);
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

    QRect mainViewPort();

    // Font size adjustment
    void setFontSize(float fontsize);
    float fontSize();

    // Set font
    void setFont(const QFont &font);
    QFont font();

    // Label transparency adjustment
    void setLabelTransparency(LabelTransparency transparency);
    LabelTransparency labelTransparency();

    // Enable or disable background grid
    void setGridEnabled(bool enable);
    bool gridEnabled();

    // Enable or disable background mesh
    void setBackgroundEnabled(bool enable);
    bool backgroundEnabled();

    // Adjust shadow quality
    void setShadowQuality(ShadowQuality quality);
    ShadowQuality shadowQuality();

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

public slots:
    void updateSelectionMode(SelectionMode newMode);
    void updateSlicingActive(bool isSlicing);
    void updateDataSet(QDataSetPrivate *newDataSet);
    void updateLimits(QPair<GLfloat, GLfloat> newLimits);
    void updateSampleSpace(int columnCount, int rowCount);
    void updateZoomLevel(int newZoomLevel);

    // Requests that upon next render pass the column and row under the given point is inspected for selection.
    // Only one request can be queued per render pass at this point. New request will override any pending requests.
    // After inspection the selectionUpdated signal is emitted.
    void inspectSelectionAtPoint(const QPoint &point);

signals:
    void selectionUpdated(QVector3D selection);

private:
    void initializeOpenGL();
    void drawSlicedScene(QDataSetPrivate *dataSet, CameraHelper *camera, const LabelItem &xLabel, const LabelItem &yLabel, const LabelItem &zLabel);
    void drawScene(QDataSetPrivate *dataSet, CameraHelper *camera, const GLuint defaultFboHandle);
    void handleResize();

    Q_DISABLE_COPY(Bars3dRenderer)
};


QT_DATAVIS3D_END_NAMESPACE

#endif
