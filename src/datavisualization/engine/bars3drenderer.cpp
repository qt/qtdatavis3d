/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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

#include "bars3drenderer_p.h"
#include "bars3dcontroller_p.h"
#include "q3dcamera_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"
#include "theme_p.h"
#include "utils_p.h"
#include "drawer_p.h"
#include "qbardataitem.h"
#include "q3dlight.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QThread>
#include <qmath.h>
#include <QDebug>

// You can verify that depth buffer drawing works correctly by uncommenting this.
// You should see the scene from  where the light is
//#define SHOW_DEPTH_TEXTURE_SCENE

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

const GLfloat labelMargin = 0.05f;
const GLfloat gridLineWidth = 0.005f;
const int smallerVPSize = 5;

Bars3DRenderer::Bars3DRenderer(Bars3DController *controller)
    : Abstract3DRenderer(controller),
      m_controller(controller),
      m_cachedIsSlicingActivated(false),
      m_cachedRowCount(0),
      m_cachedColumnCount(0),
      m_cachedInputState(QDataVis::InputStateNone),
      m_selectedBar(0),
      m_sliceSelection(0),
      m_sliceCache(0),
      m_sliceTitleItem(0),
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
      m_bgrTexture(0),
      m_depthTexture(0),
      m_selectionTexture(0),
      m_depthFrameBuffer(0),
      m_selectionFrameBuffer(0),
      m_selectionDepthBuffer(0),
      m_shadowQualityToShader(100.0f),
      m_shadowQualityMultiplier(3),
      m_heightNormalizer(1.0f),
      m_yAdjustment(0.0f),
      m_rowWidth(0),
      m_columnDepth(0),
      m_maxDimension(0),
      m_scaleX(0),
      m_scaleZ(0),
      m_scaleFactor(0),
      m_maxSceneSize(40.0f),
      m_visualSelectedBarPos(Bars3DController::noSelectionPoint()),
      m_clickedBarColor(invalidColorVector),
      m_hasHeightAdjustmentChanged(true),
      m_selectedBarPos(Bars3DController::noSelectionPoint())
{
    initializeOpenGLFunctions();
    initializeOpenGL();
}

Bars3DRenderer::~Bars3DRenderer()
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
    delete m_labelObj;
    delete m_labelShader;
}

void Bars3DRenderer::initializeOpenGL()
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
    glViewport(m_sliceViewPort.x(), m_sliceViewPort.y(),
               m_sliceViewPort.width(), m_sliceViewPort.height());

    // Load background mesh (we need to be initialized first)
    loadBackgroundMesh();
}

void Bars3DRenderer::updateDataModel(QBarDataProxy *dataProxy)
{
    int minRow = m_axisCacheX.min();
    int maxRow = m_axisCacheX.max();
    int minCol = m_axisCacheZ.min();
    int maxCol = m_axisCacheZ.max();
    int newRows = maxRow - minRow + 1;
    int newColumns = maxCol - minCol + 1;
    if (newRows != m_renderItemArray.size() || newColumns != m_renderItemArray.at(0).size()) {
        // Destroy old render items and reallocate new array
        m_renderItemArray.clear();
        m_renderItemArray.resize(newRows);
        for (int i = 0; i < newRows; i++)
            m_renderItemArray[i].resize(newColumns);

        // Force update for selection related items
        m_sliceCache = 0;
        m_sliceTitleItem = 0;
        if (m_sliceSelection)
            m_sliceSelection->clear();

        m_cachedColumnCount = newColumns;
        m_cachedRowCount = newRows;
        // Calculate max scene size
        GLfloat sceneRatio = qMin(GLfloat(newColumns) / GLfloat(newRows),
                                  GLfloat(newRows) / GLfloat(newColumns));
        m_maxSceneSize = 2.0f * qSqrt(sceneRatio * newColumns * newRows);
        // Calculate here and at setting bar specs
        calculateSceneScalingFactors();
    }

    // Update cached data window
    int dataRowCount = dataProxy->rowCount();
    int dataRowIndex = minRow;
    int updateSize = 0;
    for (int i = 0; i < newRows; i++) {
        int j = 0;
        if (dataRowIndex < dataRowCount) {
            const QBarDataRow *dataRow = dataProxy->rowAt(dataRowIndex);
            updateSize = qMin((dataRow->size() - minCol), m_renderItemArray[i].size());
            if (dataRow) {
                int dataColIndex = minCol;
                for (; j < updateSize ; j++) {
                    qreal value = dataRow->at(dataColIndex).value();
                    m_renderItemArray[i][j].setValue(value);
                    m_renderItemArray[i][j].setHeight(GLfloat(value) / m_heightNormalizer);
                    dataColIndex++;
                }
            }
        }
        for (; j < m_renderItemArray[i].size(); j++) {
            m_renderItemArray[i][j].setValue(0.0);
            m_renderItemArray[i][j].setHeight(0.0f);
        }
        dataRowIndex++;
    }

    m_renderColumns = updateSize;
    m_renderRows = qMin((dataRowCount - minRow), m_renderItemArray.size());

    // Reset selected bar to update selection
    updateSelectedBar(m_selectedBarPos);

    Abstract3DRenderer::updateDataModel(dataProxy);
}

void Bars3DRenderer::updateScene(Q3DScene *scene)
{
    // TODO: Move these to more suitable place e.g. controller should be controlling the viewports.
    scene->setSecondarySubViewport(m_sliceViewPort);
    scene->setPrimarySubViewport(m_mainViewPort);

    // TODO: See QTRD-2374
    if (m_hasNegativeValues)
        scene->activeCamera()->setMinYRotation(-90.0);
    else
        scene->activeCamera()->setMinYRotation(0.0);

    if (m_hasHeightAdjustmentChanged) {
        // Set initial camera position. Also update if height adjustment has changed.
        scene->activeCamera()->setBaseOrientation(cameraDistanceVector,
                                                  QVector3D(0.0f, -m_yAdjustment, 0.0f),
                                                  upVector);
        m_hasHeightAdjustmentChanged = false;
    }

    scene->activeCamera()->d_ptr->updateViewMatrix(m_autoScaleAdjustment);
    // Set light position (rotate light with camera, a bit above it (as set in defaultLightPos))
    scene->setLightPositionRelativeToCamera(defaultLightPos);

    Abstract3DRenderer::updateScene(scene);

    updateSlicingActive(scene->isSlicingActive());
}

void Bars3DRenderer::render(GLuint defaultFboHandle)
{
    bool slicingChanged = m_cachedIsSlicingActivated != m_cachedScene->isSlicingActive();

    // TODO: Can't call back to controller here! (QTRD-2216)
    // TODO: Needs to be added to synchronization
    QDataVis::InputState currentInputState = m_controller->inputState();
    if (currentInputState != m_cachedInputState) {
        if (currentInputState == QDataVis::InputStateOnScene)
            m_clickedBarColor = invalidColorVector;
        m_cachedInputState = currentInputState;
    }

    // Handle GL state setup for FBO buffers and clearing of the render surface
    Abstract3DRenderer::render(defaultFboHandle);

    // Draw bars scene
    drawScene(defaultFboHandle);

    // If slice selection is on, draw the sliced scene
    if (m_cachedIsSlicingActivated)
        drawSlicedScene(m_axisCacheX.titleItem(), m_axisCacheY.titleItem(), m_axisCacheZ.titleItem());

    // Trigger another render always when slicing changes in general to ensure
    // final draw is correct.
    if (slicingChanged)
        emit needRender();
}

void Bars3DRenderer::drawSlicedScene(const LabelItem &xLabel,
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
    projectionMatrix.perspective(40.0f, (GLfloat)m_sliceViewPort.width()
                                 / (GLfloat)m_sliceViewPort.height(), 0.1f, 10.0f);

    // Set view matrix
    QMatrix4x4 viewMatrix;

    // Adjust scaling (zoom rate based on aspect ratio)
    GLfloat camZPosSliced = cameraDistance / m_autoScaleAdjustment;

    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, camZPosSliced), zeroVector, upVector);

    // Set light position
    lightPos = QVector3D(0.0f, -m_yAdjustment, camZPosSliced * 2.0f);

    // Bind bar shader
    m_barShader->bind();

    // Set common bar shader bindings
    m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
    m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
    m_barShader->setUniformValue(m_barShader->lightS(), 0.5f);
    m_barShader->setUniformValue(m_barShader->ambientS(),
                                 m_cachedTheme.m_ambientStrength * 2.0f);

    // Draw the selected row / column
    GLfloat barPosYAdjustment = 0.2f; // Add a bit of space for labels
    if (m_hasNegativeValues)
        barPosYAdjustment += m_yAdjustment / 2.0f;
    QVector3D modelMatrixScaler(m_scaleX, 0.0f, m_scaleZ);
    QMatrix4x4 projectionViewMatrix = projectionMatrix * viewMatrix;
    QVector3D barHighlightColor(Utils::vectorFromColor(m_cachedTheme.m_highlightBarColor));
    QVector3D rowHighlightColor(Utils::vectorFromColor(m_cachedTheme.m_highlightRowColor));
    QVector3D columnHighlightColor(Utils::vectorFromColor(m_cachedTheme.m_highlightColumnColor));
    bool rowMode = m_cachedSelectionMode.testFlag(QDataVis::SelectionRow);
    bool itemMode = m_cachedSelectionMode.testFlag(QDataVis::SelectionItem);

    for (int bar = startBar; bar != stopBar; bar += stepBar) {
        BarRenderItem *item = m_sliceSelection->at(bar);
        if (!item)
            continue;

        if (item->height() < 0)
            glCullFace(GL_FRONT);
        else
            glCullFace(GL_BACK);

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 itModelMatrix;

        GLfloat barPosY = item->translation().y() + barPosYAdjustment;
        if (rowMode)
            barPosX = item->translation().x();
        else
            barPosX = -(item->translation().z()); // flip z; frontmost bar to the left
        modelMatrix.translate(barPosX, barPosY, 0.0f);
        modelMatrixScaler.setY(item->height());
        modelMatrix.scale(modelMatrixScaler);
        itModelMatrix.scale(modelMatrixScaler);

        MVPMatrix = projectionViewMatrix * modelMatrix;

#if 0
        QVector3D baseColor;
        if (m_visualSelectedBarPos.x() == item->position().x()
                && m_visualSelectedBarPos.y() == item->position().y()) {
            baseColor = barHighlightColor;
        } else if (QDataVis::SelectionSliceRow == m_cachedSelectionMode) {
            baseColor = rowHighlightColor;
        } else {
            baseColor = columnHighlightColor;
        }

        QVector3D heightColor = Utils::vectorFromColor(m_cachedTheme.m_heightColor) * item->height();
        QVector3D barColor = baseColor + heightColor;
#else
        QVector3D barColor;
        if (itemMode && m_visualSelectedBarPos.x() == item->position().x()
                && m_visualSelectedBarPos.y() == item->position().y()) {
            barColor = barHighlightColor;
        } else if (rowMode) {
            barColor = rowHighlightColor;
        } else {
            barColor = columnHighlightColor;
        }
#endif

        if (item->height() != 0) {
            // Set shader bindings
            m_barShader->setUniformValue(m_barShader->model(), modelMatrix);
            m_barShader->setUniformValue(m_barShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_barShader->setUniformValue(m_barShader->MVP(), MVPMatrix);
            m_barShader->setUniformValue(m_barShader->color(), barColor);

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    BarRenderItem *dummyItem(0);
    const LabelItem &sliceSelectionLabel = *m_sliceTitleItem;
    QVector3D positionComp(0.0f, m_autoScaleAdjustment, 0.0f);
    const Q3DCamera *activeCamera = m_cachedScene->activeCamera();

    // Draw labels for bars
    QVector3D valuePositionComp = zeroVector;
    if (m_hasNegativeValues)
        valuePositionComp.setY(-barPosYAdjustment);
    QVector3D sliceValueRotation(0.0f, 0.0f, 90.0f);
    QVector3D sliceLabelRotation(0.0f, 0.0f, -45.0f);

    // Labels in axis caches can be in inverted order depending in orientation
    bool flipped = (m_xFlipped && rowMode) || (m_zFlipped && !rowMode);

    for (int col = 0; col < stopBar; col++) {
        BarRenderItem *item = m_sliceSelection->at(col);
        // Draw values
        m_drawer->drawLabel(*item, item->sliceLabelItem(), viewMatrix, projectionMatrix,
                            valuePositionComp, sliceValueRotation, item->height(),
                            m_cachedSelectionMode, m_labelShader, m_labelObj, activeCamera,
                            false, false, Drawer::LabelOver, Qt::AlignTop, true);

        // Draw labels
        if (m_sliceCache->labelItems().size() > col) {
            int labelIndex = flipped ? m_sliceCache->labelItems().size() - 1 - col : col;
            m_drawer->drawLabel(*item, *m_sliceCache->labelItems().at(labelIndex), viewMatrix,
                                projectionMatrix, positionComp, sliceLabelRotation,
                                item->height(), m_cachedSelectionMode, m_labelShader,
                                m_labelObj, activeCamera, false, false, Drawer::LabelBelow,
                                Qt::AlignCenter, true);
        }
    }

    // Draw labels for axes
    if (rowMode) {
        if (m_sliceTitleItem) {
            m_drawer->drawLabel(*dummyItem, sliceSelectionLabel, viewMatrix, projectionMatrix,
                                positionComp, zeroVector, 0, m_cachedSelectionMode, m_labelShader,
                                m_labelObj, activeCamera, false, false, Drawer::LabelTop,
                                Qt::AlignCenter, true);
        }
        m_drawer->drawLabel(*dummyItem, zLabel, viewMatrix, projectionMatrix,
                            positionComp, zeroVector, 0, m_cachedSelectionMode, m_labelShader,
                            m_labelObj, activeCamera, false, false, Drawer::LabelBottom,
                            Qt::AlignCenter, true);
    } else {
        m_drawer->drawLabel(*dummyItem, xLabel, viewMatrix, projectionMatrix,
                            positionComp, zeroVector, 0, m_cachedSelectionMode, m_labelShader,
                            m_labelObj, activeCamera, false, false, Drawer::LabelBottom,
                            Qt::AlignCenter, true);
        if (m_sliceTitleItem) {
            m_drawer->drawLabel(*dummyItem, sliceSelectionLabel, viewMatrix, projectionMatrix,
                                positionComp, zeroVector, 0, m_cachedSelectionMode, m_labelShader,
                                m_labelObj, activeCamera, false, false, Drawer::LabelTop,
                                Qt::AlignCenter, true);
        }
    }
    m_drawer->drawLabel(*dummyItem, yLabel, viewMatrix, projectionMatrix,
                        positionComp, QVector3D(0.0f, 0.0f, 90.0f), 0,
                        m_cachedSelectionMode, m_labelShader, m_labelObj, activeCamera,
                        false, false, Drawer::LabelLeft, Qt::AlignCenter, true);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // Release label shader
    m_labelShader->release();
}

void Bars3DRenderer::drawScene(GLuint defaultFboHandle)
{
    GLint startBar = 0;
    GLint stopBar = 0;
    GLint stepBar = 0;

    GLint startRow = 0;
    GLint stopRow = 0;
    GLint stepRow = 0;

    GLfloat backgroundRotation = 0;

    GLfloat colPos = 0;
    GLfloat rowPos = 0;

    const Q3DCamera *activeCamera = m_cachedScene->activeCamera();

    // Specify viewport
    glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
               m_mainViewPort.width(), m_mainViewPort.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    GLfloat viewPortRatio = (GLfloat)m_mainViewPort.width() / (GLfloat)m_mainViewPort.height();
    projectionMatrix.perspective(45.0f, viewPortRatio, 0.1f, 100.0f);

    // Get the view matrix
    QMatrix4x4 viewMatrix = activeCamera->viewMatrix();

    // Calculate drawing order
    // Draw order is reversed to optimize amount of drawing (ie. draw front objects first,
    // depth test handles not needing to draw objects behind them)
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

    // Get light position from the scene
    QVector3D lightPos =  m_cachedScene->activeLight()->position();

    // Skip depth rendering if we're in slice mode
    // Introduce regardless of shadow quality to simplify logic
    QMatrix4x4 depthViewMatrix;
    QMatrix4x4 depthProjectionMatrix;
    QMatrix4x4 depthProjectionViewMatrix;

    QMatrix4x4 projectionViewMatrix = projectionMatrix * viewMatrix;

    bool rowMode = m_cachedSelectionMode.testFlag(QDataVis::SelectionRow);

#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
        // Render scene into a depth texture for using with shadow mapping
        // Enable drawing to depth framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthFrameBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Bind depth shader
        m_depthShader->bind();

        // Set viewport for depth map rendering. Must match texture size. Larger values give smoother shadows.
        // Depth viewport must always start from 0, 0, as it is rendered into a texture, not screen
        glViewport(0, 0,
                   m_mainViewPort.width() * m_shadowQualityMultiplier,
                   m_mainViewPort.height() * m_shadowQualityMultiplier);

        // Get the depth view matrix
        // It may be possible to hack lightPos here if we want to make some tweaks to shadow
        QVector3D depthLightPos = activeCamera->calculatePositionRelativeToCamera(
                    zeroVector, 0.0f, 3.5f / m_autoScaleAdjustment);
        depthViewMatrix.lookAt(depthLightPos, QVector3D(0.0f, -m_yAdjustment, 0.0f),
                               upVector);

        // TODO: Why does depthViewMatrix.column(3).y() goes to zero when we're directly above?
        // That causes the scene to be not drawn from above -> must be fixed
        // qDebug() << lightPos << depthViewMatrix << depthViewMatrix.column(3);

        // Set the depth projection matrix
        depthProjectionMatrix.perspective(10.0f, viewPortRatio, 3.0f, 100.0f);
        depthProjectionViewMatrix = depthProjectionMatrix * depthViewMatrix;

        // Draw bars to depth buffer
        QVector3D shadowScaler(m_scaleX * 0.9f, 0.0f, m_scaleZ * 0.9f);
        for (int row = startRow; row != stopRow; row += stepRow) {
            for (int bar = startBar; bar != stopBar; bar += stepBar) {
                const BarRenderItem &item = m_renderItemArray.at(row).at(bar);
                if (!item.value())
                    continue;

                GLfloat shadowOffset = 0.0f;

                // Set front face culling for negative valued bars and back face culling for
                // positive valued bars to remove peter-panning issues
                if (item.height() > 0) {
                    glCullFace(GL_BACK);
                    if (m_yFlipped)
                        shadowOffset = 0.015f;
                } else {
                    glCullFace(GL_FRONT);
                    if (!m_yFlipped)
                        shadowOffset = -0.015f;
                }

                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;

                colPos = (bar + 0.5f) * (m_cachedBarSpacing.width());
                rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());

                // Draw shadows for bars "on the other side" a bit off ground to avoid seeing
                // shadows through the ground
                modelMatrix.translate((colPos - m_rowWidth) / m_scaleFactor,
                                      item.height() - m_yAdjustment + shadowOffset,
                                      (m_columnDepth - rowPos) / m_scaleFactor);
                // Scale the bars down in X and Z to reduce self-shadowing issues
                shadowScaler.setY(item.height());
                modelMatrix.scale(shadowScaler);

                MVPMatrix = depthProjectionViewMatrix * modelMatrix;

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
        viewmatrix.lookAt(QVector3D(0.0f, 0.0f, 2.5f), zeroVector, upVector);
        QMatrix4x4 MVPMatrix = projectionViewMatrix * modelMatrix;
        m_labelShader->setUniformValue(m_labelShader->MVP(), MVPMatrix);
        m_drawer->drawObject(m_labelShader, m_labelObj, m_depthTexture);
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

    // TODO: Selection must be enabled currently to support clicked signal. (QTRD-2517)
    // Skip selection mode drawing if we're slicing or have no selection mode
    if (!m_cachedIsSlicingActivated && m_cachedSelectionMode > QDataVis::SelectionNone
            && m_cachedInputState == QDataVis::InputStateOnScene) {
        // Bind selection shader
        m_selectionShader->bind();

        // Draw bars to selection buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (= selectionSkipColor)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled
        for (int row = startRow; row != stopRow; row += stepRow) {
            for (int bar = startBar; bar != stopBar; bar += stepBar) {
                const BarRenderItem &item = m_renderItemArray.at(row).at(bar);
                if (!item.value())
                    continue;

                if (item.height() < 0)
                    glCullFace(GL_FRONT);
                else
                    glCullFace(GL_BACK);

                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;

                colPos = (bar + 0.5f) * (m_cachedBarSpacing.width());
                rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());

                modelMatrix.translate((colPos - m_rowWidth) / m_scaleFactor,
                                      item.height() - m_yAdjustment,
                                      (m_columnDepth - rowPos) / m_scaleFactor);
                modelMatrix.scale(QVector3D(m_scaleX, item.height(), m_scaleZ));

                MVPMatrix = projectionViewMatrix * modelMatrix;

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
        // TODO: Can't call back to controller here! (QTRD-2216)
        QVector3D clickedColor = Utils::getSelection(m_controller->inputPosition(),
                                                     m_cachedBoundingRect.height());
        if (m_clickedBarColor == invalidColorVector) {
            m_clickedBarColor = clickedColor;
            emit barClicked(selectionColorToArrayPosition(m_clickedBarColor));
        }

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
        viewmatrix.lookAt(QVector3D(0.0f, 0.0f, 2.0f), zeroVector, upVector);
        QMatrix4x4 MVPMatrix = projectionViewMatrix * modelMatrix;
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

    // Set common bar shader bindings
    m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
    m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
    m_barShader->setUniformValue(m_barShader->ambientS(),
                                 m_cachedTheme.m_ambientStrength);


    // TODO: Can't call back to controller here! (QTRD-2216)
    if (m_selectionDirty) {
        if (m_cachedIsSlicingActivated) {
            if (m_sliceSelection && m_sliceSelection->size()) {
                // Slice doesn't own its items, no need to delete them - just clear
                m_sliceSelection->clear();
            }
            // Set slice cache, i.e. axis cache from where slice labels are taken
            if (rowMode)
                m_sliceCache = &m_axisCacheZ;
            else
                m_sliceCache = &m_axisCacheX;
            m_sliceTitleItem = 0;
        }
    }

    // Draw bars
    QVector3D barHighlightColor(Utils::vectorFromColor(m_cachedTheme.m_highlightBarColor));
    QVector3D rowHighlightColor(Utils::vectorFromColor(m_cachedTheme.m_highlightRowColor));
    QVector3D columnHighlightColor(Utils::vectorFromColor(m_cachedTheme.m_highlightColumnColor));
    QVector3D baseColor(Utils::vectorFromColor(m_cachedTheme.m_baseColor));

    GLfloat adjustedLightStrength = m_cachedTheme.m_lightStrength / 10.0f;
    GLfloat adjustedHighlightStrength = m_cachedTheme.m_highlightLightStrength / 10.0f;

    bool barSelectionFound = false;
    BarRenderItem *selectedBar(0);
    QVector3D modelScaler(m_scaleX, 0.0f, m_scaleZ);
    bool somethingSelected = (m_visualSelectedBarPos != Bars3DController::noSelectionPoint());
    for (int row = startRow; row != stopRow; row += stepRow) {
        for (int bar = startBar; bar != stopBar; bar += stepBar) {
            BarRenderItem &item = m_renderItemArray[row][bar];

            if (item.height() < 0)
                glCullFace(GL_FRONT);
            else
                glCullFace(GL_BACK);

            QMatrix4x4 modelMatrix;
            QMatrix4x4 itModelMatrix;
            QMatrix4x4 MVPMatrix;

            colPos = (bar + 0.5f) * (m_cachedBarSpacing.width());
            rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());

            modelMatrix.translate((colPos - m_rowWidth) / m_scaleFactor,
                                  item.height() - m_yAdjustment,
                                  (m_columnDepth - rowPos) / m_scaleFactor);
            modelScaler.setY(item.height());
            modelMatrix.scale(modelScaler);
            itModelMatrix.scale(modelScaler);
#ifdef SHOW_DEPTH_TEXTURE_SCENE
            MVPMatrix = depthProjectionViewMatrix * modelMatrix;
#else
            MVPMatrix = projectionViewMatrix * modelMatrix;
#endif

#if 0
            QVector3D heightColor = Utils::vectorFromColor(m_cachedTheme.m_heightColor)
                    * item.height();
            QVector3D depthColor = Utils::vectorFromColor(m_cachedTheme.m_depthColor)
                    * (float(row) / GLfloat(m_cachedRowCount));

            QVector3D barColor = baseColor + heightColor + depthColor;
#else
            QVector3D barColor = baseColor;
#endif

            GLfloat lightStrength = m_cachedTheme.m_lightStrength;
            GLfloat shadowLightStrength = adjustedLightStrength;

            if (m_cachedSelectionMode > QDataVis::SelectionNone) {
                Bars3DController::SelectionType selectionType = Bars3DController::SelectionNone;
                if (somethingSelected)
                    selectionType = isSelected(row, bar);

                switch (selectionType) {
                case Bars3DController::SelectionItem: {
                    barColor = barHighlightColor;
                    lightStrength = m_cachedTheme.m_highlightLightStrength;
                    shadowLightStrength = adjustedHighlightStrength;
                    // Insert position data into render item. We have no ownership, don't delete the previous one
                    if (!m_cachedIsSlicingActivated) {
                        selectedBar = &item;
                        selectedBar->setPosition(QPoint(row, bar));
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        barSelectionFound = true;
                    }
                    if (m_selectionDirty && m_cachedIsSlicingActivated) {
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        item.setPosition(QPoint(row, bar));
                        m_sliceSelection->append(&item);
                        barSelectionFound = true;
                    }
                    break;
                }
                case Bars3DController::SelectionRow: {
                    // Current bar is on the same row as the selected bar
                    barColor = rowHighlightColor;
                    lightStrength = m_cachedTheme.m_highlightLightStrength;
                    shadowLightStrength = adjustedHighlightStrength;
                    if (m_cachedIsSlicingActivated) {
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        item.setPosition(QPoint(row, bar));
                        if (m_selectionDirty && bar < m_renderColumns) {
                            if (!m_sliceTitleItem && m_axisCacheX.labelItems().size() > row)
                                m_sliceTitleItem = m_axisCacheX.labelItems().at(row);
                            m_sliceSelection->append(&item);
                        }
                    }
                    break;
                }
                case Bars3DController::SelectionColumn: {
                    // Current bar is on the same column as the selected bar
                    barColor = columnHighlightColor;
                    lightStrength = m_cachedTheme.m_highlightLightStrength;
                    shadowLightStrength = adjustedHighlightStrength;
                    if (m_cachedIsSlicingActivated) {
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        item.setPosition(QPoint(row, bar));
                        if (m_selectionDirty && row < m_renderRows) {
                            if (!m_sliceTitleItem && m_axisCacheZ.labelItems().size() > bar)
                                m_sliceTitleItem = m_axisCacheZ.labelItems().at(bar);
                            m_sliceSelection->append(&item);
                        }
                    }
                    break;
                }
                case Bars3DController::SelectionNone: {
                    // Current bar is not selected, nor on a row or column
                    // do nothing
                    break;
                }
                }
            }

            // Skip drawing of 0 -height bars
            if (item.height() != 0) {
                // Set shader bindings
                m_barShader->setUniformValue(m_barShader->model(), modelMatrix);
                m_barShader->setUniformValue(m_barShader->nModel(),
                                             itModelMatrix.transposed().inverted());
                m_barShader->setUniformValue(m_barShader->MVP(), MVPMatrix);
                m_barShader->setUniformValue(m_barShader->color(), barColor);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
                    // Set shadow shader bindings
                    QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                    m_barShader->setUniformValue(m_barShader->shadowQ(), m_shadowQualityToShader);
                    m_barShader->setUniformValue(m_barShader->depth(), depthMVPMatrix);
                    m_barShader->setUniformValue(m_barShader->lightS(), shadowLightStrength);

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

    // Reset culling
    glCullFace(GL_BACK);

    // Draw background
    GLfloat rowScaleFactor = m_rowWidth / m_scaleFactor;
    GLfloat columnScaleFactor = m_columnDepth / m_scaleFactor;

    if (m_cachedIsBackgroundEnabled && m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 itModelMatrix;

        QVector3D backgroundScaler(rowScaleFactor, 1.0f, columnScaleFactor);
        if (m_hasNegativeValues) {
            backgroundScaler.setY(0.5f);
            modelMatrix.translate(0.0f, m_yAdjustment - 0.5f, 0.0f);
        } else {
            modelMatrix.translate(0.0f, 1.0f - m_yAdjustment, 0.0f);
        }
        modelMatrix.scale(backgroundScaler);
        itModelMatrix.scale(backgroundScaler);
        modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);
        itModelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);

#ifdef SHOW_DEPTH_TEXTURE_SCENE
        MVPMatrix = depthProjectionViewMatrix * modelMatrix;
#else
        MVPMatrix = projectionViewMatrix * modelMatrix;
#endif
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
        if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
            // Set shadow shader bindings
            QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
            m_backgroundShader->setUniformValue(m_backgroundShader->shadowQ(),
                                                m_shadowQualityToShader);
            m_backgroundShader->setUniformValue(m_backgroundShader->depth(), depthMVPMatrix);
            m_backgroundShader->setUniformValue(m_backgroundShader->lightS(),
                                                adjustedLightStrength);

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

        // Draw floor for graph with negatives
        if (m_hasNegativeValues) {
            modelMatrix = QMatrix4x4();
            itModelMatrix = QMatrix4x4();

            modelMatrix.translate(0.0f, -m_yAdjustment, 0.0f);
            modelMatrix.scale(backgroundScaler);
            itModelMatrix.scale(backgroundScaler);

            if (m_yFlipped) {
                modelMatrix.rotate(90.0f, 1.0f, 0.0f, 0.0f);
                itModelMatrix.rotate(90.0f, 1.0f, 0.0f, 0.0f);
            } else {
                modelMatrix.rotate(-90.0f, 1.0f, 0.0f, 0.0f);
                itModelMatrix.rotate(-90.0f, 1.0f, 0.0f, 0.0f);
            }

#ifdef SHOW_DEPTH_TEXTURE_SCENE
            MVPMatrix = depthProjectionViewMatrix * modelMatrix;
#else
            MVPMatrix = projectionViewMatrix * modelMatrix;
#endif
            // Set changed shader bindings
            m_backgroundShader->setUniformValue(m_backgroundShader->model(), modelMatrix);
            m_backgroundShader->setUniformValue(m_backgroundShader->nModel(),
                                                itModelMatrix.inverted().transposed());
            m_backgroundShader->setUniformValue(m_backgroundShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
                // Set shadow shader bindings
                QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                m_backgroundShader->setUniformValue(m_backgroundShader->depth(), depthMVPMatrix);
                // Draw the object
                m_drawer->drawObject(m_backgroundShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Draw the object
                m_drawer->drawObject(m_backgroundShader, m_gridLineObj);
            }
        }
    }

    // Release background shader
    m_backgroundShader->release();

    // Disable textures
    glDisable(GL_TEXTURE_2D);

    // Draw grid lines
    if (m_cachedIsGridEnabled && m_heightNormalizer) {
        ShaderHelper *lineShader = m_backgroundShader;
        QQuaternion lineRotation = QQuaternion();

        // Bind bar shader
        lineShader->bind();

        // Set unchanging shader bindings
        QVector3D barColor = Utils::vectorFromColor(m_cachedTheme.m_gridLine);
        lineShader->setUniformValue(lineShader->lightP(), lightPos);
        lineShader->setUniformValue(lineShader->view(), viewMatrix);
        lineShader->setUniformValue(lineShader->color(), barColor);
        lineShader->setUniformValue(lineShader->ambientS(), m_cachedTheme.m_ambientStrength);
#if !defined(QT_OPENGL_ES_2)
        if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
            // Set shadowed shader bindings
            lineShader->setUniformValue(lineShader->shadowQ(), m_shadowQualityToShader);
            lineShader->setUniformValue(lineShader->lightS(),
                                        m_cachedTheme.m_lightStrength / 20.0f);
        } else
#endif
        {
            // Set shadowless shader bindings
            lineShader->setUniformValue(lineShader->lightS(), m_cachedTheme.m_lightStrength / 2.5f);
        }

        GLfloat yFloorLinePosition = -m_yAdjustment;
        if (m_yFlipped)
            yFloorLinePosition -= gridLineOffset;
        else
            yFloorLinePosition += gridLineOffset;

        QVector3D gridLineScaler(rowScaleFactor, gridLineWidth, gridLineWidth);

        if (m_yFlipped)
            lineRotation = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f);
        else
            lineRotation = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -90.0f);

        // Floor lines: rows
        for (GLfloat row = 0.0f; row <= m_cachedRowCount; row++) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 itModelMatrix;

            rowPos = row * m_cachedBarSpacing.height();
            modelMatrix.translate(0.0f, yFloorLinePosition,
                                  (m_columnDepth - rowPos) / m_scaleFactor);
            modelMatrix.scale(gridLineScaler);
            itModelMatrix.scale(gridLineScaler);
            modelMatrix.rotate(lineRotation);
            itModelMatrix.rotate(lineRotation);

            MVPMatrix = projectionViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            lineShader->setUniformValue(lineShader->model(), modelMatrix);
            lineShader->setUniformValue(lineShader->nModel(),
                                        itModelMatrix.inverted().transposed());
            lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
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
        }

        // Floor lines: columns
        gridLineScaler = QVector3D(gridLineWidth, gridLineWidth, columnScaleFactor);
        for (GLfloat bar = 0.0f; bar <= m_cachedColumnCount; bar++) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 itModelMatrix;

            colPos = bar * m_cachedBarSpacing.width();
            modelMatrix.translate((m_rowWidth - colPos) / m_scaleFactor,
                                  yFloorLinePosition, 0.0f);
            modelMatrix.scale(gridLineScaler);
            itModelMatrix.scale(gridLineScaler);
            modelMatrix.rotate(lineRotation);
            itModelMatrix.rotate(lineRotation);

            MVPMatrix = projectionViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            lineShader->setUniformValue(lineShader->model(), modelMatrix);
            lineShader->setUniformValue(lineShader->nModel(),
                                        itModelMatrix.inverted().transposed());
            lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
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
        }

        if (m_axisCacheY.segmentCount() > 0) {
            // Wall lines: back wall
            GLfloat heightStep = m_axisCacheY.subSegmentStep();
            GLfloat startLine = 0.0f;
            int segmentCount = m_axisCacheY.segmentCount() * m_axisCacheY.subSegmentCount();

            GLfloat zWallLinePosition = -columnScaleFactor + gridLineOffset;
            if (m_zFlipped)
                zWallLinePosition = -zWallLinePosition;

            if (m_hasNegativeValues)
                startLine = m_axisCacheY.min();

            GLfloat lineHeight = startLine;
            gridLineScaler = QVector3D(rowScaleFactor, gridLineWidth, gridLineWidth);
            for (int segment = 0; segment <= segmentCount; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(0.0f,
                                      2.0f * lineHeight / m_heightNormalizer - m_yAdjustment,
                                      zWallLinePosition);
                modelMatrix.scale(gridLineScaler);
                itModelMatrix.scale(gridLineScaler);
                if (m_zFlipped) {
                    modelMatrix.rotate(180.0f, 1.0, 0.0, 0.0);
                    itModelMatrix.rotate(180.0f, 1.0, 0.0, 0.0);
                }

                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
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
                lineHeight += heightStep;
            }

            // Wall lines: side wall
            GLfloat xWallLinePosition = -rowScaleFactor + gridLineOffset;
            if (m_xFlipped)
                xWallLinePosition = -xWallLinePosition;

            if (m_xFlipped)
                lineRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, -90.0f);
            else
                lineRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 90.0f);

            lineHeight = startLine;
            gridLineScaler = QVector3D(gridLineWidth, gridLineWidth, columnScaleFactor);
            for (int segment = 0; segment <= segmentCount; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(xWallLinePosition,
                                      2.0f * lineHeight / m_heightNormalizer - m_yAdjustment,
                                      0.0f);
                modelMatrix.scale(gridLineScaler);
                itModelMatrix.scale(gridLineScaler);
                modelMatrix.rotate(lineRotation);
                itModelMatrix.rotate(lineRotation);

                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
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
                lineHeight += heightStep;
            }
        }
        // Release bar shader
        lineShader->release();
    }

    // Bind label shader
    m_labelShader->bind();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_OFFSET_FILL);

    // Calculate the positions for row and column labels and store them
    GLfloat labelYAdjustment = -m_yAdjustment + 0.005f;
    GLfloat scaledRowWidth = rowScaleFactor;
    GLfloat scaledColumnDepth = columnScaleFactor;
    GLfloat colPosValue = scaledRowWidth + labelMargin;
    GLfloat rowPosValue = scaledColumnDepth + labelMargin;
    QVector3D positionComp(0.0f, m_yAdjustment, 0.0f);
    QVector3D labelRotation(-90.0f, 0.0f, 0.0f);
    if (m_zFlipped)
        labelRotation.setY(180.0f);
    if (m_yFlipped) {
        if (m_zFlipped)
            labelRotation.setY(0.0f);
        else
            labelRotation.setY(180.0f);
        labelRotation.setZ(180.0f);
    }

    Qt::AlignmentFlag alignment = m_xFlipped ? Qt::AlignLeft : Qt::AlignRight;
    for (int row = 0; row != m_cachedRowCount; row++) {
        if (m_axisCacheX.labelItems().size() > row) {
            // Go through all rows and get position of max+1 or min-1 column, depending on x flip
            // We need only positions for them, labels have already been generated at QDataSetPrivate. Just add LabelItems
            rowPos = (row + 0.5f) * m_cachedBarSpacing.height();
            if (m_xFlipped)
                colPos = -colPosValue;
            else
                colPos = colPosValue;

            glPolygonOffset(GLfloat(row) / -10.0f, 1.0f);

            QVector3D labelPos = QVector3D(colPos,
                                           labelYAdjustment, // raise a bit over background to avoid depth "glimmering"
                                           (m_columnDepth - rowPos) / m_scaleFactor);

            m_dummyBarRenderItem.setTranslation(labelPos);
            const LabelItem &axisLabelItem = *m_axisCacheX.labelItems().at(row);
            //qDebug() << "labelPos, row" << row + 1 << ":" << labelPos << m_axisCacheX.labels().at(row);

            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                positionComp, labelRotation, 0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, activeCamera,
                                true, true, Drawer::LabelMid, alignment, m_cachedIsSlicingActivated);
        }
    }
    labelRotation = QVector3D(-90.0f, 90.0f, 0.0f);
    if (m_xFlipped)
        labelRotation.setY(-90.0f);
    if (m_yFlipped) {
        if (m_xFlipped)
            labelRotation.setY(90.0f);
        else
            labelRotation.setY(-90.0f);
        labelRotation.setZ(180.0f);
    }

    alignment = m_zFlipped ? Qt::AlignRight : Qt::AlignLeft;
    for (int column = 0; column != m_cachedColumnCount; column++) {
        if (m_axisCacheZ.labelItems().size() > column) {
            // Go through all columns and get position of max+1 or min-1 row, depending on z flip
            // We need only positions for them, labels have already been generated at QDataSetPrivate. Just add LabelItems
            colPos = (column + 0.5f) * m_cachedBarSpacing.width();
            if (m_zFlipped)
                rowPos = -rowPosValue;
            else
                rowPos = rowPosValue;

            glPolygonOffset(GLfloat(column) / -10.0f, 1.0f);

            QVector3D labelPos = QVector3D((colPos - m_rowWidth) / m_scaleFactor,
                                           labelYAdjustment, // raise a bit over background to avoid depth "glimmering"
                                           rowPos);

            m_dummyBarRenderItem.setTranslation(labelPos);
            const LabelItem &axisLabelItem = *m_axisCacheZ.labelItems().at(column);

            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                positionComp, labelRotation, 0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, activeCamera,
                                true, true, Drawer::LabelMid, alignment);
        }
    }

    // Y Labels
    int labelNbr = 0;
    GLfloat heightStep = m_axisCacheY.segmentStep();
    GLfloat startLine = 0.0f;
    int labelCount = m_axisCacheY.labels().size();
    if (m_hasNegativeValues)
        startLine = m_axisCacheY.min();
    GLfloat labelPos = startLine;
    GLfloat labelMarginXTrans = labelMargin;
    GLfloat labelMarginZTrans = labelMargin;
    GLfloat labelXTrans = rowScaleFactor;
    GLfloat labelZTrans = columnScaleFactor;
    QVector3D backLabelRotation(0.0f, -90.0f, 0.0f);
    QVector3D sideLabelRotation(0.0f, 0.0f, 0.0f);
    Qt::AlignmentFlag backAlignment = Qt::AlignLeft;
    Qt::AlignmentFlag sideAlignment = Qt::AlignLeft;
    if (!m_xFlipped) {
        labelXTrans = -labelXTrans;
        labelMarginXTrans = -labelMargin;
        backLabelRotation.setY(90.0f);
        sideAlignment = Qt::AlignRight;
    }
    if (m_zFlipped) {
        labelZTrans = -labelZTrans;
        labelMarginZTrans = -labelMargin;
        backAlignment = Qt::AlignRight;
        sideLabelRotation.setY(180.f);
    }
    QVector3D backLabelTrans = QVector3D(labelXTrans, 0.0f,
                                         labelZTrans + labelMarginZTrans);
    QVector3D sideLabelTrans = QVector3D(-labelXTrans - labelMarginXTrans,
                                         0.0f, -labelZTrans);

    for (int i = 0; i < labelCount; i++) {
        if (m_axisCacheY.labelItems().size() > labelNbr) {
            backLabelTrans.setY(2.0f * labelPos / m_heightNormalizer - m_yAdjustment);
            sideLabelTrans.setY(backLabelTrans.y());

            glPolygonOffset(GLfloat(i) / -10.0f, 1.0f);

            const LabelItem &axisLabelItem = *m_axisCacheY.labelItems().at(labelNbr);

            // Back wall
            m_dummyBarRenderItem.setTranslation(backLabelTrans);
            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                positionComp, backLabelRotation, 0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, activeCamera,
                                true, true, Drawer::LabelMid, backAlignment);

            // Side wall
            m_dummyBarRenderItem.setTranslation(sideLabelTrans);
            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                positionComp, sideLabelRotation, 0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, activeCamera,
                                true, true, Drawer::LabelMid, sideAlignment);
        }
        labelNbr++;
        labelPos += heightStep;
    }
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Handle slice and bar label generation
    if (m_cachedIsSlicingActivated && m_selectionDirty) {
        // Create label textures
        for (int col = 0; col < m_sliceSelection->size(); col++) {
            BarRenderItem *item = m_sliceSelection->at(col);
            if (item->sliceLabel().isNull())
                item->setSliceLabel(generateValueLabel(m_axisCacheY.labelFormat(), item->value()));
            m_drawer->generateLabelItem(item->sliceLabelItem(), item->sliceLabel());
        }
    } else if (barSelectionFound) {
        // Print value of selected bar
        glDisable(GL_DEPTH_TEST);
        // Draw the selection label
        LabelItem &labelItem = selectedBar->selectionLabelItem();
        if (m_selectedBar != selectedBar || m_updateLabels || !labelItem.textureId()) {
            QString labelText = selectedBar->selectionLabel();
            if (labelText.isNull()) {
                static const QString rowIndexTag(QStringLiteral("@rowIdx"));
                static const QString rowLabelTag(QStringLiteral("@rowLabel"));
                static const QString rowTitleTag(QStringLiteral("@rowTitle"));
                static const QString colIndexTag(QStringLiteral("@colIdx"));
                static const QString colLabelTag(QStringLiteral("@colLabel"));
                static const QString colTitleTag(QStringLiteral("@colTitle"));
                static const QString valueTitleTag(QStringLiteral("@valueTitle"));
                static const QString valueLabelTag(QStringLiteral("@valueLabel"));

                // Custom format expects printf format specifier. There is no tag for it.
                labelText = generateValueLabel(itemLabelFormat(), selectedBar->value());

                int selBarPosX = selectedBar->position().x();
                int selBarPosY = selectedBar->position().y();
                labelText.replace(rowIndexTag, QString::number(selBarPosX));
                if (m_axisCacheX.labels().size() > selBarPosX)
                    labelText.replace(rowLabelTag, m_axisCacheX.labels().at(selBarPosX));
                else
                    labelText.replace(rowLabelTag, QString());
                labelText.replace(rowTitleTag, m_axisCacheX.title());
                labelText.replace(colIndexTag, QString::number(selBarPosY));
                if (m_axisCacheZ.labels().size() > selBarPosY)
                    labelText.replace(colLabelTag, m_axisCacheZ.labels().at(selBarPosY));
                else
                    labelText.replace(colLabelTag, QString());
                labelText.replace(colTitleTag, m_axisCacheZ.title());
                labelText.replace(valueTitleTag, m_axisCacheY.title());

                if (labelText.contains(valueLabelTag)) {
                    QString labelFormat = m_axisCacheY.labelFormat();
                    if (labelFormat.isEmpty())
                        labelFormat = Utils::defaultLabelFormat();
                    QString valueLabelText = generateValueLabel(labelFormat, selectedBar->value());
                    labelText.replace(valueLabelTag, valueLabelText);
                }

                selectedBar->setSelectionLabel(labelText);
            }
            m_drawer->generateLabelItem(labelItem, labelText);
            m_selectedBar = selectedBar;
        }

        m_drawer->drawLabel(*selectedBar, labelItem, viewMatrix, projectionMatrix,
                            positionComp, zeroVector, selectedBar->height(),
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, activeCamera, true, false);

        // Reset label update flag; they should have been updated when we get here
        m_updateLabels = false;

        glEnable(GL_DEPTH_TEST);
    } else {
        m_selectedBar = 0;
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    // Release label shader
    m_labelShader->release();

    m_selectionDirty = false;
}

void Bars3DRenderer::handleResize()
{
    if (m_cachedBoundingRect.width() == 0 || m_cachedBoundingRect.height() == 0)
        return;

    setViewPorts();

    Abstract3DRenderer::handleResize();
}

void Bars3DRenderer::updateBarSpecs(GLfloat thicknessRatio, const QSizeF &spacing, bool relative)
{
    // Convert ratio to QSizeF, as we need it in that format for autoscaling calculations
    m_cachedBarThickness.setWidth(1.0f);
    m_cachedBarThickness.setHeight(1.0f / thicknessRatio);

    if (relative) {
        m_cachedBarSpacing.setWidth((m_cachedBarThickness.width() * 2) * (spacing.width() + 1.0f));
        m_cachedBarSpacing.setHeight((m_cachedBarThickness.height() * 2) * (spacing.height() + 1.0f));
    } else {
        m_cachedBarSpacing = m_cachedBarThickness * 2 + spacing * 2;
    }

    // Calculate here and at setting sample space
    calculateSceneScalingFactors();
}

void Bars3DRenderer::updateAxisRange(Q3DAbstractAxis::AxisOrientation orientation, qreal min, qreal max)
{
    Abstract3DRenderer::updateAxisRange(orientation, min, max);

    if (orientation == Q3DAbstractAxis::AxisOrientationY) {
        calculateHeightAdjustment();
        // Check if we have negative values
        if (min < 0 && !m_hasNegativeValues) {
            m_hasNegativeValues = true;
            // Reload background
            loadBackgroundMesh();
            emit needRender();
        } else if (min >= 0 && m_hasNegativeValues) {
            m_hasNegativeValues = false;
            // Reload background
            loadBackgroundMesh();
            emit needRender();
        }
    }
}

void Bars3DRenderer::updateBackgroundEnabled(bool enable)
{
    if (enable != m_cachedIsBackgroundEnabled) {
        Abstract3DRenderer::updateBackgroundEnabled(enable);
        loadMeshFile(); // Load changed bar type
    }
}

void Bars3DRenderer::updateSelectedBar(const QPoint &position)
{
    m_selectedBarPos = position;

    int adjustedX = m_selectedBarPos.x() - int(m_axisCacheX.min());
    int adjustedZ = m_selectedBarPos.y() - int(m_axisCacheZ.min());
    int maxX = m_renderItemArray.size() - 1;
    int maxZ = maxX >= 0 ? m_renderItemArray.at(0).size() - 1 : -1;

    if (m_selectedBarPos == Bars3DController::noSelectionPoint()
            || adjustedX < 0 || adjustedX > maxX
            || adjustedZ < 0 || adjustedZ > maxZ) {
        m_visualSelectedBarPos = Bars3DController::noSelectionPoint();
    } else {
        m_visualSelectedBarPos = QPoint(adjustedX, adjustedZ);
    }
    m_selectionDirty = true;
}

void Bars3DRenderer::updateShadowQuality(QDataVis::ShadowQuality quality)
{
    m_cachedShadowQuality = quality;
    switch (quality) {
    case QDataVis::ShadowQualityLow:
        m_shadowQualityToShader = 33.3f;
        m_shadowQualityMultiplier = 1;
        break;
    case QDataVis::ShadowQualityMedium:
        m_shadowQualityToShader = 100.0f;
        m_shadowQualityMultiplier = 3;
        break;
    case QDataVis::ShadowQualityHigh:
        m_shadowQualityToShader = 200.0f;
        m_shadowQualityMultiplier = 5;
        break;
    case QDataVis::ShadowQualitySoftLow:
        m_shadowQualityToShader = 7.5f;
        m_shadowQualityMultiplier = 1;
        break;
    case QDataVis::ShadowQualitySoftMedium:
        m_shadowQualityToShader = 10.0f;
        m_shadowQualityMultiplier = 3;
        break;
    case QDataVis::ShadowQualitySoftHigh:
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

void Bars3DRenderer::loadMeshFile()
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

void Bars3DRenderer::loadBackgroundMesh()
{
    if (m_backgroundObj)
        delete m_backgroundObj;
    if (m_hasNegativeValues)
        m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/negativeBackground"));
    else
        m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/background"));
    m_backgroundObj->load();
}

void Bars3DRenderer::loadGridLineMesh()
{
    if (m_gridLineObj)
        delete m_gridLineObj;
    m_gridLineObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/plane"));
    m_gridLineObj->load();
}

void Bars3DRenderer::loadLabelMesh()
{
    if (m_labelObj)
        delete m_labelObj;
    m_labelObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/plane"));
    m_labelObj->load();
}

void Bars3DRenderer::updateTextures()
{
    // Drawer has changed; this flag needs to be checked when checking if we need to update labels
    m_updateLabels = true;
}

void Bars3DRenderer::calculateSceneScalingFactors()
{
    // Calculate scene scaling and translation factors
    m_rowWidth = (m_cachedColumnCount * m_cachedBarSpacing.width()) / 2.0f;
    m_columnDepth = (m_cachedRowCount * m_cachedBarSpacing.height()) / 2.0f;
    m_maxDimension = qMax(m_rowWidth, m_columnDepth);
    m_scaleFactor = qMin((m_cachedColumnCount * (m_maxDimension / m_maxSceneSize)),
                         (m_cachedRowCount * (m_maxDimension / m_maxSceneSize)));
    m_scaleX = m_cachedBarThickness.width() / m_scaleFactor;
    m_scaleZ = m_cachedBarThickness.height() / m_scaleFactor;
}

void Bars3DRenderer::calculateHeightAdjustment()
{
    m_heightNormalizer = GLfloat(qFabs(m_axisCacheY.min()) + qFabs(m_axisCacheY.max()));
    GLfloat maxAbs = qFabs(m_axisCacheY.max());

    GLfloat newAdjustment = maxAbs / m_heightNormalizer;
    if (newAdjustment != m_yAdjustment) {
        m_hasHeightAdjustmentChanged = true;
        m_yAdjustment = newAdjustment;
    }
}

Bars3DController::SelectionType Bars3DRenderer::isSelected(GLint row, GLint bar)
{
    Bars3DController::SelectionType isSelectedType = Bars3DController::SelectionNone;

    if (row == m_visualSelectedBarPos.x() && bar == m_visualSelectedBarPos.y()
            && (m_cachedSelectionMode.testFlag(QDataVis::SelectionItem))) {
        isSelectedType = Bars3DController::SelectionItem;
    } else if (row == m_visualSelectedBarPos.x()
             && (m_cachedSelectionMode.testFlag(QDataVis::SelectionRow))) {
        isSelectedType = Bars3DController::SelectionRow;
    } else if (bar == m_visualSelectedBarPos.y()
             && (m_cachedSelectionMode.testFlag(QDataVis::SelectionColumn))) {
        isSelectedType = Bars3DController::SelectionColumn;
    }
    return isSelectedType;
}

QPoint Bars3DRenderer::selectionColorToArrayPosition(const QVector3D &selectionColor)
{
    QPoint position;
    if (selectionColor == selectionSkipColor) {
        position = Bars3DController::noSelectionPoint();
    } else {
        position = QPoint(int(selectionColor.x() + int(m_axisCacheX.min())),
                          int(selectionColor.y()) + int(m_axisCacheZ.min()));
    }
    return position;
}

void Bars3DRenderer::updateSlicingActive(bool isSlicing)
{
    if (isSlicing == m_cachedIsSlicingActivated)
        return;

    m_cachedIsSlicingActivated = isSlicing;

    if (m_cachedIsSlicingActivated && !m_sliceSelection)
        m_sliceSelection = new QList<BarRenderItem *>;

    setViewPorts();

    if (!m_cachedIsSlicingActivated)
        initSelectionBuffer(); // We need to re-init selection buffer in case there has been a resize

#if !defined(QT_OPENGL_ES_2)
    updateDepthBuffer(); // Re-init depth buffer as well
#endif

    m_selectionDirty = true;
}

void Bars3DRenderer::setViewPorts()
{
    // Update view ports
    if (m_cachedIsSlicingActivated) {
        m_mainViewPort = QRect(0,
                               m_cachedBoundingRect.height()
                               - (m_cachedBoundingRect.height() / smallerVPSize),
                               m_cachedBoundingRect.width() / smallerVPSize,
                               m_cachedBoundingRect.height() / smallerVPSize);
        m_sliceViewPort = QRect(0, 0, m_cachedBoundingRect.width(), m_cachedBoundingRect.height());
    } else {
        m_mainViewPort = QRect(0, 0, m_cachedBoundingRect.width(), m_cachedBoundingRect.height());
        m_sliceViewPort = QRect(0, 0, 0, 0);
    }
}

QRect Bars3DRenderer::mainViewPort()
{
    return m_mainViewPort;
}

void Bars3DRenderer::initShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_barShader)
        delete m_barShader;
    m_barShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_barShader->initialize();
}

void Bars3DRenderer::initSelectionShader()
{
    if (m_selectionShader)
        delete m_selectionShader;
    m_selectionShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexSelection"),
                                         QStringLiteral(":/shaders/fragmentSelection"));
    m_selectionShader->initialize();
}

void Bars3DRenderer::initSelectionBuffer()
{
    if (m_selectionTexture) {
        m_textureHelper->deleteTexture(&m_selectionTexture);
        m_selectionTexture = 0;
    }

    if (m_cachedIsSlicingActivated || m_mainViewPort.size().isEmpty())
        return;

    m_selectionTexture = m_textureHelper->createSelectionTexture(m_mainViewPort.size(),
                                                                 m_selectionFrameBuffer,
                                                                 m_selectionDepthBuffer);
}

#if !defined(QT_OPENGL_ES_2)
void Bars3DRenderer::initDepthShader()
{
    if (m_depthShader)
        delete m_depthShader;
    m_depthShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexDepth"),
                                     QStringLiteral(":/shaders/fragmentDepth"));
    m_depthShader->initialize();
}

void Bars3DRenderer::updateDepthBuffer()
{
    if (m_depthTexture) {
        m_textureHelper->deleteTexture(&m_depthTexture);
        m_depthTexture = 0;
    }

    if (m_mainViewPort.size().isEmpty())
        return;

    if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
        m_depthTexture = m_textureHelper->createDepthTexture(m_mainViewPort.size(),
                                                             m_depthFrameBuffer,
                                                             m_shadowQualityMultiplier);
        if (!m_depthTexture) {
            switch (m_cachedShadowQuality) {
            case QDataVis::ShadowQualityHigh:
                qWarning("Creating high quality shadows failed. Changing to medium quality.");
                (void)m_controller->setShadowQuality(QDataVis::ShadowQualityMedium);
                updateShadowQuality(QDataVis::ShadowQualityMedium);
                break;
            case QDataVis::ShadowQualityMedium:
                qWarning("Creating medium quality shadows failed. Changing to low quality.");
                (void)m_controller->setShadowQuality(QDataVis::ShadowQualityLow);
                updateShadowQuality(QDataVis::ShadowQualityLow);
                break;
            case QDataVis::ShadowQualityLow:
                qWarning("Creating low quality shadows failed. Switching shadows off.");
                (void)m_controller->setShadowQuality(QDataVis::ShadowQualityNone);
                updateShadowQuality(QDataVis::ShadowQualityNone);
                break;
            case QDataVis::ShadowQualitySoftHigh:
                qWarning("Creating soft high quality shadows failed. Changing to soft medium quality.");
                (void)m_controller->setShadowQuality(QDataVis::ShadowQualitySoftMedium);
                updateShadowQuality(QDataVis::ShadowQualitySoftMedium);
                break;
            case QDataVis::ShadowQualitySoftMedium:
                qWarning("Creating soft medium quality shadows failed. Changing to soft low quality.");
                (void)m_controller->setShadowQuality(QDataVis::ShadowQualitySoftLow);
                updateShadowQuality(QDataVis::ShadowQualitySoftLow);
                break;
            case QDataVis::ShadowQualitySoftLow:
                qWarning("Creating soft low quality shadows failed. Switching shadows off.");
                (void)m_controller->setShadowQuality(QDataVis::ShadowQualityNone);
                updateShadowQuality(QDataVis::ShadowQualityNone);
                break;
            default:
                // You'll never get here
                break;
            }
        }
    }
}
#endif

void Bars3DRenderer::initBackgroundShaders(const QString &vertexShader,
                                           const QString &fragmentShader)
{
    if (m_backgroundShader)
        delete m_backgroundShader;
    m_backgroundShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_backgroundShader->initialize();
}

void Bars3DRenderer::initLabelShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_labelShader)
        delete m_labelShader;
    m_labelShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_labelShader->initialize();
}

QT_DATAVISUALIZATION_END_NAMESPACE
