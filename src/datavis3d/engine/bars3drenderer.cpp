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

#include "bars3drenderer_p.h"
#include "bars3dcontroller_p.h"
#include "camerahelper_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"
#include "theme_p.h"
#include "utils_p.h"
#include "drawer_p.h"
#include "qabstractaxis_p.h"
#include "qbardataitem.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QThread>
#include <qmath.h>
#include <QDebug>
#include <QMutexLocker>

// Uncommenting this draws the shadow map with wider FOV than scene itself, making the light
// seem to be closer to scene than it actually is. This way shadows look slightly better (to me anyway)
#define USE_WIDER_SHADOWS

// You can verify that depth buffer drawing works correctly by uncommenting this.
// You should see the scene from  where the light is
//#define SHOW_DEPTH_TEXTURE_SCENE

#ifdef DISPLAY_RENDER_SPEED
#include <QTime>
#endif

QT_DATAVIS3D_BEGIN_NAMESPACE

#define DISPLAY_FULL_DATA_ON_SELECTION // Append selection value text with row and column labels

const GLfloat gridLineWidth = 0.005f;
static QVector3D selectionSkipColor = QVector3D(255, 255, 255); // Selection texture's background color

Bars3dRenderer::Bars3dRenderer(Bars3dController *controller)
    : QObject(controller),
      m_controller(controller),
      m_hasNegativeValues(false),
      m_selectedBar(0),
      m_previouslySelectedBar(0),
      m_sliceSelection(0),
      m_sliceAxisP(0),
      m_sliceTitleItem(0),
      m_tickCount(0),
      m_tickStep(0),
      m_xFlipped(false),
      m_zFlipped(false),
      m_yFlipped(false),
      m_updateLabels(false),
      m_barShader(0),
      m_depthShader(0),
      m_selectionShader(0),
      m_backgroundShader(0),
      m_labelShader(0),
      m_barObj(0),
      m_backgroundObj(0),
      m_gridLineObj(0),
      m_labelObj(0),
      m_drawer(new Drawer(m_cachedTheme, m_cachedFont, m_cachedLabelTransparency)),
      m_bgrTexture(0),
      m_depthTexture(0),
      m_selectionTexture(0),
      m_depthFrameBuffer(0),
      m_selectionFrameBuffer(0),
      m_selectionDepthBuffer(0),
      m_shadowQualityToShader(33.3f),
      m_autoScaleAdjustment(1.0f),
      m_heightNormalizer(0.0f),
      m_yAdjustment(0.0f),
      m_rowWidth(0),
      m_columnDepth(0),
      m_maxDimension(0),
      m_scaleX(0),
      m_scaleZ(0),
      m_scaleFactor(0),
      m_maxSceneSize(40.0),
      m_selection(selectionSkipColor),
      m_hasHeightAdjustmentChanged(true),
      m_dataProxy(0),
      m_dataWindowChanged(false)
    #ifdef DISPLAY_RENDER_SPEED
    ,m_isFirstFrame(true),
      m_numFrames(0)
    #endif
{
    m_dummyBarRenderItem.setRenderer(this);

    // Listen to changes in the drawer
    QObject::connect(m_drawer, &Drawer::drawerChanged, this, &Bars3dRenderer::updateTextures);

    // Listen to changes in the controller
    QObject::connect(m_controller, &Bars3dController::themeChanged, this,
                     &Bars3dRenderer::updateTheme);
    QObject::connect(m_controller, &Bars3dController::selectionModeChanged, this,
                     &Bars3dRenderer::updateSelectionMode);
    QObject::connect(m_controller, &Bars3dController::slicingActiveChanged, this,
                     &Bars3dRenderer::updateSlicingActive);
    QObject::connect(m_controller, &Bars3dController::limitsChanged, this,
                     &Bars3dRenderer::updateLimits);
    QObject::connect(m_controller, &Bars3dController::sampleSpaceChanged, this,
                     &Bars3dRenderer::updateSampleSpace);
    QObject::connect(m_controller, &Bars3dController::barSpecsChanged, this,
                     &Bars3dRenderer::updateBarSpecs);
    QObject::connect(m_controller, &Bars3dController::objFileChanged, this,
                     &Bars3dRenderer::updateMeshFileName);
    QObject::connect(m_controller, &Bars3dController::boundingRectChanged, this,
                     &Bars3dRenderer::updateBoundingRect);
    QObject::connect(m_controller, &Bars3dController::sizeChanged, this,
                     &Bars3dRenderer::updateBoundingRect);
    QObject::connect(m_controller, &Bars3dController::positionChanged, this,
                     &Bars3dRenderer::updatePosition);
    QObject::connect(m_controller, &Bars3dController::fontChanged, this,
                     &Bars3dRenderer::updateFont);
    QObject::connect(m_controller, &Bars3dController::labelTransparencyUpdated, this,
                     &Bars3dRenderer::updateLabelTransparency);
    QObject::connect(m_controller, &Bars3dController::gridEnabledChanged, this,
                     &Bars3dRenderer::updateGridEnabled);
    QObject::connect(m_controller, &Bars3dController::backgroundEnabledChanged, this,
                     &Bars3dRenderer::updateBackgroundEnabled);
    QObject::connect(m_controller, &Bars3dController::shadowQualityChanged, this,
                     &Bars3dRenderer::updateShadowQuality);
    QObject::connect(m_controller, &Bars3dController::tickCountChanged, this,
                     &Bars3dRenderer::updateTickCount);
    QObject::connect(m_controller, &Bars3dController::zoomLevelChanged, this,
                     &Bars3dRenderer::updateZoomLevel);

    updateTheme(m_controller->theme());
    updateSampleSpace(m_controller->rowCount(), m_controller->columnCount());
    updateSelectionMode(m_controller->selectionMode());
    updateSlicingActive(m_controller->isSlicingActive());
    updateLimits(m_controller->limits());
    updateZoomLevel(m_controller->zoomLevel());
    updateBarSpecs(m_controller->barThickness(), m_controller->barSpacing(),
                   m_controller->isBarSpecRelative());
    updateMeshFileName(m_controller->objFile());
    updateFont(m_controller->font());
    updateLabelTransparency(m_controller->labelTransparency());
    updateGridEnabled(m_controller->gridEnabled());
    updateBackgroundEnabled(m_controller->backgroundEnabled());

    initializeOpenGL();

    updateBoundingRect(m_controller->boundingRect());
    updateShadowQuality(m_controller->shadowQuality());

    // TODO: Protect with mutex or redesign how axes create label items?
    if (m_controller->axisX())
        m_controller->axisX()->d_ptr->setDrawer(m_drawer);
    if (m_controller->axisY())
        m_controller->axisY()->d_ptr->setDrawer(m_drawer);
    if (m_controller->axisZ())
        m_controller->axisZ()->d_ptr->setDrawer(m_drawer);
}

Bars3dRenderer::~Bars3dRenderer()
{
    m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);
    m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
    m_textureHelper->deleteTexture(&m_selectionTexture);
    m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
    m_textureHelper->deleteTexture(&m_bgrTexture);
    if (m_sliceSelection) {
        m_sliceSelection->clear(); // Slice doesn't own its items
        delete m_sliceSelection;
    }
    delete m_barShader;
    delete m_depthShader;
    delete m_selectionShader;
    delete m_backgroundShader;
    delete m_barObj;
    delete m_backgroundObj;
    delete m_gridLineObj;
    delete m_textureHelper;
    delete m_drawer;
}

void Bars3dRenderer::initializeOpenGL()
{
    initializeOpenGLFunctions();

    m_textureHelper = new TextureHelper();
    m_drawer->initializeOpenGL();

    // Initialize shaders
#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > ShadowNone) {
        if (!m_cachedTheme.m_uniformColor) {
            initShaders(QStringLiteral(":/shaders/vertexShadow"),
                        QStringLiteral(":/shaders/fragmentShadowNoTexColorOnY"));
        } else {
            initShaders(QStringLiteral(":/shaders/vertexShadow"),
                        QStringLiteral(":/shaders/fragmentShadowNoTex"));
        }
        initBackgroundShaders(QStringLiteral(":/shaders/vertexShadow"),
                              QStringLiteral(":/shaders/fragmentShadowNoTex"));
    } else {
        if (!m_cachedTheme.m_uniformColor) {
            initShaders(QStringLiteral(":/shaders/vertex"),
                        QStringLiteral(":/shaders/fragmentColorOnY"));
        } else {
            initShaders(QStringLiteral(":/shaders/vertex"),
                        QStringLiteral(":/shaders/fragment"));
        }
        initBackgroundShaders(QStringLiteral(":/shaders/vertex"),
                              QStringLiteral(":/shaders/fragment"));
    }
#else
    if (!m_cachedTheme.m_uniformColor) {
        initShaders(QStringLiteral(":/shaders/vertexES2"),
                    QStringLiteral(":/shaders/fragmentColorOnYES2"));
    } else {
        initShaders(QStringLiteral(":/shaders/vertexES2"),
                    QStringLiteral(":/shaders/fragmentES2"));
    }
    initBackgroundShaders(QStringLiteral(":/shaders/vertexES2"),
                          QStringLiteral(":/shaders/fragmentES2"));
#endif

    initLabelShaders(QStringLiteral(":/shaders/vertexLabel"),
                     QStringLiteral(":/shaders/fragmentLabel"));

#if !defined(QT_OPENGL_ES_2)
    // Init depth shader (for shadows). Init in any case, easier to handle shadow activation if done via api.
    initDepthShader();
#endif

    // Init selection shader
    initSelectionShader();

    // Load default mesh
    loadBarMesh();

    // Load grid line mesh
    loadGridLineMesh();

    // Load label mesh
    loadLabelMesh();

    // Set OpenGL features
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

#if !defined(QT_OPENGL_ES_2)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
#endif

    // Set view port
    glViewport(m_sliceViewPort.x(), m_sliceViewPort.y(),
               m_sliceViewPort.width(), m_sliceViewPort.height());

    // Resize in case we've missed resize events
    // Resize calls initSelectionBuffer and initDepthBuffer, so they don't need to be called here
    handleResize();

    // Load background mesh (we need to be initialized first)
    loadBackgroundMesh();
}

void Bars3dRenderer::render(QBarDataProxy *dataProxy,
                            bool valuesDirty,
                            CameraHelper *camera,
                            const LabelItem &xLabel,
                            const LabelItem &yLabel,
                            const LabelItem &zLabel,
                            const GLuint defaultFboHandle)
{
#ifdef DISPLAY_RENDER_SPEED
    // For speed computation
    if (m_isFirstFrame) {
        m_lastFrameTime.start();
        m_isFirstFrame = false;
    }

    // Measure speed (as milliseconds per frame)
    m_numFrames++;
    if (m_lastFrameTime.elapsed() >= 1000) { // print only if last measurement was more than 1s ago
        qDebug() << qreal(m_lastFrameTime.elapsed()) / qreal(m_numFrames) << "ms/frame (=" << qreal(m_numFrames) << "fps)";
        m_numFrames = 0;
        m_lastFrameTime.restart();
    }
#endif

    // TODO Would it be enough to just mutex cache update?
    // TODO --> Only if there is no need to store m_dataProxy for later, e.g. for string formatting
    QMutexLocker(dataProxy->mutex());
    m_dataProxy = dataProxy;

    // Update cached data window
    // TODO Should data changes be notified via signal instead of reading data in render?
    // TODO this cache initialization assumes data window starts at 0,0 offset from array
    if (valuesDirty || m_dataWindowChanged) {
        m_dataWindowChanged = false;
        int dataRowCount = dataProxy->rowCount();
        for (int i = 0; i < m_renderItemArray.size(); i++) {
            int j = 0;
            if (i < dataRowCount) {
                const QBarDataRow *dataRow = dataProxy->rowAt(i);
                int updateSize = qMin(dataRow->size(), m_renderItemArray[i].size());
                if (dataRow) {
                    for (; j < updateSize ; j++)
                        m_renderItemArray[i][j].setValue(dataRow->at(j).value());
                }
            }
            for (; j < m_renderItemArray[i].size(); j++)
                m_renderItemArray[i][j].setValue(0);
        }
    }

    if (defaultFboHandle) {
        glDepthMask(true);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    QVector3D clearColor = Utils::vectorFromColor(m_cachedTheme.m_windowColor);
    glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_hasHeightAdjustmentChanged) {
        // Set initial camera position. Also update if height adjustment has changed.
        camera->setDefaultCameraOrientation(QVector3D(0.0f, 0.0f, 6.0f + zComp),
                                            QVector3D(0.0f, -m_yAdjustment, zComp),
                                            QVector3D(0.0f, 1.0f, 0.0f));
    }

    // If slice selection is on, draw the sliced scene
    if (m_cachedIsSlicingActivated)
        drawSlicedScene(camera, xLabel, yLabel, zLabel);

    // Draw bars scene
    drawScene(camera, defaultFboHandle);
}

void Bars3dRenderer::drawSlicedScene(CameraHelper *camera,
                                     const LabelItem &xLabel,
                                     const LabelItem &yLabel,
                                     const LabelItem &zLabel)
{
    GLfloat barPosX = 0;
    GLint startBar = 0;
    GLint stopBar = m_sliceSelection->size();
    GLint stepBar = 1;
    QVector3D lightPos;

    // Specify viewport
    glViewport(m_sliceViewPort.x(), m_sliceViewPort.y(),
               m_sliceViewPort.width(), m_sliceViewPort.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, (GLfloat)m_sliceViewPort.width()
                                 / (GLfloat)m_sliceViewPort.height(), 0.1f, 100.0f);

#ifdef ROTATE_ZOOM_SELECTION
    // Calculate view matrix
    QMatrix4x4 viewMatrix = m_controller->calculateViewMatrix(m_cachedZoomLevel * m_autoScaleAdjustment,
                                                              m_sliceViewPort.width(),
                                                              m_sliceViewPort.height());

    // Get light position (rotate light with camera, a bit above it (as set in defaultLightPos))
    lightPos = camera->calculateLightPosition(defaultLightPos);

    if (viewMatrix.row(0).z() <= 0) {
        startBar = m_sliceSelection->size() - 1;
        stopBar = -1;
        stepBar = -1;
    }
#else
    // Set view matrix
    QMatrix4x4 viewMatrix;

    // Adjust scaling (zoom rate based on aspect ratio)
    GLfloat camPosZoomed = 5.0f / m_autoScaleAdjustment + zComp;

    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, camPosZoomed),
                      QVector3D(0.0f, 0.0f, zComp),
                      QVector3D(0.0f, 1.0f, 0.0f));

    // Set light position a bit below the camera to reduce glare (depends on do we have row or column zoom)
    QVector3D sliceLightPos = defaultLightPos;
    sliceLightPos.setY(-10.0f);
    if (ModeZoomColumn == m_cachedSelectionMode)
        lightPos = camera->calculateLightPosition(sliceLightPos, -85.0f);
    else
        lightPos = camera->calculateLightPosition(sliceLightPos, 5.0f);
#endif

    // Bind bar shader
    m_barShader->bind();

    // Draw bars
    // Draw the selected row / column
    for (int bar = startBar; bar != stopBar; bar += stepBar) {
        BarRenderItem *item = m_sliceSelection->at(bar);
        if (!item)
            continue;

        GLfloat barHeight = item->value() / m_heightNormalizer;

        if (barHeight < 0)
            glCullFace(GL_FRONT);
        else
            glCullFace(GL_BACK);

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 itModelMatrix;

        GLfloat barPosY = item->translation().y() - m_yAdjustment / 2.0f + 0.2f; // we need some room for labels underneath; add +0.2f
        if (ModeZoomRow == m_cachedSelectionMode)
            barPosX = item->translation().x();
        else
            barPosX = -(item->translation().z() - zComp); // flip z; frontmost bar to the left
        modelMatrix.translate(barPosX, barPosY, zComp);
        modelMatrix.scale(QVector3D(m_scaleX, barHeight, m_scaleZ));
        itModelMatrix.scale(QVector3D(m_scaleX, barHeight, m_scaleZ));

        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

        QVector3D baseColor = Utils::vectorFromColor(m_cachedTheme.m_baseColor);
        QVector3D heightColor = Utils::vectorFromColor(m_cachedTheme.m_heightColor) * barHeight;

        QVector3D barColor = baseColor + heightColor;

        GLfloat lightStrength = m_cachedTheme.m_lightStrength;

        if (barHeight != 0) {
            // Set shader bindings
            m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
            m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
            m_barShader->setUniformValue(m_barShader->model(), modelMatrix);
            m_barShader->setUniformValue(m_barShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_barShader->setUniformValue(m_barShader->MVP(), MVPMatrix);
            m_barShader->setUniformValue(m_barShader->color(), barColor);
            m_barShader->setUniformValue(m_barShader->lightS(), lightStrength);
            m_barShader->setUniformValue(m_barShader->ambientS(), m_cachedTheme.m_ambientStrength);

            // Draw the object
            m_drawer->drawObject(m_barShader, m_barObj);
        }
    }

    // Release bar shader
    m_barShader->release();

    // Draw labels
    m_labelShader->bind();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glCullFace(GL_BACK);
    if (m_cachedLabelTransparency > TransparencyNone) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Draw labels for axes
    BarRenderItem *dummyItem(0);
    const LabelItem &sliceSelectionLabel = *m_sliceTitleItem;
    if (ModeZoomRow == m_cachedSelectionMode) {
        if (m_sliceTitleItem) {
            m_drawer->drawLabel(*dummyItem, sliceSelectionLabel, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(0.0f, 0.0f, 0.0f), 0,
                                m_cachedSelectionMode, m_labelShader,
                                m_labelObj, camera, false, false, LabelTop);
        }
        m_drawer->drawLabel(*dummyItem, zLabel, viewMatrix, projectionMatrix,
                            QVector3D(0.0f, m_yAdjustment, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), 0,
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, camera, false, false, LabelBottom);
    } else {
        m_drawer->drawLabel(*dummyItem, xLabel, viewMatrix, projectionMatrix,
                            QVector3D(0.0f, m_yAdjustment, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), 0,
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, camera, false, false, LabelBottom);
        if (m_sliceTitleItem) {
            m_drawer->drawLabel(*dummyItem, sliceSelectionLabel, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(0.0f, 0.0f, 0.0f), 0,
                                m_cachedSelectionMode, m_labelShader,
                                m_labelObj, camera, false, false, LabelTop);
        }
    }
    m_drawer->drawLabel(*dummyItem, yLabel, viewMatrix, projectionMatrix,
                        QVector3D(0.0f, m_yAdjustment, zComp),
                        QVector3D(0.0f, 0.0f, 90.0f), 0,
                        m_cachedSelectionMode, m_labelShader,
                        m_labelObj, camera, false, false, LabelLeft);

    // Draw labels for bars
    for (int col = 0; col < m_sliceSelection->size(); col++) {
        BarRenderItem *item = m_sliceSelection->at(col);
        // Draw values
        m_drawer->drawLabel(*item, item->labelItem(), viewMatrix, projectionMatrix,
                            QVector3D(0.0f, m_yAdjustment, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), (item->value() / m_heightNormalizer),
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, camera);

        // Draw labels
        // TODO: Protect with mutex or redesign axis label handling?
        if (m_sliceAxisP->labelItems().size() > col) {
            const LabelItem *labelItem(0);
            // If draw order of bars is flipped, label draw order should be too
            if (m_xFlipped) {
                labelItem = m_sliceAxisP->labelItems().at(
                            m_sliceAxisP->labelItems().size() - col - 1);
            } else {
                labelItem = m_sliceAxisP->labelItems().at(col);
            }
            m_drawer->drawLabel(*item, *labelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(0.0f, 0.0f, -45.0f), (item->value() / m_heightNormalizer),
                                m_cachedSelectionMode, m_labelShader,
                                m_labelObj, camera, false, false, LabelBelow);
        }
    }

    glDisable(GL_TEXTURE_2D);
    if (m_cachedLabelTransparency > TransparencyNone)
        glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // Release label shader
    m_labelShader->release();
}

void Bars3dRenderer::drawScene(CameraHelper *camera,
                               const GLuint defaultFboHandle)
{
    GLint startBar = 0;
    GLint stopBar = 0;
    GLint stepBar = 0;

    GLint startRow = 0;
    GLint stopRow = 0;
    GLint stepRow = 0;

    GLfloat backgroundRotation = 0;

    GLfloat barPos = 0;
    GLfloat rowPos = 0;

    //m_selection = selectionSkipColor;

    // Specify viewport
    glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
               m_mainViewPort.width(), m_mainViewPort.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, (GLfloat)m_mainViewPort.width()
                                 / (GLfloat)m_mainViewPort.height(), 0.1f, 100.0f);

    // Calculate view matrix
    QMatrix4x4 viewMatrix = m_controller->calculateViewMatrix(
                m_cachedZoomLevel * m_autoScaleAdjustment,
                m_mainViewPort.width(),
                m_mainViewPort.height(),
                m_hasNegativeValues);

    // Calculate drawing order
    // Draw order is reversed to optimize amount of drawing (ie. draw front objects first, depth test handles not needing to draw objects behind them)
    if (viewMatrix.row(0).x() > 0) {
        startRow = 0;
        stopRow = m_cachedRowCount;
        stepRow = 1;
        m_zFlipped = false;
    } else {
        startRow = m_cachedRowCount - 1;
        stopRow = -1;
        stepRow = -1;
        m_zFlipped = true;
    }
    if (viewMatrix.row(0).z() <= 0) {
        startBar = 0;
        stopBar = m_cachedColumnCount;
        stepBar = 1;
        m_xFlipped = false;
    } else {
        startBar = m_cachedColumnCount - 1;
        stopBar = -1;
        stepBar = -1;
        m_xFlipped = true;
    }

    // Check if we're viewing the scene from below
    if (viewMatrix.row(2).y() < 0)
        m_yFlipped = true;
    else
        m_yFlipped = false;

    // calculate background rotation based on view matrix rotation
    if (viewMatrix.row(0).x() > 0 && viewMatrix.row(0).z() <= 0)
        backgroundRotation = 270.0f;
    else if (viewMatrix.row(0).x() > 0 && viewMatrix.row(0).z() > 0)
        backgroundRotation = 180.0f;
    else if (viewMatrix.row(0).x() <= 0 && viewMatrix.row(0).z() > 0)
        backgroundRotation = 90.0f;
    else if (viewMatrix.row(0).x() <= 0 && viewMatrix.row(0).z() <= 0)
        backgroundRotation = 0.0f;

    // Get light position (rotate light with camera, a bit above it (as set in defaultLightPos))
    QVector3D lightPos = camera->calculateLightPosition(defaultLightPos);
    //lightPos = QVector3D(0.0f, 4.0f, zComp); // center of bars, 4.0f above - for testing

    // Skip depth rendering if we're in slice mode
    // TODO: Fix this, causes problems if depth rendering is off in slice mode
    // Introduce regardless of shadow quality to simplify logic
    QMatrix4x4 depthViewMatrix;
    QMatrix4x4 depthProjectionMatrix;

#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > ShadowNone/*!m_cachedIsSlicingActivated*/) {
        // Render scene into a depth texture for using with shadow mapping
        // Enable drawing to depth framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthFrameBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Bind depth shader
        m_depthShader->bind();

        // Set viewport for depth map rendering. Must match texture size. Larger values give smoother shadows.
        glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
                   m_mainViewPort.width() * m_cachedShadowQuality,
                   m_mainViewPort.height() * m_cachedShadowQuality);

        // Get the depth view matrix
        // It may be possible to hack lightPos here if we want to make some tweaks to shadow
        depthViewMatrix.lookAt(lightPos, QVector3D(0.0f, -m_yAdjustment, zComp),
                               QVector3D(0.0f, 1.0f, 0.0f));
        // TODO: Why does depthViewMatrix.column(3).y() goes to zero when we're directly above? That causes the scene to be not drawn from above -> must be fixed
        //qDebug() << lightPos << depthViewMatrix << depthViewMatrix.column(3);
        // Set the depth projection matrix
#ifdef USE_WIDER_SHADOWS
        // Use this for a bit exaggerated shadows
        depthProjectionMatrix.perspective(20.0f, (GLfloat)m_mainViewPort.width()
                                          / (GLfloat)m_mainViewPort.height(), 3.0f, 100.0f);
#else
        // Use these for normal shadows, with the light further away
        depthProjectionMatrix = projectionMatrix;
#endif
        // Draw bars to depth buffer
        for (int row = startRow; row != stopRow; row += stepRow) {
            for (int bar = startBar; bar != stopBar; bar += stepBar) {
                BarRenderItem &item = m_renderItemArray[row][bar];
                if (!item.value())
                    continue;

                // TODO Cache barHeights to render items
                GLfloat barHeight = item.value() / m_heightNormalizer;

                // skip shadows for 0 -height bars
                if (barHeight == 0)
                    continue;

                // Set front face culling for positive valued bars and back face culling for
                // negative valued bars to reduce self-shadowing issues
                if (barHeight < 0)
                    glCullFace(GL_BACK);
                else
                    glCullFace(GL_FRONT);

                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;

                barPos = (bar + 1) * (m_cachedBarSpacing.width());
                rowPos = (row + 1) * (m_cachedBarSpacing.height());

                modelMatrix.translate((m_rowWidth - barPos) / m_scaleFactor,
                                      barHeight - m_yAdjustment,
                                      (m_columnDepth - rowPos) / m_scaleFactor + zComp);
                modelMatrix.scale(QVector3D(m_scaleX, barHeight, m_scaleZ));

                MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

                m_depthShader->setUniformValue(m_depthShader->MVP(), MVPMatrix);

                // 1st attribute buffer : vertices
                glEnableVertexAttribArray(m_depthShader->posAtt());
                glBindBuffer(GL_ARRAY_BUFFER, m_barObj->vertexBuf());
                glVertexAttribPointer(m_depthShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0,
                                      (void *)0);

                // Index buffer
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_barObj->elementBuf());

                // Draw the triangles
                glDrawElements(GL_TRIANGLES, m_barObj->indexCount(), GL_UNSIGNED_SHORT,
                               (void *)0);

                // Free buffers
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDisableVertexAttribArray(m_depthShader->posAtt());
            }
        }

        // Disable drawing to depth framebuffer (= enable drawing to screen)
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);

        // Release depth shader
        m_depthShader->release();

#if 0 // Use this if you want to see what is being drawn to the framebuffer
        // You'll also have to comment out GL_COMPARE_R_TO_TEXTURE -line in texturehelper (if using it)
        m_labelShader->bind();
        glCullFace(GL_BACK);
        glEnable(GL_TEXTURE_2D);
        QMatrix4x4 modelMatrix;
        QMatrix4x4 viewmatrix;
        viewmatrix.lookAt(QVector3D(0.0f, 0.0f, 2.5f + zComp),
                          QVector3D(0.0f, 0.0f, zComp),
                          QVector3D(0.0f, 1.0f, 0.0f));
        modelMatrix.translate(0.0, 0.0, zComp);
        QMatrix4x4 MVPMatrix = projectionMatrix * viewmatrix * modelMatrix;
        m_labelShader->setUniformValue(m_labelShader->MVP(), MVPMatrix);
        m_drawer->drawObject(m_labelShader, m_labelObj,
                             m_depthTexture);
        glDisable(GL_TEXTURE_2D);
        m_labelShader->release();
#endif
        // Reset culling to normal
        glCullFace(GL_BACK);

        // Revert to original viewport
        glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
                   m_mainViewPort.width(), m_mainViewPort.height());
    }
#endif

    // Skip selection mode drawing if we're slicing or have no selection mode
    if (!m_cachedIsSlicingActivated && m_cachedSelectionMode > ModeNone) {
        // Bind selection shader
        m_selectionShader->bind();

        // Draw bars to selection buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(selectionSkipColor.x() / 255, selectionSkipColor.y() / 255,
                     selectionSkipColor.z() / 255, 1.0f); // Set clear color to white (= selectionSkipColor)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled
        for (int row = startRow; row != stopRow; row += stepRow) {
            for (int bar = startBar; bar != stopBar; bar += stepBar) {
                BarRenderItem &item = m_renderItemArray[row][bar];
                if (!item.value())
                    continue;
                // TODO resolved bar heights should be cached?
                GLfloat barHeight = item.value() / m_heightNormalizer;

                if (barHeight < 0)
                    glCullFace(GL_FRONT);
                else
                    glCullFace(GL_BACK);

                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;

                barPos = (bar + 1) * (m_cachedBarSpacing.width());
                rowPos = (row + 1) * (m_cachedBarSpacing.height());

                modelMatrix.translate((m_rowWidth - barPos) / m_scaleFactor,
                                      barHeight - m_yAdjustment,
                                      (m_columnDepth - rowPos) / m_scaleFactor + zComp);
                modelMatrix.scale(QVector3D(m_scaleX, barHeight, m_scaleZ));

                MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

                // TODO: Save position to qdataitem, so that we don't need to calculate it each time?

                //#if !defined(QT_OPENGL_ES_2)
                //                QVector3D barColor = QVector3D((GLdouble)row / 32767.0,
                //                                               (GLdouble)bar / 32767.0,
                //                                               0.0);
                //#else
                QVector3D barColor = QVector3D((GLdouble)row / 255.0,
                                               (GLdouble)bar / 255.0,
                                               0.0);
                //#endif

                m_selectionShader->setUniformValue(m_selectionShader->MVP(), MVPMatrix);
                m_selectionShader->setUniformValue(m_selectionShader->color(), barColor);

                // 1st attribute buffer : vertices
                glEnableVertexAttribArray(m_selectionShader->posAtt());
                glBindBuffer(GL_ARRAY_BUFFER, m_barObj->vertexBuf());
                glVertexAttribPointer(m_selectionShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0,
                                      (void *)0);

                // Index buffer
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_barObj->elementBuf());

                // Draw the triangles
                glDrawElements(GL_TRIANGLES, m_barObj->indexCount(), GL_UNSIGNED_SHORT, (void *)0);

                // Free buffers
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDisableVertexAttribArray(m_selectionShader->posAtt());
            }
        }
        glEnable(GL_DITHER);

        // Read color under cursor
        if (Bars3dController::MouseOnScene == m_controller->mouseState())
            m_selection = Utils::getSelection(m_controller->mousePosition(), m_cachedBoundingRect.height());

        QMutexLocker locker(&m_mutex);
        if (m_isSelectionPointRequestActive) {
            m_isSelectionPointRequestActive = false;
            m_selection = Utils::getSelection(m_selectionPointRequest, m_cachedBoundingRect.height());
            emit selectionUpdated(m_selection);
        }
        locker.unlock();

        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);

        // Release selection shader
        m_selectionShader->release();

#if 0 // Use this if you want to see what is being drawn to the framebuffer
        glCullFace(GL_BACK);
        m_labelShader->bind();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        QMatrix4x4 modelMatrix;
        QMatrix4x4 viewmatrix;
        viewmatrix.lookAt(QVector3D(0.0f, 0.0f, 2.0f + zComp),
                          QVector3D(0.0f, 0.0f, zComp),
                          QVector3D(0.0f, 1.0f, 0.0f));
        modelMatrix.translate(0.0, 0.0, zComp);
        QMatrix4x4 MVPMatrix = projectionMatrix * viewmatrix * modelMatrix;
        m_labelShader->setUniformValue(m_labelShader->MVP(), MVPMatrix);
        m_drawer->drawObject(m_labelShader, m_labelObj, m_selectionTexture);
        glDisable(GL_TEXTURE_2D);
        m_labelShader->release();
#endif

    }

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    // Bind bar shader
    m_barShader->bind();

    // Draw bars
    if (!m_cachedIsSlicingActivated && m_sliceSelection) {
        m_sliceSelection->clear(); // Slice doesn't own its items
        m_sliceAxisP = 0;
        m_sliceTitleItem = 0;
    }
    bool barSelectionFound = false;
    for (int row = startRow; row != stopRow; row += stepRow) {
        for (int bar = startBar; bar != stopBar; bar += stepBar) {
            BarRenderItem &item = m_renderItemArray[row][bar];
            if (!item.value())
                continue;

            // TODO resolved bar heights should be cached?
            GLfloat barHeight = item.value() / m_heightNormalizer;

            if (barHeight < 0)
                glCullFace(GL_FRONT);
            else
                glCullFace(GL_BACK);

            QMatrix4x4 modelMatrix;
            QMatrix4x4 itModelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;

            barPos = (bar + 1) * (m_cachedBarSpacing.width());
            rowPos = (row + 1) * (m_cachedBarSpacing.height());
            modelMatrix.translate((m_rowWidth - barPos) / m_scaleFactor,
                                  barHeight - m_yAdjustment,
                                  (m_columnDepth - rowPos) / m_scaleFactor + zComp);
            modelMatrix.scale(QVector3D(m_scaleX, barHeight, m_scaleZ));
            itModelMatrix.scale(QVector3D(m_scaleX, barHeight, m_scaleZ));
#ifdef SHOW_DEPTH_TEXTURE_SCENE
            MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;
#else
            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
#endif
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            QVector3D baseColor = Utils::vectorFromColor(m_cachedTheme.m_baseColor);
            QVector3D heightColor = Utils::vectorFromColor(m_cachedTheme.m_heightColor) * barHeight;
            QVector3D depthColor = Utils::vectorFromColor(m_cachedTheme.m_depthColor)
                    * (float(row) / GLfloat(m_cachedRowCount));

            QVector3D barColor = baseColor + heightColor + depthColor;

            GLfloat lightStrength = m_cachedTheme.m_lightStrength;

            if (m_cachedSelectionMode > ModeNone) {

                Bars3dController::SelectionType selectionType = isSelected(row, bar);

                switch (selectionType) {
                case Bars3dController::SelectionBar: {
                    barColor = Utils::vectorFromColor(m_cachedTheme.m_highlightBarColor);
                    lightStrength = m_cachedTheme.m_highlightLightStrength;
                    // Insert data to QDataItem. We have no ownership, don't delete the previous one
                    if (!m_cachedIsSlicingActivated) {
                        m_selectedBar = &item;
                        m_selectedBar->setPosition(QPoint(row, bar));
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        barSelectionFound = true;
                        if (m_cachedSelectionMode >= ModeZoomRow) {
                            item.setTranslation(modelMatrix.column(3).toVector3D());
                            m_sliceSelection->append(&item);
                        }
                    }
                    break;
                }
                case Bars3dController::SelectionRow: {
                    // Current bar is on the same row as the selected bar
                    barColor = Utils::vectorFromColor(m_cachedTheme.m_highlightRowColor);
                    lightStrength = m_cachedTheme.m_highlightLightStrength;
                    if (!m_cachedIsSlicingActivated && ModeZoomRow == m_cachedSelectionMode) {
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        m_sliceSelection->append(&item);
                        if (!m_sliceAxisP) {
                            // m_sliceAxisP is the axis for labels, while title comes from different axis.
                            m_sliceAxisP = m_controller->axisZ()->d_ptr.data();
                            if (m_controller->axisX()->d_ptr->labelItems().size() > row)
                                m_sliceTitleItem = m_controller->axisX()->d_ptr->labelItems().at(row);
                        }
                    }
                    break;
                }
                case Bars3dController::SelectionColumn: {
                    // Current bar is on the same column as the selected bar
                    barColor = Utils::vectorFromColor(m_cachedTheme.m_highlightColumnColor);
                    lightStrength = m_cachedTheme.m_highlightLightStrength;
                    if (!m_cachedIsSlicingActivated && ModeZoomColumn == m_cachedSelectionMode) {
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        m_sliceSelection->append(&item);
                        if (!m_sliceAxisP) {
                            // m_sliceAxisP is the axis for labels, while title comes from different axis.
                            m_sliceAxisP = m_controller->axisX()->d_ptr.data();
                            if (m_controller->axisZ()->d_ptr->labelItems().size() > bar)
                                m_sliceTitleItem = m_controller->axisZ()->d_ptr->labelItems().at(bar);
                        }
                    }
                    break;
                }
                case Bars3dController::SelectionNone: {
                    // Current bar is not selected, nor on a row or column
                    // do nothing
                    break;
                }
                }
            }

            if (barHeight != 0) {
                // Set shader bindings
                m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
                m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
                m_barShader->setUniformValue(m_barShader->model(), modelMatrix);
                m_barShader->setUniformValue(m_barShader->nModel(),
                                             itModelMatrix.transposed().inverted());
                m_barShader->setUniformValue(m_barShader->MVP(), MVPMatrix);
                m_barShader->setUniformValue(m_barShader->color(), barColor);
                m_barShader->setUniformValue(m_barShader->ambientS(), m_cachedTheme.m_ambientStrength);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > ShadowNone) {
                    // Set shadow shader bindings
                    m_barShader->setUniformValue(m_barShader->shadowQ(), m_shadowQualityToShader);
                    m_barShader->setUniformValue(m_barShader->depth(), depthMVPMatrix);
                    m_barShader->setUniformValue(m_barShader->lightS(), lightStrength / 10.0f);

                    // Draw the object
                    m_drawer->drawObject(m_barShader, m_barObj, 0, m_depthTexture);
                } else
#endif
                {
                    // Set shadowless shader bindings
                    m_barShader->setUniformValue(m_barShader->lightS(), lightStrength);

                    // Draw the object
                    m_drawer->drawObject(m_barShader, m_barObj);
                }
            }
        }
    }

    // Release bar shader
    m_barShader->release();

    // Bind background shader
    m_backgroundShader->bind();

    if (m_hasNegativeValues)
        glDisable(GL_CULL_FACE);
    else
        glCullFace(GL_BACK);

    // Draw background
    if (m_cachedIsBackgroundEnabled && m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 depthMVPMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(0.0f, 1.0f - m_yAdjustment, zComp);
        modelMatrix.scale(QVector3D(m_rowWidth / m_scaleFactor,
                                    1.0f,
                                    m_columnDepth / m_scaleFactor));
        modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);
        itModelMatrix.scale(QVector3D(m_rowWidth / m_scaleFactor,
                                      1.0f,
                                      m_columnDepth / m_scaleFactor));

#ifdef SHOW_DEPTH_TEXTURE_SCENE
        MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;
#else
        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
#endif
        depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

        QVector3D backgroundColor = Utils::vectorFromColor(m_cachedTheme.m_backgroundColor);

        // Set shader bindings
        m_backgroundShader->setUniformValue(m_backgroundShader->lightP(), lightPos);
        m_backgroundShader->setUniformValue(m_backgroundShader->view(), viewMatrix);
        m_backgroundShader->setUniformValue(m_backgroundShader->model(), modelMatrix);
        m_backgroundShader->setUniformValue(m_backgroundShader->nModel(),
                                            itModelMatrix.inverted().transposed());
        m_backgroundShader->setUniformValue(m_backgroundShader->MVP(), MVPMatrix);
        m_backgroundShader->setUniformValue(m_backgroundShader->color(), backgroundColor);
        m_backgroundShader->setUniformValue(m_backgroundShader->ambientS(),
                                            m_cachedTheme.m_ambientStrength * 2.0f);

#if !defined(QT_OPENGL_ES_2)
        if (m_cachedShadowQuality > ShadowNone) {
            // Set shadow shader bindings
            m_backgroundShader->setUniformValue(m_backgroundShader->shadowQ(),
                                                m_shadowQualityToShader);
            m_backgroundShader->setUniformValue(m_backgroundShader->depth(), depthMVPMatrix);
            m_backgroundShader->setUniformValue(m_backgroundShader->lightS(),
                                                m_cachedTheme.m_lightStrength / 10.0f);

            // Draw the object
            m_drawer->drawObject(m_backgroundShader, m_backgroundObj, 0, m_depthTexture);
        } else
#endif
        {
            // Set shadowless shader bindings
            m_backgroundShader->setUniformValue(m_backgroundShader->lightS(),
                                                m_cachedTheme.m_lightStrength);

            // Draw the object
            m_drawer->drawObject(m_backgroundShader, m_backgroundObj);
        }
    }

    // Release background shader
    m_backgroundShader->release();

    // Disable textures
    glDisable(GL_TEXTURE_2D);

    // Reset culling
    if (m_hasNegativeValues) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    // Draw grid lines
    if (m_cachedIsGridEnabled && m_heightNormalizer) {
        // Bind bar shader
        m_barShader->bind();

        // Set unchanging shader bindings
        QVector3D barColor = Utils::vectorFromColor(m_cachedTheme.m_gridLine);
        m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
        m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
        m_barShader->setUniformValue(m_barShader->color(), barColor);
        m_barShader->setUniformValue(m_barShader->ambientS(), m_cachedTheme.m_ambientStrength);

        // Floor lines: rows
        for (GLfloat row = 0.0f; row <= m_cachedRowCount; row++) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;
            QMatrix4x4 itModelMatrix;

            rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());
            modelMatrix.translate(0.0f, -m_yAdjustment,
                                  (m_columnDepth - rowPos) / m_scaleFactor + zComp);
            modelMatrix.scale(QVector3D(m_rowWidth / m_scaleFactor, gridLineWidth,
                                        gridLineWidth));
            itModelMatrix.scale(QVector3D(m_rowWidth / m_scaleFactor, gridLineWidth,
                                          gridLineWidth));
            // If we're viewing from below, grid line object must be flipped
            if (m_yFlipped)
                modelMatrix.rotate(180.0f, 1.0, 0.0, 0.0);

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            m_barShader->setUniformValue(m_barShader->model(), modelMatrix);
            m_barShader->setUniformValue(m_barShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_barShader->setUniformValue(m_barShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > ShadowNone) {
                // Set shadow shader bindings
                m_barShader->setUniformValue(m_barShader->shadowQ(), m_shadowQualityToShader);
                m_barShader->setUniformValue(m_barShader->depth(), depthMVPMatrix);
                m_barShader->setUniformValue(m_barShader->lightS(),
                                             m_cachedTheme.m_lightStrength / 10.0f);

                // Draw the object
                m_drawer->drawObject(m_barShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                m_barShader->setUniformValue(m_barShader->lightS(), m_cachedTheme.m_lightStrength);

                // Draw the object
                m_drawer->drawObject(m_barShader, m_gridLineObj);
            }
        }

        // Floor lines: columns
        for (GLfloat bar = 0.0f; bar <= m_cachedColumnCount; bar++) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;
            QMatrix4x4 itModelMatrix;

            barPos = (bar + 0.5f) * (m_cachedBarSpacing.width());
            modelMatrix.translate((m_rowWidth - barPos) / m_scaleFactor,
                                  -m_yAdjustment, zComp);
            modelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                        m_columnDepth / m_scaleFactor));
            itModelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                          m_columnDepth / m_scaleFactor));

            // If we're viewing from below, grid line object must be flipped
            if (m_yFlipped)
                modelMatrix.rotate(180.0f, 1.0, 0.0, 0.0);

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            m_barShader->setUniformValue(m_barShader->model(), modelMatrix);
            m_barShader->setUniformValue(m_barShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_barShader->setUniformValue(m_barShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > ShadowNone) {
                // Set shadow shader bindings
                m_barShader->setUniformValue(m_barShader->shadowQ(), m_shadowQualityToShader);
                m_barShader->setUniformValue(m_barShader->depth(), depthMVPMatrix);
                m_barShader->setUniformValue(m_barShader->lightS(),
                                             m_cachedTheme.m_lightStrength / 10.0f);

                // Draw the object
                m_drawer->drawObject(m_barShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                m_barShader->setUniformValue(m_barShader->lightS(), m_cachedTheme.m_lightStrength);

                // Draw the object
                m_drawer->drawObject(m_barShader, m_gridLineObj);
            }
        }

        // Wall lines: back wall
        GLfloat heightStep = m_heightNormalizer / 5.0f; // default to 5 lines
        GLfloat startLine;

        if (m_tickCount > 0)
            heightStep = m_tickStep;

        if (m_hasNegativeValues)
            startLine = -m_heightNormalizer;
        else
            startLine = heightStep;

        for (GLfloat lineHeight = startLine; lineHeight <= m_heightNormalizer;
             lineHeight += heightStep) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;
            QMatrix4x4 itModelMatrix;

            if (m_zFlipped) {
                modelMatrix.translate(0.0f,
                                      2.0f * lineHeight / m_heightNormalizer - m_yAdjustment,
                                      m_columnDepth / m_scaleFactor + zComp);
            } else {
                modelMatrix.translate(0.0f,
                                      2.0f * lineHeight / m_heightNormalizer - m_yAdjustment,
                                      -m_columnDepth / m_scaleFactor + zComp);
            }
            modelMatrix.scale(QVector3D(m_rowWidth / m_scaleFactor, gridLineWidth,
                                        gridLineWidth));
            itModelMatrix.scale(QVector3D(m_rowWidth / m_scaleFactor, gridLineWidth,
                                          gridLineWidth));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            m_barShader->setUniformValue(m_barShader->model(), modelMatrix);
            m_barShader->setUniformValue(m_barShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_barShader->setUniformValue(m_barShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > ShadowNone) {
                // Set shadow shader bindings
                m_barShader->setUniformValue(m_barShader->shadowQ(), m_shadowQualityToShader);
                m_barShader->setUniformValue(m_barShader->depth(), depthMVPMatrix);
                m_barShader->setUniformValue(m_barShader->lightS(),
                                             m_cachedTheme.m_lightStrength / 10.0f);

                // Draw the object
                m_drawer->drawObject(m_barShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                m_barShader->setUniformValue(m_barShader->lightS(), m_cachedTheme.m_lightStrength);

                // Draw the object
                m_drawer->drawObject(m_barShader, m_gridLineObj);
            }
        }

        // Wall lines: side wall
        for (GLfloat lineHeight = startLine; lineHeight <= m_heightNormalizer;
             lineHeight += heightStep) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;
            QMatrix4x4 itModelMatrix;

            if (m_xFlipped) {
                modelMatrix.translate(m_rowWidth / m_scaleFactor,
                                      2.0f * lineHeight / m_heightNormalizer - m_yAdjustment,
                                      zComp);
            } else {
                modelMatrix.translate(-m_rowWidth / m_scaleFactor,
                                      2.0f * lineHeight / m_heightNormalizer - m_yAdjustment,
                                      zComp);
            }
            modelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                        m_columnDepth / m_scaleFactor));
            itModelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                          m_columnDepth / m_scaleFactor));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            m_barShader->setUniformValue(m_barShader->model(), modelMatrix);
            m_barShader->setUniformValue(m_barShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_barShader->setUniformValue(m_barShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > ShadowNone) {
                // Set shadow shader bindings
                m_barShader->setUniformValue(m_barShader->shadowQ(), m_shadowQualityToShader);
                m_barShader->setUniformValue(m_barShader->depth(), depthMVPMatrix);
                m_barShader->setUniformValue(m_barShader->lightS(),
                                             m_cachedTheme.m_lightStrength / 10.0f);

                // Draw the object
                m_drawer->drawObject(m_barShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                m_barShader->setUniformValue(m_barShader->lightS(), m_cachedTheme.m_lightStrength);

                // Draw the object
                m_drawer->drawObject(m_barShader, m_gridLineObj);
            }
        }

        // Release bar shader
        m_barShader->release();
    }

    // TODO: Draw y labels

    // Generate label textures for slice selection if m_updateLabels is set
    if (m_cachedIsSlicingActivated && m_updateLabels) {
        // Create label textures
        for (int col = 0; col < m_sliceSelection->size(); col++) {
            BarRenderItem *item = m_sliceSelection->at(col);
            m_drawer->generateLabelTexture(item);
        }
    }

    // Handle slice activation and label drawing
    if (!barSelectionFound) {
        // We have no ownership, don't delete. Just NULL the pointer.
        m_selectedBar = NULL;
        if (m_cachedIsSlicingActivated && Bars3dController::MouseOnOverview == m_controller->mouseState())
            m_controller->setSlicingActive(false);
    } else if (m_cachedSelectionMode >= ModeZoomRow
               && Bars3dController::MouseOnScene == m_controller->mouseState()) {
        // Activate slice mode
        m_controller->setSlicingActive(true);

        // Create label textures
        for (int col = 0; col < m_sliceSelection->size(); col++) {
            BarRenderItem *item = m_sliceSelection->at(col);
            m_drawer->generateLabelTexture(item);
        }
    } else {
        // Print value of selected bar
        m_labelShader->bind();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        if (m_cachedLabelTransparency > TransparencyNone) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
#ifndef DISPLAY_FULL_DATA_ON_SELECTION
        // Draw just the value string of the selected bar
        if (m_previouslySelectedBar != m_selectedBar || m_updateLabels) {
            m_drawer->generateLabelTexture(m_selectedBar);
            m_previouslySelectedBar = m_selectedBar;
        }

        m_drawer->drawLabel(*m_selectedBar, m_selectedBar->label(),
                            viewMatrix, projectionMatrix,
                            QVector3D(0.0f, m_yAdjustment, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), m_heightNormalizer,
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, true);
#else
        // Draw the value string followed by row label and column label
        LabelItem &labelItem = m_selectedBar->selectionLabel();
        if (m_previouslySelectedBar != m_selectedBar || m_updateLabels || !labelItem.textureId()) {
            QString labelText = m_selectedBar->label();
            if ((m_controller->axisZ()->labels().size() > m_selectedBar->position().y())
                    && (m_controller->axisX()->labels().size() > m_selectedBar->position().x())) {
                labelText.append(QStringLiteral(" ("));
                labelText.append(m_controller->axisX()->labels().at(m_selectedBar->position().x()));
                labelText.append(QStringLiteral(", "));
                labelText.append(m_controller->axisZ()->labels().at(m_selectedBar->position().y()));
                labelText.append(QStringLiteral(")"));
                //qDebug() << labelText;
            }
            m_drawer->generateLabelItem(labelItem, labelText);
            m_previouslySelectedBar = m_selectedBar;
        }

        m_drawer->drawLabel(*m_selectedBar, labelItem, viewMatrix, projectionMatrix,
                            QVector3D(0.0f, m_yAdjustment, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), (m_selectedBar->value() / m_heightNormalizer),
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, camera, true, false);
#endif
        glDisable(GL_TEXTURE_2D);
        if (m_cachedLabelTransparency > TransparencyNone)
            glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // Release label shader
        m_labelShader->release();

        // Reset label update flag; they should have been updated when we get here
        m_updateLabels = false;
    }

    // TODO: Calculations done temporarily here. When optimizing, move to after data set addition? Keep drawing of the labels here.
    // Bind label shader
    m_labelShader->bind();

    glEnable(GL_TEXTURE_2D);
    if (m_cachedLabelTransparency > TransparencyNone) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Calculate the positions for row and column labels and store them into QDataItems (and QDataRows?)
    for (int row = 0; row != m_cachedRowCount; row++) {
        if (m_controller->axisX()->d_ptr->labelItems().size() > row) {
            // Go through all rows and get position of max+1 or min-1 column, depending on x flip
            // We need only positions for them, labels have already been generated at QDataSetPrivate. Just add LabelItems
            rowPos = (row + 1) * (m_cachedBarSpacing.height());
            barPos = 0;
            GLfloat rotLabelX = -90.0f;
            GLfloat rotLabelY = 0.0f;
            GLfloat rotLabelZ = 0.0f;
            Qt::AlignmentFlag alignment = Qt::AlignRight;
            if (m_zFlipped)
                rotLabelY = 180.0f;
            if (m_xFlipped) {
                barPos = (m_cachedColumnCount + 1) * (m_cachedBarSpacing.width());
                alignment = Qt::AlignLeft;
            }
            if (m_yFlipped) {
                if (m_zFlipped)
                    rotLabelY = 0.0f;
                else
                    rotLabelY = 180.0f;
                rotLabelZ = 180.0f;
            }
            QVector3D labelPos = QVector3D((m_rowWidth - barPos) / m_scaleFactor,
                                           -m_yAdjustment + 0.005f, // raise a bit over background to avoid depth "glimmering"
                                           (m_columnDepth - rowPos) / m_scaleFactor + zComp);

            // TODO: Try it; draw the label here

            m_dummyBarRenderItem.setTranslation(labelPos);
            const LabelItem &axisLabelItem = *m_controller->axisX()->d_ptr->labelItems().at(row);
            //qDebug() << "labelPos, row" << row + 1 << ":" << labelPos << dataSet->rowLabels().at(row);

            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, camera, true, true, LabelMid,
                                alignment);
        }

    }
    for (int bar = 0; bar != m_cachedColumnCount; bar += 1) {
        if (m_controller->axisZ()->d_ptr->labelItems().size() > bar) {
            // Go through all columns and get position of max+1 or min-1 row, depending on z flip
            // We need only positions for them, labels have already been generated at QDataSetPrivate. Just add LabelItems
            barPos = (bar + 1) * (m_cachedBarSpacing.width());
            rowPos = 0;
            GLfloat rotLabelX = -90.0f;
            GLfloat rotLabelY = 90.0f;
            GLfloat rotLabelZ = 0.0f;
            Qt::AlignmentFlag alignment = Qt::AlignLeft;
            if (m_xFlipped)
                rotLabelY = -90.0f;
            if (m_zFlipped) {
                rowPos = (m_cachedRowCount + 1) * (m_cachedBarSpacing.height());
                alignment = Qt::AlignRight;
            }
            if (m_yFlipped) {
                if (m_xFlipped)
                    rotLabelY = -90.0f;
                else
                    rotLabelY = 90.0f;
                rotLabelZ = 180.0f;
            }
            QVector3D labelPos = QVector3D((m_rowWidth - barPos) / m_scaleFactor,
                                           -m_yAdjustment + 0.005f, // raise a bit over background to avoid depth "glimmering"
                                           (m_columnDepth - rowPos) / m_scaleFactor + zComp);

            // TODO: Try it; draw the label here

            m_dummyBarRenderItem.setTranslation(labelPos);
            const LabelItem &axisLabelItem = *m_controller->axisZ()->d_ptr->labelItems().at(bar);
            //qDebug() << "labelPos, col" << bar + 1 << ":" << labelPos << dataSet->columnLabels().at(bar);

            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, camera, true, true, LabelMid,
                                alignment);
        }
    }
    glDisable(GL_TEXTURE_2D);
    if (m_cachedLabelTransparency > TransparencyNone)
        glDisable(GL_BLEND);

    // Release label shader
    m_labelShader->release();
}

void Bars3dRenderer::requestSelectionAtPoint(const QPoint &point)
{
    QMutexLocker locker(&m_mutex);
    m_selectionPointRequest.setX(point.x());
    m_selectionPointRequest.setY(point.y());
    m_isSelectionPointRequestActive = true;
}

void Bars3dRenderer::handleResize()
{
    if (m_cachedBoundingRect.width() == 0 || m_cachedBoundingRect.height() == 0)
        return;
    qDebug() << "Bars3dRenderer::resizeEvent " << m_cachedBoundingRect.width() << "x" <<m_cachedBoundingRect.height();

    // Set view port
    if (m_cachedIsSlicingActivated) {
        m_mainViewPort = QRect(0,
                               m_cachedBoundingRect.height() - m_cachedBoundingRect.height() / 5,
                               m_cachedBoundingRect.width() / 5,
                               m_cachedBoundingRect.height() / 5);
    } else {
        m_mainViewPort = QRect(0, 0, m_cachedBoundingRect.width(), m_cachedBoundingRect.height());
    }
    m_sliceViewPort = QRect(0, 0, m_cachedBoundingRect.width(), m_cachedBoundingRect.height());

    // Calculate zoom level based on aspect ratio
    GLfloat div;
    GLfloat zoomAdjustment;
    div = qMin(m_cachedBoundingRect.width(), m_cachedBoundingRect.height());
    zoomAdjustment = defaultRatio * ((m_cachedBoundingRect.width() / div) / (m_cachedBoundingRect.height() / div));
    //qDebug() << "zoom adjustment" << zoomAdjustment;
    m_autoScaleAdjustment = qMin(zoomAdjustment, 1.0f); // clamp to 1.0f

    // Re-init selection buffer
    initSelectionBuffer();

#if !defined(QT_OPENGL_ES_2)
    // Re-init depth buffer
    updateDepthBuffer();
#endif
}

void Bars3dRenderer::updateBarSpecs(QSizeF thickness, QSizeF spacing, bool relative)
{
    m_cachedBarThickness = thickness;
    if (relative) {
        m_cachedBarSpacing.setWidth((thickness.width() * 2) * (spacing.width() + 1.0f));
        m_cachedBarSpacing.setHeight((thickness.height() * 2) * (spacing.height() + 1.0f));
    } else {
        m_cachedBarSpacing = thickness * 2 + spacing * 2;
    }

    // Calculate here and at setting sample space
    calculateSceneScalingFactors();
}

void Bars3dRenderer::updateMeshFileName(const QString &objFileName)
{
    m_cachedObjFile = objFileName;
    loadBarMesh();
}

void Bars3dRenderer::updateSampleSpace(int rowCount, int columnCount)
{
    // Destroy old render items and reallocate new array
    // TODO is there a way to allocate the whole array with one allocation?
    m_renderItemArray.clear();
    m_renderItemArray.resize(rowCount);
    for (int i = 0; i < rowCount; i++) {
        m_renderItemArray[i].resize(columnCount);
        for (int j = 0; j < columnCount; j++)
            m_renderItemArray[i][j].setRenderer(this);
    }

    m_dataWindowChanged = true;

    // Force update for selection related items
    m_sliceAxisP = 0;
    m_sliceTitleItem = 0;
    if (m_sliceSelection)
        m_sliceSelection->clear();

    m_cachedColumnCount = columnCount;
    m_cachedRowCount    = rowCount;
    // TODO: Invent "idiotproof" max scene size formula..
    // This seems to work ok if spacing is not negative (and row/column or column/row ratio is not too high)
    m_maxSceneSize = 2 * qSqrt(columnCount * rowCount);
    //qDebug() << "maxSceneSize" << m_maxSceneSize;
    // Calculate here and at setting bar specs
    calculateSceneScalingFactors();
}

void Bars3dRenderer::updateTheme(Theme theme)
{
    m_cachedTheme.setFromTheme(theme);

    m_drawer->setTheme(m_cachedTheme);
    // Re-initialize shaders
#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > ShadowNone) {
        if (!m_cachedTheme.m_uniformColor) {
            initShaders(QStringLiteral(":/shaders/vertexShadow"),
                        QStringLiteral(":/shaders/fragmentShadowNoTexColorOnY"));
        } else {
            initShaders(QStringLiteral(":/shaders/vertexShadow"),
                        QStringLiteral(":/shaders/fragmentShadowNoTex"));
        }
    } else {
        if (!m_cachedTheme.m_uniformColor) {
            initShaders(QStringLiteral(":/shaders/vertex"),
                        QStringLiteral(":/shaders/fragmentColorOnY"));
        } else {
            initShaders(QStringLiteral(":/shaders/vertex"),
                        QStringLiteral(":/shaders/fragment"));
        }
    }
#else
    if (!m_cachedTheme.m_uniformColor) {
        initShaders(QStringLiteral(":/shaders/vertexES2"),
                    QStringLiteral(":/shaders/fragmentColorOnYES2"));
    } else {
        initShaders(QStringLiteral(":/shaders/vertexES2"),
                    QStringLiteral(":/shaders/fragmentES2"));
    }
#endif
}

void Bars3dRenderer::updateSelectionMode(SelectionMode mode)
{
    m_cachedSelectionMode      = mode;

    // Create zoom selection if there isn't one
    if (mode >= ModeZoomRow && !m_sliceSelection) {
        m_sliceSelection = new QList<BarRenderItem *>;
        if (mode == ModeZoomRow)
            m_sliceSelection->reserve(m_cachedRowCount);
        else
            m_sliceSelection->reserve(m_cachedColumnCount);
    }
}

void Bars3dRenderer::updateFont(const QFont &font)
{
    m_cachedFont = font;
    m_drawer->setFont(font);
}

void Bars3dRenderer::updateLabelTransparency(LabelTransparency transparency)
{
    m_cachedLabelTransparency = transparency;
    m_drawer->setTransparency(transparency);
}

void Bars3dRenderer::updateGridEnabled(bool enable)
{
    m_cachedIsGridEnabled = enable;
}

void Bars3dRenderer::updateBackgroundEnabled(bool enable)
{
    if (m_cachedIsBackgroundEnabled != enable) {
        m_cachedIsBackgroundEnabled = enable;
        // Load changed bar type
        loadBarMesh();
    }
}

void Bars3dRenderer::updateShadowQuality(ShadowQuality quality)
{
    qDebug() << "Bars3dRenderer::setShadowQuality";
    m_cachedShadowQuality = quality;
    switch (quality) {
    case ShadowLow:
        m_shadowQualityToShader = 33.3f;
        break;
    case ShadowMedium:
        m_shadowQualityToShader = 100.0f;
        break;
    case ShadowHigh:
        m_shadowQualityToShader = 200.0f;
        break;
    default:
        m_shadowQualityToShader = 0.0f;
        break;
    }
#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > ShadowNone) {
        // Re-init shaders
        if (!m_cachedTheme.m_uniformColor) {
            initShaders(QStringLiteral(":/shaders/vertexShadow"),
                        QStringLiteral(":/shaders/fragmentShadowNoTexColorOnY"));
        } else {
            initShaders(QStringLiteral(":/shaders/vertexShadow"),
                        QStringLiteral(":/shaders/fragmentShadowNoTex"));
        }
        initBackgroundShaders(QStringLiteral(":/shaders/vertexShadow"),
                              QStringLiteral(":/shaders/fragmentShadowNoTex"));
    } else {
        // Re-init shaders
        if (!m_cachedTheme.m_uniformColor) {
            initShaders(QStringLiteral(":/shaders/vertex"),
                        QStringLiteral(":/shaders/fragmentColorOnY"));
        } else {
            initShaders(QStringLiteral(":/shaders/vertex"),
                        QStringLiteral(":/shaders/fragment"));
        }
        initBackgroundShaders(QStringLiteral(":/shaders/vertex"),
                              QStringLiteral(":/shaders/fragment"));
    }
    // Re-init depth buffer
    updateDepthBuffer();
#else
    if (!m_cachedTheme.m_uniformColor) {
        initShaders(QStringLiteral(":/shaders/vertexES2"),
                    QStringLiteral(":/shaders/fragmentColorOnYES2"));
    } else {
        initShaders(QStringLiteral(":/shaders/vertexES2"),
                    QStringLiteral(":/shaders/fragmentES2"));
    }
    initBackgroundShaders(QStringLiteral(":/shaders/vertexES2"),
                          QStringLiteral(":/shaders/fragmentES2"));
#endif
}

void Bars3dRenderer::updateTickCount(GLint tickCount, GLfloat step, GLfloat minimum)
{
    m_tickCount = tickCount;
    m_tickStep = step;
    if (tickCount > 0 && step > 0) {
        m_heightNormalizer = tickCount * step;
        calculateHeightAdjustment(QPair<float, float>(minimum, m_heightNormalizer));
    }
}

void Bars3dRenderer::updateBoundingRect(const QRect boundingRect)
{
    m_cachedBoundingRect = boundingRect;
    handleResize();
}

void Bars3dRenderer::updatePosition(const QRect boundingRect)
{
    m_cachedBoundingRect = boundingRect;
}

void Bars3dRenderer::loadBarMesh()
{
    QString objectFileName = m_cachedObjFile;
    if (m_barObj)
        delete m_barObj;
    // If background is disabled, load full version of bar mesh
    if (!m_cachedIsBackgroundEnabled)
        objectFileName.append(QStringLiteral("Full"));
    m_barObj = new ObjectHelper(objectFileName);
    m_barObj->load();
}

void Bars3dRenderer::loadBackgroundMesh()
{
    if (m_backgroundObj)
        delete m_backgroundObj;
    if (m_hasNegativeValues)
        m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/negativeBackground"));
    else
        m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/background"));
    m_backgroundObj->load();
}

void Bars3dRenderer::loadGridLineMesh()
{
    if (m_gridLineObj)
        delete m_gridLineObj;
    m_gridLineObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/bar"));
    m_gridLineObj->load();
}

void Bars3dRenderer::loadLabelMesh()
{
    if (m_labelObj)
        delete m_labelObj;
    m_labelObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/label"));
    m_labelObj->load();
}

void Bars3dRenderer::updateTextures()
{
    // Drawer has changed; this flag needs to be checked when checking if we need to update labels
    m_updateLabels = true;
}

void Bars3dRenderer::calculateSceneScalingFactors()
{
    // Calculate scene scaling and translation factors
    m_rowWidth = ((m_cachedColumnCount + 1) * m_cachedBarSpacing.width()) / 2.0f;
    m_columnDepth = ((m_cachedRowCount + 1) * m_cachedBarSpacing.height()) / 2.0f;
    m_maxDimension = qMax(m_rowWidth, m_columnDepth);
    m_scaleFactor = qMin((m_cachedColumnCount * (m_maxDimension / m_maxSceneSize)),
                         (m_cachedRowCount * (m_maxDimension / m_maxSceneSize)));
    m_scaleX = m_cachedBarThickness.width() / m_scaleFactor;
    m_scaleZ = m_cachedBarThickness.height() / m_scaleFactor;
    //qDebug() << "m_scaleX" << m_scaleX << "m_scaleFactor" << m_scaleFactor;
    //qDebug() << "m_scaleZ" << m_scaleZ << "m_scaleFactor" << m_scaleFactor;
    //qDebug() << "m_rowWidth:" << m_rowWidth << "m_columnDepth:" << m_columnDepth << "m_maxDimension:" << m_maxDimension;
}

void Bars3dRenderer::calculateHeightAdjustment(const QPair<GLfloat, GLfloat> &limits)
{
    // 2.0f = max difference between minimum and maximum value after scaling with m_heightNormalizer
    GLfloat newAdjustment = 2.0f - ((limits.second - limits.first) / m_heightNormalizer);
    if (newAdjustment != m_yAdjustment) {
        m_hasHeightAdjustmentChanged = true;
        m_yAdjustment = newAdjustment;
    }
    //qDebug() << m_yAdjustment;
}

Bars3dController::SelectionType Bars3dRenderer::isSelected(GLint row, GLint bar)
{
    //static QVector3D prevSel = m_selection; // TODO: For debugging
    Bars3dController::SelectionType isSelectedType = Bars3dController::SelectionNone;
    if (m_selection == selectionSkipColor)
        return isSelectedType; // skip window

    //#if !defined(QT_OPENGL_ES_2)
    //    QVector3D current = QVector3D((GLuint)row, (GLuint)bar, 0);
    //#else
    QVector3D current = QVector3D((GLubyte)row, (GLubyte)bar, 0);
    //#endif

    // TODO: For debugging
    //if (selection != prevSel) {
    //    qDebug() << "current" << current.x() << current .y() << current.z();
    //    qDebug() << "selection" << selection.x() << selection .y() << selection.z();
    //    prevSel = selection;
    //}
    if (current == m_selection) {
        isSelectedType = Bars3dController::SelectionBar;
    }
    else if (current.y() == m_selection.y() && (m_cachedSelectionMode == ModeBarAndColumn
                                                || m_cachedSelectionMode == ModeBarRowAndColumn
                                                || m_cachedSelectionMode == ModeZoomColumn)) {
        isSelectedType = Bars3dController::SelectionColumn;
    }
    else if (current.x() == m_selection.x() && (m_cachedSelectionMode == ModeBarAndRow
                                                || m_cachedSelectionMode == ModeBarRowAndColumn
                                                || m_cachedSelectionMode == ModeZoomRow)) {
        isSelectedType = Bars3dController::SelectionRow;
    }
    return isSelectedType;
}

void Bars3dRenderer::updateLimits(QPair<GLfloat, GLfloat> limits)
{
    m_limits.first  = limits.first;
    m_limits.second = limits.second;

    // TODO: What if we have only negative values?

    // Check if we have negative values
    if (limits.first < 0 && !m_hasNegativeValues) {
        m_hasNegativeValues = true;
        // Reload background
        loadBackgroundMesh();
    } else if (limits.first >= 0 && m_hasNegativeValues) {
        m_hasNegativeValues = false;
        // Reload background
        loadBackgroundMesh();
    }

    // Don't auto-adjust height if tick count is set
    if (m_tickCount == 0) {
        m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
        calculateHeightAdjustment(limits);
    }
}

void Bars3dRenderer::updateSlicingActive(bool isSlicing)
{
    m_cachedIsSlicingActivated = isSlicing;
    if (isSlicing)
        m_mainViewPort = QRect(0, m_cachedBoundingRect.height() - m_cachedBoundingRect.height() / 5, m_cachedBoundingRect.width() / 5, m_cachedBoundingRect.height() / 5);
    else
        m_mainViewPort = QRect(0, 0, this->m_cachedBoundingRect.width(), this->m_cachedBoundingRect.height());
}

void Bars3dRenderer::updateZoomLevel(int newZoomLevel)
{
    m_cachedZoomLevel = newZoomLevel;
}


QRect Bars3dRenderer::mainViewPort()
{
    return m_mainViewPort;
}

void Bars3dRenderer::initShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_barShader)
        delete m_barShader;
    m_barShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_barShader->initialize();
}

void Bars3dRenderer::initSelectionShader()
{
    if (m_selectionShader)
        delete m_selectionShader;
    m_selectionShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexSelection"),
                                         QStringLiteral(":/shaders/fragmentSelection"));
    m_selectionShader->initialize();
}

void Bars3dRenderer::initSelectionBuffer()
{
    if (m_selectionTexture)
        m_textureHelper->deleteTexture(&m_selectionTexture);

    m_selectionTexture = m_textureHelper->createSelectionTexture(m_mainViewPort.size(),
                                                                 m_selectionFrameBuffer,
                                                                 m_selectionDepthBuffer);
}

#if !defined(QT_OPENGL_ES_2)
void Bars3dRenderer::initDepthShader()
{
    if (m_depthShader)
        delete m_depthShader;
    m_depthShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexDepth"),
                                     QStringLiteral(":/shaders/fragmentDepth"));
    m_depthShader->initialize();
}

void Bars3dRenderer::updateDepthBuffer()
{
    if (m_depthTexture) {
        m_textureHelper->deleteTexture(&m_depthTexture);
        m_depthTexture = 0;
    }

    if (m_cachedShadowQuality > ShadowNone) {
        m_depthTexture = m_textureHelper->createDepthTexture(m_mainViewPort.size(),
                                                             m_depthFrameBuffer,
                                                             m_cachedShadowQuality);
        if (!m_depthTexture) {
            switch (m_cachedShadowQuality) {
            case ShadowHigh:
                qWarning("Creating high quality shadows failed. Changing to medium quality.");
                (void)m_controller->setShadowQuality(ShadowMedium);
                break;
            case ShadowMedium:
                qWarning("Creating medium quality shadows failed. Changing to low quality.");
                (void)m_controller->setShadowQuality(ShadowLow);
                break;
            case ShadowLow:
                qWarning("Creating low quality shadows failed. Switching shadows off.");
                (void)m_controller->setShadowQuality(ShadowNone);
                break;
            default:
                // You'll never get here
                break;
            }
        }
    }
}
#endif

void Bars3dRenderer::initBackgroundShaders(const QString &vertexShader,
                                           const QString &fragmentShader)
{
    if (m_backgroundShader)
        delete m_backgroundShader;
    m_backgroundShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_backgroundShader->initialize();
}

void Bars3dRenderer::initLabelShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_labelShader)
        delete m_labelShader;
    m_labelShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_labelShader->initialize();
}

QT_DATAVIS3D_END_NAMESPACE
