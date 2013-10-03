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
static QVector3D selectionSkipColor = QVector3D(255, 255, 255); // Selection texture's background color

Bars3DRenderer::Bars3DRenderer(Bars3DController *controller)
    : Abstract3DRenderer(controller),
      m_controller(controller),
      m_cachedIsSlicingActivated(false),
      m_cachedRowCount(0),
      m_cachedColumnCount(0),
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
      m_maxSceneSize(40.0),
      m_selection(selectionSkipColor),
      m_previousSelection(selectionSkipColor),
      m_hasHeightAdjustmentChanged(true)
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
        m_cachedRowCount    = newRows;
        // TODO: Invent foolproof max scene size formula
        // This seems to work ok if spacing is not negative (and row/column or column/row ratio is not too high)
        m_maxSceneSize = 2 * qSqrt(newColumns * newRows);
        // Calculate here and at setting bar specs
        calculateSceneScalingFactors();
    }

    // Update cached data window
    int dataRowCount = dataProxy->rowCount();
    int dataRowIndex = minRow;
    for (int i = 0; i < newRows; i++) {
        int j = 0;
        if (dataRowIndex < dataRowCount) {
            const QBarDataRow *dataRow = dataProxy->rowAt(dataRowIndex);
            int updateSize = qMin((dataRow->size() - minCol), m_renderItemArray[i].size());
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

    Abstract3DRenderer::updateDataModel(dataProxy);
}

void Bars3DRenderer::updateScene(Q3DScene *scene)
{
    // TODO: Move these to more suitable place e.g. controller should be controlling the viewports.
    scene->setSecondarySubViewport(m_sliceViewPort);
    scene->setPrimarySubViewport(m_mainViewPort);
    scene->setUnderSideCameraEnabled(m_hasNegativeValues);
    if (m_hasHeightAdjustmentChanged) {
        // Set initial camera position. Also update if height adjustment has changed.
        scene->activeCamera()->setBaseOrientation(QVector3D(0.0f, 0.0f, cameraDistance + zComp),
                                                  QVector3D(0.0f, -m_yAdjustment, zComp),
                                                  QVector3D(0.0f, 1.0f, 0.0f));
        m_hasHeightAdjustmentChanged = false;
    }

    scene->activeCamera()->d_ptr->updateViewMatrix(m_autoScaleAdjustment);
    // Set light position (rotate light with camera, a bit above it (as set in defaultLightPos))
    scene->setLightPositionRelativeToCamera(defaultLightPos);

    Abstract3DRenderer::updateScene(scene);
}

void Bars3DRenderer::render(GLuint defaultFboHandle)
{
    bool slicingActivated = m_cachedScene->isSlicingActive();
    updateSlicingActive(slicingActivated);

    // Handle GL state setup for FBO buffers and clearing of the render surface
    Abstract3DRenderer::render(defaultFboHandle);

    // If slice selection is on, draw the sliced scene
    if (m_cachedIsSlicingActivated)
        drawSlicedScene(m_axisCacheX.titleItem(), m_axisCacheY.titleItem(), m_axisCacheZ.titleItem());

    // Draw bars scene
    drawScene(defaultFboHandle);

    // If slicing has been activated by this render pass, we need another render
    if (slicingActivated != m_cachedScene->isSlicingActive())
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
    GLfloat negativesComp = 1.0f;

    // Compensate bar scaling a bit to avoid drawing on axis titles when we have negative values
    if (m_cachedScene->isUnderSideCameraEnabled())
        negativesComp = 0.67f;

    // Specify viewport
    glViewport(m_sliceViewPort.x(), m_sliceViewPort.y(),
               m_sliceViewPort.width(), m_sliceViewPort.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, (GLfloat)m_sliceViewPort.width()
                                 / (GLfloat)m_sliceViewPort.height(), 0.1f, 100.0f);

    // Set view matrix
    QMatrix4x4 viewMatrix;

    // Adjust scaling (zoom rate based on aspect ratio)
    GLfloat camZPosSliced = 5.0f / m_autoScaleAdjustment + zComp;

    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, camZPosSliced),
                      QVector3D(0.0f, 0.0f, zComp),
                      QVector3D(0.0f, 1.0f, 0.0f));

    // Set light position
    lightPos = QVector3D(0.0f, -m_yAdjustment, zComp);

    // Bind bar shader
    m_barShader->bind();

    // Draw bars
    // Draw the selected row / column
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

        GLfloat barPosY = negativesComp * item->translation().y() - m_yAdjustment / 2.0f + 0.2f; // we need some room for labels underneath; add +0.2f
        if (QDataVis::SelectionModeSliceRow == m_cachedSelectionMode)
            barPosX = item->translation().x();
        else
            barPosX = -(item->translation().z() - zComp); // flip z; frontmost bar to the left
        modelMatrix.translate(barPosX, barPosY, zComp);
        modelMatrix.scale(QVector3D(m_scaleX, negativesComp * item->height(), m_scaleZ));
        itModelMatrix.scale(QVector3D(m_scaleX, negativesComp * item->height(), m_scaleZ));

        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

#if 0
        QVector3D baseColor;
        if (m_selection.x() == item->position().x() && m_selection.y() == item->position().y())
            baseColor = Utils::vectorFromColor(m_cachedTheme.m_highlightBarColor);
        else if (QDataVis::SelectionModeSliceRow == m_cachedSelectionMode)
            baseColor = Utils::vectorFromColor(m_cachedTheme.m_highlightRowColor);
        else
            baseColor = Utils::vectorFromColor(m_cachedTheme.m_highlightColumnColor);

        QVector3D heightColor = Utils::vectorFromColor(m_cachedTheme.m_heightColor) * item->height();
        QVector3D barColor = baseColor + heightColor;
#else
        QVector3D barColor;
        if (m_selection.x() == item->position().x() && m_selection.y() == item->position().y())
            barColor = Utils::vectorFromColor(m_cachedTheme.m_highlightBarColor);
        else if (QDataVis::SelectionModeSliceRow == m_cachedSelectionMode)
            barColor = Utils::vectorFromColor(m_cachedTheme.m_highlightRowColor);
        else
            barColor = Utils::vectorFromColor(m_cachedTheme.m_highlightColumnColor);
#endif

        if (item->height() != 0) {
            // Set shader bindings
            m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
            m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
            m_barShader->setUniformValue(m_barShader->model(), modelMatrix);
            m_barShader->setUniformValue(m_barShader->nModel(),
                                         itModelMatrix.inverted().transposed());
            m_barShader->setUniformValue(m_barShader->MVP(), MVPMatrix);
            m_barShader->setUniformValue(m_barShader->color(), barColor);
            m_barShader->setUniformValue(m_barShader->lightS(), 0.5f);
            m_barShader->setUniformValue(m_barShader->ambientS(),
                                         m_cachedTheme.m_ambientStrength * 2.0f);

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

    // Draw labels for axes
    BarRenderItem *dummyItem(0);
    const LabelItem &sliceSelectionLabel = *m_sliceTitleItem;
    if (QDataVis::SelectionModeSliceRow == m_cachedSelectionMode) {
        if (m_sliceTitleItem) {
            m_drawer->drawLabel(*dummyItem, sliceSelectionLabel, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_autoScaleAdjustment, zComp),
                                QVector3D(0.0f, 0.0f, 0.0f), 0,
                                m_cachedSelectionMode, m_labelShader,
                                m_labelObj, m_cachedScene->activeCamera(), false, false, Drawer::LabelTop);
        }
        m_drawer->drawLabel(*dummyItem, zLabel, viewMatrix, projectionMatrix,
                            QVector3D(0.0f, m_autoScaleAdjustment, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), 0,
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, m_cachedScene->activeCamera(), false, false, Drawer::LabelBottom);
    } else {
        m_drawer->drawLabel(*dummyItem, xLabel, viewMatrix, projectionMatrix,
                            QVector3D(0.0f, m_autoScaleAdjustment, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), 0,
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, m_cachedScene->activeCamera(), false, false, Drawer::LabelBottom);
        if (m_sliceTitleItem) {
            m_drawer->drawLabel(*dummyItem, sliceSelectionLabel, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_autoScaleAdjustment, zComp),
                                QVector3D(0.0f, 0.0f, 0.0f), 0,
                                m_cachedSelectionMode, m_labelShader,
                                m_labelObj, m_cachedScene->activeCamera(), false, false, Drawer::LabelTop);
        }
    }
    m_drawer->drawLabel(*dummyItem, yLabel, viewMatrix, projectionMatrix,
                        QVector3D(0.0f, m_autoScaleAdjustment, zComp),
                        QVector3D(0.0f, 0.0f, 90.0f), 0,
                        m_cachedSelectionMode, m_labelShader,
                        m_labelObj, m_cachedScene->activeCamera(), false, false, Drawer::LabelLeft);

    // Draw labels for bars
    for (int col = 0; col < m_sliceSelection->size(); col++) {
        BarRenderItem *item = m_sliceSelection->at(col);
        // Draw values
        if (negativesComp == 1.0f) {
            m_drawer->drawLabel(*item, item->sliceLabelItem(), viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(0.0f, 0.0f, 90.0f),
                                item->height(),
                                m_cachedSelectionMode, m_labelShader,
                                m_labelObj, m_cachedScene->activeCamera(), false, false,
                                Drawer::LabelOver, Qt::AlignTop);
        } else {
            m_drawer->drawLabel(*item, item->sliceLabelItem(), viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(0.0f, 0.0f, 0.0f),
                                negativesComp * negativesComp * item->height(),
                                m_cachedSelectionMode, m_labelShader,
                                m_labelObj, m_cachedScene->activeCamera());
        }

        // Draw labels
        if (m_sliceCache->labelItems().size() > col) {
            const LabelItem *labelItem(0);
            labelItem = m_sliceCache->labelItems().at(col);
            m_drawer->drawLabel(*item, *labelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_autoScaleAdjustment, zComp),
                                QVector3D(0.0f, 0.0f, -45.0f), item->height(),
                                m_cachedSelectionMode, m_labelShader,
                                m_labelObj, m_cachedScene->activeCamera(), false, false,
                                Drawer::LabelBelow);
        }
    }

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

    // Specify viewport
    glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
               m_mainViewPort.width(), m_mainViewPort.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, (GLfloat)m_mainViewPort.width()
                                 / (GLfloat)m_mainViewPort.height(), 0.1f, 100.0f);

    // Get the view matrix
    QMatrix4x4 viewMatrix = m_cachedScene->activeCamera()->viewMatrix();

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

    // Get light position from the scene
    QVector3D lightPos =  m_cachedScene->activeLight()->position();

    // Skip depth rendering if we're in slice mode
    // Introduce regardless of shadow quality to simplify logic
    QMatrix4x4 depthViewMatrix;
    QMatrix4x4 depthProjectionMatrix;

#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > QDataVis::ShadowQualityNone && !m_cachedIsSlicingActivated) {
        // Render scene into a depth texture for using with shadow mapping
        // Enable drawing to depth framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthFrameBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Bind depth shader
        m_depthShader->bind();

        // Set viewport for depth map rendering. Must match texture size. Larger values give smoother shadows.
        glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
                   m_mainViewPort.width() * m_shadowQualityMultiplier,
                   m_mainViewPort.height() * m_shadowQualityMultiplier);

        // Get the depth view matrix
        // It may be possible to hack lightPos here if we want to make some tweaks to shadow
        QVector3D depthLightPos = m_cachedScene->activeCamera()->calculatePositionRelativeToCamera(
                    QVector3D(0.0f, 0.0f, zComp), 0.0f, 1.5f / m_autoScaleAdjustment);
        depthViewMatrix.lookAt(depthLightPos, QVector3D(0.0f, -m_yAdjustment, zComp),
                               QVector3D(0.0f, 1.0f, 0.0f));

        // TODO: Why does depthViewMatrix.column(3).y() goes to zero when we're directly above?
        // That causes the scene to be not drawn from above -> must be fixed
        // qDebug() << lightPos << depthViewMatrix << depthViewMatrix.column(3);

        // Set the depth projection matrix
        depthProjectionMatrix.perspective(10.0f, (GLfloat)m_mainViewPort.width()
                                          / (GLfloat)m_mainViewPort.height(), 3.0f, 100.0f);

        // Draw bars to depth buffer
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
                                      (m_columnDepth - rowPos) / m_scaleFactor + zComp);
                // Scale the bars down in X and Z to reduce self-shadowing issues
                modelMatrix.scale(QVector3D(m_scaleX * 0.9f, item.height(), m_scaleZ * 0.9f));

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
    if (!m_cachedIsSlicingActivated && m_cachedSelectionMode > QDataVis::SelectionModeNone) {
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
                                      (m_columnDepth - rowPos) / m_scaleFactor + zComp);
                modelMatrix.scale(QVector3D(m_scaleX, item.height(), m_scaleZ));

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
        if (QDataVis::InputStateOnScene == m_controller->inputState()) {
            m_selection = Utils::getSelection(m_controller->inputPosition(),
                                              m_cachedBoundingRect.height());
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

    bool selectionDirty = (m_selection != m_previousSelection
            || (m_selection != selectionSkipColor
            && QDataVis::InputStateOnScene == m_controller->inputState()
            && !m_cachedIsSlicingActivated));
    if (selectionDirty) {
        m_previousSelection = m_selection;
        if (m_sliceSelection) {
            if (!m_cachedIsSlicingActivated) {
                m_sliceCache = 0;
                m_sliceTitleItem = 0;
            }
            if (m_sliceSelection->size()) {
                // Slice doesn't own its items, no need to delete them - just clear
                m_sliceSelection->clear();
            }
        }
    }

    // Draw bars
    bool barSelectionFound = false;
    BarRenderItem *selectedBar(0);
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
            QMatrix4x4 depthMVPMatrix;

            colPos = (bar + 0.5f) * (m_cachedBarSpacing.width());
            rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());

            modelMatrix.translate((colPos - m_rowWidth) / m_scaleFactor,
                                  item.height() - m_yAdjustment,
                                  (m_columnDepth - rowPos) / m_scaleFactor + zComp);
            modelMatrix.scale(QVector3D(m_scaleX, item.height(), m_scaleZ));
            itModelMatrix.scale(QVector3D(m_scaleX, item.height(), m_scaleZ));
#ifdef SHOW_DEPTH_TEXTURE_SCENE
            MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;
#else
            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
#endif
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

#if 0
            QVector3D baseColor = Utils::vectorFromColor(m_cachedTheme.m_baseColor);
            QVector3D heightColor = Utils::vectorFromColor(m_cachedTheme.m_heightColor) * item.height();
            QVector3D depthColor = Utils::vectorFromColor(m_cachedTheme.m_depthColor)
                    * (float(row) / GLfloat(m_cachedRowCount));

            QVector3D barColor = baseColor + heightColor + depthColor;
#else
            QVector3D barColor = Utils::vectorFromColor(m_cachedTheme.m_baseColor);
#endif

            GLfloat lightStrength = m_cachedTheme.m_lightStrength;

            if (m_cachedSelectionMode > QDataVis::SelectionModeNone) {
                Bars3DController::SelectionType selectionType = isSelected(row, bar);

                switch (selectionType) {
                case Bars3DController::SelectionItem: {
                    barColor = Utils::vectorFromColor(m_cachedTheme.m_highlightBarColor);
                    lightStrength = m_cachedTheme.m_highlightLightStrength;
                    // Insert position data into render item. We have no ownership, don't delete the previous one
                    if (!m_cachedIsSlicingActivated) {
                        selectedBar = &item;
                        selectedBar->setPosition(QPoint(row, bar));
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        barSelectionFound = true;
                    }
                    if (selectionDirty && m_cachedSelectionMode >= QDataVis::SelectionModeSliceRow) {
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        item.setPosition(QPoint(row, bar));
                        m_sliceSelection->append(&item);
                        barSelectionFound = true;
                        if (m_cachedSelectionMode == QDataVis::SelectionModeSliceRow) {
                            if (m_axisCacheX.labelItems().size() > row)
                                m_sliceTitleItem = m_axisCacheX.labelItems().at(row);
                            if (!m_sliceCache) {
                                // m_sliceCache is the axis for labels, while title comes from different axis.
                                m_sliceCache = &m_axisCacheZ;
                            }
                        } else if (m_cachedSelectionMode == QDataVis::SelectionModeSliceColumn) {
                            if (m_axisCacheZ.labelItems().size() > bar)
                                m_sliceTitleItem = m_axisCacheZ.labelItems().at(bar);
                            if (!m_sliceCache) {
                                // m_sliceCache is the axis for labels, while title comes from different axis.
                                m_sliceCache = &m_axisCacheX;
                            }
                        }
                    }
                    break;
                }
                case Bars3DController::SelectionRow: {
                    // Current bar is on the same row as the selected bar
                    barColor = Utils::vectorFromColor(m_cachedTheme.m_highlightRowColor);
                    lightStrength = m_cachedTheme.m_highlightLightStrength;
                    if (QDataVis::SelectionModeSliceRow == m_cachedSelectionMode) {
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        item.setPosition(QPoint(row, bar));
                        if (selectionDirty)
                            m_sliceSelection->append(&item);
                    }
                    break;
                }
                case Bars3DController::SelectionColumn: {
                    // Current bar is on the same column as the selected bar
                    barColor = Utils::vectorFromColor(m_cachedTheme.m_highlightColumnColor);
                    lightStrength = m_cachedTheme.m_highlightLightStrength;
                    if (QDataVis::SelectionModeSliceColumn == m_cachedSelectionMode) {
                        item.setTranslation(modelMatrix.column(3).toVector3D());
                        item.setPosition(QPoint(row, bar));
                        if (selectionDirty)
                            m_sliceSelection->append(&item);
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
                m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
                m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
                m_barShader->setUniformValue(m_barShader->model(), modelMatrix);
                m_barShader->setUniformValue(m_barShader->nModel(),
                                             itModelMatrix.transposed().inverted());
                m_barShader->setUniformValue(m_barShader->MVP(), MVPMatrix);
                m_barShader->setUniformValue(m_barShader->color(), barColor);
                m_barShader->setUniformValue(m_barShader->ambientS(), m_cachedTheme.m_ambientStrength);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
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

    if (selectionDirty)
        emit selectedBarPosChanged(QPoint(int(m_selection.x()), int(m_selection.y())));

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
        if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
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
        ShaderHelper *lineShader = m_backgroundShader;
        // Bind bar shader
        lineShader->bind();

        // Set unchanging shader bindings
        QVector3D barColor = Utils::vectorFromColor(m_cachedTheme.m_gridLine);
        lineShader->setUniformValue(lineShader->lightP(), lightPos);
        lineShader->setUniformValue(lineShader->view(), viewMatrix);
        lineShader->setUniformValue(lineShader->color(), barColor);
        lineShader->setUniformValue(lineShader->ambientS(), m_cachedTheme.m_ambientStrength);

        // Floor lines: rows
        for (GLfloat row = 0.0f; row <= m_cachedRowCount; row++) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;
            QMatrix4x4 itModelMatrix;

            rowPos = row * m_cachedBarSpacing.height();
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
            lineShader->setUniformValue(lineShader->model(), modelMatrix);
            lineShader->setUniformValue(lineShader->nModel(),
                                        itModelMatrix.inverted().transposed());
            lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
                // Set shadow shader bindings
                lineShader->setUniformValue(lineShader->shadowQ(), m_shadowQualityToShader);
                lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
                lineShader->setUniformValue(lineShader->lightS(),
                                            m_cachedTheme.m_lightStrength / 10.0f);

                // Draw the object
                m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                lineShader->setUniformValue(lineShader->lightS(), m_cachedTheme.m_lightStrength);

                // Draw the object
                m_drawer->drawObject(lineShader, m_gridLineObj);
            }
        }

        // Floor lines: columns
        for (GLfloat bar = 0.0f; bar <= m_cachedColumnCount; bar++) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;
            QMatrix4x4 itModelMatrix;

            colPos = bar * m_cachedBarSpacing.width();
            modelMatrix.translate((m_rowWidth - colPos) / m_scaleFactor,
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
            lineShader->setUniformValue(lineShader->model(), modelMatrix);
            lineShader->setUniformValue(lineShader->nModel(),
                                        itModelMatrix.inverted().transposed());
            lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
                // Set shadow shader bindings
                lineShader->setUniformValue(lineShader->shadowQ(), m_shadowQualityToShader);
                lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
                lineShader->setUniformValue(lineShader->lightS(),
                                            m_cachedTheme.m_lightStrength / 10.0f);

                // Draw the object
                m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                lineShader->setUniformValue(lineShader->lightS(), m_cachedTheme.m_lightStrength);

                // Draw the object
                m_drawer->drawObject(lineShader, m_gridLineObj);
            }
        }

        if (m_axisCacheY.segmentCount() > 0) {
            // Wall lines: back wall
            GLfloat heightStep = m_axisCacheY.subSegmentStep();
            GLfloat startLine = 0.0f;

            if (m_hasNegativeValues)
                startLine = -m_heightNormalizer;

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
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
                    // Set shadow shader bindings
                    lineShader->setUniformValue(lineShader->shadowQ(), m_shadowQualityToShader);
                    lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
                    lineShader->setUniformValue(lineShader->lightS(),
                                                m_cachedTheme.m_lightStrength / 10.0f);

                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
                } else
#endif
                {
                    // Set shadowless shader bindings
                    lineShader->setUniformValue(lineShader->lightS(), m_cachedTheme.m_lightStrength);

                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
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
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

#if !defined(QT_OPENGL_ES_2)
                if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
                    // Set shadow shader bindings
                    lineShader->setUniformValue(lineShader->shadowQ(), m_shadowQualityToShader);
                    lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
                    lineShader->setUniformValue(lineShader->lightS(),
                                                m_cachedTheme.m_lightStrength / 10.0f);

                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
                } else
#endif
                {
                    // Set shadowless shader bindings
                    lineShader->setUniformValue(lineShader->lightS(), m_cachedTheme.m_lightStrength);

                    // Draw the object
                    m_drawer->drawObject(lineShader, m_gridLineObj);
                }
            }
        }
        // Release bar shader
        lineShader->release();
    }

    // TODO: Calculations done temporarily here. When optimizing, move to after data set addition? Keep drawing of the labels here.
    // Bind label shader
    m_labelShader->bind();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Calculate the positions for row and column labels and store them
    for (int row = 0; row != m_cachedRowCount; row++) {
        if (m_axisCacheX.labelItems().size() > row) {
            // Go through all rows and get position of max+1 or min-1 column, depending on x flip
            // We need only positions for them, labels have already been generated at QDataSetPrivate. Just add LabelItems
            rowPos = (row + 0.5f) * m_cachedBarSpacing.height();
            colPos = (m_rowWidth / m_scaleFactor) + labelMargin;
            GLfloat rotLabelX = -90.0f;
            GLfloat rotLabelY = 0.0f;
            GLfloat rotLabelZ = 0.0f;
            Qt::AlignmentFlag alignment = Qt::AlignRight;
            if (m_zFlipped)
                rotLabelY = 180.0f;
            if (m_xFlipped) {
                colPos = -(m_rowWidth / m_scaleFactor) - labelMargin;
                alignment = Qt::AlignLeft;
            }
            if (m_yFlipped) {
                if (m_zFlipped)
                    rotLabelY = 0.0f;
                else
                    rotLabelY = 180.0f;
                rotLabelZ = 180.0f;
            }
            QVector3D labelPos = QVector3D(colPos,
                                           -m_yAdjustment + 0.005f, // raise a bit over background to avoid depth "glimmering"
                                           (m_columnDepth - rowPos) / m_scaleFactor + zComp);

            m_dummyBarRenderItem.setTranslation(labelPos);
            const LabelItem &axisLabelItem = *m_axisCacheX.labelItems().at(row);
            //qDebug() << "labelPos, row" << row + 1 << ":" << labelPos << m_axisCacheX.labels().at(row);

            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, m_cachedScene->activeCamera(), true, true, Drawer::LabelMid,
                                alignment);
        }
    }
    for (int column = 0; column != m_cachedColumnCount; column += 1) {
        if (m_axisCacheZ.labelItems().size() > column) {
            // Go through all columns and get position of max+1 or min-1 row, depending on z flip
            // We need only positions for them, labels have already been generated at QDataSetPrivate. Just add LabelItems
            colPos = (column + 0.5f) * m_cachedBarSpacing.width();
            rowPos = (m_columnDepth / m_scaleFactor) + labelMargin;
            GLfloat rotLabelX = -90.0f;
            GLfloat rotLabelY = 90.0f;
            GLfloat rotLabelZ = 0.0f;
            Qt::AlignmentFlag alignment = Qt::AlignLeft;
            if (m_xFlipped)
                rotLabelY = -90.0f;
            if (m_zFlipped) {
                rowPos = -(m_columnDepth / m_scaleFactor) - labelMargin;
                alignment = Qt::AlignRight;
            }
            if (m_yFlipped) {
                if (m_xFlipped)
                    rotLabelY = -90.0f;
                else
                    rotLabelY = 90.0f;
                rotLabelZ = 180.0f;
            }
            QVector3D labelPos = QVector3D((colPos - m_rowWidth) / m_scaleFactor,
                                           -m_yAdjustment + 0.005f, // raise a bit over background to avoid depth "glimmering"
                                           rowPos + zComp);

            m_dummyBarRenderItem.setTranslation(labelPos);
            const LabelItem &axisLabelItem = *m_axisCacheZ.labelItems().at(column);

            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, m_cachedScene->activeCamera(), true, true, Drawer::LabelMid,
                                alignment);
        }
    }

    // Y Labels
    int labelNbr = 0;
    GLfloat heightStep = m_axisCacheY.segmentStep();
    GLfloat startLine = 0.0f;
    int labelCount = m_axisCacheY.labels().size();
    if (m_hasNegativeValues)
        startLine = -m_heightNormalizer;
    GLfloat labelPos = startLine;

    for (int i = 0; i < labelCount; i++) {
        if (m_axisCacheY.labelItems().size() > labelNbr) {
            GLfloat labelMarginXTrans = labelMargin;
            GLfloat labelMarginZTrans = labelMargin;
            GLfloat labelXTrans = m_rowWidth / m_scaleFactor;
            GLfloat labelZTrans = m_columnDepth / m_scaleFactor;
            GLfloat labelYTrans = 2.0f * labelPos / m_heightNormalizer - m_yAdjustment;
            GLfloat rotLabelX = 0.0f;
            GLfloat rotLabelY = -90.0f;
            GLfloat rotLabelZ = 0.0f;
            Qt::AlignmentFlag alignment = Qt::AlignLeft;
            if (!m_xFlipped) {
                labelXTrans = -labelXTrans;
                labelMarginXTrans = -labelMargin;
                rotLabelY = 90.0f;
            }
            if (m_zFlipped) {
                labelZTrans = -labelZTrans;
                labelMarginZTrans = -labelMargin;
                alignment = Qt::AlignRight;
            }

            const LabelItem &axisLabelItem = *m_axisCacheY.labelItems().at(labelNbr);

            // Back wall
            QVector3D labelTrans = QVector3D(labelXTrans, labelYTrans,
                                             labelZTrans + labelMarginZTrans + zComp);

            //qDebug() << "labelPos, value:" << labelTrans;

            m_dummyBarRenderItem.setTranslation(labelTrans);
            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, m_cachedScene->activeCamera(), true, true, Drawer::LabelMid,
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

            labelTrans = QVector3D(-labelXTrans - labelMarginXTrans, labelYTrans,
                                   -labelZTrans + zComp);

            m_dummyBarRenderItem.setTranslation(labelTrans);
            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                QVector3D(0.0f, m_yAdjustment, zComp),
                                QVector3D(rotLabelX, rotLabelY, rotLabelZ),
                                0, m_cachedSelectionMode,
                                m_labelShader, m_labelObj, m_cachedScene->activeCamera(), true, true, Drawer::LabelMid,
                                alignment);
        }
        labelNbr++;
        labelPos += heightStep;
    }

    // Handle slice activation and selection label drawing
    if (!barSelectionFound) {
        // We have no ownership, don't delete. Just NULL the pointer.
        m_selectedBar = NULL;
        if (m_cachedIsSlicingActivated
                && (m_selection == selectionSkipColor
                    || QDataVis::InputStateOnOverview == m_controller->inputState())) {
            m_cachedScene->setSlicingActive(false);
        }
    } else if (m_cachedSelectionMode >= QDataVis::SelectionModeSliceRow && selectionDirty) {
        // Activate slice mode
        m_cachedScene->setSlicingActive(true);

        // Create label textures
        for (int col = 0; col < m_sliceSelection->size(); col++) {
            BarRenderItem *item = m_sliceSelection->at(col);
            if (item->sliceLabel().isNull())
                item->setSliceLabel(generateValueLabel(m_axisCacheY.labelFormat(), item->value()));
            m_drawer->generateLabelItem(item->sliceLabelItem(), item->sliceLabel());
        }
    } else {
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
                            QVector3D(0.0f, m_yAdjustment, zComp),
                            QVector3D(0.0f, 0.0f, 0.0f), selectedBar->height(),
                            m_cachedSelectionMode, m_labelShader,
                            m_labelObj, m_cachedScene->activeCamera(), true, false);

        // Reset label update flag; they should have been updated when we get here
        m_updateLabels = false;

        glEnable(GL_DEPTH_TEST);
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    // Release label shader
    m_labelShader->release();
}

void Bars3DRenderer::handleResize()
{
    if (m_cachedBoundingRect.width() == 0 || m_cachedBoundingRect.height() == 0)
        return;

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

        // TODO Currently bargraph only supports zero centered or zero minimum ranges
        if (min > 0.0 || (min != 0.0 && (qFabs(min) != qFabs(max)))) {
            qWarning() << __FUNCTION__ << "Bar graph currently properly supports only "
                          "zero-centered and zero minimum ranges for Y-axis.";
        }
    }
}

void Bars3DRenderer::updateSelectionMode(QDataVis::SelectionMode mode)
{
    Abstract3DRenderer::updateSelectionMode(mode);

    // Create zoom selection if there isn't one
    if (mode >= QDataVis::SelectionModeSliceRow && !m_sliceSelection) {
        m_sliceSelection = new QList<BarRenderItem *>;
        if (mode == QDataVis::SelectionModeSliceRow)
            m_sliceSelection->reserve(m_cachedRowCount);
        else
            m_sliceSelection->reserve(m_cachedColumnCount);
    }
}

void Bars3DRenderer::updateBackgroundEnabled(bool enable)
{
    if (enable != m_cachedIsBackgroundEnabled) {
        Abstract3DRenderer::updateBackgroundEnabled(enable);
        loadMeshFile(); // Load changed bar type
    }
}

void Bars3DRenderer::updateSelectedBarPos(const QPoint &position)
{
    if (position == Bars3DController::noSelectionPoint())
        m_selection = selectionSkipColor;
    else
        m_selection = QVector3D(position.x(), position.y(), 0);
    emit needRender();
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
    m_gridLineObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/bar"));
    m_gridLineObj->load();
}

void Bars3DRenderer::loadLabelMesh()
{
    if (m_labelObj)
        delete m_labelObj;
    m_labelObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/label"));
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
    //qDebug() << "m_scaleX" << m_scaleX << "m_scaleFactor" << m_scaleFactor;
    //qDebug() << "m_scaleZ" << m_scaleZ << "m_scaleFactor" << m_scaleFactor;
    //qDebug() << "m_rowWidth:" << m_rowWidth << "m_columnDepth:" << m_columnDepth << "m_maxDimension:" << m_maxDimension;
}

void Bars3DRenderer::calculateHeightAdjustment()
{
    m_heightNormalizer = (GLfloat)qMax(qFabs(m_axisCacheY.min()), qFabs(m_axisCacheY.max()));

    // 2.0f = max difference between minimum and maximum value after scaling with m_heightNormalizer
    GLfloat newAdjustment = 2.0f - ((m_heightNormalizer - m_axisCacheY.min()) / m_heightNormalizer);
    if (newAdjustment != m_yAdjustment) {
        m_hasHeightAdjustmentChanged = true;
        m_yAdjustment = newAdjustment;
    }
    //qDebug() << m_yAdjustment;
}

Bars3DController::SelectionType Bars3DRenderer::isSelected(GLint row, GLint bar)
{
    //static QVector3D prevSel = m_selection; // TODO: For debugging
    Bars3DController::SelectionType isSelectedType = Bars3DController::SelectionNone;
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
        isSelectedType = Bars3DController::SelectionItem;
    }
    else if (current.y() == m_selection.y() && (m_cachedSelectionMode == QDataVis::SelectionModeItemAndColumn
                                                || m_cachedSelectionMode == QDataVis::SelectionModeItemRowAndColumn
                                                || m_cachedSelectionMode == QDataVis::SelectionModeSliceColumn)) {
        isSelectedType = Bars3DController::SelectionColumn;
    }
    else if (current.x() == m_selection.x() && (m_cachedSelectionMode == QDataVis::SelectionModeItemAndRow
                                                || m_cachedSelectionMode == QDataVis::SelectionModeItemRowAndColumn
                                                || m_cachedSelectionMode == QDataVis::SelectionModeSliceRow)) {
        isSelectedType = Bars3DController::SelectionRow;
    }
    return isSelectedType;
}

void Bars3DRenderer::updateSlicingActive(bool isSlicing)
{
    if (isSlicing == m_cachedIsSlicingActivated)
        return;

    m_cachedIsSlicingActivated = isSlicing;
    if (isSlicing) {
        m_mainViewPort = QRect(0, m_cachedBoundingRect.height() - m_cachedBoundingRect.height() / 5,
                               m_cachedBoundingRect.width() / 5, m_cachedBoundingRect.height() / 5);
        if (m_depthTexture) {
            m_textureHelper->deleteTexture(&m_depthTexture);
            m_depthTexture = 0;
        }
    } else {
        m_mainViewPort = QRect(0, 0, this->m_cachedBoundingRect.width(),
                               this->m_cachedBoundingRect.height());
        initSelectionBuffer(); // We need to re-init selection buffer in case there has been a resize
#if !defined(QT_OPENGL_ES_2)
        updateDepthBuffer(); // Re-init depth buffer as well
#endif
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
    if (m_cachedIsSlicingActivated)
        return;

    if (m_selectionTexture)
        m_textureHelper->deleteTexture(&m_selectionTexture);

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
    if (m_cachedIsSlicingActivated)
        return;

    if (m_depthTexture) {
        m_textureHelper->deleteTexture(&m_depthTexture);
        m_depthTexture = 0;
    }

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
