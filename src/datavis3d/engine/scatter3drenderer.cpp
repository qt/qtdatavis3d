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

#include "scatter3drenderer_p.h"
#include "scatter3dcontroller_p.h"
#include "camerahelper_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"
#include "utils_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QThread>
#include <qmath.h>
#include <QDebug>
#include <QMutexLocker>

// Commenting this draws the shadow map with perspective projection. Otherwise it's drawn in
// orthographic projection.
//#define USE_WIDER_SHADOWS

// You can verify that depth buffer drawing works correctly by uncommenting this.
// You should see the scene from  where the light is
//#define SHOW_DEPTH_TEXTURE_SCENE

#ifdef DISPLAY_RENDER_SPEED
#include <QTime>
#endif

QT_DATAVIS3D_BEGIN_NAMESPACE

#define DISPLAY_FULL_DATA_ON_SELECTION // Append selection value text with row and column labels

const GLfloat aspectRatio = 2.0f; // Forced ratio of x and z to y. Dynamic will make it look odd.
// TODO: Make margin modifiable?
const GLfloat backgroundMargin = 1.1f; // Margin for background (1.1f = make it 10% larger to avoid items being drawn inside background)
const GLfloat gridLineWidth = 0.005f;
static QVector3D selectionSkipColor = QVector3D(255, 255, 255); // Selection texture's background color

Scatter3DRenderer::Scatter3DRenderer(Scatter3DController *controller)
    : Abstract3DRenderer(controller),
      m_controller(controller),
      m_selectedItem(0),
      m_previouslySelectedItem(0),
      m_tickCount(5),
      m_tickStep(0.2f),
      m_xFlipped(false),
      m_zFlipped(false),
      m_updateLabels(false),
      m_dotShader(0),
      m_depthShader(0),
      m_selectionShader(0),
      m_backgroundShader(0),
      m_labelShader(0),
      m_dotObj(0),
      m_backgroundObj(0),
      m_gridLineObj(0),
      m_labelObj(0),
      m_bgrTexture(0),
      m_depthTexture(0),
      m_selectionTexture(0),
      m_depthFrameBuffer(0),
      m_selectionFrameBuffer(0),
      m_selectionDepthBuffer(0),
      m_shadowQualityToShader(33.3f),
      m_heightNormalizer(1.0f),
      m_yAdjustment(0.0f),
      m_scaleFactor(0),
      m_selection(selectionSkipColor),
      m_areaSize(QSizeF(2.0f, 2.0f)),
      m_autoAdjust(true),
      m_hasHeightAdjustmentChanged(true),
      m_dataProxy(0),
      m_valueUpdateNeeded(false)
    #ifdef DISPLAY_RENDER_SPEED
    , m_isFirstFrame(true),
      m_numFrames(0)
    #endif
{
    //qDebug() << __FUNCTION__;
    m_dummyRenderItem.setRenderer(this);
    initializePreOpenGL();
    initializeOpenGLFunctions();
    initializeOpenGL();
}

Scatter3DRenderer::~Scatter3DRenderer()
{
    //qDebug() << __FUNCTION__;
    m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);
    m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
    m_textureHelper->deleteTexture(&m_selectionTexture);
    m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
    m_textureHelper->deleteTexture(&m_bgrTexture);
    delete m_dotShader;
    delete m_depthShader;
    delete m_selectionShader;
    delete m_backgroundShader;
    delete m_dotObj;
    delete m_backgroundObj;
    delete m_gridLineObj;
    delete m_textureHelper;
}

void Scatter3DRenderer::initializePreOpenGL()
{
    Abstract3DRenderer::initializePreOpenGL();

    QObject::connect(m_controller, &Scatter3DController::selectionModeChanged, this,
                     &Scatter3DRenderer::updateSelectionMode);
    QObject::connect(m_controller, &Scatter3DController::objFileChanged, this,
                     &Scatter3DRenderer::updateMeshFileName);
    QObject::connect(m_controller, &Scatter3DController::positionChanged, this,
                     &Scatter3DRenderer::updatePosition);
    QObject::connect(m_controller, &Scatter3DController::gridEnabledChanged, this,
                     &Scatter3DRenderer::updateGridEnabled);
    QObject::connect(m_controller, &Scatter3DController::backgroundEnabledChanged, this,
                     &Scatter3DRenderer::updateBackgroundEnabled);
    QObject::connect(m_controller, &Scatter3DController::tickCountChanged, this,
                     &Scatter3DRenderer::updateTickCount);
    QObject::connect(m_controller, &Scatter3DController::zoomLevelChanged, this,
                     &Scatter3DRenderer::updateZoomLevel);

    // TODO Should all this initial setup be mutexed?
    updateSelectionMode(m_controller->selectionMode());
    updateZoomLevel(m_controller->zoomLevel());
    updateMeshFileName(m_controller->objFile());
    updateGridEnabled(m_controller->gridEnabled());
    updateBackgroundEnabled(m_controller->backgroundEnabled());
}


void Scatter3DRenderer::initializeOpenGL()
{
    //qDebug() << __FUNCTION__;
    m_textureHelper = new TextureHelper();
    m_drawer->initializeOpenGL();

    // Initialize shaders
    handleShadowQualityChange();

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
    glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
               m_mainViewPort.width(), m_mainViewPort.height());

    // Resize in case we've missed resize events
    // Resize calls initSelectionBuffer and initDepthBuffer, so they don't need to be called here
    handleResize();

    // Load background mesh (we need to be initialized first)
    loadBackgroundMesh();

    Abstract3DRenderer::initializeOpenGL();
}

void Scatter3DRenderer::render(QScatterDataProxy *dataProxy,
                               bool valuesDirty,
                               CameraHelper *camera,
                               const GLuint defaultFboHandle)
{
    //qDebug() << __FUNCTION__;

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

    m_dataProxy = dataProxy;

    // Update cached data window
    // TODO Should data changes be notified via signal instead of reading data in render?
    // TODO this cache initialization assumes data window starts at 0,0 offset from array
    // Update cached values
    if (valuesDirty) {
        const QScatterDataArray &dataArray = m_dataProxy->array();
        int dataSize = dataArray.size();
        m_renderItemArray.resize(dataSize);
        for (int i = 0; i < dataSize ; i++) {
            qreal value = dataArray.at(i).position().y();
            m_renderItemArray[i].setValue(value);
            m_renderItemArray[i].setPosition(dataArray.at(i).position());
            //m_renderItemArray[i].setHeight(value / m_heightNormalizer);
            //m_renderItemArray[i].setItemLabel(dataArray.at(i).label());
            calculateSceneScalingFactors(dataArray.at(i).position());
        }
        for (int i = 0; i < dataSize ; i++) {
            calculateTranslation(m_renderItemArray[i]);
            m_renderItemArray[i].setRenderer(this);
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
        m_hasHeightAdjustmentChanged = false;
    }

    // Draw bars scene
    drawScene(camera, defaultFboHandle);
}

void Scatter3DRenderer::drawScene(CameraHelper *camera,
                                  const GLuint defaultFboHandle)
{
    //qDebug() << __FUNCTION__;
    GLfloat backgroundRotation = 0;

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
                m_mainViewPort.height());

    // Calculate label flipping
    if (viewMatrix.row(0).x() > 0)
        m_zFlipped = false;
    else
        m_zFlipped = true;
    if (viewMatrix.row(0).z() <= 0)
        m_xFlipped = false;
    else
        m_xFlipped = true;

    // Calculate background rotation
    if (!m_zFlipped && !m_xFlipped)
        backgroundRotation = 270.0f;
    else if (!m_zFlipped && m_xFlipped)
        backgroundRotation = 180.0f;
    else if (m_zFlipped && m_xFlipped)
        backgroundRotation = 90.0f;
    else if (m_zFlipped && !m_xFlipped)
        backgroundRotation = 0.0f;

    // Get light position (rotate light with camera, a bit above it (as set in defaultLightPos))
    QVector3D lightPos = camera->calculateLightPosition(defaultLightPos);

    //    // Map adjustment direction to model matrix scaling
    GLfloat heightMultiplier = 1.0f;
    GLfloat widthMultiplier = 1.0f;
    GLfloat depthMultiplier = 1.0f;
    GLfloat heightScaler = 0.0f;
    GLfloat widthScaler = 0.0f;
    GLfloat depthScaler = 0.0f;
    //    switch (m_adjustDirection) {
    //    case Q3DMaps::AdjustHeight:
    //        widthMultiplier = 0.0f;
    //        heightMultiplier = 1.0f;
    //        depthMultiplier = 0.0f;
    //        widthScaler = m_barThickness.x() / m_scaleFactor;
    //        heightScaler = 0.0f;
    //        depthScaler = m_barThickness.z() / m_scaleFactor;
    //        break;
    //    case Q3DMaps::AdjustWidth:
    //        widthMultiplier = 1.0f;
    //        heightMultiplier = 0.0f;
    //        depthMultiplier = 0.0f;
    //        widthScaler = 0.0f;
    //        heightScaler = m_barThickness.y() / m_scaleFactor;
    //        depthScaler = m_barThickness.z() / m_scaleFactor;
    //        break;
    //    case Q3DMaps::AdjustDepth:
    //        widthMultiplier = 0.0f;
    //        heightMultiplier = 0.0f;
    //        depthMultiplier = 1.0f;
    //        widthScaler = m_barThickness.x() / m_scaleFactor;
    //        heightScaler = m_barThickness.y() / m_scaleFactor;
    //        depthScaler = 0.0f;
    //        break;
    //    case Q3DMaps::AdjustRadius:
    //        widthMultiplier = 1.0f;
    //        heightMultiplier = 0.0f;
    //        depthMultiplier = 1.0f;
    //        widthScaler = 0.0f;
    //        heightScaler = m_barThickness.y() / m_scaleFactor;
    //        depthScaler = 0.0f;
    //        break;
    //    case Q3DMaps::AdjustAll:
    //        widthMultiplier = 1.0f;
    //        heightMultiplier = 1.0f;
    //        depthMultiplier = 1.0f;
    //        widthScaler = 0.0f;
    //        heightScaler = 0.0f;
    //        depthScaler = 0.0f;
    //        break;
    //    }

    // Introduce regardless of shadow quality to simplify logic
    QMatrix4x4 depthViewMatrix;
    QMatrix4x4 depthProjectionMatrix;

#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > ShadowNone) {
        // Render scene into a depth texture for using with shadow mapping
        // Bind depth shader
        m_depthShader->bind();

        // Set viewport for depth map rendering. Must match texture size. Larger values give smoother shadows.
        glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
                   m_mainViewPort.width() * m_cachedShadowQuality,
                   m_mainViewPort.height() * m_cachedShadowQuality);

        // Enable drawing to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthFrameBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Set front face culling to reduce self-shadowing issues
        glCullFace(GL_FRONT);

        // Get the depth view matrix
        // It may be possible to hack lightPos here if we want to make some tweaks to shadow
        depthViewMatrix.lookAt(lightPos, QVector3D(0.0f, -m_yAdjustment, zComp),
                               QVector3D(0.0f, 1.0f, 0.0f));
        // TODO: Why does depthViewMatrix.column(3).y() goes to zero when we're directly above? That causes the scene to be not drawn from above -> must be fixed
        //qDebug() << lightPos << depthViewMatrix << depthViewMatrix.column(3);
        // Set the depth projection matrix
#ifndef USE_WIDER_SHADOWS
        // Use this for perspective shadows
        depthProjectionMatrix.perspective(15.0f, (GLfloat)m_mainViewPort.width()
                                          / (GLfloat)m_mainViewPort.height(), 3.0f, 100.0f);
#else
        // Use these for orthographic shadows
        //depthProjectionMatrix.ortho(-aspectRatio * 2.0f, aspectRatio * 2.0f,
        //                            -m_heightNormalizer * 2.0f, m_heightNormalizer * 2.0f,
        //                            0.0f, 100.0f);
        GLfloat testAspectRatio = (GLfloat)m_mainViewPort.width() / (GLfloat)m_mainViewPort.height();
        depthProjectionMatrix.ortho(-testAspectRatio * 2.0f, testAspectRatio * 2.0f,
                                    -m_heightNormalizer * 2.0f, m_heightNormalizer * 2.0f,
                                    0.0f, 100.0f);
#endif
        // Draw bars to depth buffer
        for (int bar = 0; bar < m_renderItemArray.size(); bar++) {
            const ScatterRenderItem &item = m_renderItemArray.at(bar);
            if (!item.value())
                continue;

            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;

            modelMatrix.translate(item.translation().x(),
                                  item.translation().y(),
                                  item.translation().z());
            // TODO: We should adjust scaling of items based on item count?
            modelMatrix.scale(QVector3D(widthMultiplier * 0.1f + widthScaler,
                                        heightMultiplier * 0.1f + heightScaler,
                                        depthMultiplier * 0.1f + depthScaler));
            //modelMatrix.scale(QVector3D(widthMultiplier * item.size() + widthScaler,
            //                            heightMultiplier * item.size() + heightScaler,
            //                            depthMultiplier * item.size() + depthScaler));

            MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            m_depthShader->setUniformValue(m_depthShader->MVP(), MVPMatrix);

            // 1st attribute buffer : vertices
            glEnableVertexAttribArray(m_depthShader->posAtt());
            glBindBuffer(GL_ARRAY_BUFFER, m_dotObj->vertexBuf());
            glVertexAttribPointer(m_depthShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dotObj->elementBuf());

            // Draw the triangles
            glDrawElements(GL_TRIANGLES, m_dotObj->indexCount(), GL_UNSIGNED_SHORT, (void *)0);

            // Free buffers
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDisableVertexAttribArray(m_depthShader->posAtt());
        }

        // Disable drawing to framebuffer (= enable drawing to screen)
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);

        // Reset culling to normal
        glCullFace(GL_BACK);

        // Release depth shader
        m_depthShader->release();

        // Revert to original viewport
        glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
                   m_mainViewPort.width(), m_mainViewPort.height());

#if 0 // Use this if you want to see what is being drawn to the framebuffer
        // You'll also have to comment out GL_COMPARE_R_TO_TEXTURE -line in texturehelper (if using it)
        m_labelShader->bind();
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
    }
#endif

    // Skip selection mode drawing if we have no selection mode
    if (m_cachedSelectionMode > ModeNone) {
        // Bind selection shader
        m_selectionShader->bind();

        // Draw bars to selection buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(selectionSkipColor.x() / 255, selectionSkipColor.y() / 255,
                     selectionSkipColor.z() / 255, 1.0f); // Set clear color to white (= skipColor)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled
        GLint barIdxRed = 0;
        GLint barIdxGreen = 0;
        GLint barIdxBlue = 0;
        for (int bar = 0; bar < m_renderItemArray.size(); bar++, barIdxRed++) {
            const ScatterRenderItem &item = m_renderItemArray.at(bar);
            if (!item.value())
                continue;

            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;

            modelMatrix.translate(item.translation().x(),
                                  item.translation().y(),
                                  item.translation().z());
            // TODO: We should adjust scaling of items based on item count?
            modelMatrix.scale(QVector3D(widthMultiplier * 0.1f + widthScaler,
                                        heightMultiplier * 0.1f + heightScaler,
                                        depthMultiplier * 0.1f + depthScaler));
            //modelMatrix.scale(QVector3D(widthMultiplier * item.size() + widthScaler,
            //                            heightMultiplier * item.size() + heightScaler,
            //                            depthMultiplier * item.size() + depthScaler));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

            if (barIdxRed > 0 && barIdxRed % 256 == 0) {
                barIdxRed = 0;
                barIdxGreen++;
            }
            if (barIdxGreen > 0 && barIdxGreen % 256 == 0) {
                barIdxGreen = 0;
                barIdxBlue++;
            }
            if (barIdxBlue > 255)
                qFatal("Too many objects");

            QVector3D barColor = QVector3D((GLfloat)barIdxRed / 255.0f,
                                           (GLfloat)barIdxGreen / 255.0f,
                                           (GLfloat)barIdxBlue / 255.0f);

            m_selectionShader->setUniformValue(m_selectionShader->MVP(), MVPMatrix);
            m_selectionShader->setUniformValue(m_selectionShader->color(), barColor);

            // 1st attribute buffer : vertices
            glEnableVertexAttribArray(m_selectionShader->posAtt());
            glBindBuffer(GL_ARRAY_BUFFER, m_dotObj->vertexBuf());
            glVertexAttribPointer(m_selectionShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dotObj->elementBuf());

            // Draw the triangles
            glDrawElements(GL_TRIANGLES, m_dotObj->indexCount(), GL_UNSIGNED_SHORT, (void *)0);

            // Free buffers
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDisableVertexAttribArray(m_selectionShader->posAtt());
        }
        glEnable(GL_DITHER);

        // Read color under cursor
        if (Scatter3DController::MouseOnScene == m_controller->mouseState())
            m_selection = Utils::getSelection(m_controller->mousePosition(),
                                              m_cachedBoundingRect.height());

        QMutexLocker locker(&m_mutex);
        if (m_isSelectionPointRequestActive) {
            m_isSelectionPointRequestActive = false;
            m_selection = Utils::getSelection(m_selectionPointRequest,
                                              m_cachedBoundingRect.height());
            emit selectionUpdated(m_selection);
        }
        locker.unlock();

        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);

        // Release selection shader
        m_selectionShader->release();

#if 0 // Use this if you want to see what is being drawn to the framebuffer
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
        m_drawer->drawObject(m_labelShader, m_labelObj,
                             m_selectionTexture);
        glDisable(GL_TEXTURE_2D);
        m_labelShader->release();
#endif
    }

    // Bind bar shader
    m_dotShader->bind();

    // Enable texture
    glEnable(GL_TEXTURE_2D);

    // Draw bars
    bool barSelectionFound = false;
    for (int bar = 0; bar < m_renderItemArray.size(); bar++) {
        ScatterRenderItem &item = m_renderItemArray[bar];
        if (!item.value())
            continue;

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 depthMVPMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(item.translation().x(),
                              item.translation().y(),
                              item.translation().z());
        // TODO: We should adjust scaling of items based on item count?
        modelMatrix.scale(QVector3D(widthMultiplier * 0.1f + widthScaler,
                                    heightMultiplier * 0.1f + heightScaler,
                                    depthMultiplier * 0.1f + depthScaler));
        //modelMatrix.scale(QVector3D(widthMultiplier * item.size() + widthScaler,
        //                            heightMultiplier * item.size() + heightScaler,
        //                            depthMultiplier * item.size() + depthScaler));
        itModelMatrix.scale(QVector3D(widthMultiplier * 0.1f + widthScaler,
                                      heightMultiplier * 0.1f + heightScaler,
                                      depthMultiplier * 0.1f + depthScaler));
        //itModelMatrix.scale(QVector3D(widthMultiplier * item.size() + widthScaler,
        //                              heightMultiplier * item.size() + heightScaler,
        //                              depthMultiplier * item.size() + depthScaler));

#ifdef SHOW_DEPTH_TEXTURE_SCENE
        MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;
#else
        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
#endif
        depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

        QVector3D baseColor = Utils::vectorFromColor(m_cachedTheme.m_baseColor);
        QVector3D heightColor =
                Utils::vectorFromColor(m_cachedTheme.m_heightColor) * item.translation().y();

        QVector3D barColor = baseColor + heightColor;

        GLfloat lightStrength = m_cachedTheme.m_lightStrength;
        if (m_cachedSelectionMode > ModeNone) {
            Scatter3DController::SelectionType selectionType = isSelected(bar, m_selection);
            switch (selectionType) {
            case Scatter3DController::SelectionBar: {
                barColor = Utils::vectorFromColor(m_cachedTheme.m_highlightBarColor);
                lightStrength = m_cachedTheme.m_highlightLightStrength;
                // Insert data to QDataItem. We have no ownership, don't delete the previous one
                m_selectedItem = &item;
                barSelectionFound = true;
                break;
            }
            case Scatter3DController::SelectionNone: {
                // Current bar is not selected, nor on a row or column
                // do nothing
                break;
            }
            default: {
                // Unsupported selection mode
                // do nothing
                break;
            }
            }
        }

        // Set shader bindings
        m_dotShader->setUniformValue(m_dotShader->lightP(), lightPos);
        m_dotShader->setUniformValue(m_dotShader->view(), viewMatrix);
        m_dotShader->setUniformValue(m_dotShader->model(), modelMatrix);
        m_dotShader->setUniformValue(m_dotShader->nModel(),
                                     itModelMatrix.inverted().transposed());
        m_dotShader->setUniformValue(m_dotShader->MVP(), MVPMatrix);
        m_dotShader->setUniformValue(m_dotShader->color(), barColor);
        m_dotShader->setUniformValue(m_dotShader->ambientS(), m_cachedTheme.m_ambientStrength);

#if !defined(QT_OPENGL_ES_2)
        if (m_cachedShadowQuality > ShadowNone) {
            // Set shadow shader bindings
            m_dotShader->setUniformValue(m_dotShader->shadowQ(), m_shadowQualityToShader);
            m_dotShader->setUniformValue(m_dotShader->depth(), depthMVPMatrix);
            m_dotShader->setUniformValue(m_dotShader->lightS(), lightStrength / 10.0f);

            // Draw the object
            m_drawer->drawObject(m_dotShader, m_dotObj, 0, m_depthTexture);
        } else
#endif
        {
            // Set shadowless shader bindings
            m_dotShader->setUniformValue(m_dotShader->lightS(), lightStrength);

            // Draw the object
            m_drawer->drawObject(m_dotShader, m_dotObj);
        }
    }

    // Release bar shader
    m_dotShader->release();

    // Bind background shader
    m_backgroundShader->bind();

    glCullFace(GL_BACK);

    // Draw background
    if (m_cachedIsBackgroundEnabled && m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 depthMVPMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(0.0f, -m_yAdjustment, zComp);
        modelMatrix.scale(
                    QVector3D(
                        (aspectRatio * backgroundMargin * m_areaSize.width()) / m_scaleFactor,
                        backgroundMargin,
                        (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));
        modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);
        itModelMatrix.scale(
                    QVector3D(
                        (aspectRatio * backgroundMargin * m_areaSize.width()) / m_scaleFactor,
                        backgroundMargin,
                        (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));

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

    // TODO: Grid lines cannot be done as in bars; we need configurable lines. Let's use ticks for now.
    // Draw grid lines
    if (m_cachedIsGridEnabled && m_heightNormalizer) {
        // Bind bar shader
        m_dotShader->bind();

        // Set unchanging shader bindings
        QVector3D barColor = Utils::vectorFromColor(m_cachedTheme.m_gridLine);
        m_dotShader->setUniformValue(m_dotShader->lightP(), lightPos);
        m_dotShader->setUniformValue(m_dotShader->view(), viewMatrix);
        m_dotShader->setUniformValue(m_dotShader->color(), barColor);
        m_dotShader->setUniformValue(m_dotShader->ambientS(), m_cachedTheme.m_ambientStrength);

        // Floor lines: rows
        GLfloat lineStep = (2.0f * m_scaleFactor * aspectRatio) / m_tickCount;
        GLfloat startLine = -m_scaleFactor * aspectRatio;

        for (GLfloat linePos = startLine; linePos <= -startLine; linePos += lineStep) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;
            QMatrix4x4 itModelMatrix;

            modelMatrix.translate(0.0f,
                                  -m_yAdjustment - backgroundMargin,
                                  linePos / m_scaleFactor + zComp);
            modelMatrix.scale(
                        QVector3D(
                            (aspectRatio * backgroundMargin * m_areaSize.width()) / m_scaleFactor,
                            gridLineWidth, gridLineWidth));
            itModelMatrix.scale(
                        QVector3D(
                            (aspectRatio * backgroundMargin * m_areaSize.width()) / m_scaleFactor,
                            gridLineWidth, gridLineWidth));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            m_dotShader->setUniformValue(m_dotShader->model(), modelMatrix);
            m_dotShader->setUniformValue(m_dotShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_dotShader->setUniformValue(m_dotShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > ShadowNone) {
                // Set shadow shader bindings
                m_dotShader->setUniformValue(m_dotShader->shadowQ(), m_shadowQualityToShader);
                m_dotShader->setUniformValue(m_dotShader->depth(), depthMVPMatrix);
                m_dotShader->setUniformValue(m_dotShader->lightS(),
                                             m_cachedTheme.m_lightStrength / 10.0f);

                // Draw the object
                m_drawer->drawObject(m_dotShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                m_dotShader->setUniformValue(m_dotShader->lightS(), m_cachedTheme.m_lightStrength);

                // Draw the object
                m_drawer->drawObject(m_dotShader, m_gridLineObj);
            }
        }

        // Floor lines: columns
        for (GLfloat linePos = startLine; linePos <= -startLine; linePos += lineStep) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;
            QMatrix4x4 itModelMatrix;

            modelMatrix.translate(linePos / m_scaleFactor,
                                  -m_yAdjustment - backgroundMargin,
                                  zComp);
            modelMatrix.scale(
                        QVector3D(
                            gridLineWidth, gridLineWidth,
                            (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));
            itModelMatrix.scale(
                        QVector3D(
                            gridLineWidth, gridLineWidth,
                            (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            m_dotShader->setUniformValue(m_dotShader->model(), modelMatrix);
            m_dotShader->setUniformValue(m_dotShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_dotShader->setUniformValue(m_dotShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > ShadowNone) {
                // Set shadow shader bindings
                m_dotShader->setUniformValue(m_dotShader->shadowQ(), m_shadowQualityToShader);
                m_dotShader->setUniformValue(m_dotShader->depth(), depthMVPMatrix);
                m_dotShader->setUniformValue(m_dotShader->lightS(),
                                             m_cachedTheme.m_lightStrength / 10.0f);

                // Draw the object
                m_drawer->drawObject(m_dotShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                m_dotShader->setUniformValue(m_dotShader->lightS(), m_cachedTheme.m_lightStrength);

                // Draw the object
                m_drawer->drawObject(m_dotShader, m_gridLineObj);
            }
        }

        // Wall lines: back wall
        lineStep = 2.0f * m_tickStep;
        startLine = -m_heightNormalizer;

        for (GLfloat linePos = startLine; linePos <= -startLine; linePos += lineStep) {
            GLfloat lineZTrans = (aspectRatio * backgroundMargin * m_areaSize.height())
                    / m_scaleFactor;
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;
            QMatrix4x4 itModelMatrix;

            if (!m_zFlipped)
                lineZTrans = -lineZTrans;

            modelMatrix.translate(0.0f,
                                  linePos / m_heightNormalizer - m_yAdjustment,
                                  lineZTrans + zComp);
            modelMatrix.scale(
                        QVector3D(
                            (aspectRatio * backgroundMargin * m_areaSize.width() / m_scaleFactor),
                            gridLineWidth, gridLineWidth));
            itModelMatrix.scale(
                        QVector3D(
                            (aspectRatio * backgroundMargin * m_areaSize.width() / m_scaleFactor),
                            gridLineWidth, gridLineWidth));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            m_dotShader->setUniformValue(m_dotShader->model(), modelMatrix);
            m_dotShader->setUniformValue(m_dotShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_dotShader->setUniformValue(m_dotShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > ShadowNone) {
                // Set shadow shader bindings
                m_dotShader->setUniformValue(m_dotShader->shadowQ(), m_shadowQualityToShader);
                m_dotShader->setUniformValue(m_dotShader->depth(), depthMVPMatrix);
                m_dotShader->setUniformValue(m_dotShader->lightS(),
                                             m_cachedTheme.m_lightStrength / 10.0f);

                // Draw the object
                m_drawer->drawObject(m_dotShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                m_dotShader->setUniformValue(m_dotShader->lightS(), m_cachedTheme.m_lightStrength);

                // Draw the object
                m_drawer->drawObject(m_dotShader, m_gridLineObj);
            }
        }

        // Wall lines: side wall
        for (GLfloat linePos = startLine; linePos <= -startLine; linePos += lineStep) {
            GLfloat lineXTrans = (aspectRatio * backgroundMargin * m_areaSize.width())
                    / m_scaleFactor;
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;
            QMatrix4x4 itModelMatrix;

            if (!m_xFlipped)
                lineXTrans = -lineXTrans;

            modelMatrix.translate(lineXTrans,
                                  linePos / m_heightNormalizer - m_yAdjustment,
                                  zComp);
            modelMatrix.scale(
                        QVector3D(
                            gridLineWidth, gridLineWidth,
                            (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));
            itModelMatrix.scale(
                        QVector3D(
                            gridLineWidth, gridLineWidth,
                            (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            m_dotShader->setUniformValue(m_dotShader->model(), modelMatrix);
            m_dotShader->setUniformValue(m_dotShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_dotShader->setUniformValue(m_dotShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > ShadowNone) {
                // Set shadow shader bindings
                m_dotShader->setUniformValue(m_dotShader->shadowQ(), m_shadowQualityToShader);
                m_dotShader->setUniformValue(m_dotShader->depth(), depthMVPMatrix);
                m_dotShader->setUniformValue(m_dotShader->lightS(),
                                             m_cachedTheme.m_lightStrength / 10.0f);

                // Draw the object
                m_drawer->drawObject(m_dotShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                m_dotShader->setUniformValue(m_dotShader->lightS(), m_cachedTheme.m_lightStrength);

                // Draw the object
                m_drawer->drawObject(m_dotShader, m_gridLineObj);
            }
        }

        // Release bar shader
        m_dotShader->release();
    }

    // Handle selection clearing and selection label drawing
    if (!barSelectionFound) {
        // We have no ownership, don't delete. Just NULL the pointer.
        m_selectedItem = NULL;
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

        m_drawer->drawLabel(*m_selectedBar, m_selectedBar->labelItem(),
                            viewMatrix, projectionMatrix,
                            QVector3D(0.0f, m_yAdjustment, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), m_selectedBar->height(),
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, m_camera, true);
#else
        // Draw the value string followed by row label and column label
        LabelItem &labelItem = m_selectedItem->selectionLabel();
        if (m_previouslySelectedItem != m_selectedItem || m_updateLabels || !labelItem.textureId()) {
            QString labelText = m_selectedItem->label();
            // TODO More elaborate label?
            m_drawer->generateLabelItem(labelItem, labelText);
            m_previouslySelectedItem = m_selectedItem;
        }

        m_drawer->drawLabel(*m_selectedItem, labelItem, viewMatrix, projectionMatrix,
                            QVector3D(0.0f, m_yAdjustment, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), m_selectedItem->height(),
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, camera, true, false, LabelMid);
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

    // Draw axis labels
    // TODO: Calculations done temporarily here. Should be done when calculating lines to avoid
    // extra for -loops?
    // Bind label shader
    m_labelShader->bind();

    glEnable(GL_TEXTURE_2D);
    if (m_cachedLabelTransparency > TransparencyNone) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Z Labels
    GLfloat posStep = (2.0f * m_scaleFactor * aspectRatio) / m_tickCount;
    GLfloat startPos = -m_scaleFactor * aspectRatio;
    int labelNbr = 0;
    for (GLfloat labelPos = startPos; labelPos <= -startPos; labelPos += posStep) {
        if (m_axisCacheX.labelItems().size() > labelNbr) {
            GLfloat labelXTrans = (aspectRatio * backgroundMargin * m_areaSize.width())
                    / m_scaleFactor;
            GLfloat rotLabelX = -90.0f;
            GLfloat rotLabelY = 0.0f;
            GLfloat rotLabelZ = 0.0f;
            Qt::AlignmentFlag alignment = Qt::AlignRight;
            if (m_zFlipped)
                rotLabelY = 180.0f;
            if (m_xFlipped) {
                labelXTrans = -labelXTrans;
                alignment = Qt::AlignLeft;
            }
            QVector3D labelTrans = QVector3D(
                        labelXTrans,
                        -m_yAdjustment - (m_heightNormalizer * backgroundMargin),
                        labelPos / m_scaleFactor + zComp);

            // TODO: Try it; draw the label here
            m_dummyRenderItem.setTranslation(labelTrans);
            const LabelItem &axisLabelItem =
                    *m_axisCacheZ.labelItems().at(labelNbr);

            m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, camera, true, true, LabelMid,
                                alignment);
        }
        labelNbr++;
    }
    // X Labels
    labelNbr = 0;
    for (GLfloat labelPos = startPos; labelPos <= -startPos; labelPos += posStep) {
        if (m_axisCacheX.labelItems().size() > labelNbr) {
            GLfloat labelZTrans = (aspectRatio * backgroundMargin * m_areaSize.height())
                    / m_scaleFactor;
            GLfloat rotLabelX = -90.0f;
            GLfloat rotLabelY = 90.0f;
            GLfloat rotLabelZ = 0.0f;
            Qt::AlignmentFlag alignment = Qt::AlignLeft;
            if (m_xFlipped)
                rotLabelY = -90.0f;
            if (m_zFlipped) {
                labelZTrans = -labelZTrans;
                alignment = Qt::AlignRight;
            }
            QVector3D labelTrans = QVector3D(
                        labelPos / m_scaleFactor,
                        -m_yAdjustment - (m_heightNormalizer * backgroundMargin),
                        labelZTrans + zComp);

            // TODO: Try it; draw the label here
            m_dummyRenderItem.setTranslation(labelTrans);
            const LabelItem &axisLabelItem =
                    *m_axisCacheX.labelItems().at(labelNbr);

            m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, camera, true, true, LabelMid,
                                alignment);
        }
        labelNbr++;
    }
    // Y Labels
    posStep = 2.0f * m_tickStep;
    startPos = -m_heightNormalizer;
    labelNbr = 0;
    for (GLfloat labelPos = startPos; labelPos <= -startPos; labelPos += posStep) {
        // TODO: Test with x labels
        if (m_axisCacheX.labelItems().size() > labelNbr) {
            GLfloat labelXTrans = (aspectRatio * backgroundMargin * m_areaSize.width())
                    / m_scaleFactor;
            GLfloat labelZTrans = (aspectRatio * backgroundMargin * m_areaSize.height())
                    / m_scaleFactor;
            GLfloat labelYTrans = labelPos / m_heightNormalizer - m_yAdjustment;
            GLfloat rotLabelX = 0.0f;
            GLfloat rotLabelY = -90.0f;
            GLfloat rotLabelZ = 0.0f;
            Qt::AlignmentFlag alignment = Qt::AlignLeft;
            if (!m_xFlipped) {
                labelXTrans = -labelXTrans;
                rotLabelY = 90.0f;
            }
            if (m_zFlipped) {
                labelZTrans = -labelZTrans;
                alignment = Qt::AlignRight;
            }

            // TODO: Test with x labels
            const LabelItem &axisLabelItem = *m_axisCacheX.labelItems().at(labelNbr);

            // Back wall
            QVector3D labelTrans = QVector3D(labelXTrans, labelYTrans, labelZTrans + zComp);

            m_dummyRenderItem.setTranslation(labelTrans);
            m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, camera, true, true, LabelMid,
                                alignment);

            // Side wall
            if (m_xFlipped)
                alignment = Qt::AlignLeft;
            else
                alignment = Qt::AlignRight;
            if (m_zFlipped)
                rotLabelY = 180.0f;
            else
                rotLabelY = 0.0f;

            labelTrans = QVector3D(-labelXTrans, labelYTrans, -labelZTrans + zComp);

            m_dummyRenderItem.setTranslation(labelTrans);
            m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, camera, true, true, LabelMid,
                                alignment);
        }
        labelNbr++;
    }
    glDisable(GL_TEXTURE_2D);
    if (m_cachedLabelTransparency > TransparencyNone)
        glDisable(GL_BLEND);

    // Release label shader
    m_labelShader->release();
}

void Scatter3DRenderer::requestSelectionAtPoint(const QPoint &point)
{
    //qDebug() << __FUNCTION__;
    QMutexLocker locker(&m_mutex);
    m_selectionPointRequest.setX(point.x());
    m_selectionPointRequest.setY(point.y());
    m_isSelectionPointRequestActive = true;
}

void Scatter3DRenderer::handleResize()
{
    //qDebug() << __FUNCTION__;
    if (m_cachedBoundingRect.width() == 0 || m_cachedBoundingRect.height() == 0)
        return;

    // Set view port
    m_mainViewPort = QRect(0, 0, m_cachedBoundingRect.width(), m_cachedBoundingRect.height());

    Abstract3DRenderer::handleResize();
}

void Scatter3DRenderer::updateMeshFileName(const QString &objFileName)
{
    //qDebug() << __FUNCTION__;
    m_cachedObjFile = objFileName;
    loadBarMesh();
}

void Scatter3DRenderer::updateSelectionMode(SelectionMode mode)
{
    //qDebug() << __FUNCTION__;
    m_cachedSelectionMode = mode;
}

void Scatter3DRenderer::updateGridEnabled(bool enable)
{
    //qDebug() << __FUNCTION__;
    m_cachedIsGridEnabled = enable;
}

void Scatter3DRenderer::updateBackgroundEnabled(bool enable)
{
    //qDebug() << __FUNCTION__;
    if (m_cachedIsBackgroundEnabled != enable) {
        m_cachedIsBackgroundEnabled = enable;
        // Load changed bar type
        loadBarMesh();
    }
}

void Scatter3DRenderer::updateShadowQuality(ShadowQuality quality)
{
    qDebug() << "Scatter3DRenderer::setShadowQuality" << quality;
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

void Scatter3DRenderer::updateTickCount(GLint tickCount, GLfloat step, GLfloat minimum)
{
    //qDebug() << __FUNCTION__;
    m_tickCount = tickCount;
    m_tickStep = step;
    if (tickCount > 0 && step > 0) {
        m_autoAdjust = false;
        m_heightNormalizer = tickCount * step;
        calculateHeightAdjustment(QPair<float, float>(minimum, m_heightNormalizer));
        m_valueUpdateNeeded = true;
    }
}

void Scatter3DRenderer::loadBarMesh()
{
    //qDebug() << __FUNCTION__;
    QString objectFileName = m_cachedObjFile;
    if (m_dotObj)
        delete m_dotObj;
    m_dotObj = new ObjectHelper(objectFileName);
    m_dotObj->load();
}

void Scatter3DRenderer::loadBackgroundMesh()
{
    //qDebug() << __FUNCTION__;
    if (m_backgroundObj)
        delete m_backgroundObj;
    m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/background"));
    m_backgroundObj->load();
}

void Scatter3DRenderer::loadGridLineMesh()
{
    //qDebug() << __FUNCTION__;
    if (m_gridLineObj)
        delete m_gridLineObj;
    m_gridLineObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/bar"));
    m_gridLineObj->load();
}

void Scatter3DRenderer::loadLabelMesh()
{
    //qDebug() << __FUNCTION__;
    if (m_labelObj)
        delete m_labelObj;
    m_labelObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/label"));
    m_labelObj->load();
}

void Scatter3DRenderer::updateTextures()
{
    //qDebug() << __FUNCTION__;
    // Drawer has changed; this flag needs to be checked when checking if we need to update labels
    m_updateLabels = true;
}

void Scatter3DRenderer::calculateTranslation(ScatterRenderItem &item)
{
    //qDebug() << __FUNCTION__;

    // Origin should be in the center of scene, ie. both positive and negative values are drawn
    // above background

    // We need to normalize translations
    GLfloat xTrans = (aspectRatio * 2.0f * item.position().x()) / m_scaleFactor;
    GLfloat zTrans = (aspectRatio * 2.0f * item.position().z()) / m_scaleFactor;
    GLfloat yTrans = item.position().y() / m_heightNormalizer;
    item.setTranslation(QVector3D(xTrans, yTrans, zTrans + zComp));
    //qDebug() << item.translation() << m_heightNormalizer;
}

void Scatter3DRenderer::calculateSceneScalingFactors(const QVector3D &limits)
{
    if (m_autoAdjust) {
        m_heightNormalizer = (GLfloat)qMax((qreal)m_heightNormalizer, qFabs(limits.y()));
        m_tickStep = m_heightNormalizer / m_tickCount;
    }
    // Auto-adjust these anyway (until axis -based ticks are taken into use)
    m_areaSize.setHeight(qMax(m_areaSize.height(), (qreal)limits.z()));
    m_areaSize.setWidth(qMax(m_areaSize.width(), (qreal)limits.x()));
    m_scaleFactor = qMax(qMax((qreal)m_scaleFactor, m_areaSize.width()), m_areaSize.height());
    //qDebug() << m_heightNormalizer << m_areaSize << m_scaleFactor << m_tickStep;
}

void Scatter3DRenderer::calculateHeightAdjustment(const QPair<GLfloat, GLfloat> &limits)
{
    //qDebug() << __FUNCTION__;
    // 2.0f = max difference between minimum and maximum value after scaling with m_heightNormalizer
    m_yAdjustment = 2.0f - ((limits.second - limits.first) / m_heightNormalizer);
    //qDebug() << limits << m_yAdjustment;
}

Scatter3DController::SelectionType Scatter3DRenderer::isSelected(GLint bar,
                                                                 const QVector3D &selection)
{
    //qDebug() << __FUNCTION__;
    GLubyte barIdxRed = 0;
    GLubyte barIdxGreen = 0;
    GLubyte barIdxBlue = 0;
    //static QVector3D prevSel = selection; // TODO: For debugging
    Scatter3DController::SelectionType isSelectedType = Scatter3DController::SelectionNone;

    if (selection == selectionSkipColor)
        return isSelectedType; // skip window

    if (bar <= 255) {
        barIdxRed = bar;
    } else if (bar <= 65535) {
        barIdxGreen = bar / 256;
        barIdxRed = bar % 256;
    } else {
        barIdxBlue = bar / 65535;
        barIdxGreen = bar % 65535;
        barIdxRed = bar % 256;
    }

    QVector3D current = QVector3D(barIdxRed, barIdxGreen, barIdxBlue);

    // TODO: For debugging
    //if (selection != prevSel) {
    //    qDebug() << selection.x() << selection .y() << selection.z();
    //    prevSel = selection;
    //}

    if (current == selection)
        isSelectedType = Scatter3DController::SelectionBar;

    return isSelectedType;
}

void Scatter3DRenderer::updateZoomLevel(int newZoomLevel)
{
    //qDebug() << __FUNCTION__;
    m_cachedZoomLevel = newZoomLevel;
}


QRect Scatter3DRenderer::mainViewPort()
{
    //qDebug() << __FUNCTION__;
    return m_mainViewPort;
}

void Scatter3DRenderer::initShaders(const QString &vertexShader, const QString &fragmentShader)
{
    //qDebug() << __FUNCTION__;
    if (m_dotShader)
        delete m_dotShader;
    m_dotShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_dotShader->initialize();
}

void Scatter3DRenderer::initSelectionShader()
{
    //qDebug() << __FUNCTION__;
    if (m_selectionShader)
        delete m_selectionShader;
    m_selectionShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexSelection"),
                                         QStringLiteral(":/shaders/fragmentSelection"));
    m_selectionShader->initialize();
}

void Scatter3DRenderer::initSelectionBuffer()
{
    //qDebug() << __FUNCTION__;
    if (m_selectionTexture)
        m_textureHelper->deleteTexture(&m_selectionTexture);

    m_selectionTexture = m_textureHelper->createSelectionTexture(m_mainViewPort.size(),
                                                                 m_selectionFrameBuffer,
                                                                 m_selectionDepthBuffer);
}

#if !defined(QT_OPENGL_ES_2)
void Scatter3DRenderer::initDepthShader()
{
    //qDebug() << __FUNCTION__;
    if (m_depthShader)
        delete m_depthShader;
    m_depthShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexDepth"),
                                     QStringLiteral(":/shaders/fragmentDepth"));
    m_depthShader->initialize();
}

void Scatter3DRenderer::updateDepthBuffer()
{
    //qDebug() << __FUNCTION__;
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

void Scatter3DRenderer::initBackgroundShaders(const QString &vertexShader,
                                              const QString &fragmentShader)
{
    //qDebug() << __FUNCTION__;
    if (m_backgroundShader)
        delete m_backgroundShader;
    m_backgroundShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_backgroundShader->initialize();
}

void Scatter3DRenderer::initLabelShaders(const QString &vertexShader, const QString &fragmentShader)
{
    //qDebug() << __FUNCTION__;
    if (m_labelShader)
        delete m_labelShader;
    m_labelShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_labelShader->initialize();
}

QT_DATAVIS3D_END_NAMESPACE
