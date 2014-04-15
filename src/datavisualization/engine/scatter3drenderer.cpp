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
#include "scatterseriesrendercache_p.h"

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
      #if !defined(QT_OPENGL_ES_2)
      m_gridLineObj(0),
      #endif
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
      m_selectedSeriesCache(0),
      m_areaSize(QSizeF(0.0, 0.0)),
      m_dotSizeScale(1.0f),
      m_hasHeightAdjustmentChanged(true),
      m_backgroundMargin(defaultMaxSize),
      m_maxItemSize(0.0f),
      m_clickedIndex(Scatter3DController::invalidSelectionIndex()),
      m_havePointSeries(false),
      m_haveMeshSeries(false),
      m_haveUniformColorMeshSeries(false),
      m_haveGradientMeshSeries(false)
{
    m_axisCacheY.setScale(2.0f);
    m_axisCacheY.setTranslate(-1.0f);

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
#if !defined(QT_OPENGL_ES_2)
    delete m_gridLineObj;
#endif
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

#if !defined(QT_OPENGL_ES_2)
    // Load grid line mesh
    loadGridLineMesh();
#endif

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

void Scatter3DRenderer::updateData()
{
    calculateSceneScalingFactors();
    float minX = float(m_axisCacheX.min());
    float maxX = float(m_axisCacheX.max());
    float minY = float(m_axisCacheY.min());
    float maxY = float(m_axisCacheY.max());
    float minZ = float(m_axisCacheZ.min());
    float maxZ = float(m_axisCacheZ.max());
    int totalDataSize = 0;

    foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
        ScatterSeriesRenderCache *cache = static_cast<ScatterSeriesRenderCache *>(baseCache);
        if (cache->isVisible() && cache->dataDirty()) {
            const QScatter3DSeries *currentSeries = cache->series();
            ScatterRenderItemArray &renderArray = cache->renderArray();
            QScatterDataProxy *dataProxy = currentSeries->dataProxy();
            const QScatterDataArray &dataArray = *dataProxy->array();
            int dataSize = dataArray.size();
            totalDataSize += dataSize;
            if (dataSize != renderArray.size())
                renderArray.resize(dataSize);

            for (int i = 0; i < dataSize; i++) {
                QVector3D dotPos = dataArray.at(i).position();
                ScatterRenderItem &renderItem = renderArray[i];
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
            cache->setDataDirty(false);
        }
    }

    if (totalDataSize) {
        m_dotSizeScale = GLfloat(qBound(defaultMinSize, 2.0f / float(qSqrt(qreal(totalDataSize))),
                                        defaultMaxSize));
    }

    updateSelectedItem(m_selectedItemIndex,
                       m_selectedSeriesCache ? m_selectedSeriesCache->series() : 0);
}

void Scatter3DRenderer::updateSeries(const QList<QAbstract3DSeries *> &seriesList)
{
    Abstract3DRenderer::updateSeries(seriesList);

    int seriesCount = seriesList.size();
    float maxItemSize = 0.0f;
    float itemSize = 0.0f;
    bool noSelection = true;

    m_havePointSeries = false;
    m_haveMeshSeries = false;
    m_haveUniformColorMeshSeries = false;
    m_haveGradientMeshSeries = false;

    for (int i = 0; i < seriesCount; i++) {
        QScatter3DSeries *scatterSeries = static_cast<QScatter3DSeries *>(seriesList[i]);
        if (scatterSeries->isVisible()) {
            ScatterSeriesRenderCache *cache =
                    static_cast<ScatterSeriesRenderCache *>(m_renderCacheList.value(scatterSeries));
            itemSize = scatterSeries->itemSize();
            if (maxItemSize < itemSize)
                maxItemSize = itemSize;
            if (cache->itemSize() != itemSize)
                cache->setItemSize(itemSize);
            if (noSelection
                    && scatterSeries->selectedItem() != QScatter3DSeries::invalidSelectionIndex()
                    && m_selectionLabel != cache->itemLabel()) {
                m_selectionLabelDirty = true;
                noSelection = false;
            }

            if (cache->mesh() == QAbstract3DSeries::MeshPoint) {
                m_havePointSeries = true;
            } else {
                m_haveMeshSeries = true;
                if (cache->colorStyle() == Q3DTheme::ColorStyleUniform)
                    m_haveUniformColorMeshSeries = true;
                else
                    m_haveGradientMeshSeries = true;
            }
        }
    }
    m_maxItemSize = maxItemSize;
    if (maxItemSize > defaultMaxSize)
        m_backgroundMargin = maxItemSize / itemScaler;
    else
        m_backgroundMargin = defaultMaxSize;

    if (noSelection && !selectionLabel().isEmpty())
        m_selectionLabelDirty = true;
}

void Scatter3DRenderer::updateCustomData(const QList<CustomDataItem *> &customItems)
{
    if (customItems.isEmpty() && m_customRenderCache.isEmpty())
        return;

    // There are probably not too many custom items, just recreate the array if something changes
    foreach (CustomRenderItem *item, m_customRenderCache)
        delete item;
    m_customRenderCache.clear();
    foreach (CustomDataItem *item, customItems)
        addCustomItem(item);
}

SeriesRenderCache *Scatter3DRenderer::createNewCache(QAbstract3DSeries *series)
{
    return new ScatterSeriesRenderCache(series, this);
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

    if (m_axisCacheX.positionsDirty())
        m_axisCacheX.updateAllPositions();
    if (m_axisCacheY.positionsDirty())
        m_axisCacheY.updateAllPositions();
    if (m_axisCacheZ.positionsDirty())
        m_axisCacheZ.updateAllPositions();

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

#if !defined(QT_OPENGL_ES_2)
    if (m_havePointSeries) {
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
        foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
            if (baseCache->isVisible()) {
                ScatterSeriesRenderCache *cache = static_cast<ScatterSeriesRenderCache *>(baseCache);
                ObjectHelper *dotObj = cache->object();
                QQuaternion seriesRotation(cache->meshRotation());
                const ScatterRenderItemArray &renderArray = cache->renderArray();
                const int renderArraySize = renderArray.size();
                bool drawingPoints = (cache->mesh() == QAbstract3DSeries::MeshPoint);
                float itemSize = cache->itemSize() / itemScaler;
                if (itemSize == 0.0f)
                    itemSize = m_dotSizeScale;
                if (drawingPoints) {
                    // Scale points based on shadow quality for shadows, not by zoom level
                    glPointSize(itemSize * 100.0f * m_shadowQualityMultiplier);
                }
                QVector3D modelScaler(itemSize, itemSize, itemSize);
                for (int dot = 0; dot < renderArraySize; dot++) {
                    const ScatterRenderItem &item = renderArray.at(dot);
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
        }

        drawCustomItems(RenderingDepth, m_depthShader, activeCamera, projectionMatrix,
                        depthProjectionMatrix);

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
    ShaderHelper *pointSelectionShader = m_pointShader;
#endif
    ShaderHelper *selectionShader = m_selectionShader;

    // Skip selection mode drawing if we have no selection mode
    if (m_cachedSelectionMode > QAbstract3DGraph::SelectionNone
            && SelectOnScene == m_selectionState && m_visibleSeriesCount > 0) {
        // Draw dots to selection buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
        glViewport(0, 0,
                   m_primarySubViewport.width(),
                   m_primarySubViewport.height());

        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (= skipColor)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled

        bool previousDrawingPoints = false;
        int totalIndex = 0;
        foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
            if (baseCache->isVisible()) {
                ScatterSeriesRenderCache *cache =
                        static_cast<ScatterSeriesRenderCache *>(baseCache);
                ObjectHelper *dotObj = cache->object();
                QQuaternion seriesRotation(cache->meshRotation());
                const ScatterRenderItemArray &renderArray = cache->renderArray();
                const int renderArraySize = renderArray.size();
                bool drawingPoints = (cache->mesh() == QAbstract3DSeries::MeshPoint);
                float itemSize = cache->itemSize() / itemScaler;
                if (itemSize == 0.0f)
                    itemSize = m_dotSizeScale;
#if !defined(QT_OPENGL_ES_2)
                if (drawingPoints)
                    glPointSize(itemSize * activeCamera->zoomLevel()); // Scale points based on zoom
#endif
                QVector3D modelScaler(itemSize, itemSize, itemSize);

                // Rebind selection shader if it has changed
                if (!totalIndex || drawingPoints != previousDrawingPoints) {
                    previousDrawingPoints = drawingPoints;
                    if (drawingPoints)
                        selectionShader = pointSelectionShader;
                    else
                        selectionShader = m_selectionShader;

                    selectionShader->bind();
                }
                cache->setSelectionIndexOffset(totalIndex);
                for (int dot = 0; dot < renderArraySize; dot++) {
                    const ScatterRenderItem &item = renderArray.at(dot);
                    if (!item.isVisible()) {
                        totalIndex++;
                        continue;
                    }

                    QMatrix4x4 modelMatrix;
                    QMatrix4x4 MVPMatrix;

                    modelMatrix.translate(item.translation());
                    if (!drawingPoints) {
                        if (!seriesRotation.isIdentity() || !item.rotation().isIdentity())
                            modelMatrix.rotate(seriesRotation * item.rotation());
                        modelMatrix.scale(modelScaler);
                    }

                    MVPMatrix = projectionViewMatrix * modelMatrix;

                    QVector4D dotColor = indexToSelectionColor(totalIndex++);
                    dotColor /= 255.0f;

                    selectionShader->setUniformValue(selectionShader->MVP(), MVPMatrix);
                    selectionShader->setUniformValue(selectionShader->color(), dotColor);

                    if (drawingPoints)
                        m_drawer->drawPoint(selectionShader);
                    else
                        m_drawer->drawSelectionObject(selectionShader, dotObj);
                }
            }
        }

        drawCustomItems(RenderingSelection, m_selectionShader, activeCamera, projectionMatrix,
                        depthProjectionMatrix);

        drawLabels(true, activeCamera, viewMatrix, projectionMatrix);

        glEnable(GL_DITHER);

        // Read color under cursor
        QVector4D clickedColor = Utils::getSelection(m_inputPosition,
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
    QVector3D baseColor;
    QVector3D dotColor;

    bool previousDrawingPoints = false;
    Q3DTheme::ColorStyle previousMeshColorStyle = Q3DTheme::ColorStyleUniform;
    if (m_haveMeshSeries) {
        // Set unchanging shader bindings
        if (m_haveGradientMeshSeries) {
            m_dotGradientShader->bind();
            m_dotGradientShader->setUniformValue(m_dotGradientShader->lightP(), lightPos);
            m_dotGradientShader->setUniformValue(m_dotGradientShader->view(), viewMatrix);
            m_dotGradientShader->setUniformValue(m_dotGradientShader->ambientS(),
                                                 m_cachedTheme->ambientLightStrength());
            m_dotGradientShader->setUniformValue(m_dotGradientShader->lightColor(), lightColor);
        }
        if (m_haveUniformColorMeshSeries) {
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

    foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
        if (baseCache->isVisible()) {
            ScatterSeriesRenderCache *cache =
                    static_cast<ScatterSeriesRenderCache *>(baseCache);
            ObjectHelper *dotObj = cache->object();
            QQuaternion seriesRotation(cache->meshRotation());
            ScatterRenderItemArray &renderArray = cache->renderArray();
            const int renderArraySize = renderArray.size();
            bool selectedSeries = m_cachedSelectionMode > QAbstract3DGraph::SelectionNone
                    && (m_selectedSeriesCache == cache);
            bool drawingPoints = (cache->mesh() == QAbstract3DSeries::MeshPoint);
            Q3DTheme::ColorStyle colorStyle = cache->colorStyle();
            bool colorStyleIsUniform = (colorStyle == Q3DTheme::ColorStyleUniform);
            bool useColor = colorStyleIsUniform || drawingPoints;
            bool rangeGradientPoints = drawingPoints
                    && (colorStyle == Q3DTheme::ColorStyleRangeGradient);
            float itemSize = cache->itemSize() / itemScaler;
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
                        (colorStyleIsUniform != (previousMeshColorStyle
                                                 == Q3DTheme::ColorStyleUniform)))) {
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
                    m_dotGradientShader->setUniformValue(m_dotGradientShader->gradientHeight(),
                                                         0.5f);
                } else {
                    // Each dot is of uniform color according to its Y-coordinate
                    m_dotGradientShader->setUniformValue(m_dotGradientShader->gradientHeight(),
                                                         0.0f);
                }
            }

            if (!drawingPoints)
                previousMeshColorStyle = colorStyle;

            if (useColor) {
                baseColor = cache->baseColor();
                dotColor = baseColor;
            }
            for (int i = 0; i < renderArraySize; i++) {
                ScatterRenderItem &item = renderArray[i];
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

                if (useColor) {
                    if (rangeGradientPoints) {
                        // Drawing points with range gradient
                        // Get color from gradient based on items y position converted to percent
                        int position = int(item.translation().y() * 50.0f) + 50;
                        dotColor = Utils::vectorFromColor(
                                    cache->gradientImage().pixel(0, position));
                    } else {
                        dotColor = baseColor;
                    }
                } else {
                    gradientTexture = cache->baseGradientTexture();
                }

                GLfloat lightStrength = m_cachedTheme->lightStrength();
                if (selectedSeries && (m_selectedItemIndex == i)) {
                    if (useColor)
                        dotColor = cache->singleHighlightColor();
                    else
                        gradientTexture = cache->singleHighlightGradientTexture();
                    lightStrength = m_cachedTheme->highlightLightStrength();
                    // Insert data to ScatterRenderItem
                    // We don't have ownership, so don't delete the previous one
                    selectedItem = &item;
                    dotSelectionFound = true;
                    // Save selected item size (adjusted with font size) for selection label
                    // positioning
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
            }
        }
    }

#if !defined(QT_OPENGL_ES_2)
    if (m_havePointSeries) {
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_PROGRAM_POINT_SIZE);
    }
#endif

    drawCustomItems(RenderingNormal, m_customItemShader, activeCamera, projectionMatrix,
                    depthProjectionMatrix);

    // Bind background shader
    m_backgroundShader->bind();

    glCullFace(GL_BACK);

    // Draw background
    if (m_cachedTheme->isBackgroundEnabled() && m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 itModelMatrix;

#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
        GLfloat xScale = (aspectRatio * m_areaSize.width()) / m_scaleFactor + m_backgroundMargin;
        GLfloat zScale = (aspectRatio * m_areaSize.height()) / m_scaleFactor + m_backgroundMargin;
        if (m_maxItemSize > xScale)
            xScale = m_maxItemSize;
        if (m_maxItemSize > zScale)
            zScale = m_maxItemSize;
        QVector3D bgScale(xScale, 1.0f + m_backgroundMargin, zScale);
#else // ..and this if we want uniform scaling based on largest dimension
        QVector3D bgScale((aspectRatio + m_backgroundMargin),
                          1.0f + m_backgroundMargin,
                          (aspectRatio + m_backgroundMargin));
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
    if (m_cachedTheme->isGridEnabled()) {
#if !(defined QT_OPENGL_ES_2)
        ShaderHelper *lineShader = m_backgroundShader;
#else
        ShaderHelper *lineShader = m_selectionShader; // Plain color shader for GL_LINES
#endif

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

        GLfloat yFloorLinePosition = -1.0f - m_backgroundMargin + gridLineOffset;
        if (m_yFlipped)
            yFloorLinePosition = -yFloorLinePosition;

        // Rows (= Z)
        if (m_axisCacheZ.segmentCount() > 0) {
            // Floor lines
            int gridLineCount = m_axisCacheZ.gridLineCount();

#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
            GLfloat xScale = (aspectRatio * m_areaSize.width()) / m_scaleFactor + m_backgroundMargin;
            if (m_maxItemSize > xScale)
                xScale = m_maxItemSize;
            QVector3D gridLineScaler(xScale, gridLineWidth, gridLineWidth);
#else // ..and this if we want uniform scaling based on largest dimension
            QVector3D gridLineScaler((aspectRatio + m_backgroundMargin),
                                     gridLineWidth, gridLineWidth);
#endif

            for (int line = 0; line < gridLineCount; line++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(0.0f, yFloorLinePosition, m_axisCacheZ.gridLinePosition(line));

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
                } else {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
#else
                m_drawer->drawLine(lineShader);
#endif
            }

            // Side wall lines
            gridLineScaler = QVector3D(gridLineWidth, 1.0f + m_backgroundMargin, gridLineWidth);
#ifndef USE_UNIFORM_SCALING
            GLfloat lineXTrans = (aspectRatio * m_areaSize.width())
                    / m_scaleFactor - gridLineOffset + m_backgroundMargin;
            if (m_maxItemSize > lineXTrans)
                lineXTrans = m_maxItemSize - gridLineOffset;
#else
            GLfloat lineXTrans = aspectRatio + m_backgroundMargin - gridLineOffset;
#endif
            if (!m_xFlipped)
                lineXTrans = -lineXTrans;

            for (int line = 0; line < gridLineCount; line++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(lineXTrans, 0.0f, m_axisCacheZ.gridLinePosition(line));

                modelMatrix.scale(gridLineScaler);
                itModelMatrix.scale(gridLineScaler);

#if !defined(QT_OPENGL_ES_2)
                modelMatrix.rotate(lineYRotation);
                itModelMatrix.rotate(lineYRotation);
#else
                modelMatrix.rotate(90.0f, 0.0f, 0.0f, 1.0f);
                itModelMatrix.rotate(90.0f, 0.0f, 0.0f, 1.0f);
#endif

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
                } else {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
#else
                m_drawer->drawLine(lineShader);
#endif
            }
        }

        // Columns (= X)
        if (m_axisCacheX.segmentCount() > 0) {
#if defined(QT_OPENGL_ES_2)
            lineXRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 90.0f);
#endif
            // Floor lines
            int gridLineCount = m_axisCacheX.gridLineCount();

#ifndef USE_UNIFORM_SCALING
            GLfloat zScale = (aspectRatio * m_areaSize.height()) / m_scaleFactor + m_backgroundMargin;
            if (m_maxItemSize > zScale)
                zScale = m_maxItemSize;
            QVector3D gridLineScaler(gridLineWidth, gridLineWidth, zScale);
#else
            QVector3D gridLineScaler(gridLineWidth, gridLineWidth,
                                     aspectRatio + m_backgroundMargin);
#endif

            for (int line = 0; line < gridLineCount; line++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(m_axisCacheX.gridLinePosition(line), yFloorLinePosition, 0.0f);

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
                } else {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
#else
                m_drawer->drawLine(lineShader);
#endif
            }

            // Back wall lines
#ifndef USE_UNIFORM_SCALING
            GLfloat lineZTrans = (aspectRatio * m_areaSize.height())
                    / m_scaleFactor - gridLineOffset + m_backgroundMargin;
            if (m_maxItemSize > lineZTrans)
                lineZTrans = m_maxItemSize - gridLineOffset;
#else
            GLfloat lineZTrans = aspectRatio + m_backgroundMargin - gridLineOffset;
#endif
            if (!m_zFlipped)
                lineZTrans = -lineZTrans;

            gridLineScaler = QVector3D(gridLineWidth, 1.0f + m_backgroundMargin, gridLineWidth);

            for (int line = 0; line < gridLineCount; line++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(m_axisCacheX.gridLinePosition(line), 0.0f, lineZTrans);

                modelMatrix.scale(gridLineScaler);
                itModelMatrix.scale(gridLineScaler);

#if !defined(QT_OPENGL_ES_2)
                if (m_zFlipped) {
                    modelMatrix.rotate(180.0f, 1.0f, 0.0f, 0.0f);
                    itModelMatrix.rotate(180.0f, 1.0f, 0.0f, 0.0f);
                }
#else
                modelMatrix.rotate(90.0f, 0.0f, 0.0f, 1.0f);
                itModelMatrix.rotate(90.0f, 0.0f, 0.0f, 1.0f);
#endif

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
                } else {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
#else
                m_drawer->drawLine(lineShader);
#endif
            }
        }

        // Horizontal wall lines
        if (m_axisCacheY.segmentCount() > 0) {
            // Back wall
            int gridLineCount = m_axisCacheY.gridLineCount();

#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
            GLfloat lineZTrans = (aspectRatio * m_areaSize.height())
                    / m_scaleFactor - gridLineOffset + m_backgroundMargin;
            if (m_maxItemSize > lineZTrans)
                lineZTrans = m_maxItemSize - gridLineOffset;
            GLfloat xScale = (aspectRatio * m_areaSize.width()) / m_scaleFactor + m_backgroundMargin;
            if (m_maxItemSize > xScale)
                xScale = m_maxItemSize;
            QVector3D gridLineScaler(xScale, gridLineWidth, gridLineWidth);
#else // ..and this if we want uniform scaling based on largest dimension
            GLfloat lineZTrans = aspectRatio + m_backgroundMargin - gridLineOffset;
            QVector3D gridLineScaler((aspectRatio + m_backgroundMargin),
                                     gridLineWidth, gridLineWidth);
#endif
            if (!m_zFlipped)
                lineZTrans = -lineZTrans;

            for (int line = 0; line < gridLineCount; line++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(0.0f, m_axisCacheY.gridLinePosition(line), lineZTrans);

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
                } else {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
#else
                m_drawer->drawLine(lineShader);
#endif
            }

            // Side wall
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
            GLfloat lineXTrans = (aspectRatio * m_areaSize.width())
                    / m_scaleFactor - gridLineOffset + m_backgroundMargin;
            if (m_maxItemSize > lineXTrans)
                lineXTrans = m_maxItemSize - gridLineOffset;
            GLfloat zScale = (aspectRatio * m_areaSize.height())
                    / m_scaleFactor + m_backgroundMargin;
            if (m_maxItemSize > zScale)
                zScale = m_maxItemSize;
            gridLineScaler = QVector3D(gridLineWidth, gridLineWidth, zScale);
#else // ..and this if we want uniform scaling based on largest dimension
            GLfloat lineXTrans = aspectRatio + m_backgroundMargin - gridLineOffset;
            gridLineScaler = QVector3D(gridLineWidth, gridLineWidth,
                                       aspectRatio + m_backgroundMargin);
#endif
            if (!m_xFlipped)
                lineXTrans = -lineXTrans;

            for (int line = 0; line < gridLineCount; line++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(lineXTrans, m_axisCacheY.gridLinePosition(line), 0.0f);

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
                } else {
                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
#else
                m_drawer->drawLine(lineShader);
#endif
            }
        }
    }

    drawLabels(false, activeCamera, viewMatrix, projectionMatrix);

    // Handle selection clearing and selection label drawing
    if (!dotSelectionFound) {
        // We have no ownership, don't delete. Just NULL the pointer.
        m_selectedItem = NULL;
    } else {
        glDisable(GL_DEPTH_TEST);
        // Draw the selection label
        LabelItem &labelItem = selectionLabelItem();
        if (m_selectedItem != selectedItem || m_updateLabels
                || !labelItem.textureId() || m_selectionLabelDirty) {
            QString labelText = selectionLabel();
            if (labelText.isNull() || m_selectionLabelDirty) {
                labelText = m_selectedSeriesCache->itemLabel();
                setSelectionLabel(labelText);
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

void Scatter3DRenderer::drawCustomItems(RenderingState state, ShaderHelper *shader,
                                        const Q3DCamera *activeCamera,
                                        const QMatrix4x4 &projectionMatrix,
                                        const QMatrix4x4 &depthProjectionMatrix)
{
    if (m_customRenderCache.isEmpty())
        return;

    int itemIndex = 0;
    QMatrix4x4 viewMatrix = activeCamera->d_ptr->viewMatrix();
    QMatrix4x4 depthViewMatrix;
    QMatrix4x4 projectionViewMatrix = projectionMatrix * viewMatrix;
    QVector3D depthLightPos = activeCamera->d_ptr->calculatePositionRelativeToCamera(
                zeroVector, 0.0f, 2.5f / m_autoScaleAdjustment);
    depthViewMatrix.lookAt(depthLightPos, zeroVector, upVector);
    QMatrix4x4 depthProjectionViewMatrix = depthProjectionMatrix * depthViewMatrix;

    if (RenderingNormal == state) {
        shader->bind();
        shader->setUniformValue(shader->lightP(), m_cachedScene->activeLight()->position());
        shader->setUniformValue(shader->ambientS(), m_cachedTheme->ambientLightStrength());
        shader->setUniformValue(shader->lightColor(),
                                Utils::vectorFromColor(m_cachedTheme->lightColor()));
        shader->setUniformValue(shader->view(), viewMatrix);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Draw custom items
    foreach (CustomRenderItem *item, m_customRenderCache) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(item->translation());
        modelMatrix.rotate(item->rotation());
        modelMatrix.scale(item->scaling());
        itModelMatrix.rotate(item->rotation());
        itModelMatrix.scale(item->scaling());

        if (RenderingNormal == state) {
            // Normal render
            shader->setUniformValue(shader->model(), modelMatrix);
            shader->setUniformValue(shader->MVP(), projectionViewMatrix * modelMatrix);
            shader->setUniformValue(shader->nModel(), itModelMatrix.inverted().transposed());

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                // Set shadow shader bindings
                shader->setUniformValue(shader->shadowQ(), m_shadowQualityToShader);
                shader->setUniformValue(shader->depth(), depthProjectionViewMatrix * modelMatrix);
                shader->setUniformValue(shader->lightS(), m_cachedTheme->lightStrength() / 10.0f);
                m_drawer->drawObject(shader, item->mesh(), item->texture(), m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                shader->setUniformValue(shader->lightS(), m_cachedTheme->lightStrength());
                m_drawer->drawObject(shader, item->mesh(), item->texture());
            }
        } else if (RenderingSelection == state) {
            // Selection render
            QVector4D itemColor = indexToSelectionColor(itemIndex++);
            itemColor.setW(customItemAlpha);
            itemColor /= 255.0f;
            shader->setUniformValue(shader->color(), itemColor);
            m_drawer->drawObject(shader, item->mesh());
        } else {
            // Depth render
            shader->setUniformValue(shader->MVP(), depthProjectionViewMatrix * modelMatrix);
            m_drawer->drawObject(shader, item->mesh());
        }
    }

    if (RenderingNormal == state) {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
}

void Scatter3DRenderer::drawLabels(bool drawSelection, const Q3DCamera *activeCamera,
                                   const QMatrix4x4 &viewMatrix,
                                   const QMatrix4x4 &projectionMatrix) {
    ShaderHelper *shader = 0;
    GLfloat alphaForValueSelection = labelValueAlpha / 255.0f;
    GLfloat alphaForRowSelection = labelRowAlpha / 255.0f;
    GLfloat alphaForColumnSelection = labelColumnAlpha / 255.0f;
    if (drawSelection) {
        shader = m_selectionShader;
        // m_selectionShader is already bound
    } else {
        shader = m_labelShader;
        shader->bind();

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glEnable(GL_POLYGON_OFFSET_FILL);

    // Z Labels
    if (m_axisCacheZ.segmentCount() > 0) {
        int labelCount = m_axisCacheZ.labelCount();
#ifndef USE_UNIFORM_SCALING
        GLfloat labelXTrans = (aspectRatio * m_areaSize.width())
                / m_scaleFactor + labelMargin + m_backgroundMargin;
        if (m_maxItemSize > labelXTrans)
            labelXTrans = m_maxItemSize + labelMargin;
#else
        GLfloat labelXTrans = aspectRatio + m_backgroundMargin + labelMargin;
#endif
        int labelNbr = 0;
        GLfloat labelYTrans = -1.0f - m_backgroundMargin;
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
        for (int label = 0; label < labelCount; label++) {
            if (m_axisCacheZ.labelItems().size() > labelNbr) {
                labelTrans.setZ(m_axisCacheZ.labelPosition(label));

                glPolygonOffset(GLfloat(label) / -10.0f, 1.0f);

                // Draw the label here
                m_dummyRenderItem.setTranslation(labelTrans);
                const LabelItem &axisLabelItem = *m_axisCacheZ.labelItems().at(labelNbr);

                if (drawSelection) {
                    QVector4D labelColor = QVector4D(label / 255.0f, 0.0f, 0.0f,
                                                     alphaForRowSelection);
                    shader->setUniformValue(shader->color(), labelColor);
                }

                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    zeroVector, labelRotateVector, 0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, activeCamera, true, true,
                                    Drawer::LabelMid, alignment);
            }
            labelNbr++;
        }
    }
    // X Labels
    if (m_axisCacheX.segmentCount() > 0) {
        int labelCount = m_axisCacheX.labelCount();
#ifndef USE_UNIFORM_SCALING
        GLfloat labelZTrans = (aspectRatio * m_areaSize.height())
                / m_scaleFactor + labelMargin + m_backgroundMargin;
        if (m_maxItemSize > labelZTrans)
            labelZTrans = m_maxItemSize + labelMargin;
#else
        GLfloat labelZTrans = aspectRatio + m_backgroundMargin + labelMargin;
#endif
        int labelNbr = 0;
        GLfloat labelYTrans = -1.0f - m_backgroundMargin;
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
        for (int label = 0; label < labelCount; label++) {
            if (m_axisCacheX.labelItems().size() > labelNbr) {
                labelTrans.setX(m_axisCacheX.labelPosition(label));

                glPolygonOffset(GLfloat(label) / -10.0f, 1.0f);

                // Draw the label here
                m_dummyRenderItem.setTranslation(labelTrans);
                const LabelItem &axisLabelItem = *m_axisCacheX.labelItems().at(labelNbr);

                if (drawSelection) {
                    QVector4D labelColor = QVector4D(0.0f, label / 255.0f, 0.0f,
                                                     alphaForColumnSelection);
                    shader->setUniformValue(shader->color(), labelColor);
                }

                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    zeroVector, labelRotateVector, 0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, activeCamera, true, true,
                                    Drawer::LabelMid, alignment);
            }
            labelNbr++;
        }
    }
    // Y Labels
    if (m_axisCacheY.segmentCount() > 0) {
        int labelCount = m_axisCacheY.labelCount();
        int labelNbr = 0;
#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
        GLfloat labelXTrans = (aspectRatio* m_areaSize.width())
                / m_scaleFactor + m_backgroundMargin;
        GLfloat labelZTrans = (aspectRatio * m_areaSize.height())
                / m_scaleFactor + m_backgroundMargin;
        if (m_maxItemSize > labelXTrans)
            labelXTrans = m_maxItemSize;
        if (m_maxItemSize > labelZTrans)
            labelZTrans = m_maxItemSize;
#else // ..and this if we want uniform scaling based on largest dimension
        GLfloat labelXTrans = aspectRatio + m_backgroundMargin;
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

        for (int label = 0; label < labelCount; label++) {
            if (m_axisCacheY.labelItems().size() > labelNbr) {
                const LabelItem &axisLabelItem = *m_axisCacheY.labelItems().at(labelNbr);
                const GLfloat labelYTrans = m_axisCacheY.labelPosition(label);

                glPolygonOffset(GLfloat(label) / -10.0f, 1.0f);

                if (drawSelection) {
                    QVector4D labelColor = QVector4D(0.0f, 0.0f, label / 255.0f,
                                                     alphaForValueSelection);
                    shader->setUniformValue(shader->color(), labelColor);
                }

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
        }
    }
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void Scatter3DRenderer::updateSelectedItem(int index, QScatter3DSeries *series)
{
    m_selectionDirty = true;
    m_selectionLabelDirty = true;
    m_selectedSeriesCache =
            static_cast<ScatterSeriesRenderCache *>(m_renderCacheList.value(series, 0));
    m_selectedItemIndex = Scatter3DController::invalidSelectionIndex();

    if (m_selectedSeriesCache) {
        const ScatterRenderItemArray &renderArray = m_selectedSeriesCache->renderArray();
        if (index < renderArray.size() && index >= 0)
            m_selectedItemIndex = index;
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

#if !(defined QT_OPENGL_ES_2)
void Scatter3DRenderer::loadGridLineMesh()
{
    if (m_gridLineObj)
        delete m_gridLineObj;
    m_gridLineObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/plane"));
    m_gridLineObj->load();
}
#endif

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

void Scatter3DRenderer::calculateTranslation(ScatterRenderItem &item)
{
    // We need to normalize translations
    const QVector3D &pos = item.position();
    float xTrans = m_axisCacheX.positionAt(pos.x());
    float yTrans = m_axisCacheY.positionAt(pos.y());
    float zTrans = m_axisCacheZ.positionAt(pos.z());
    item.setTranslation(QVector3D(xTrans, yTrans, zTrans));
}

void Scatter3DRenderer::calculateSceneScalingFactors()
{
    m_heightNormalizer = GLfloat(m_axisCacheY.max() - m_axisCacheY.min()) / 2.0f;
    m_areaSize.setHeight((m_axisCacheZ.max() - m_axisCacheZ.min()) / 2.0f);
    m_areaSize.setWidth((m_axisCacheX.max() - m_axisCacheX.min()) / 2.0f);
    m_scaleFactor = qMax(m_areaSize.width(), m_areaSize.height());

#ifndef USE_UNIFORM_SCALING // Use this if we want to use autoscaling for x and z
    float factorScaler = 2.0f * aspectRatio / m_scaleFactor;
    m_axisCacheX.setScale(factorScaler * m_areaSize.width());
    m_axisCacheZ.setScale(-factorScaler * m_areaSize.height());
#else // ..and this if we want uniform scaling based on largest dimension
    m_axisCacheX.setScale(2.0f * aspectRatio);
    m_axisCacheZ.setScale(-m_axisCacheX.scale());
#endif
    m_axisCacheX.setTranslate(-m_axisCacheX.scale() / 2.0f);
    m_axisCacheZ.setTranslate(-m_axisCacheZ.scale() / 2.0f);
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

QVector4D Scatter3DRenderer::indexToSelectionColor(GLint index)
{
    GLubyte dotIdxRed = index & 0xff;
    GLubyte dotIdxGreen = (index & 0xff00) >> 8;
    GLubyte dotIdxBlue = (index & 0xff0000) >> 16;

    return QVector4D(dotIdxRed, dotIdxGreen, dotIdxBlue, 0);
}

void Scatter3DRenderer::selectionColorToSeriesAndIndex(const QVector4D &color,
                                                       int &index,
                                                       QAbstract3DSeries *&series)
{
    m_clickedType = QAbstract3DGraph::ElementNone;
    if (color != selectionSkipColor) {
        qDebug() << __FUNCTION__ << color.w();
        if (color.w() == labelRowAlpha) {
            // Row selection
            index = Scatter3DController::invalidSelectionIndex();
            m_clickedType = QAbstract3DGraph::ElementAxisZLabel;
        } else if (color.w() == labelColumnAlpha) {
            // Column selection
            index = Scatter3DController::invalidSelectionIndex();
            m_clickedType = QAbstract3DGraph::ElementAxisXLabel;
        } else if (color.w() == labelValueAlpha) {
            // Value selection
            index = Scatter3DController::invalidSelectionIndex();
            m_clickedType = QAbstract3DGraph::ElementAxisYLabel;
        } else if (color.w() == customItemAlpha) {
            // Custom item selection
            index = Scatter3DController::invalidSelectionIndex();
            m_clickedType = QAbstract3DGraph::ElementCustomItem;
            qDebug() << "custom item selected";
        } else {
            int totalIndex = int(color.x())
                    + (int(color.y()) << 8)
                    + (int(color.z()) << 16);
            // Find the series and adjust the index accordingly
            foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
                if (baseCache->isVisible()) {
                    ScatterSeriesRenderCache *cache =
                            static_cast<ScatterSeriesRenderCache *>(baseCache);
                    int offset = cache->selectionIndexOffset();
                    if (totalIndex >= offset
                            && totalIndex < (offset + cache->renderArray().size())) {
                        index = totalIndex - offset;
                        series = cache->series();
                        m_clickedType = QAbstract3DGraph::ElementSeries;
                        return;
                    }
                }
            }
        }
    }

    // No valid match found
    index = Scatter3DController::invalidSelectionIndex();
    series = 0;
}

void Scatter3DRenderer::addCustomItem(CustomDataItem *item) {
    CustomRenderItem *newItem = new CustomRenderItem();
    newItem->setMesh(item->m_meshFile);
    newItem->setScaling(item->m_scaling);
    newItem->setRotation(item->m_rotation);
    newItem->setTexture(item->m_texture);
    const QVector3D &pos = item->m_position;
    float xTrans = m_axisCacheX.positionAt(pos.x());
    float yTrans = m_axisCacheY.positionAt(pos.y());
    float zTrans = m_axisCacheZ.positionAt(pos.z());
    newItem->setTranslation(QVector3D(xTrans, yTrans, zTrans));
    m_customRenderCache.append(newItem);
    qDebug() << __FUNCTION__ << item->m_meshFile << newItem->rotation() << newItem->scaling() << newItem->translation();
}

QT_END_NAMESPACE_DATAVISUALIZATION
