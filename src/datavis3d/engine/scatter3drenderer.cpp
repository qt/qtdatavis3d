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

// Commenting this draws the shadow map with perspective projection. Otherwise it's drawn in
// orthographic projection.
//#define USE_WIDER_SHADOWS

// You can verify that depth buffer drawing works correctly by uncommenting this.
// You should see the scene from  where the light is
//#define SHOW_DEPTH_TEXTURE_SCENE

QT_DATAVIS3D_BEGIN_NAMESPACE

//#define USE_UNIFORM_SCALING // Scale x and z uniformly, or based on autoscaled values

const GLfloat aspectRatio = 2.0f; // Forced ratio of x and z to y. Dynamic will make it look odd.
// TODO: Make margin modifiable?
const GLfloat backgroundMargin = 1.1f; // Margin for background (1.1f = make it 10% larger to avoid items being drawn inside background)
const GLfloat gridLineWidth = 0.005f;
static QVector3D selectionSkipColor = QVector3D(255, 255, 255); // Selection texture's background color

Scatter3DRenderer::Scatter3DRenderer(Scatter3DController *controller)
    : Abstract3DRenderer(controller),
      m_controller(controller),
      m_selectedItem(0),
      m_xFlipped(false),
      m_zFlipped(false),
      m_yFlipped(false),
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
      m_scaleFactor(0),
      m_selection(selectionSkipColor),
      m_previousSelection(selectionSkipColor),
      m_areaSize(QSizeF(0.0, 0.0)),
      m_dotSizeScale(1.0f),
      m_hasHeightAdjustmentChanged(true)
{
    //qDebug() << __FUNCTION__;
    m_dummyRenderItem.setRenderer(this);

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
}

void Scatter3DRenderer::initializeOpenGL()
{
    Abstract3DRenderer::initializeOpenGL();

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

    // Load grid line mesh
    loadGridLineMesh();

    // Load label mesh
    loadLabelMesh();

    // Set view port
    glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
               m_mainViewPort.width(), m_mainViewPort.height());

    // Load background mesh (we need to be initialized first)
    loadBackgroundMesh();
}

void Scatter3DRenderer::updateDataModel(QScatterDataProxy *dataProxy)
{
    int actualDataSize = 0;
    const QScatterDataArray &dataArray = *dataProxy->array();
    calculateSceneScalingFactors();
    int dataSize = dataArray.size();
    m_renderItemArray.resize(dataSize);
    for (int i = 0; i < dataSize ; i++) {
        QVector3D dotPos = dataArray.at(i).position();
        if ((dotPos.x() >= m_axisCacheX.min() && dotPos.x() <= m_axisCacheX.max())
                && (dotPos.y() >= m_axisCacheY.min() && dotPos.y() <= m_axisCacheY.max())
                && (dotPos.z() >= m_axisCacheZ.min() && dotPos.z() <= m_axisCacheZ.max())) {
            m_renderItemArray[actualDataSize].setPosition(dotPos);
            calculateTranslation(m_renderItemArray[actualDataSize]);
            m_renderItemArray[actualDataSize].setRenderer(this);
            actualDataSize++;
        }
    }
    m_renderItemArray.resize(actualDataSize);
    m_dotSizeScale = (GLfloat)qBound(0.01, (2.0 / qSqrt((qreal)actualDataSize)), 0.1);
    m_selectedItem = 0;

    Abstract3DRenderer::updateDataModel(dataProxy);
}

void Scatter3DRenderer::render(CameraHelper *camera, const GLuint defaultFboHandle)
{
    //qDebug() << __FUNCTION__;

    // Handle GL state setup for FBO buffers and clearing of the render surface
    Abstract3DRenderer::render(camera, defaultFboHandle);

    if (m_hasHeightAdjustmentChanged) {
        // Set initial camera position. Also update if height adjustment has changed.
        camera->setDefaultCameraOrientation(QVector3D(0.0f, 0.0f, 6.0f + zComp),
                                            QVector3D(0.0f, 0.0f, zComp),
                                            QVector3D(0.0f, 1.0f, 0.0f));
        m_hasHeightAdjustmentChanged = false;
    }

    // Draw dots scene
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
                m_mainViewPort.height(),
                true);

    // Calculate label flipping
    if (viewMatrix.row(0).x() > 0)
        m_zFlipped = false;
    else
        m_zFlipped = true;
    if (viewMatrix.row(0).z() <= 0)
        m_xFlipped = false;
    else
        m_xFlipped = true;

    // Check if we're viewing the scene from below
    if (viewMatrix.row(2).y() < 0)
        m_yFlipped = true;
    else
        m_yFlipped = false;

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

    // Map adjustment direction to model matrix scaling
    // TODO: Let's use these for testing the autoscaling of dots based on their number
    GLfloat heightMultiplier = m_dotSizeScale; //1.0f;
    GLfloat widthMultiplier = m_dotSizeScale; //1.0f;
    GLfloat depthMultiplier = m_dotSizeScale; //1.0f;
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
    if (m_cachedShadowQuality > QDataVis::ShadowNone) {
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
        QVector3D depthLightPos = camera->calculateLightPosition(
                    defaultLightPos, 0.0f, 1.0f / m_autoScaleAdjustment);
        depthViewMatrix.lookAt(depthLightPos, QVector3D(0.0f, 0.0f, zComp),
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
        // Draw dots to depth buffer
        for (int dot = 0; dot < m_renderItemArray.size(); dot++) {
            const ScatterRenderItem &item = m_renderItemArray.at(dot);

            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;

            modelMatrix.translate(item.translation().x(),
                                  item.translation().y(),
                                  item.translation().z());
            modelMatrix.scale(QVector3D(widthMultiplier + widthScaler,
                                        heightMultiplier + heightScaler,
                                        depthMultiplier + depthScaler));
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
    if (m_cachedSelectionMode > QDataVis::ModeNone) {
        // Bind selection shader
        m_selectionShader->bind();

        // Draw dots to selection buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (= skipColor)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled

        int arraySize = m_renderItemArray.size();
        if (arraySize > 0xfffffe) // Max possible different selection colors, 0xffffff being skipColor
            qFatal("Too many objects");

        for (int dot = 0; dot < arraySize; dot++) {
            const ScatterRenderItem &item = m_renderItemArray.at(dot);

            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;

            modelMatrix.translate(item.translation().x(),
                                  item.translation().y(),
                                  item.translation().z());
            modelMatrix.scale(QVector3D(widthMultiplier + widthScaler,
                                        heightMultiplier + heightScaler,
                                        depthMultiplier + depthScaler));
            //modelMatrix.scale(QVector3D(widthMultiplier * item.size() + widthScaler,
            //                            heightMultiplier * item.size() + heightScaler,
            //                            depthMultiplier * item.size() + depthScaler));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

            QVector3D dotColor = indexToSelectionColor(dot);
            dotColor /= 255.0f;

            m_selectionShader->setUniformValue(m_selectionShader->MVP(), MVPMatrix);
            m_selectionShader->setUniformValue(m_selectionShader->color(), dotColor);

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

    // Bind dot shader
    m_dotShader->bind();

    // Enable texture
    glEnable(GL_TEXTURE_2D);

    // Draw dots
    bool dotSelectionFound = false;
    int selectedIndex;
    if (m_selection == selectionSkipColor) {
        selectedIndex = Scatter3DController::noSelectionIndex();
    } else {
        selectedIndex = int(m_selection.x())
                + (int(m_selection.y()) << 8)
                + (int(m_selection.z()) << 16);
    }

    if (m_selection != m_previousSelection) {
        emit selectedItemIndexChanged(selectedIndex);
        m_previousSelection = m_selection;
    }

    ScatterRenderItem *selectedItem(0);

    for (int dot = 0; dot < m_renderItemArray.size(); dot++) {
        ScatterRenderItem &item = m_renderItemArray[dot];

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 depthMVPMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(item.translation().x(),
                              item.translation().y(),
                              item.translation().z());
        modelMatrix.scale(QVector3D(widthMultiplier + widthScaler,
                                    heightMultiplier + heightScaler,
                                    depthMultiplier + depthScaler));
        //modelMatrix.scale(QVector3D(widthMultiplier * item.size() + widthScaler,
        //                            heightMultiplier * item.size() + heightScaler,
        //                            depthMultiplier * item.size() + depthScaler));
        itModelMatrix.scale(QVector3D(widthMultiplier + widthScaler,
                                      heightMultiplier + heightScaler,
                                      depthMultiplier + depthScaler));
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

        QVector3D dotColor = baseColor + heightColor;

        GLfloat lightStrength = m_cachedTheme.m_lightStrength;
        if (m_cachedSelectionMode > QDataVis::ModeNone && (selectedIndex == dot)) {
            dotColor = Utils::vectorFromColor(m_cachedTheme.m_highlightBarColor);
            lightStrength = m_cachedTheme.m_highlightLightStrength;
            // Insert data to ScatterRenderItem. We have no ownership, don't delete the previous one
            selectedItem = &item;
            dotSelectionFound = true;
        }

        // Set shader bindings
        m_dotShader->setUniformValue(m_dotShader->lightP(), lightPos);
        m_dotShader->setUniformValue(m_dotShader->view(), viewMatrix);
        m_dotShader->setUniformValue(m_dotShader->model(), modelMatrix);
        m_dotShader->setUniformValue(m_dotShader->nModel(),
                                     itModelMatrix.inverted().transposed());
        m_dotShader->setUniformValue(m_dotShader->MVP(), MVPMatrix);
        m_dotShader->setUniformValue(m_dotShader->color(), dotColor);
        m_dotShader->setUniformValue(m_dotShader->ambientS(), m_cachedTheme.m_ambientStrength);

#if !defined(QT_OPENGL_ES_2)
        if (m_cachedShadowQuality > QDataVis::ShadowNone) {
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

    // Release dot shader
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

        modelMatrix.translate(0.0f, 0.0f, zComp);
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
        modelMatrix.scale(
                    QVector3D(
                        (aspectRatio * backgroundMargin * m_areaSize.width()) / m_scaleFactor,
                        backgroundMargin,
                        (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));
#else // ..and this if we want uniform scaling based on largest dimension
        modelMatrix.scale(QVector3D((aspectRatio * backgroundMargin),
                                    backgroundMargin,
                                    (aspectRatio * backgroundMargin)));
#endif
        // We can copy modelMatrix to itModelMatrix as it has not been translated
        itModelMatrix = modelMatrix;
        // If we're viewing from below, background object must be flipped
        if (m_yFlipped) {
            modelMatrix.rotate(180.0f, 1.0, 0.0, 0.0);
            modelMatrix.rotate(270.0f - backgroundRotation, 0.0f, 1.0f, 0.0f);
        } else {
            modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);
        }

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
        if (m_cachedShadowQuality > QDataVis::ShadowNone) {
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

    // Draw grid lines
#ifdef USE_UNIFORM_SCALING
    AxisRenderCache *axisCacheMax;
    if (m_axisCacheZ.max() > m_axisCacheX.max())
        axisCacheMax = &m_axisCacheZ;
    else
        axisCacheMax = &m_axisCacheX;
#endif

    if (m_cachedIsGridEnabled && m_heightNormalizer) {
        // Bind line shader
        m_dotShader->bind();

        // Set unchanging shader bindings
        QVector3D lineColor = Utils::vectorFromColor(m_cachedTheme.m_gridLine);
        m_dotShader->setUniformValue(m_dotShader->lightP(), lightPos);
        m_dotShader->setUniformValue(m_dotShader->view(), viewMatrix);
        m_dotShader->setUniformValue(m_dotShader->color(), lineColor);
        m_dotShader->setUniformValue(m_dotShader->ambientS(), m_cachedTheme.m_ambientStrength);

        // Floor lines: rows (= Z)
        if (m_axisCacheZ.segmentCount() > 0) {
#ifndef USE_UNIFORM_SCALING
            GLfloat lineStep = aspectRatio * m_axisCacheZ.subSegmentStep();
            GLfloat linePos = aspectRatio * m_axisCacheZ.min(); // Start line
            int lastSegment = m_axisCacheZ.subSegmentCount() * m_axisCacheZ.segmentCount();
#else
            GLfloat lineStep = aspectRatio * axisCacheMax->subSegmentStep();
            GLfloat linePos = -aspectRatio * m_scaleFactor; // Start line
            int lastSegment = axisCacheMax->subSegmentCount() * axisCacheMax->segmentCount();
#endif

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 depthMVPMatrix;
                QMatrix4x4 itModelMatrix;

                if (m_yFlipped) {
                    modelMatrix.translate(0.0f,
                                          backgroundMargin,
                                          linePos / m_scaleFactor + zComp);
                } else {
                    modelMatrix.translate(0.0f,
                                          -backgroundMargin,
                                          linePos / m_scaleFactor + zComp);
                }
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
                modelMatrix.scale(
                            QVector3D(
                                (aspectRatio * backgroundMargin * m_areaSize.width()) / m_scaleFactor,
                                gridLineWidth, gridLineWidth));
                itModelMatrix.scale(
                            QVector3D(
                                (aspectRatio * backgroundMargin * m_areaSize.width()) / m_scaleFactor,
                                gridLineWidth, gridLineWidth));
#else // ..and this if we want uniform scaling based on largest dimension
                modelMatrix.scale(QVector3D((aspectRatio * backgroundMargin),
                                            gridLineWidth, gridLineWidth));
                itModelMatrix.scale(QVector3D(aspectRatio * backgroundMargin,
                                              gridLineWidth, gridLineWidth));
#endif

                // If we're viewing from below, grid line object must be flipped
                if (m_yFlipped)
                    modelMatrix.rotate(180.0f, 1.0, 0.0, 0.0);

                MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
                depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                m_dotShader->setUniformValue(m_dotShader->model(), modelMatrix);
                m_dotShader->setUniformValue(m_dotShader->nModel(),
                                             itModelMatrix.inverted().transposed());
                m_dotShader->setUniformValue(m_dotShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QDataVis::ShadowNone) {
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
                    m_dotShader->setUniformValue(m_dotShader->lightS(),
                                                 m_cachedTheme.m_lightStrength);

                    // Draw the object
                    m_drawer->drawObject(m_dotShader, m_gridLineObj);
                }
                linePos += lineStep;
            }
        }

        // Floor lines: columns (= X)
        if (m_axisCacheX.segmentCount() > 0) {
#ifndef USE_UNIFORM_SCALING
            GLfloat lineStep = aspectRatio * m_axisCacheX.subSegmentStep();
            GLfloat linePos = aspectRatio * m_axisCacheX.min();
            int lastSegment = m_axisCacheX.subSegmentCount() * m_axisCacheX.segmentCount();
#else
            GLfloat lineStep = aspectRatio * axisCacheMax->subSegmentStep();
            GLfloat linePos = -aspectRatio * m_scaleFactor;
            int lastSegment = axisCacheMax->subSegmentCount() * axisCacheMax->segmentCount();
#endif

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 depthMVPMatrix;
                QMatrix4x4 itModelMatrix;

                if (m_yFlipped) {
                    modelMatrix.translate(linePos / m_scaleFactor,
                                          backgroundMargin,
                                          zComp);
                } else {
                    modelMatrix.translate(linePos / m_scaleFactor,
                                          -backgroundMargin,
                                          zComp);
                }
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
                modelMatrix.scale(
                            QVector3D(
                                gridLineWidth, gridLineWidth,
                                (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));
                itModelMatrix.scale(
                            QVector3D(
                                gridLineWidth, gridLineWidth,
                                (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));
#else // ..and this if we want uniform scaling based on largest dimension
                modelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                            aspectRatio * backgroundMargin));
                itModelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                              aspectRatio * backgroundMargin));
#endif

                // If we're viewing from below, grid line object must be flipped
                if (m_yFlipped)
                    modelMatrix.rotate(180.0f, 1.0, 0.0, 0.0);

                MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
                depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                m_dotShader->setUniformValue(m_dotShader->model(), modelMatrix);
                m_dotShader->setUniformValue(m_dotShader->nModel(),
                                             itModelMatrix.inverted().transposed());
                m_dotShader->setUniformValue(m_dotShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QDataVis::ShadowNone) {
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
                linePos += lineStep;
            }
        }

        // Wall lines: back wall
        if (m_axisCacheY.segmentCount() > 0) {
            GLfloat lineStep = m_axisCacheY.subSegmentStep();
            GLfloat linePos = m_axisCacheY.min();
            int lastSegment = m_axisCacheY.subSegmentCount() * m_axisCacheY.segmentCount();

            for (int segment = 0; segment <= lastSegment; segment++) {
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
                GLfloat lineZTrans = (aspectRatio * backgroundMargin * m_areaSize.height())
                                     / m_scaleFactor;
#else // ..and this if we want uniform scaling based on largest dimension
                GLfloat lineZTrans = aspectRatio * backgroundMargin;
#endif
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 depthMVPMatrix;
                QMatrix4x4 itModelMatrix;

                if (!m_zFlipped)
                    lineZTrans = -lineZTrans;

                modelMatrix.translate(0.0f,
                                      linePos / m_heightNormalizer,
                                      lineZTrans + zComp);
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
                modelMatrix.scale(
                            QVector3D(
                                (aspectRatio * backgroundMargin * m_areaSize.width() / m_scaleFactor),
                                gridLineWidth, gridLineWidth));
                itModelMatrix.scale(
                            QVector3D(
                                (aspectRatio * backgroundMargin * m_areaSize.width() / m_scaleFactor),
                                gridLineWidth, gridLineWidth));
#else // ..and this if we want uniform scaling based on largest dimension
                modelMatrix.scale(QVector3D((aspectRatio * backgroundMargin),
                                            gridLineWidth, gridLineWidth));
                itModelMatrix.scale(QVector3D(aspectRatio * backgroundMargin,
                                              gridLineWidth, gridLineWidth));
#endif

                MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
                depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                m_dotShader->setUniformValue(m_dotShader->model(), modelMatrix);
                m_dotShader->setUniformValue(m_dotShader->nModel(),
                                             itModelMatrix.inverted().transposed());
                m_dotShader->setUniformValue(m_dotShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QDataVis::ShadowNone) {
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
                linePos += lineStep;
            }

            // Wall lines: side wall
            linePos = m_axisCacheY.min();
            lastSegment = m_axisCacheY.subSegmentCount() * m_axisCacheY.segmentCount();

            for (int segment = 0; segment <= lastSegment; segment++) {
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
                GLfloat lineXTrans = (aspectRatio * backgroundMargin * m_areaSize.width())
                                     / m_scaleFactor;
#else // ..and this if we want uniform scaling based on largest dimension
                GLfloat lineXTrans = aspectRatio * backgroundMargin;
#endif
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 depthMVPMatrix;
                QMatrix4x4 itModelMatrix;

                if (!m_xFlipped)
                    lineXTrans = -lineXTrans;

                modelMatrix.translate(lineXTrans,
                                      linePos / m_heightNormalizer,
                                      zComp);
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
                modelMatrix.scale(
                            QVector3D(
                                gridLineWidth, gridLineWidth,
                                (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));
                itModelMatrix.scale(
                            QVector3D(
                                gridLineWidth, gridLineWidth,
                                (aspectRatio * backgroundMargin * m_areaSize.height()) / m_scaleFactor));

#else // ..and this if we want uniform scaling based on largest dimension
                modelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                            (aspectRatio * backgroundMargin)));
                itModelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                              aspectRatio * backgroundMargin));

#endif
                MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
                depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                m_dotShader->setUniformValue(m_dotShader->model(), modelMatrix);
                m_dotShader->setUniformValue(m_dotShader->nModel(),
                                             itModelMatrix.inverted().transposed());
                m_dotShader->setUniformValue(m_dotShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QDataVis::ShadowNone) {
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
                linePos += lineStep;
            }
        }

        // Release line shader
        m_dotShader->release();
    }

    // Draw axis labels
    // TODO: Calculations done temporarily here. Should be done when calculating lines to avoid
    // extra for -loops?
    // Bind label shader
    m_labelShader->bind();

    glEnable(GL_TEXTURE_2D);
    if (m_cachedLabelTransparency > QDataVis::TransparencyNone) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Z Labels
    if (m_axisCacheZ.segmentCount() > 0) {
#ifndef USE_UNIFORM_SCALING
        GLfloat posStep = aspectRatio * m_axisCacheZ.segmentStep();
        GLfloat labelPos = aspectRatio * m_axisCacheZ.max();
        int lastSegment = m_axisCacheZ.segmentCount();
#else
        GLfloat posStep = aspectRatio * axisCacheMax->segmentStep();
        GLfloat labelPos = aspectRatio * m_scaleFactor;
        int lastSegment = axisCacheMax->segmentCount();
#endif
        int labelNbr = 0;
        for (int segment = 0; segment <= lastSegment; segment++) {
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
            if (m_axisCacheZ.labelItems().size() > labelNbr) {
                GLfloat labelXTrans = (aspectRatio * backgroundMargin * m_areaSize.width())
                                      / m_scaleFactor;
#else // ..and this if we want uniform scaling based on largest dimension
            if (axisCacheMax->labelItems().size() > labelNbr) {
                GLfloat labelXTrans = aspectRatio * backgroundMargin;
#endif
                GLfloat labelYTrans = -backgroundMargin;
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
                if (m_yFlipped) {
                    rotLabelZ += 180.0f;
                    rotLabelY += 180.0f;
                    labelYTrans = -labelYTrans;
                }
                QVector3D labelTrans = QVector3D(labelXTrans,
                                                 labelYTrans,
                                                 labelPos / m_scaleFactor + zComp);


                // Draw the label here
                m_dummyRenderItem.setTranslation(labelTrans);
#ifndef USE_UNIFORM_SCALING
                const LabelItem &axisLabelItem = *m_axisCacheZ.labelItems().at(labelNbr);
#else
                const LabelItem &axisLabelItem = *axisCacheMax->labelItems().at(labelNbr);
#endif

                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    QVector3D(0.0f, 0.0f, zComp),
                                    QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                    0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, camera, true, true, Drawer::LabelMid,
                                    alignment);
            }
            labelNbr++;
            labelPos -= posStep;
        }
    }
    // X Labels
    if (m_axisCacheX.segmentCount() > 0) {
#ifndef USE_UNIFORM_SCALING
        GLfloat posStep = aspectRatio * m_axisCacheX.segmentStep();
        GLfloat labelPos = aspectRatio * m_axisCacheX.min();
        int lastSegment = m_axisCacheX.segmentCount();
#else
        GLfloat posStep = aspectRatio * axisCacheMax->segmentStep();
        GLfloat labelPos = -aspectRatio * m_scaleFactor;
        int lastSegment = axisCacheMax->segmentCount();
#endif
        int labelNbr = 0;
        for (int segment = 0; segment <= lastSegment; segment++) {
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
            if (m_axisCacheX.labelItems().size() > labelNbr) {
                GLfloat labelZTrans = (aspectRatio * backgroundMargin * m_areaSize.height())
                                      / m_scaleFactor;
#else // ..and this if we want uniform scaling based on largest dimension
            if (axisCacheMax->labelItems().size() > labelNbr) {
                GLfloat labelZTrans = aspectRatio * backgroundMargin;
#endif
                GLfloat labelYTrans = -backgroundMargin;
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
                if (m_yFlipped) {
                    rotLabelZ += 180.0f;
                    rotLabelY += 180.0f;
                    labelYTrans = -labelYTrans;
                }
                QVector3D labelTrans = QVector3D(labelPos / m_scaleFactor,
                                                 labelYTrans,
                                                 labelZTrans + zComp);

                // Draw the label here
                m_dummyRenderItem.setTranslation(labelTrans);
#ifndef USE_UNIFORM_SCALING
                const LabelItem &axisLabelItem = *m_axisCacheX.labelItems().at(labelNbr);
#else
                const LabelItem &axisLabelItem = *axisCacheMax->labelItems().at(labelNbr);
#endif

                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    QVector3D(0.0f, 0.0f, zComp),
                                    QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                    0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, camera, true, true, Drawer::LabelMid,
                                    alignment);
            }
            labelNbr++;
            labelPos += posStep;
        }
    }
    // Y Labels
    if (m_axisCacheY.segmentCount() > 0) {
        GLfloat posStep = m_axisCacheY.segmentStep();
        GLfloat labelPos = m_axisCacheY.min();
        int labelNbr = 0;
        for (int segment = 0; segment <= m_axisCacheY.segmentCount(); segment++) {
            if (m_axisCacheY.labelItems().size() > labelNbr) {
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
                GLfloat labelXTrans = (aspectRatio * backgroundMargin * m_areaSize.width())
                                      / m_scaleFactor;
                GLfloat labelZTrans = (aspectRatio * backgroundMargin * m_areaSize.height())
                                      / m_scaleFactor;
#else // ..and this if we want uniform scaling based on largest dimension
                GLfloat labelXTrans = aspectRatio * backgroundMargin;
                GLfloat labelZTrans = labelXTrans;
#endif
                GLfloat labelYTrans = labelPos / m_heightNormalizer;
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

                const LabelItem &axisLabelItem = *m_axisCacheY.labelItems().at(labelNbr);

                // Back wall
                QVector3D labelTrans = QVector3D(labelXTrans, labelYTrans, labelZTrans + zComp);

                // Draw the label here
                m_dummyRenderItem.setTranslation(labelTrans);
                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    QVector3D(0.0f, 0.0f, zComp),
                                    QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                    0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, camera, true, true, Drawer::LabelMid,
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

                // Draw the label here
                m_dummyRenderItem.setTranslation(labelTrans);
                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    QVector3D(0.0f, 0.0f, zComp),
                                    QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                    0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, camera, true, true, Drawer::LabelMid,
                                    alignment);
            }
            labelNbr++;
            labelPos += posStep;
        }
    }

    // Handle selection clearing and selection label drawing
    if (!dotSelectionFound) {
        // We have no ownership, don't delete. Just NULL the pointer.
        m_selectedItem = NULL;
    } else {
        glDisable(GL_DEPTH_TEST);
        // Draw the selection label
        LabelItem &labelItem = selectedItem->selectionLabelItem();
        if (m_selectedItem != selectedItem || m_updateLabels
                || !labelItem.textureId()) {
            QString labelText = selectedItem->selectionLabel();
            if (labelText.isNull()) {
                static const QString xTitleTag(QStringLiteral("@xTitle"));
                static const QString yTitleTag(QStringLiteral("@yTitle"));
                static const QString zTitleTag(QStringLiteral("@zTitle"));
                static const QString xLabelTag(QStringLiteral("@xLabel"));
                static const QString yLabelTag(QStringLiteral("@yLabel"));
                static const QString zLabelTag(QStringLiteral("@zLabel"));

                labelText = itemLabelFormat();

                labelText.replace(xTitleTag, m_axisCacheX.title());
                labelText.replace(yTitleTag, m_axisCacheY.title());
                labelText.replace(zTitleTag, m_axisCacheZ.title());

                if (labelText.contains(xLabelTag)) {
                    QString valueLabelText = generateValueLabel(m_axisCacheX.labelFormat(),
                                                                selectedItem->position().x());
                    labelText.replace(xLabelTag, valueLabelText);
                }
                if (labelText.contains(yLabelTag)) {
                    QString valueLabelText = generateValueLabel(m_axisCacheY.labelFormat(),
                                                                selectedItem->position().y());
                    labelText.replace(yLabelTag, valueLabelText);
                }
                if (labelText.contains(zLabelTag)) {
                    QString valueLabelText = generateValueLabel(m_axisCacheZ.labelFormat(),
                                                                selectedItem->position().z());
                    labelText.replace(zLabelTag, valueLabelText);
                }

                selectedItem->setSelectionLabel(labelText);
            }
            m_drawer->generateLabelItem(labelItem, labelText);
            m_selectedItem = selectedItem;
        }

        m_drawer->drawLabel(*selectedItem, labelItem, viewMatrix, projectionMatrix,
                            QVector3D(0.0f, 0.0f, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), 0,
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, camera, true, false, Drawer::LabelMid);

        // Reset label update flag; they should have been updated when we get here
        m_updateLabels = false;
        glEnable(GL_DEPTH_TEST);
    }

    glDisable(GL_TEXTURE_2D);
    if (m_cachedLabelTransparency > QDataVis::TransparencyNone)
        glDisable(GL_BLEND);

    // Release label shader
    m_labelShader->release();
}

void Scatter3DRenderer::updateSelectedItemIndex(int index)
{
    if (index == Scatter3DController::noSelectionIndex())
        m_selection = selectionSkipColor;
    else
        m_selection = indexToSelectionColor(index);
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

void Scatter3DRenderer::updateBackgroundEnabled(bool enable)
{
    if (enable != m_cachedIsBackgroundEnabled) {
        Abstract3DRenderer::updateBackgroundEnabled(enable);
        loadMeshFile(); // Load changed dot type
    }
}

void Scatter3DRenderer::updateShadowQuality(QDataVis::ShadowQuality quality)
{
    qDebug() << __FUNCTION__ << quality;
    m_cachedShadowQuality = quality;
    switch (quality) {
    case QDataVis::ShadowLow:
        m_shadowQualityToShader = 33.3f;
        break;
    case QDataVis::ShadowMedium:
        m_shadowQualityToShader = 100.0f;
        break;
    case QDataVis::ShadowHigh:
        m_shadowQualityToShader = 200.0f;
        break;
    default:
        m_shadowQualityToShader = 0.0f;
        break;
    }

    handleShadowQualityChange();

#if !defined(QT_OPENGL_ES_2)
    // Re-init depth buffer
    updateDepthBuffer();
#endif
}

void Scatter3DRenderer::loadMeshFile()
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

void Scatter3DRenderer::updateAxisRange(QAbstractAxis::AxisOrientation orientation,
                                        qreal min, qreal max)
{
    Abstract3DRenderer::updateAxisRange(orientation, min, max);
}

void Scatter3DRenderer::calculateTranslation(ScatterRenderItem &item)
{
    //qDebug() << __FUNCTION__;

    // Origin should be in the center of scene, ie. both positive and negative values are drawn
    // above background

    // We need to normalize translations
    GLfloat xTrans = (aspectRatio * item.position().x()) / m_scaleFactor;
    GLfloat zTrans = -(aspectRatio * item.position().z()) / m_scaleFactor;
    GLfloat yTrans = item.position().y() / m_heightNormalizer;
    item.setTranslation(QVector3D(xTrans, yTrans, zTrans + zComp));
    //qDebug() << item.translation();
}

void Scatter3DRenderer::calculateSceneScalingFactors()
{
    m_heightNormalizer = (GLfloat)qMax(qAbs(m_axisCacheY.max()), qAbs(m_axisCacheY.min()));
    // TODO: Get rid of m_areaSize and use m_axisCaches directly?
    m_areaSize.setHeight(m_axisCacheZ.max());
    m_areaSize.setWidth(m_axisCacheX.max());
    m_scaleFactor = qMax(m_areaSize.width(), m_areaSize.height());
    //qDebug() << m_heightNormalizer << m_areaSize << m_scaleFactor << m_axisCacheY.max() << m_axisCacheX.max() << m_axisCacheZ.max();
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
    if (m_depthTexture) {
        m_textureHelper->deleteTexture(&m_depthTexture);
        m_depthTexture = 0;
    }

    if (m_cachedShadowQuality > QDataVis::ShadowNone) {
        m_depthTexture = m_textureHelper->createDepthTexture(m_mainViewPort.size(),
                                                             m_depthFrameBuffer,
                                                             m_cachedShadowQuality);
        if (!m_depthTexture) {
            switch (m_cachedShadowQuality) {
            case QDataVis::ShadowHigh:
                qWarning("Creating high quality shadows failed. Changing to medium quality.");
                (void)m_controller->setShadowQuality(QDataVis::ShadowMedium);
                updateShadowQuality(QDataVis::ShadowMedium);
                break;
            case QDataVis::ShadowMedium:
                qWarning("Creating medium quality shadows failed. Changing to low quality.");
                (void)m_controller->setShadowQuality(QDataVis::ShadowLow);
                updateShadowQuality(QDataVis::ShadowLow);
                break;
            case QDataVis::ShadowLow:
                qWarning("Creating low quality shadows failed. Switching shadows off.");
                (void)m_controller->setShadowQuality(QDataVis::ShadowNone);
                updateShadowQuality(QDataVis::ShadowNone);
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

QVector3D Scatter3DRenderer::indexToSelectionColor(GLint index)
{
    GLubyte dotIdxRed = index & 0xff;
    GLubyte dotIdxGreen = (index & 0xff00) >> 8;
    GLubyte dotIdxBlue = (index & 0xff0000) >> 16;

    return QVector3D(dotIdxRed, dotIdxGreen, dotIdxBlue);
}

QT_DATAVIS3D_END_NAMESPACE
