/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
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
#include "q3dcamera.h"
#include "q3dcamera_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"
#include "utils_p.h"
#include "q3dlight.h"
#include "qscatter3dseries_p.h"

#include <QtGui/QMatrix4x4>
#include <QtGui/QMouseEvent>
#include <QtCore/QThread>
#include <QtCore/qmath.h>

// Commenting this draws the shadow map with perspective projection. Otherwise it's drawn in
// orthographic projection.
//#define USE_WIDER_SHADOWS

// You can verify that depth buffer drawing works correctly by uncommenting this.
// You should see the scene from  where the light is
//#define SHOW_DEPTH_TEXTURE_SCENE

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

//#define USE_UNIFORM_SCALING // Scale x and z uniformly, or based on autoscaled values

const GLfloat aspectRatio = 2.0f; // Forced ratio of x and z to y. Dynamic will make it look odd.
const GLfloat labelMargin = 0.05f;
const GLfloat defaultMinSize = 0.01f;
const GLfloat defaultMaxSize = 0.1f;
const GLfloat defaultMargin = 1.0f + defaultMaxSize; // Default margin for background
const GLfloat itemScaler = 3.0f;
const GLfloat gridLineWidth = 0.005f;

Scatter3DRenderer::Scatter3DRenderer(Scatter3DController *controller)
    : Abstract3DRenderer(controller),
      m_selectedItem(0),
      m_xFlipped(false),
      m_zFlipped(false),
      m_yFlipped(false),
      m_updateLabels(false),
      m_dotShader(0),
      m_dotGradientShader(0),
      #if defined(QT_OPENGL_ES_2)
      m_pointShader(0),
      #endif
      m_depthShader(0),
      m_selectionShader(0),
      m_backgroundShader(0),
      m_labelShader(0),
      m_backgroundObj(0),
      m_gridLineObj(0),
      m_labelObj(0),
      m_bgrTexture(0),
      m_depthTexture(0),
      m_selectionTexture(0),
      m_depthFrameBuffer(0),
      m_selectionFrameBuffer(0),
      m_selectionDepthBuffer(0),
      m_shadowQualityToShader(100.0f),
      m_shadowQualityMultiplier(3),
      m_heightNormalizer(1.0f),
      m_scaleFactor(0),
      m_selectedItemIndex(Scatter3DController::invalidSelectionIndex()),
      m_selectedItemTotalIndex(Scatter3DController::invalidSelectionIndex()),
      m_selectedItemSeriesIndex(Scatter3DController::invalidSelectionIndex()),
      m_selectedSeries(0),
      m_areaSize(QSizeF(0.0, 0.0)),
      m_dotSizeScale(1.0f),
      m_hasHeightAdjustmentChanged(true),
      m_backgroundMargin(defaultMargin),
      m_maxItemSize(0.0f),
      m_clickedIndex(Scatter3DController::invalidSelectionIndex())
{
    initializeOpenGLFunctions();
    initializeOpenGL();
}

Scatter3DRenderer::~Scatter3DRenderer()
{
    if (QOpenGLContext::currentContext()) {
        m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);
        m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
        m_textureHelper->deleteTexture(&m_selectionTexture);
        m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
        m_textureHelper->deleteTexture(&m_bgrTexture);
    }
    delete m_dotShader;
    delete m_dotGradientShader;
    delete m_depthShader;
    delete m_selectionShader;
    delete m_backgroundShader;
    delete m_labelShader;
    delete m_backgroundObj;
    delete m_gridLineObj;
    delete m_labelObj;
}

void Scatter3DRenderer::initializeOpenGL()
{
    Abstract3DRenderer::initializeOpenGL();

    // Initialize shaders
    initLabelShaders(QStringLiteral(":/shaders/vertexLabel"),
                     QStringLiteral(":/shaders/fragmentLabel"));

#if !defined(QT_OPENGL_ES_2)
    // Init depth shader (for shadows). Init in any case, easier to handle shadow activation if done via api.
    initDepthShader();
#else
    // Init point shader
    initPointShader();
#endif

    // Init selection shader
    initSelectionShader();

    // Load grid line mesh
    loadGridLineMesh();

    // Load label mesh
    loadLabelMesh();

    // Set view port
    glViewport(m_primarySubViewport.x(),
               m_primarySubViewport.y(),
               m_primarySubViewport.width(),
               m_primarySubViewport.height());

    // Load background mesh (we need to be initialized first)
    loadBackgroundMesh();
}

void Scatter3DRenderer::updateSeries(const QList<QAbstract3DSeries *> &seriesList,
                                     bool updateVisibility)
{
    Abstract3DRenderer::updateSeries(seriesList, updateVisibility);

    int seriesCount = m_visibleSeriesList.size();
    float maxItemSize = 0.0f;
    float itemSize = 0.0f;

    if (m_cachedItemSize.size() != seriesCount)
        m_cachedItemSize.resize(seriesCount);

    for (int series = 0; series < seriesCount; series++) {
        itemSize = static_cast<QScatter3DSeries *>(m_visibleSeriesList.at(series).series())->itemSize();
        if (maxItemSize < itemSize)
            maxItemSize = itemSize;
        if (m_cachedItemSize.at(series) != itemSize)
            m_cachedItemSize[series] = itemSize;
    }
    m_backgroundMargin = defaultMargin;
    m_maxItemSize = maxItemSize;
    if (maxItemSize > defaultMaxSize)
        m_backgroundMargin += maxItemSize / itemScaler;
}

void Scatter3DRenderer::updateData()
{
    int seriesCount = m_visibleSeriesList.size();
    calculateSceneScalingFactors();
    float minX = float(m_axisCacheX.min());
    float maxX = float(m_axisCacheX.max());
    float minY = float(m_axisCacheY.min());
    float maxY = float(m_axisCacheY.max());
    float minZ = float(m_axisCacheZ.min());
    float maxZ = float(m_axisCacheZ.max());
    int totalDataSize = 0;

    if (m_renderingArrays.size() != seriesCount)
        m_renderingArrays.resize(seriesCount);

    for (int series = 0; series < seriesCount; series++) {
        QScatterDataProxy *dataProxy =
                static_cast<QScatter3DSeries *>(m_visibleSeriesList.at(series).series())->dataProxy();
        const QScatterDataArray &dataArray = *dataProxy->array();
        int dataSize = dataArray.size();
        totalDataSize += dataSize;

        if (dataSize != m_renderingArrays.at(series).size())
            m_renderingArrays[series].resize(dataSize);

        for (int i = 0; i < dataSize; i++) {
            QVector3D dotPos = dataArray.at(i).position();
            ScatterRenderItem &renderItem = m_renderingArrays[series][i];
            if ((dotPos.x() >= minX && dotPos.x() <= maxX )
                    && (dotPos.y() >= minY && dotPos.y() <= maxY)
                    && (dotPos.z() >= minZ && dotPos.z() <= maxZ)) {
                renderItem.setPosition(dotPos);
                renderItem.setVisible(true);
                if (!dataArray.at(i).rotation().isIdentity())
                    renderItem.setRotation(dataArray.at(i).rotation().normalized());
                else
                    renderItem.setRotation(identityQuaternion);
                calculateTranslation(renderItem);
            } else {
                renderItem.setVisible(false);
            }
        }
    }
    m_dotSizeScale = GLfloat(qBound(defaultMinSize, 2.0f / float(qSqrt(qreal(totalDataSize))),
                                    defaultMaxSize));

    updateSelectedItem(m_selectedItemIndex, m_selectedSeries);
}

void Scatter3DRenderer::updateScene(Q3DScene *scene)
{
    scene->activeCamera()->d_ptr->setMinYRotation(-90.0f);

    if (m_hasHeightAdjustmentChanged) {
        // Set initial camera position. Also update if height adjustment has changed.
        scene->activeCamera()->d_ptr->setBaseOrientation(cameraDistanceVector, zeroVector, upVector);
        m_hasHeightAdjustmentChanged = false;
    }

    Abstract3DRenderer::updateScene(scene);
}

void Scatter3DRenderer::resetClickedStatus()
{
    m_clickedIndex = Scatter3DController::invalidSelectionIndex();
    m_clickedSeries = 0;
}

void Scatter3DRenderer::render(GLuint defaultFboHandle)
{
    // Handle GL state setup for FBO buffers and clearing of the render surface
    Abstract3DRenderer::render(defaultFboHandle);

    // Draw dots scene
    drawScene(defaultFboHandle);
}

void Scatter3DRenderer::drawScene(const GLuint defaultFboHandle)
{
    GLfloat backgroundRotation = 0;
    GLfloat selectedItemSize = 0.0f;

    const Q3DCamera *activeCamera = m_cachedScene->activeCamera();

    QVector3D lightColor = Utils::vectorFromColor(m_cachedTheme->lightColor());

    // Specify viewport
    glViewport(m_primarySubViewport.x(),
               m_primarySubViewport.y(),
               m_primarySubViewport.width(),
               m_primarySubViewport.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    GLfloat viewPortRatio = (GLfloat)m_primarySubViewport.width()
            / (GLfloat)m_primarySubViewport.height();
    projectionMatrix.perspective(45.0f, viewPortRatio, 0.1f, 100.0f);

    // Calculate view matrix
    QMatrix4x4 viewMatrix = activeCamera->d_ptr->viewMatrix();
    QMatrix4x4 projectionViewMatrix = projectionMatrix * viewMatrix;

    int seriesCount = m_visibleSeriesList.size();

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

    // Get light position from the scene
    QVector3D lightPos = m_cachedScene->activeLight()->position();

    // Introduce regardless of shadow quality to simplify logic
    QMatrix4x4 depthViewMatrix;
    QMatrix4x4 depthProjectionMatrix;
    QMatrix4x4 depthProjectionViewMatrix;

    // Check if we have any series with points
    bool havePointSeries = false;
    bool haveMeshSeries = false;
    bool haveUniformColorMeshSeries = false;
    bool haveGradientMeshSeries = false;
    for (int i = 0; i < seriesCount; i++) {
        if (m_visibleSeriesList.at(i).mesh() == QAbstract3DSeries::MeshPoint) {
            havePointSeries = true;
        } else {
            haveMeshSeries = true;
            if (m_visibleSeriesList.at(i).colorStyle() == Q3DTheme::ColorStyleUniform)
                haveUniformColorMeshSeries = true;
            else
                haveGradientMeshSeries = true;
        }
    }

#if !defined(QT_OPENGL_ES_2)
    if (havePointSeries) {
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_PROGRAM_POINT_SIZE);
    }

    if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
        // Render scene into a depth texture for using with shadow mapping
        // Bind depth shader
        m_depthShader->bind();

        // Set viewport for depth map rendering. Must match texture size. Larger values give smoother shadows.
        glViewport(0, 0,
                   m_primarySubViewport.width() * m_shadowQualityMultiplier,
                   m_primarySubViewport.height() * m_shadowQualityMultiplier);

        // Enable drawing to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthFrameBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Set front face culling to reduce self-shadowing issues
        glCullFace(GL_FRONT);

        // Get the depth view matrix
        // It may be possible to hack lightPos here if we want to make some tweaks to shadow
        QVector3D depthLightPos = activeCamera->d_ptr->calculatePositionRelativeToCamera(
                    zeroVector, 0.0f, 2.5f / m_autoScaleAdjustment);
        depthViewMatrix.lookAt(depthLightPos, zeroVector, upVector);
        // Set the depth projection matrix
#ifndef USE_WIDER_SHADOWS
        // Use this for perspective shadows
        depthProjectionMatrix.perspective(15.0f, viewPortRatio, 3.0f, 100.0f);
#else
        // Use these for orthographic shadows
        GLfloat testAspectRatio = viewPortRatio;
        depthProjectionMatrix.ortho(-testAspectRatio * 2.0f, testAspectRatio * 2.0f,
                                    -m_heightNormalizer * 2.0f, m_heightNormalizer * 2.0f,
                                    0.0f, 100.0f);
#endif
        depthProjectionViewMatrix = depthProjectionMatrix * depthViewMatrix;

        // Draw dots to depth buffer
        for (int series = 0; series < seriesCount; series++) {
            ObjectHelper *dotObj = m_visibleSeriesList.at(series).object();
            QQuaternion seriesRotation = m_visibleSeriesList.at(series).meshRotation();
            bool drawingPoints = (m_visibleSeriesList.at(series).mesh() == QAbstract3DSeries::MeshPoint);

            float itemSize = m_cachedItemSize.at(series) / itemScaler;
            if (itemSize == 0.0f)
                itemSize = m_dotSizeScale;
            if (drawingPoints) {
                // Scale points based on shadow quality for shadows, not by zoom level
                glPointSize(itemSize * 100.0f * m_shadowQualityMultiplier);
            }
            QVector3D modelScaler(itemSize, itemSize, itemSize);

            for (int dot = 0; dot < m_renderingArrays.at(series).size(); dot++) {
                const ScatterRenderItem &item = m_renderingArrays.at(series).at(dot);
                if (!item.isVisible())
                    continue;

                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;

                modelMatrix.translate(item.translation());
                if (!drawingPoints) {
                    if (!seriesRotation.isIdentity() || !item.rotation().isIdentity())
                        modelMatrix.rotate(seriesRotation * item.rotation());
                    modelMatrix.scale(modelScaler);
                }

                MVPMatrix = depthProjectionViewMatrix * modelMatrix;

                m_depthShader->setUniformValue(m_depthShader->MVP(), MVPMatrix);

                if (drawingPoints) {
                    m_drawer->drawPoint(m_depthShader);
                } else {
                    // 1st attribute buffer : vertices
                    glEnableVertexAttribArray(m_depthShader->posAtt());
                    glBindBuffer(GL_ARRAY_BUFFER, dotObj->vertexBuf());
                    glVertexAttribPointer(m_depthShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0,
                                          (void *)0);

                    // Index buffer
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dotObj->elementBuf());

                    // Draw the triangles
                    glDrawElements(GL_TRIANGLES, dotObj->indexCount(), GL_UNSIGNED_SHORT,
                                   (void *)0);

                    // Free buffers
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);

                    glDisableVertexAttribArray(m_depthShader->posAtt());
                }
            }
        }

        // Disable drawing to framebuffer (= enable drawing to screen)
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);

        // Reset culling to normal
        glCullFace(GL_BACK);

        // Revert to original viewport
        glViewport(m_primarySubViewport.x(),
                   m_primarySubViewport.y(),
                   m_primarySubViewport.width(),
                   m_primarySubViewport.height());
    }

    ShaderHelper *pointSelectionShader = m_selectionShader;
#else
    Q_UNUSED(havePointSeries);
    ShaderHelper *pointSelectionShader = m_pointShader;
#endif
    ShaderHelper *selectionShader = m_selectionShader;

    // Skip selection mode drawing if we have no selection mode
    if (m_cachedSelectionMode > QAbstract3DGraph::SelectionNone
            && SelectOnScene == m_selectionState && seriesCount > 0) {
        // Draw dots to selection buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
        glViewport(0, 0,
                   m_primarySubViewport.width(),
                   m_primarySubViewport.height());

        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (= skipColor)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled

        int arraySize = 0;
        int totalArraySize = 0;
        int dotNo = 0;

        // Init previous to opposite of first to ensure we change binding for first series
        bool previousDrawingPoints = (m_visibleSeriesList.at(0).mesh() != QAbstract3DSeries::MeshPoint);
        for (int series = 0; series < seriesCount; series++) {
            ObjectHelper *dotObj = m_visibleSeriesList.at(series).object();
            QQuaternion seriesRotation = m_visibleSeriesList.at(series).meshRotation();
            bool drawingPoints = (m_visibleSeriesList.at(series).mesh() == QAbstract3DSeries::MeshPoint);

            float itemSize = m_cachedItemSize.at(series) / itemScaler;
            if (itemSize == 0.0f)
                itemSize = m_dotSizeScale;
#if !defined(QT_OPENGL_ES_2)
            if (drawingPoints)
                glPointSize(itemSize * activeCamera->zoomLevel()); // Scale points based on zoom
#endif
            QVector3D modelScaler(itemSize, itemSize, itemSize);

            // Rebind selection shader if it has changed
            if (drawingPoints != previousDrawingPoints) {
                previousDrawingPoints = drawingPoints;
                if (drawingPoints)
                    selectionShader = pointSelectionShader;
                else
                    selectionShader = m_selectionShader;

                selectionShader->bind();
            }
            arraySize = m_renderingArrays.at(series).size();
            totalArraySize += arraySize;

            if (totalArraySize > 0xfffffe) // Max possible different selection colors, 0xffffff being skipColor
                qFatal("Too many objects");

            for (int dot = 0; dot < arraySize; dot++) {
                const ScatterRenderItem &item = m_renderingArrays.at(series).at(dot);
                if (!item.isVisible())
                    continue;

                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;

                modelMatrix.translate(item.translation());
                if (!drawingPoints) {
                    if (!seriesRotation.isIdentity() || !item.rotation().isIdentity())
                        modelMatrix.rotate(seriesRotation * item.rotation());
                    modelMatrix.scale(modelScaler);
                }

                MVPMatrix = projectionViewMatrix * modelMatrix;

                QVector3D dotColor = indexToSelectionColor(dotNo);
                dotColor /= 255.0f;

                selectionShader->setUniformValue(selectionShader->MVP(), MVPMatrix);
                selectionShader->setUniformValue(selectionShader->color(), dotColor);

                if (drawingPoints) {
                    m_drawer->drawPoint(selectionShader);
                } else {
                    // 1st attribute buffer : vertices
                    glEnableVertexAttribArray(selectionShader->posAtt());
                    glBindBuffer(GL_ARRAY_BUFFER, dotObj->vertexBuf());
                    glVertexAttribPointer(selectionShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0,
                                          (void *)0);

                    // Index buffer
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dotObj->elementBuf());

                    // Draw the triangles
                    glDrawElements(GL_TRIANGLES, dotObj->indexCount(), GL_UNSIGNED_SHORT,
                                   (void *)0);

                    // Free buffers
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);

                    glDisableVertexAttribArray(selectionShader->posAtt());
                }
                dotNo++;
            }
        }
        glEnable(GL_DITHER);

        // Read color under cursor
        QVector3D clickedColor = Utils::getSelection(m_inputPosition,
                                                     m_viewport.height());
        selectionColorToSeriesAndIndex(clickedColor, m_clickedIndex, m_clickedSeries);

        emit needRender();

        // Revert to original fbo and viewport
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);
        glViewport(m_primarySubViewport.x(),
                   m_primarySubViewport.y(),
                   m_primarySubViewport.width(),
                   m_primarySubViewport.height());
    }

    // Draw dots
    ShaderHelper *dotShader = 0;
    GLuint gradientTexture = 0;
    bool dotSelectionFound = false;
    ScatterRenderItem *selectedItem(0);
    int dotNo = 0;
    QVector3D baseColor;
    QVector3D dotColor;

    bool previousDrawingPoints = false;
    Q3DTheme::ColorStyle previousMeshColorStyle = Q3DTheme::ColorStyleUniform;
    if (haveMeshSeries) {
        // Set unchanging shader bindings
        if (haveGradientMeshSeries) {
            m_dotGradientShader->bind();
            m_dotGradientShader->setUniformValue(m_dotGradientShader->lightP(), lightPos);
            m_dotGradientShader->setUniformValue(m_dotGradientShader->view(), viewMatrix);
            m_dotGradientShader->setUniformValue(m_dotGradientShader->ambientS(),
                                                 m_cachedTheme->ambientLightStrength());
            m_dotGradientShader->setUniformValue(m_dotGradientShader->lightColor(), lightColor);
        }
        if (haveUniformColorMeshSeries) {
            m_dotShader->bind();
            m_dotShader->setUniformValue(m_dotShader->lightP(), lightPos);
            m_dotShader->setUniformValue(m_dotShader->view(), viewMatrix);
            m_dotShader->setUniformValue(m_dotShader->ambientS(),
                                         m_cachedTheme->ambientLightStrength());
            m_dotShader->setUniformValue(m_dotShader->lightColor(), lightColor);
            dotShader = m_dotShader;
        } else {
            dotShader = m_dotGradientShader;
            previousMeshColorStyle = Q3DTheme::ColorStyleRangeGradient;
            m_dotGradientShader->setUniformValue(m_dotGradientShader->gradientHeight(), 0.0f);
        }
        glEnable(GL_TEXTURE_2D);
    } else {
        dotShader = pointSelectionShader;
        previousDrawingPoints = true;
        dotShader->bind();
    }

    for (int series = 0; series < seriesCount; series++) {
        const SeriesRenderCache &currentSeries = m_visibleSeriesList.at(series);
        QQuaternion seriesRotation = currentSeries.meshRotation();
        ObjectHelper *dotObj = currentSeries.object();
        bool drawingPoints = (currentSeries.mesh() == QAbstract3DSeries::MeshPoint);
        Q3DTheme::ColorStyle colorStyle = currentSeries.colorStyle();
        bool colorStyleIsUniform = (colorStyle == Q3DTheme::ColorStyleUniform);
        bool useColor = colorStyleIsUniform || drawingPoints;

        float itemSize = m_cachedItemSize.at(series) / itemScaler;
        if (itemSize == 0.0f)
            itemSize = m_dotSizeScale;
#if !defined(QT_OPENGL_ES_2)
        if (drawingPoints)
            glPointSize(itemSize * activeCamera->zoomLevel()); // Scale points based on zoom
#endif
        QVector3D modelScaler(itemSize, itemSize, itemSize);

        // Rebind shader if it has changed
        if (drawingPoints != previousDrawingPoints
                || (!drawingPoints &&
                    (colorStyleIsUniform != (previousMeshColorStyle == Q3DTheme::ColorStyleUniform)))) {
            previousDrawingPoints = drawingPoints;
            if (drawingPoints) {
                dotShader = pointSelectionShader;
            } else {
                if (colorStyleIsUniform)
                    dotShader = m_dotShader;
                else
                    dotShader = m_dotGradientShader;
            }
            dotShader->bind();
        }

        if (!drawingPoints && !colorStyleIsUniform && previousMeshColorStyle != colorStyle) {
            if (colorStyle == Q3DTheme::ColorStyleObjectGradient) {
                m_dotGradientShader->setUniformValue(m_dotGradientShader->gradientMin(), 0.0f);
                m_dotGradientShader->setUniformValue(m_dotGradientShader->gradientHeight(), 0.5f);
            } else {
                // Each ball is of uniform color according to its Y-coordinate
                m_dotGradientShader->setUniformValue(m_dotGradientShader->gradientHeight(), 0.0f);
            }
        }

        if (!drawingPoints)
            previousMeshColorStyle = colorStyle;

        if (useColor) {
            baseColor = currentSeries.baseColor();
            dotColor = baseColor;
        }

        int seriesSize = m_renderingArrays.at(series).size();
        for (int dot = 0; dot < seriesSize; dot++) {
            ScatterRenderItem &item = m_renderingArrays[series][dot];
            if (!item.isVisible())
                continue;

            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 itModelMatrix;

            modelMatrix.translate(item.translation());
            if (!drawingPoints) {
                if (!seriesRotation.isIdentity() || !item.rotation().isIdentity()) {
                    QQuaternion totalRotation = seriesRotation * item.rotation();
                    modelMatrix.rotate(totalRotation);
                    itModelMatrix.rotate(totalRotation);
                }
                modelMatrix.scale(modelScaler);
                itModelMatrix.scale(modelScaler);
            }
#ifdef SHOW_DEPTH_TEXTURE_SCENE
            MVPMatrix = depthProjectionViewMatrix * modelMatrix;
#else
            MVPMatrix = projectionViewMatrix * modelMatrix;
#endif

            if (useColor)
                dotColor = baseColor;
            else
                gradientTexture = currentSeries.baseGradientTexture();

            GLfloat lightStrength = m_cachedTheme->lightStrength();
            if (m_cachedSelectionMode > QAbstract3DGraph::SelectionNone
                    && (m_selectedItemTotalIndex == dotNo)) {
                if (useColor)
                    dotColor = currentSeries.singleHighlightColor();
                else
                    gradientTexture = currentSeries.singleHighlightGradientTexture();
                lightStrength = m_cachedTheme->highlightLightStrength();
                // Insert data to ScatterRenderItem. We have no ownership, don't delete the previous one
                selectedItem = &item;
                dotSelectionFound = true;
                // Save selected item size (adjusted with font size) for selection label positioning
                selectedItemSize = itemSize + (m_cachedTheme->font().pointSizeF() / 500.0f);
            }

            if (!drawingPoints) {
                // Set shader bindings
                dotShader->setUniformValue(dotShader->model(), modelMatrix);
                dotShader->setUniformValue(dotShader->nModel(),
                                           itModelMatrix.inverted().transposed());
            }

            dotShader->setUniformValue(dotShader->MVP(), MVPMatrix);
            if (useColor) {
                dotShader->setUniformValue(dotShader->color(), dotColor);
            } else if (colorStyle == Q3DTheme::ColorStyleRangeGradient) {
                dotShader->setUniformValue(dotShader->gradientMin(),
                                           (item.translation().y() + 1.0f) / 2.0f);
            }
#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                if (!drawingPoints) {
                    // Set shadow shader bindings
                    QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                    dotShader->setUniformValue(dotShader->shadowQ(), m_shadowQualityToShader);
                    dotShader->setUniformValue(dotShader->depth(), depthMVPMatrix);
                    dotShader->setUniformValue(dotShader->lightS(), lightStrength / 10.0f);

                    // Draw the object
                    m_drawer->drawObject(dotShader, dotObj, gradientTexture, m_depthTexture);
                } else {
                    // Draw the object
                    m_drawer->drawPoint(dotShader);
                }
            } else
#endif
            {
                if (!drawingPoints) {
                    // Set shadowless shader bindings
                    dotShader->setUniformValue(dotShader->lightS(), lightStrength);
                    // Draw the object
                    m_drawer->drawObject(dotShader, dotObj, gradientTexture);
                } else {
                    // Draw the object
                    m_drawer->drawPoint(dotShader);
                }
            }
            dotNo++;
        }
    }

#if !defined(QT_OPENGL_ES_2)
    if (havePointSeries) {
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_PROGRAM_POINT_SIZE);
    }
#endif

    // Bind background shader
    m_backgroundShader->bind();

    glCullFace(GL_BACK);

    // Draw background
    if (m_cachedTheme->isBackgroundEnabled() && m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 itModelMatrix;

#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
        GLfloat xScale = (aspectRatio * m_backgroundMargin * m_areaSize.width()) / m_scaleFactor;
        GLfloat zScale = (aspectRatio * m_backgroundMargin * m_areaSize.height()) / m_scaleFactor;
        if (m_maxItemSize > xScale)
            xScale = m_maxItemSize;
        if (m_maxItemSize > zScale)
            zScale = m_maxItemSize;
        QVector3D bgScale(xScale, m_backgroundMargin, zScale);
#else // ..and this if we want uniform scaling based on largest dimension
        QVector3D bgScale((aspectRatio * m_backgroundMargin),
                          m_backgroundMargin,
                          (aspectRatio * m_backgroundMargin));
#endif
        modelMatrix.scale(bgScale);
        // If we're viewing from below, background object must be flipped
        if (m_yFlipped) {
            modelMatrix.rotate(180.0f, 1.0, 0.0, 0.0);
            modelMatrix.rotate(270.0f - backgroundRotation, 0.0f, 1.0f, 0.0f);
        } else {
            modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);
        }
        itModelMatrix = modelMatrix; // Only scaling and rotations, can be used directly

#ifdef SHOW_DEPTH_TEXTURE_SCENE
        MVPMatrix = depthProjectionViewMatrix * modelMatrix;
#else
        MVPMatrix = projectionViewMatrix * modelMatrix;
#endif
        QVector3D backgroundColor = Utils::vectorFromColor(m_cachedTheme->backgroundColor());

        // Set shader bindings
        m_backgroundShader->setUniformValue(m_backgroundShader->lightP(), lightPos);
        m_backgroundShader->setUniformValue(m_backgroundShader->view(), viewMatrix);
        m_backgroundShader->setUniformValue(m_backgroundShader->model(), modelMatrix);
        m_backgroundShader->setUniformValue(m_backgroundShader->nModel(),
                                            itModelMatrix.inverted().transposed());
        m_backgroundShader->setUniformValue(m_backgroundShader->MVP(), MVPMatrix);
        m_backgroundShader->setUniformValue(m_backgroundShader->color(), backgroundColor);
        m_backgroundShader->setUniformValue(m_backgroundShader->ambientS(),
                                            m_cachedTheme->ambientLightStrength() * 2.0f);
        m_backgroundShader->setUniformValue(m_backgroundShader->lightColor(), lightColor);

#if !defined(QT_OPENGL_ES_2)
        if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
            // Set shadow shader bindings
            QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
            m_backgroundShader->setUniformValue(m_backgroundShader->shadowQ(),
                                                m_shadowQualityToShader);
            m_backgroundShader->setUniformValue(m_backgroundShader->depth(), depthMVPMatrix);
            m_backgroundShader->setUniformValue(m_backgroundShader->lightS(),
                                                m_cachedTheme->lightStrength() / 10.0f);

            // Draw the object
            m_drawer->drawObject(m_backgroundShader, m_backgroundObj, 0, m_depthTexture);
        } else
#endif
        {
            // Set shadowless shader bindings
            m_backgroundShader->setUniformValue(m_backgroundShader->lightS(),
                                                m_cachedTheme->lightStrength());

            // Draw the object
            m_drawer->drawObject(m_backgroundShader, m_backgroundObj);
        }
    }

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

    if (m_cachedTheme->isGridEnabled() && m_heightNormalizer) {
        ShaderHelper *lineShader = m_backgroundShader;

        // Bind line shader
        lineShader->bind();

        // Set unchanging shader bindings
        QVector3D lineColor = Utils::vectorFromColor(m_cachedTheme->gridLineColor());
        lineShader->setUniformValue(lineShader->lightP(), lightPos);
        lineShader->setUniformValue(lineShader->view(), viewMatrix);
        lineShader->setUniformValue(lineShader->color(), lineColor);
        lineShader->setUniformValue(lineShader->ambientS(), m_cachedTheme->ambientLightStrength());
        lineShader->setUniformValue(lineShader->lightColor(), lightColor);
#if !defined(QT_OPENGL_ES_2)
        if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
            // Set shadowed shader bindings
            lineShader->setUniformValue(lineShader->shadowQ(), m_shadowQualityToShader);
            lineShader->setUniformValue(lineShader->lightS(),
                                        m_cachedTheme->lightStrength() / 20.0f);
        } else
#endif
        {
            // Set shadowless shader bindings
            lineShader->setUniformValue(lineShader->lightS(),
                                        m_cachedTheme->lightStrength() / 2.5f);
        }

        QQuaternion lineYRotation;
        QQuaternion lineXRotation;

        if (m_xFlipped)
            lineYRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, -90.0f);
        else
            lineYRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 90.0f);

        if (m_yFlipped)
            lineXRotation = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f);
        else
            lineXRotation = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -90.0f);

        GLfloat yFloorLinePosition = -m_backgroundMargin + gridLineOffset;
        if (m_yFlipped)
            yFloorLinePosition = -yFloorLinePosition;

        // Rows (= Z)
        if (m_axisCacheZ.segmentCount() > 0) {
            // Floor lines
#ifndef USE_UNIFORM_SCALING
            GLfloat lineStep = aspectRatio * m_axisCacheZ.subSegmentStep();
            GLfloat linePos = -aspectRatio * (m_axisCacheZ.min() - m_translationOffset.z()); // Start line
            int lastSegment = m_axisCacheZ.subSegmentCount() * m_axisCacheZ.segmentCount();
#else
            GLfloat lineStep = aspectRatio * axisCacheMax->subSegmentStep();
            GLfloat linePos = -aspectRatio * m_scaleFactor; // Start line
            int lastSegment = axisCacheMax->subSegmentCount() * axisCacheMax->segmentCount();
#endif

#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
            GLfloat xScale = (aspectRatio * m_backgroundMargin * m_areaSize.width()) / m_scaleFactor;
            if (m_maxItemSize > xScale)
                xScale = m_maxItemSize;
            QVector3D gridLineScaler(xScale, gridLineWidth, gridLineWidth);
#else // ..and this if we want uniform scaling based on largest dimension
            QVector3D gridLineScaler((aspectRatio * m_backgroundMargin),
                                     gridLineWidth, gridLineWidth);
#endif

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(0.0f, yFloorLinePosition, linePos / m_scaleFactor);

                modelMatrix.scale(gridLineScaler);
                itModelMatrix.scale(gridLineScaler);

                modelMatrix.rotate(lineXRotation);
                itModelMatrix.rotate(lineXRotation);

                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                    QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                    // Set shadow shader bindings
                    lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
                } else
#endif
                {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
                linePos -= lineStep;
            }

            // Side wall lines
            gridLineScaler = QVector3D(gridLineWidth, m_backgroundMargin, gridLineWidth);
#ifndef USE_UNIFORM_SCALING
            GLfloat lineXTrans = (aspectRatio * m_backgroundMargin * m_areaSize.width())
                    / m_scaleFactor - gridLineOffset;
            if (m_maxItemSize > lineXTrans)
                lineXTrans = m_maxItemSize - gridLineOffset;
            linePos = -aspectRatio * (m_axisCacheZ.min() - m_translationOffset.z()); // Start line
#else
            GLfloat lineXTrans = aspectRatio * m_backgroundMargin - gridLineOffset;
            linePos = -aspectRatio * m_scaleFactor; // Start line
#endif
            if (!m_xFlipped)
                lineXTrans = -lineXTrans;

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(lineXTrans, 0.0f, linePos / m_scaleFactor);

                modelMatrix.scale(gridLineScaler);
                itModelMatrix.scale(gridLineScaler);

                modelMatrix.rotate(lineYRotation);
                itModelMatrix.rotate(lineYRotation);

                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                    // Set shadow shader bindings
                    QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                    lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
                } else
#endif
                {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
                linePos -= lineStep;
            }
        }

        // Columns (= X)
        if (m_axisCacheX.segmentCount() > 0) {
            // Floor lines
#ifndef USE_UNIFORM_SCALING
            GLfloat lineStep = aspectRatio * m_axisCacheX.subSegmentStep();
            GLfloat linePos = aspectRatio * (m_axisCacheX.min() - m_translationOffset.x());
            int lastSegment = m_axisCacheX.subSegmentCount() * m_axisCacheX.segmentCount();
            GLfloat zScale = (aspectRatio * m_backgroundMargin * m_areaSize.height()) / m_scaleFactor;
            if (m_maxItemSize > zScale)
                zScale = m_maxItemSize;
            QVector3D gridLineScaler(gridLineWidth, gridLineWidth, zScale);
#else
            GLfloat lineStep = aspectRatio * axisCacheMax->subSegmentStep();
            GLfloat linePos = -aspectRatio * m_scaleFactor;
            int lastSegment = axisCacheMax->subSegmentCount() * axisCacheMax->segmentCount();
            QVector3D gridLineScaler(gridLineWidth, gridLineWidth,
                                     aspectRatio * m_backgroundMargin);
#endif

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(linePos / m_scaleFactor, yFloorLinePosition, 0.0f);

                modelMatrix.scale(gridLineScaler);
                itModelMatrix.scale(gridLineScaler);

                modelMatrix.rotate(lineXRotation);
                itModelMatrix.rotate(lineXRotation);

                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                    // Set shadow shader bindings
                    QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                    lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
                } else
#endif
                {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
                linePos += lineStep;
            }

            // Back wall lines
#ifndef USE_UNIFORM_SCALING
            GLfloat lineZTrans = (aspectRatio * m_backgroundMargin * m_areaSize.height())
                    / m_scaleFactor - gridLineOffset;
            if (m_maxItemSize > lineZTrans)
                lineZTrans = m_maxItemSize - gridLineOffset;
            linePos = aspectRatio * (m_axisCacheX.min() - m_translationOffset.x());
#else
            GLfloat lineZTrans = aspectRatio * m_backgroundMargin - gridLineOffset;
            linePos = -aspectRatio * m_scaleFactor;
#endif
            if (!m_zFlipped)
                lineZTrans = -lineZTrans;

            gridLineScaler = QVector3D(gridLineWidth, m_backgroundMargin, gridLineWidth);

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(linePos / m_scaleFactor, 0.0f, lineZTrans);

                modelMatrix.scale(gridLineScaler);
                itModelMatrix.scale(gridLineScaler);

                if (m_zFlipped) {
                    modelMatrix.rotate(180.0f, 1.0f, 0.0f, 0.0f);
                    itModelMatrix.rotate(180.0f, 1.0f, 0.0f, 0.0f);
                }

                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                    // Set shadow shader bindings
                    QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                    lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
                } else
#endif
                {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
                linePos += lineStep;
            }
        }

        // Horizontal wall lines
        if (m_axisCacheY.segmentCount() > 0) {
            // Back wall
            GLfloat lineStep = m_axisCacheY.subSegmentStep();
            GLfloat linePos = m_axisCacheY.min() - m_translationOffset.y();
            int lastSegment = m_axisCacheY.subSegmentCount() * m_axisCacheY.segmentCount();

#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
            GLfloat lineZTrans = (aspectRatio * m_backgroundMargin * m_areaSize.height())
                    / m_scaleFactor - gridLineOffset;
            if (m_maxItemSize > lineZTrans)
                lineZTrans = m_maxItemSize - gridLineOffset;
            GLfloat xScale = (aspectRatio * m_backgroundMargin * m_areaSize.width()) / m_scaleFactor;
            if (m_maxItemSize > xScale)
                xScale = m_maxItemSize;
            QVector3D gridLineScaler(xScale, gridLineWidth, gridLineWidth);
#else // ..and this if we want uniform scaling based on largest dimension
            GLfloat lineZTrans = aspectRatio * m_backgroundMargin - gridLineOffset;
            QVector3D gridLineScaler((aspectRatio * m_backgroundMargin),
                                     gridLineWidth, gridLineWidth);
#endif
            if (!m_zFlipped)
                lineZTrans = -lineZTrans;

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(0.0f, linePos / m_heightNormalizer, lineZTrans);

                modelMatrix.scale(gridLineScaler);
                itModelMatrix.scale(gridLineScaler);

                if (m_zFlipped) {
                    modelMatrix.rotate(180.0f, 1.0f, 0.0f, 0.0f);
                    itModelMatrix.rotate(180.0f, 1.0f, 0.0f, 0.0f);
                }

                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                    // Set shadow shader bindings
                    QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                    lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
                } else
#endif
                {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
                linePos += lineStep;
            }

            // Side wall
            linePos = m_axisCacheY.min() - m_translationOffset.y();
            lastSegment = m_axisCacheY.subSegmentCount() * m_axisCacheY.segmentCount();
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
            GLfloat lineXTrans = (aspectRatio * m_backgroundMargin * m_areaSize.width())
                    / m_scaleFactor - gridLineOffset;
            if (m_maxItemSize > lineXTrans)
                lineXTrans = m_maxItemSize - gridLineOffset;
            GLfloat zScale = (aspectRatio * m_backgroundMargin * m_areaSize.height())
                    / m_scaleFactor;
            if (m_maxItemSize > zScale)
                zScale = m_maxItemSize;
            gridLineScaler = QVector3D(gridLineWidth, gridLineWidth, zScale);
#else // ..and this if we want uniform scaling based on largest dimension
            GLfloat lineXTrans = aspectRatio * m_backgroundMargin - gridLineOffset;
            gridLineScaler = QVector3D(gridLineWidth, gridLineWidth,
                                       aspectRatio * m_backgroundMargin);
#endif
            if (!m_xFlipped)
                lineXTrans = -lineXTrans;

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(lineXTrans, linePos / m_heightNormalizer, 0.0f);

                modelMatrix.scale(gridLineScaler);
                itModelMatrix.scale(gridLineScaler);

                modelMatrix.rotate(lineYRotation);
                itModelMatrix.rotate(lineYRotation);

                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                    // Set shadow shader bindings
                    QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                    lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
                } else
#endif
                {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
                linePos += lineStep;
            }
        }
    }

    // Draw axis labels
    // Bind label shader
    m_labelShader->bind();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_OFFSET_FILL);

    // Z Labels
    if (m_axisCacheZ.segmentCount() > 0) {
#ifndef USE_UNIFORM_SCALING
        GLfloat posStep = aspectRatio * m_axisCacheZ.segmentStep();
        GLfloat labelPos = -aspectRatio * (m_axisCacheZ.min() - m_translationOffset.z());
        int lastSegment = m_axisCacheZ.segmentCount();
        GLfloat labelXTrans = (aspectRatio * m_backgroundMargin * m_areaSize.width())
                / m_scaleFactor + labelMargin;
        if (m_maxItemSize > labelXTrans)
            labelXTrans = m_maxItemSize + labelMargin;
#else
        GLfloat posStep = aspectRatio * axisCacheMax->segmentStep();
        GLfloat labelPos = aspectRatio * m_scaleFactor;
        int lastSegment = axisCacheMax->segmentCount();
        GLfloat labelXTrans = aspectRatio * m_backgroundMargin + labelMargin;
#endif
        int labelNbr = 0;
        GLfloat labelYTrans = -m_backgroundMargin;
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
        QVector3D labelRotateVector(rotLabelX, rotLabelY, rotLabelZ);
        QVector3D labelTrans = QVector3D(labelXTrans, labelYTrans, 0.0f);
        for (int segment = 0; segment <= lastSegment; segment++) {
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
            if (m_axisCacheZ.labelItems().size() > labelNbr) {
#else // ..and this if we want uniform scaling based on largest dimension
            if (axisCacheMax->labelItems().size() > labelNbr) {
#endif
                labelTrans.setZ(labelPos / m_scaleFactor);

                glPolygonOffset(GLfloat(segment) / -10.0f, 1.0f);

                // Draw the label here
                m_dummyRenderItem.setTranslation(labelTrans);
#ifndef USE_UNIFORM_SCALING
                const LabelItem &axisLabelItem = *m_axisCacheZ.labelItems().at(labelNbr);
#else
                const LabelItem &axisLabelItem = *axisCacheMax->labelItems().at(labelNbr);
#endif

                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    zeroVector, labelRotateVector, 0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, activeCamera, true, true,
                                    Drawer::LabelMid, alignment);
            }
            labelNbr++;
            labelPos -= posStep;
        }
    }
    // X Labels
    if (m_axisCacheX.segmentCount() > 0) {
#ifndef USE_UNIFORM_SCALING
        GLfloat posStep = aspectRatio * m_axisCacheX.segmentStep();
        GLfloat labelPos = aspectRatio * (m_axisCacheX.min() - m_translationOffset.x());
        int lastSegment = m_axisCacheX.segmentCount();
        GLfloat labelZTrans = (aspectRatio * m_backgroundMargin * m_areaSize.height())
                / m_scaleFactor + labelMargin;
        if (m_maxItemSize > labelZTrans)
            labelZTrans = m_maxItemSize + labelMargin;
#else
        GLfloat posStep = aspectRatio * axisCacheMax->segmentStep();
        GLfloat labelPos = -aspectRatio * m_scaleFactor;
        int lastSegment = axisCacheMax->segmentCount();
        GLfloat labelZTrans = aspectRatio * m_backgroundMargin + labelMargin;
#endif
        int labelNbr = 0;
        GLfloat labelYTrans = -m_backgroundMargin;
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
        QVector3D labelRotateVector(rotLabelX, rotLabelY, rotLabelZ);
        QVector3D labelTrans = QVector3D(0.0f, labelYTrans, labelZTrans);
        for (int segment = 0; segment <= lastSegment; segment++) {
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
            if (m_axisCacheX.labelItems().size() > labelNbr) {
#else // ..and this if we want uniform scaling based on largest dimension
            if (axisCacheMax->labelItems().size() > labelNbr) {
#endif
                labelTrans.setX(labelPos / m_scaleFactor);

                glPolygonOffset(GLfloat(segment) / -10.0f, 1.0f);

                // Draw the label here
                m_dummyRenderItem.setTranslation(labelTrans);
#ifndef USE_UNIFORM_SCALING
                const LabelItem &axisLabelItem = *m_axisCacheX.labelItems().at(labelNbr);
#else
                const LabelItem &axisLabelItem = *axisCacheMax->labelItems().at(labelNbr);
#endif

                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    zeroVector, labelRotateVector, 0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, activeCamera, true, true,
                                    Drawer::LabelMid, alignment);
            }
            labelNbr++;
            labelPos += posStep;
        }
    }
    // Y Labels
    if (m_axisCacheY.segmentCount() > 0) {
        GLfloat posStep = m_axisCacheY.segmentStep();
        GLfloat labelPos = m_axisCacheY.min() - m_translationOffset.y();
        int labelNbr = 0;
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
        GLfloat labelXTrans = (aspectRatio * m_backgroundMargin * m_areaSize.width())
                / m_scaleFactor;
        GLfloat labelZTrans = (aspectRatio * m_backgroundMargin * m_areaSize.height())
                / m_scaleFactor;
        if (m_maxItemSize > labelXTrans)
            labelXTrans = m_maxItemSize;
        if (m_maxItemSize > labelZTrans)
            labelZTrans = m_maxItemSize;
#else // ..and this if we want uniform scaling based on largest dimension
        GLfloat labelXTrans = aspectRatio * m_backgroundMargin;
        GLfloat labelZTrans = labelXTrans;
#endif
        // Back wall init
        GLfloat labelMarginXTrans = labelMargin;
        GLfloat labelMarginZTrans = labelMargin;
        GLfloat rotLabelX = 0.0f;
        GLfloat rotLabelY = -90.0f;
        GLfloat rotLabelZ = 0.0f;
        Qt::AlignmentFlag alignmentBack = Qt::AlignLeft;
        if (!m_xFlipped) {
            labelXTrans = -labelXTrans;
            labelMarginXTrans = -labelMargin;
            rotLabelY = 90.0f;
        }
        if (m_zFlipped) {
            labelZTrans = -labelZTrans;
            labelMarginZTrans = -labelMargin;
            alignmentBack = Qt::AlignRight;
        }
        QVector3D labelRotateVectorBack(rotLabelX, rotLabelY, rotLabelZ);
        QVector3D labelTransBack = QVector3D(labelXTrans, 0.0f, labelZTrans + labelMarginZTrans);

        // Side wall init
        Qt::AlignmentFlag alignmentSide = Qt::AlignLeft;
        if (m_xFlipped)
            alignmentSide = Qt::AlignLeft;
        else
            alignmentSide = Qt::AlignRight;
        if (m_zFlipped)
            rotLabelY = 180.0f;
        else
            rotLabelY = 0.0f;

        QVector3D labelRotateVectorSide(rotLabelX, rotLabelY, rotLabelZ);
        QVector3D labelTransSide(-labelXTrans - labelMarginXTrans, 0.0f, -labelZTrans);

        for (int segment = 0; segment <= m_axisCacheY.segmentCount(); segment++) {
            if (m_axisCacheY.labelItems().size() > labelNbr) {
                const LabelItem &axisLabelItem = *m_axisCacheY.labelItems().at(labelNbr);
                const GLfloat labelYTrans = labelPos / m_heightNormalizer;

                glPolygonOffset(GLfloat(segment) / -10.0f, 1.0f);

                // Back wall
                labelTransBack.setY(labelYTrans);
                m_dummyRenderItem.setTranslation(labelTransBack);
                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    zeroVector, labelRotateVectorBack, 0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, activeCamera, true, true,
                                    Drawer::LabelMid, alignmentBack);

                // Side wall
                labelTransSide.setY(labelYTrans);
                m_dummyRenderItem.setTranslation(labelTransSide);
                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    zeroVector, labelRotateVectorSide, 0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, activeCamera, true, true,
                                    Drawer::LabelMid, alignmentSide);
            }
            labelNbr++;
            labelPos += posStep;
        }
    }
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Handle selection clearing and selection label drawing
    if (!dotSelectionFound) {
        // We have no ownership, don't delete. Just NULL the pointer.
        m_selectedItem = NULL;
    } else {
        glDisable(GL_DEPTH_TEST);
        // Draw the selection label
        LabelItem &labelItem = selectedItem->selectionLabelItem();
        if (m_selectedItem != selectedItem || m_updateLabels
                || !labelItem.textureId() || m_selectionLabelDirty) {
            QString labelText = selectedItem->selectionLabel();
            if (labelText.isNull() || m_selectionLabelDirty) {
                static const QString xTitleTag(QStringLiteral("@xTitle"));
                static const QString yTitleTag(QStringLiteral("@yTitle"));
                static const QString zTitleTag(QStringLiteral("@zTitle"));
                static const QString xLabelTag(QStringLiteral("@xLabel"));
                static const QString yLabelTag(QStringLiteral("@yLabel"));
                static const QString zLabelTag(QStringLiteral("@zLabel"));
                static const QString seriesNameTag(QStringLiteral("@seriesName"));

                labelText = m_visibleSeriesList[m_selectedItemSeriesIndex].itemLabelFormat();

                labelText.replace(xTitleTag, m_axisCacheX.title());
                labelText.replace(yTitleTag, m_axisCacheY.title());
                labelText.replace(zTitleTag, m_axisCacheZ.title());

                if (labelText.contains(xLabelTag)) {
                    QString labelFormat = m_axisCacheX.labelFormat();
                    if (labelFormat.isEmpty())
                        labelFormat = Utils::defaultLabelFormat();
                    QString valueLabelText = generateValueLabel(labelFormat,
                                                                selectedItem->position().x());
                    labelText.replace(xLabelTag, valueLabelText);
                }
                if (labelText.contains(yLabelTag)) {
                    QString labelFormat = m_axisCacheY.labelFormat();
                    if (labelFormat.isEmpty())
                        labelFormat = Utils::defaultLabelFormat();
                    QString valueLabelText = generateValueLabel(labelFormat,
                                                                selectedItem->position().y());
                    labelText.replace(yLabelTag, valueLabelText);
                }
                if (labelText.contains(zLabelTag)) {
                    QString labelFormat = m_axisCacheZ.labelFormat();
                    if (labelFormat.isEmpty())
                        labelFormat = Utils::defaultLabelFormat();
                    QString valueLabelText = generateValueLabel(labelFormat,
                                                                selectedItem->position().z());
                    labelText.replace(zLabelTag, valueLabelText);
                }
                labelText.replace(seriesNameTag, m_visibleSeriesList[m_selectedItemSeriesIndex].name());

                selectedItem->setSelectionLabel(labelText);
                m_selectionLabelDirty = false;
            }
            m_drawer->generateLabelItem(labelItem, labelText);
            m_selectedItem = selectedItem;
        }

        m_drawer->drawLabel(*selectedItem, labelItem, viewMatrix, projectionMatrix,
                            zeroVector, zeroVector, selectedItemSize, m_cachedSelectionMode,
                            m_labelShader, m_labelObj, activeCamera, true, false,
                            Drawer::LabelOver);

        // Reset label update flag; they should have been updated when we get here
        m_updateLabels = false;
        glEnable(GL_DEPTH_TEST);
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    // Release shader
    glUseProgram(0);

    m_selectionDirty = false;
}

void Scatter3DRenderer::updateSelectedItem(int index, const QScatter3DSeries *series)
{
    m_selectionDirty = true;
    m_selectedSeries = series;
    m_selectedItemIndex = Scatter3DController::invalidSelectionIndex();
    m_selectedItemTotalIndex = Scatter3DController::invalidSelectionIndex();
    m_selectedItemSeriesIndex = Scatter3DController::invalidSelectionIndex();

    if (!m_renderingArrays.isEmpty() && index != Scatter3DController::invalidSelectionIndex()) {
        int totalIndex = 0;
        for (int i = 0; i < m_visibleSeriesList.size(); i++) {
            if (m_visibleSeriesList.at(i).series() == series) {
                m_selectedItemSeriesIndex = i;
                m_selectedItemIndex = index;
                m_selectedItemTotalIndex = index + totalIndex;
                break;
            }
            totalIndex += m_renderingArrays.at(i).size();
        }
    }
}

void Scatter3DRenderer::updateShadowQuality(QAbstract3DGraph::ShadowQuality quality)
{
    m_cachedShadowQuality = quality;
    switch (quality) {
    case QAbstract3DGraph::ShadowQualityLow:
        m_shadowQualityToShader = 33.3f;
        m_shadowQualityMultiplier = 1;
        break;
    case QAbstract3DGraph::ShadowQualityMedium:
        m_shadowQualityToShader = 100.0f;
        m_shadowQualityMultiplier = 3;
        break;
    case QAbstract3DGraph::ShadowQualityHigh:
        m_shadowQualityToShader = 200.0f;
        m_shadowQualityMultiplier = 5;
        break;
    case QAbstract3DGraph::ShadowQualitySoftLow:
        m_shadowQualityToShader = 5.0f;
        m_shadowQualityMultiplier = 1;
        break;
    case QAbstract3DGraph::ShadowQualitySoftMedium:
        m_shadowQualityToShader = 10.0f;
        m_shadowQualityMultiplier = 3;
        break;
    case QAbstract3DGraph::ShadowQualitySoftHigh:
        m_shadowQualityToShader = 15.0f;
        m_shadowQualityMultiplier = 4;
        break;
    default:
        m_shadowQualityToShader = 0.0f;
        m_shadowQualityMultiplier = 1;
        break;
    }

    handleShadowQualityChange();

#if !defined(QT_OPENGL_ES_2)
    // Re-init depth buffer
    updateDepthBuffer();
#endif
}

void Scatter3DRenderer::loadBackgroundMesh()
{
    if (m_backgroundObj)
        delete m_backgroundObj;
    m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/background"));
    m_backgroundObj->load();
}

void Scatter3DRenderer::loadGridLineMesh()
{
    if (m_gridLineObj)
        delete m_gridLineObj;
    m_gridLineObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/plane"));
    m_gridLineObj->load();
}

void Scatter3DRenderer::loadLabelMesh()
{
    if (m_labelObj)
        delete m_labelObj;
    m_labelObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/plane"));
    m_labelObj->load();
}

void Scatter3DRenderer::updateTextures()
{
    // Drawer has changed; this flag needs to be checked when checking if we need to update labels
    m_updateLabels = true;
}

void Scatter3DRenderer::fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh mesh)
{
    // Load full version of meshes that have it available
    if (mesh != QAbstract3DSeries::MeshSphere
            && mesh != QAbstract3DSeries::MeshMinimal
            && mesh != QAbstract3DSeries::MeshPoint
            && mesh != QAbstract3DSeries::MeshArrow) {
        fileName.append(QStringLiteral("Full"));
    }
}

void Scatter3DRenderer::updateAxisRange(QAbstract3DAxis::AxisOrientation orientation,
                                        float min, float max)
{
    Abstract3DRenderer::updateAxisRange(orientation, min, max);
}

void Scatter3DRenderer::calculateTranslation(ScatterRenderItem &item)
{
    // We need to normalize translations
    GLfloat xTrans = (aspectRatio * (item.position().x() - m_translationOffset.x()))
            / m_scaleFactor;
    GLfloat zTrans = -(aspectRatio * (item.position().z() - m_translationOffset.z()))
            / m_scaleFactor;
    GLfloat yTrans = (item.position().y() - m_translationOffset.y()) / m_heightNormalizer;
    item.setTranslation(QVector3D(xTrans, yTrans, zTrans));
}

void Scatter3DRenderer::calculateSceneScalingFactors()
{
    m_heightNormalizer = GLfloat(m_axisCacheY.max() - m_axisCacheY.min()) / 2.0f;
    m_areaSize.setHeight((m_axisCacheZ.max() - m_axisCacheZ.min()) / 2.0f);
    m_areaSize.setWidth((m_axisCacheX.max() - m_axisCacheX.min()) / 2.0f);
    m_scaleFactor = qMax(m_areaSize.width(), m_areaSize.height());

    // Calculate translation offsets
    m_translationOffset = QVector3D((m_axisCacheX.max() + m_axisCacheX.min()) / 2.0f,
                                    (m_axisCacheY.max() + m_axisCacheY.min()) / 2.0f,
                                    (m_axisCacheZ.max() + m_axisCacheZ.min()) / 2.0f);
}

void Scatter3DRenderer::initShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_dotShader)
        delete m_dotShader;
    m_dotShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_dotShader->initialize();
}

void Scatter3DRenderer::initGradientShaders(const QString &vertexShader,
                                            const QString &fragmentShader)
{
    if (m_dotGradientShader)
        delete m_dotGradientShader;
    m_dotGradientShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_dotGradientShader->initialize();
}

void Scatter3DRenderer::initSelectionShader()
{
    if (m_selectionShader)
        delete m_selectionShader;
    m_selectionShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexPlainColor"),
                                         QStringLiteral(":/shaders/fragmentPlainColor"));
    m_selectionShader->initialize();
}

void Scatter3DRenderer::initSelectionBuffer()
{
    if (m_selectionTexture) {
        m_textureHelper->deleteTexture(&m_selectionTexture);
        m_selectionTexture = 0;
    }

    if (m_primarySubViewport.size().isEmpty())
        return;

    m_selectionTexture = m_textureHelper->createSelectionTexture(m_primarySubViewport.size(),
                                                                 m_selectionFrameBuffer,
                                                                 m_selectionDepthBuffer);
}

#if !defined(QT_OPENGL_ES_2)
void Scatter3DRenderer::initDepthShader()
{
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

    if (m_primarySubViewport.size().isEmpty())
        return;

    if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
        m_depthTexture = m_textureHelper->createDepthTextureFrameBuffer(m_primarySubViewport.size(),
                                                                        m_depthFrameBuffer,
                                                                        m_shadowQualityMultiplier);
        if (!m_depthTexture)
            lowerShadowQuality();
    }
}
#else
void Scatter3DRenderer::initPointShader()
{
    if (m_pointShader)
        delete m_pointShader;
    m_pointShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexPointES2"),
                                     QStringLiteral(":/shaders/fragmentPlainColor"));
    m_pointShader->initialize();
}
#endif

void Scatter3DRenderer::initBackgroundShaders(const QString &vertexShader,
                                              const QString &fragmentShader)
{
    if (m_backgroundShader)
        delete m_backgroundShader;
    m_backgroundShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_backgroundShader->initialize();
}

void Scatter3DRenderer::initLabelShaders(const QString &vertexShader, const QString &fragmentShader)
{
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

void Scatter3DRenderer::selectionColorToSeriesAndIndex(const QVector3D &color,
                                                       int &index,
                                                       QAbstract3DSeries *&series)
{
    if (color != selectionSkipColor) {
        index = int(color.x())
                + (int(color.y()) << 8)
                + (int(color.z()) << 16);
        // Find the series and adjust the index accordingly
        for (int i = 0; i < m_renderingArrays.size(); i++) {
            if (index < m_renderingArrays.at(i).size()) {
                series = m_visibleSeriesList.at(i).series();
                return; // Valid found and already set to return parameters, so we can return
            } else {
                index -= m_renderingArrays.at(i).size();
            }
        }
    }

    // No valid match found
    index = Scatter3DController::invalidSelectionIndex();
    series = 0;
}

QT_END_NAMESPACE_DATAVISUALIZATION
