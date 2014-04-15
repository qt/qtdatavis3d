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

#include "bars3drenderer_p.h"
#include "bars3dcontroller_p.h"
#include "q3dcamera_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"
#include "utils_p.h"
#include "drawer_p.h"
#include "qbardataitem.h"
#include "q3dlight.h"
#include "qbar3dseries_p.h"
#include "barseriesrendercache_p.h"

#include <QtGui/QMatrix4x4>
#include <QtGui/QMouseEvent>
#include <QtCore/QThread>
#include <QtCore/qmath.h>

// You can verify that depth buffer drawing works correctly by uncommenting this.
// You should see the scene from  where the light is
//#define SHOW_DEPTH_TEXTURE_SCENE

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

const GLfloat labelMargin = 0.05f;
const GLfloat gridLineWidth = 0.005f;

const bool sliceGridLabels = true;

Bars3DRenderer::Bars3DRenderer(Bars3DController *controller)
    : Abstract3DRenderer(controller),
      m_cachedIsSlicingActivated(false),
      m_cachedRowCount(0),
      m_cachedColumnCount(0),
      m_selectedBar(0),
      m_sliceCache(0),
      m_sliceTitleItem(0),
      m_xFlipped(false),
      m_zFlipped(false),
      m_yFlipped(false),
      m_updateLabels(false),
      m_barShader(0),
      m_barGradientShader(0),
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
      m_negativeBackgroundAdjustment(0.0f),
      m_rowWidth(0),
      m_columnDepth(0),
      m_maxDimension(0),
      m_scaleX(0),
      m_scaleZ(0),
      m_scaleFactor(0),
      m_maxSceneSize(40.0f),
      m_visualSelectedBarPos(Bars3DController::invalidSelectionPosition()),
      m_resetCameraBaseOrientation(true),
      m_selectedBarPos(Bars3DController::invalidSelectionPosition()),
      m_selectedSeriesCache(0),
      m_noZeroInRange(false),
      m_seriesScaleX(0.0f),
      m_seriesScaleZ(0.0f),
      m_seriesStep(0.0f),
      m_seriesStart(0.0f),
      m_clickedPosition(Bars3DController::invalidSelectionPosition()),
      m_keepSeriesUniform(false),
      m_haveUniformColorSeries(false),
      m_haveGradientSeries(false)
{
    m_axisCacheY.setScale(2.0f);
    m_axisCacheY.setTranslate(-1.0f);

    initializeOpenGLFunctions();
    initializeOpenGL();
}

Bars3DRenderer::~Bars3DRenderer()
{
    if (QOpenGLContext::currentContext()) {
        m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);
        m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
        m_textureHelper->deleteTexture(&m_selectionTexture);
        m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
        m_textureHelper->deleteTexture(&m_bgrTexture);
    }

    delete m_barShader;
    delete m_barGradientShader;
    delete m_depthShader;
    delete m_selectionShader;
    delete m_backgroundShader;
    delete m_backgroundObj;
    delete m_gridLineObj;
    delete m_labelObj;
    delete m_labelShader;
}

void Bars3DRenderer::initializeOpenGL()
{
    Abstract3DRenderer::initializeOpenGL();

    // Initialize shaders
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

    // Load background mesh (we need to be initialized first)
    loadBackgroundMesh();
}

void Bars3DRenderer::updateData()
{
    int minRow = m_axisCacheZ.min();
    int maxRow = m_axisCacheZ.max();
    int minCol = m_axisCacheX.min();
    int maxCol = m_axisCacheX.max();
    int newRows = maxRow - minRow + 1;
    int newColumns = maxCol - minCol + 1;
    int updateSize = 0;
    int dataRowCount = 0;
    int maxDataRowCount = 0;

    m_seriesScaleX = 1.0f / float(m_visibleSeriesCount);
    m_seriesStep = 1.0f / float(m_visibleSeriesCount);
    m_seriesStart = -((float(m_visibleSeriesCount) - 1.0f) / 2.0f) * m_seriesStep;

    if (m_keepSeriesUniform)
        m_seriesScaleZ = m_seriesScaleX;
    else
        m_seriesScaleZ = 1.0f;

    if (m_cachedRowCount != newRows || m_cachedColumnCount != newColumns) {
        // Force update for selection related items
        m_sliceCache = 0;
        m_sliceTitleItem = 0;

        m_cachedColumnCount = newColumns;
        m_cachedRowCount = newRows;
        // Calculate max scene size
        GLfloat sceneRatio = qMin(GLfloat(newColumns) / GLfloat(newRows),
                                  GLfloat(newRows) / GLfloat(newColumns));
        m_maxSceneSize = 2.0f * qSqrt(sceneRatio * newColumns * newRows);
        // Calculate here and at setting bar specs
        calculateSceneScalingFactors();
    }

    const QValue3DAxisFormatter *axisFormatter = m_axisCacheY.formatter();
    float zeroPosition = axisFormatter->positionAt(0.0f);

    foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
        BarSeriesRenderCache *cache = static_cast<BarSeriesRenderCache *>(baseCache);
        if (cache->isVisible()) {
            const QBar3DSeries *currentSeries = cache->series();
            BarRenderItemArray &renderArray = cache->renderArray();
            bool dimensionsChanged = false;
            if (newRows != renderArray.size()
                    || newColumns != renderArray.at(0).size()) {
                // Destroy old render items and reallocate new array
                dimensionsChanged = true;
                renderArray.resize(newRows);
                for (int i = 0; i < newRows; i++)
                    renderArray[i].resize(newColumns);
                cache->sliceArray().clear();
            }

            if (cache->dataDirty() || dimensionsChanged) {
                QBarDataProxy *dataProxy = currentSeries->dataProxy();
                dataRowCount = dataProxy->rowCount();
                if (maxDataRowCount < dataRowCount)
                    maxDataRowCount = qMin(dataRowCount, newRows);
                int dataRowIndex = minRow;
                GLfloat heightValue = 0.0f;
                for (int i = 0; i < newRows; i++) {
                    int j = 0;
                    BarRenderItemRow &renderRow = renderArray[i];
                    if (dataRowIndex < dataRowCount) {
                        const QBarDataRow *dataRow = dataProxy->rowAt(dataRowIndex);
                        updateSize = qMin((dataRow->size() - minCol), renderRow.size());
                        if (dataRow) {
                            int dataColIndex = minCol;
                            for (; j < updateSize ; j++) {
                                float value = dataRow->at(dataColIndex).value();
                                heightValue = axisFormatter->positionAt(value);
                                if (m_noZeroInRange) {
                                    if (m_hasNegativeValues) {
                                        heightValue = -1.0f + heightValue;
                                        if (heightValue > 0.0f)
                                            heightValue = 0.0f;
                                    } else {
                                        if (heightValue < 0.0f)
                                            heightValue = 0.0f;
                                    }
                                } else {
                                    heightValue -= zeroPosition;
                                }
                                renderRow[j].setValue(value);
                                renderRow[j].setHeight(heightValue);

                                float angle = dataRow->at(dataColIndex).rotation();
                                if (angle) {
                                    renderRow[j].setRotation(
                                                QQuaternion::fromAxisAndAngle(
                                                    upVector, angle));
                                } else {
                                    renderRow[j].setRotation(identityQuaternion);
                                }
                                dataColIndex++;
                            }
                        }
                    }
                    for (; j < newColumns; j++) {
                        renderRow[j].setValue(0.0f);
                        renderRow[j].setHeight(0.0f);
                        renderRow[j].setRotation(identityQuaternion);
                    }
                    dataRowIndex++;
                }
                cache->setDataDirty(false);
            }
        }
    }

    // Reset selected bar to update selection
    updateSelectedBar(m_selectedBarPos,
                      m_selectedSeriesCache ? m_selectedSeriesCache->series() : 0);
}

void Bars3DRenderer::updateSeries(const QList<QAbstract3DSeries *> &seriesList)
{
    Abstract3DRenderer::updateSeries(seriesList);

    bool noSelection = true;
    int seriesCount = seriesList.size();
    int visualIndex = 0;
    m_haveUniformColorSeries = false;
    m_haveGradientSeries = false;
    for (int i = 0; i < seriesCount; i++) {
        QBar3DSeries *barSeries = static_cast<QBar3DSeries *>(seriesList[i]);
        if (barSeries->isVisible()) {
            BarSeriesRenderCache *cache =
                    static_cast<BarSeriesRenderCache *>(m_renderCacheList.value(barSeries));
            if (noSelection
                    && barSeries->selectedBar() != QBar3DSeries::invalidSelectionPosition()
                    && selectionLabel() != cache->itemLabel()) {
                m_selectionLabelDirty = true;
                noSelection = false;
            }
            cache->setVisualIndex(visualIndex++);
            if (cache->colorStyle() == Q3DTheme::ColorStyleUniform)
                m_haveUniformColorSeries = true;
            else
                m_haveGradientSeries = true;
        }
    }
    if (noSelection && !selectionLabel().isEmpty())
        m_selectionLabelDirty = true;
}

void Bars3DRenderer::updateCustomData(const QList<CustomDataItem *> &customItems)
{
    // TODO
}

SeriesRenderCache *Bars3DRenderer::createNewCache(QAbstract3DSeries *series)
{
    return new BarSeriesRenderCache(series, this);
}

void Bars3DRenderer::updateScene(Q3DScene *scene)
{
    if (m_hasNegativeValues)
        scene->activeCamera()->d_ptr->setMinYRotation(-90.0);
    else
        scene->activeCamera()->d_ptr->setMinYRotation(0.0f);

    if (m_resetCameraBaseOrientation) {
        // Set initial camera position. Also update if height adjustment has changed.
        scene->activeCamera()->d_ptr->setBaseOrientation(cameraDistanceVector,
                                                         zeroVector,
                                                         upVector);
        m_resetCameraBaseOrientation = false;
    }

    Abstract3DRenderer::updateScene(scene);

    updateSlicingActive(scene->isSlicingActive());
}

void Bars3DRenderer::render(GLuint defaultFboHandle)
{
    // Handle GL state setup for FBO buffers and clearing of the render surface
    Abstract3DRenderer::render(defaultFboHandle);

    if (m_axisCacheY.positionsDirty())
        m_axisCacheY.updateAllPositions();

    drawScene(defaultFboHandle);
    if (m_cachedIsSlicingActivated)
        drawSlicedScene();
}

void Bars3DRenderer::drawSlicedScene()
{
    GLfloat barPosX = 0;
    QVector3D lightPos;
    QVector3D lightColor = Utils::vectorFromColor(m_cachedTheme->lightColor());
    static QQuaternion ninetyDegreeRotation = QQuaternion::fromAxisAndAngle(upVector, 90.0f);

    // Specify viewport
    glViewport(m_secondarySubViewport.x(),
               m_secondarySubViewport.y(),
               m_secondarySubViewport.width(),
               m_secondarySubViewport.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(35.0f, (GLfloat)m_secondarySubViewport.width()
                                 / (GLfloat)m_secondarySubViewport.height(), 0.1f, 100.0f);

    // Set view matrix
    QMatrix4x4 viewMatrix;

    // Adjust scaling (zoom rate based on aspect ratio)
    GLfloat camZPosSliced = cameraDistance / m_autoScaleAdjustment;

    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, camZPosSliced), zeroVector, upVector);

    // Set light position
    lightPos = QVector3D(0.0f, 0.0f, camZPosSliced * 2.0f);

    const Q3DCamera *activeCamera = m_cachedScene->activeCamera();

    // Draw the selected row / column
    QMatrix4x4 projectionViewMatrix = projectionMatrix * viewMatrix;
    bool rowMode = m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionRow);
    bool itemMode = m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionItem);

    GLfloat barPosYAdjustment = -0.8f; // Translate to -1.0 + 0.2 for row/column labels
    GLfloat gridAdjustment = 1.0f + barPosYAdjustment - m_negativeBackgroundAdjustment;
    GLfloat scaleFactor = 0.0f;
    if (rowMode)
        scaleFactor = (1.1f * m_rowWidth) / m_scaleFactor;
    else
        scaleFactor = (1.1f * m_columnDepth) / m_scaleFactor;
    GLfloat barLabelYPos = barPosYAdjustment - 0.4f - labelMargin; // 0.4 for labels
    GLfloat zeroPosAdjustment = 0.0f;
    if (!m_noZeroInRange)
        zeroPosAdjustment = 2.0f * m_axisCacheY.min() / m_heightNormalizer;
    else if (m_hasNegativeValues)
        zeroPosAdjustment = -2.0f;

    // Draw grid lines
    if (m_cachedTheme->isGridEnabled()) {
        glDisable(GL_DEPTH_TEST);
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
        lineShader->setUniformValue(lineShader->ambientS(),
                                    m_cachedTheme->ambientLightStrength() * 2.3f);
        lineShader->setUniformValue(lineShader->lightS(), 0.0f);
        lineShader->setUniformValue(lineShader->lightColor(), lightColor);

        // Horizontal lines
        if (m_axisCacheY.segmentCount() > 0) {
            int gridLineCount = m_axisCacheY.gridLineCount();

            QVector3D gridLineScale(scaleFactor, gridLineWidth, gridLineWidth);
            bool noZero = true;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 itModelMatrix;

            for (int line = 0; line < gridLineCount; line++) {
                QMatrix4x4 modelMatrix;
                GLfloat gridPos = m_axisCacheY.gridLinePosition(line) + gridAdjustment;
                modelMatrix.translate(0.0f, gridPos, 0.0f);
                modelMatrix.scale(gridLineScale);
                itModelMatrix = modelMatrix;
                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

                // Draw the object
#if !(defined QT_OPENGL_ES_2)
                m_drawer->drawObject(lineShader, m_gridLineObj);
#else
                m_drawer->drawLine(lineShader);
#endif

                // Check if we have a line at zero position already
                if (gridPos == (barPosYAdjustment + zeroPosAdjustment))
                    noZero = false;
            }

            // Draw a line at zero, if none exists
            if (!m_noZeroInRange && noZero) {
                QMatrix4x4 modelMatrix;
                modelMatrix.translate(0.0f, barPosYAdjustment - zeroPosAdjustment, 0.0f);
                modelMatrix.scale(gridLineScale);
                itModelMatrix = modelMatrix;
                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);
                lineShader->setUniformValue(lineShader->color(),
                                            Utils::vectorFromColor(m_cachedTheme->labelTextColor()));

                // Draw the object
#if !(defined QT_OPENGL_ES_2)
                m_drawer->drawObject(lineShader, m_gridLineObj);
#else
                m_drawer->drawLine(lineShader);
#endif
            }
        }

        if (sliceGridLabels) {
            // Bind label shader
            m_labelShader->bind();
            glEnable(GL_TEXTURE_2D);
            glCullFace(GL_BACK);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Draw grid labels
            int labelNbr = 0;
            int labelCount = m_axisCacheY.labelCount();
            QVector3D backLabelRotation(0.0f, 0.0f, 0.0f);
            QVector3D labelTrans = QVector3D(scaleFactor + labelMargin, 0.0f, 0.0f);

            for (int i = 0; i < labelCount; i++) {
                if (m_axisCacheY.labelItems().size() > labelNbr) {
                    const LabelItem &axisLabelItem = *m_axisCacheY.labelItems().at(labelNbr);
                    GLfloat gridPos = m_axisCacheY.labelPosition(i) + gridAdjustment;
                    labelTrans.setY(gridPos);
                    m_dummyBarRenderItem.setTranslation(labelTrans);
                    m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix,
                                        projectionMatrix, zeroVector, backLabelRotation, 0,
                                        m_cachedSelectionMode, m_labelShader, m_labelObj,
                                        activeCamera, true, true, Drawer::LabelMid, Qt::AlignRight);
                }
                labelNbr++;
            }
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }
    }

    // Draw bars
    QVector3D modelMatrixScaler(m_scaleX * m_seriesScaleX, 0.0f, m_scaleZ * m_seriesScaleZ);
    if (!rowMode) {
        modelMatrixScaler.setX(m_scaleZ * m_seriesScaleZ);
        modelMatrixScaler.setZ(m_scaleX * m_seriesScaleX);
    }

    // Set common bar shader bindings
    m_barShader->bind();
    m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
    m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
    m_barShader->setUniformValue(m_barShader->lightS(), 0.15f);
    m_barShader->setUniformValue(m_barShader->ambientS(),
                                 m_cachedTheme->ambientLightStrength() * 2.3f);
    m_barShader->setUniformValue(m_barShader->lightColor(), lightColor);
    m_barGradientShader->bind();
    m_barGradientShader->setUniformValue(m_barGradientShader->lightP(), lightPos);
    m_barGradientShader->setUniformValue(m_barGradientShader->view(), viewMatrix);
    m_barGradientShader->setUniformValue(m_barGradientShader->lightS(), 0.15f);
    m_barGradientShader->setUniformValue(m_barGradientShader->ambientS(),
                                         m_cachedTheme->ambientLightStrength() * 2.3f);
    m_barGradientShader->setUniformValue(m_barGradientShader->gradientMin(), 0.0f);
    m_barGradientShader->setUniformValue(m_barGradientShader->lightColor(), lightColor);

    // Default to uniform shader
    ShaderHelper *barShader = m_barShader;
    barShader->bind();

    Q3DTheme::ColorStyle previousColorStyle = Q3DTheme::ColorStyleUniform;
    Q3DTheme::ColorStyle colorStyle = Q3DTheme::ColorStyleUniform;
    ObjectHelper *barObj = 0;
    QVector3D highlightColor;
    QVector3D baseColor;
    GLuint highlightGradientTexture = 0;
    GLuint baseGradientTexture = 0;
    bool colorStyleIsUniform = true;
    int firstVisualIndex = m_renderCacheList.size();
    QVector<BarRenderSliceItem> *firstVisualSliceArray = 0;
    BarRenderSliceItem *selectedItem = 0;

    QQuaternion seriesRotation;
    foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
        if (baseCache->isVisible()) {
            BarSeriesRenderCache *cache = static_cast<BarSeriesRenderCache *>(baseCache);
            QVector<BarRenderSliceItem> &sliceArray = cache->sliceArray();
            int sliceCount = sliceArray.size();
            if (firstVisualIndex > cache->visualIndex()) {
                firstVisualIndex = cache->visualIndex();
                firstVisualSliceArray = &sliceArray;
            }

            barObj = cache->object();
            colorStyle = cache->colorStyle();
            colorStyleIsUniform = (colorStyle == Q3DTheme::ColorStyleUniform);
            if (colorStyleIsUniform) {
                highlightColor = cache->singleHighlightColor();
                baseColor = cache->baseColor();
            } else {
                highlightGradientTexture = cache->singleHighlightGradientTexture();
                baseGradientTexture = cache->baseGradientTexture();
            }

            // Rebind shader if it has changed
            if (colorStyleIsUniform != (previousColorStyle == Q3DTheme::ColorStyleUniform)) {
                if (colorStyleIsUniform)
                    barShader = m_barShader;
                else
                    barShader = m_barGradientShader;
                barShader->bind();

            }

            if (!colorStyleIsUniform && (previousColorStyle != colorStyle)
                    && (colorStyle == Q3DTheme::ColorStyleObjectGradient)) {
                m_barGradientShader->setUniformValue(m_barGradientShader->gradientHeight(), 0.5f);
            }

            previousColorStyle = colorStyle;
            seriesRotation = cache->meshRotation();
            bool selectedSeries = (cache == m_selectedSeriesCache);

            for (int bar = 0; bar < sliceCount; bar++) {
                BarRenderSliceItem &item = cache->sliceArray()[bar];
                if (selectedSeries && itemMode && sliceGridLabels
                        && m_visualSelectedBarPos.x() == item.position().x()
                        && m_visualSelectedBarPos.y() == item.position().y()) {
                    selectedItem = &item;
                }
                if (!item.value())
                    continue;

                if (item.height() < 0)
                    glCullFace(GL_FRONT);
                else
                    glCullFace(GL_BACK);

                QMatrix4x4 MVPMatrix;
                QMatrix4x4 modelMatrix;
                QMatrix4x4 itModelMatrix;
                QQuaternion barRotation = item.rotation();
                GLfloat barPosY = item.translation().y() + barPosYAdjustment - zeroPosAdjustment;

                if (rowMode) {
                    barPosX = item.translation().x();
                } else {
                    barPosX = -(item.translation().z()); // flip z; frontmost bar to the left
                    barRotation *= ninetyDegreeRotation;
                }

                modelMatrix.translate(barPosX, barPosY, 0.0f);
                modelMatrixScaler.setY(item.height());

                if (!seriesRotation.isIdentity())
                    barRotation *= seriesRotation;

                if (!barRotation.isIdentity()) {
                    modelMatrix.rotate(barRotation);
                    itModelMatrix.rotate(barRotation);
                }

                modelMatrix.scale(modelMatrixScaler);
                itModelMatrix.scale(modelMatrixScaler);

                MVPMatrix = projectionViewMatrix * modelMatrix;

                QVector3D barColor;
                GLuint gradientTexture = 0;

                if (itemMode && m_visualSelectedBarPos.x() == item.position().x()
                        && m_visualSelectedBarPos.y() == item.position().y()) {
                    if (colorStyleIsUniform)
                        barColor = highlightColor;
                    else
                        gradientTexture = highlightGradientTexture;
                } else {
                    if (colorStyleIsUniform)
                        barColor = baseColor;
                    else
                        gradientTexture = baseGradientTexture;
                }

                if (item.height() != 0) {
                    // Set shader bindings
                    barShader->setUniformValue(barShader->model(), modelMatrix);
                    barShader->setUniformValue(barShader->nModel(),
                                               itModelMatrix.inverted().transposed());
                    barShader->setUniformValue(barShader->MVP(), MVPMatrix);
                    if (colorStyleIsUniform) {
                        barShader->setUniformValue(barShader->color(), barColor);
                    } else if (colorStyle == Q3DTheme::ColorStyleRangeGradient) {
                        barShader->setUniformValue(barShader->gradientHeight(),
                                                   (qAbs(item.height()) / m_gradientFraction));
                    }

                    // Draw the object
                    m_drawer->drawObject(barShader,
                                         barObj,
                                         gradientTexture);
                }
            }
        }
    }

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

    // Draw labels for bars
    QVector3D sliceValueRotation(0.0f, 0.0f, 90.0f);
    QVector3D sliceLabelRotation(0.0f, 0.0f, -45.0f);

    int labelCount = m_sliceCache->labelItems().size();

    for (int labelNo = 0; labelNo < labelCount; labelNo++) {
        // Get labels from first series only
        const BarRenderSliceItem &item = firstVisualSliceArray->at(labelNo);
        m_dummyBarRenderItem.setTranslation(QVector3D(item.translation().x(),
                                                      barLabelYPos,
                                                      item.translation().z()));
        // Draw labels
        m_drawer->drawLabel(m_dummyBarRenderItem, *m_sliceCache->labelItems().at(labelNo),
                            viewMatrix, projectionMatrix, positionComp, sliceLabelRotation,
                            0, m_cachedSelectionMode, m_labelShader,
                            m_labelObj, activeCamera, false, false, Drawer::LabelMid,
                            Qt::AlignRight, true);
    }

    if (!sliceGridLabels) {
        foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
            if (baseCache->isVisible()) {
                BarSeriesRenderCache *cache = static_cast<BarSeriesRenderCache *>(baseCache);
                QVector<BarRenderSliceItem> &sliceArray = cache->sliceArray();
                int sliceCount = sliceArray.size();
                for (int col = 0; col < sliceCount; col++) {
                    BarRenderSliceItem &item = sliceArray[col];

                    // Draw values
                    if (item.height() != 0.0f || (!m_noZeroInRange && item.value() == 0.0f)) {
                        // Create label texture if we need it
                        if (item.sliceLabel().isNull() || m_updateLabels) {
                            QString valueLabelText = m_axisCacheY.formatter()->stringForValue(
                                        qreal(item.value()), m_axisCacheY.labelFormat());
                            item.setSliceLabel(valueLabelText);
                            m_drawer->generateLabelItem(item.sliceLabelItem(), item.sliceLabel());
                            m_updateLabels = false;
                        }
                        Qt::AlignmentFlag alignment = (item.height() < 0) ? Qt::AlignBottom : Qt::AlignTop;
                        Drawer::LabelPosition labelPos = (item.height() < 0) ? Drawer::LabelBelow : Drawer::LabelOver;
                        m_dummyBarRenderItem.setTranslation(QVector3D(item.translation().x(),
                                                                      barPosYAdjustment - zeroPosAdjustment
                                                                      + item.height(),
                                                                      item.translation().z()));

                        m_drawer->drawLabel(m_dummyBarRenderItem, item.sliceLabelItem(), viewMatrix,
                                            projectionMatrix, zeroVector, sliceValueRotation,
                                            item.height(), m_cachedSelectionMode, m_labelShader,
                                            m_labelObj, activeCamera, false, false, labelPos,
                                            alignment, true);
                    }
                }
            }
        }
    } else {
        // Only draw value for selected item when grid labels are on
        // Create label texture if we need it
        if (selectedItem->sliceLabel().isNull() || m_updateLabels) {
            QString valueLabelText = m_axisCacheY.formatter()->stringForValue(
                        qreal(selectedItem->value()), m_axisCacheY.labelFormat());
            selectedItem->setSliceLabel(valueLabelText);
            m_drawer->generateLabelItem(selectedItem->sliceLabelItem(), selectedItem->sliceLabel());
            m_updateLabels = false;
        }
        Qt::AlignmentFlag alignment = (selectedItem->height() < 0) ? Qt::AlignBottom : Qt::AlignTop;
        Drawer::LabelPosition labelPos = (selectedItem->height() < 0) ? Drawer::LabelBelow : Drawer::LabelOver;
        m_dummyBarRenderItem.setTranslation(QVector3D(selectedItem->translation().x(),
                                                      barPosYAdjustment - zeroPosAdjustment
                                                      + selectedItem->height(),
                                                      selectedItem->translation().z()));

        m_drawer->drawLabel(m_dummyBarRenderItem, selectedItem->sliceLabelItem(), viewMatrix,
                            projectionMatrix, zeroVector, sliceValueRotation,
                            selectedItem->height(), m_cachedSelectionMode, m_labelShader,
                            m_labelObj, activeCamera, false, false, labelPos,
                            alignment, true);
    }

    // Draw labels for axes
    if (rowMode) {
        if (m_sliceTitleItem) {
            m_drawer->drawLabel(*dummyItem, sliceSelectionLabel, viewMatrix, projectionMatrix,
                                positionComp, zeroVector, 0, m_cachedSelectionMode, m_labelShader,
                                m_labelObj, activeCamera, false, false, Drawer::LabelTop,
                                Qt::AlignCenter, true);
        }
        m_drawer->drawLabel(*dummyItem, m_axisCacheX.titleItem(), viewMatrix, projectionMatrix,
                            positionComp, zeroVector, 0, m_cachedSelectionMode, m_labelShader,
                            m_labelObj, activeCamera, false, false, Drawer::LabelBottom,
                            Qt::AlignCenter, true);
    } else {
        m_drawer->drawLabel(*dummyItem, m_axisCacheZ.titleItem(), viewMatrix, projectionMatrix,
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
    // Y-axis label
    QVector3D labelTrans = QVector3D(-scaleFactor - labelMargin, 0.2f, 0.0f); // y = 0.2 for row/column labels (see barPosYAdjustment)
    m_dummyBarRenderItem.setTranslation(labelTrans);
    m_drawer->drawLabel(m_dummyBarRenderItem, m_axisCacheY.titleItem(), viewMatrix,
                        projectionMatrix, zeroVector, QVector3D(0.0f, 0.0f, 90.0f), 0,
                        m_cachedSelectionMode, m_labelShader, m_labelObj, activeCamera,
                        false, false, Drawer::LabelMid, Qt::AlignHCenter);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // Release shader
    glUseProgram(0);
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

    QVector3D lightColor = Utils::vectorFromColor(m_cachedTheme->lightColor());

    const Q3DCamera *activeCamera = m_cachedScene->activeCamera();

    glViewport(m_primarySubViewport.x(),
               m_primarySubViewport.y(),
               m_primarySubViewport.width(),
               m_primarySubViewport.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    GLfloat viewPortRatio = (GLfloat)m_primarySubViewport.width()
            / (GLfloat)m_primarySubViewport.height();
    projectionMatrix.perspective(45.0f, viewPortRatio, 0.1f, 100.0f);

    // Get the view matrix
    QMatrix4x4 viewMatrix = activeCamera->d_ptr->viewMatrix();

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

    bool rowMode = m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionRow);

    GLfloat rowScaleFactor = m_rowWidth / m_scaleFactor;
    GLfloat columnScaleFactor = m_columnDepth / m_scaleFactor;

    BarRenderItem *selectedBar(0);

#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
        // Render scene into a depth texture for using with shadow mapping
        // Enable drawing to depth framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthFrameBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Bind depth shader
        m_depthShader->bind();

        // Set viewport for depth map rendering. Must match texture size. Larger values give smoother shadows.
        // Depth viewport must always start from 0, 0, as it is rendered into a texture, not screen
        glViewport(0, 0,
                   m_primarySubViewport.width() * m_shadowQualityMultiplier,
                   m_primarySubViewport.height() * m_shadowQualityMultiplier);

        // Get the depth view matrix
        // It may be possible to hack lightPos here if we want to make some tweaks to shadow
        QVector3D depthLightPos = activeCamera->d_ptr->calculatePositionRelativeToCamera(
                    zeroVector, 0.0f, 3.5f / m_autoScaleAdjustment);
        depthViewMatrix.lookAt(depthLightPos, zeroVector, upVector);

        // Set the depth projection matrix
        depthProjectionMatrix.perspective(10.0f, viewPortRatio, 3.0f, 100.0f);
        depthProjectionViewMatrix = depthProjectionMatrix * depthViewMatrix;

        // Draw bars to depth buffer
        QVector3D shadowScaler(m_scaleX * m_seriesScaleX * 0.9f, 0.0f,
                               m_scaleZ * m_seriesScaleZ * 0.9f);
        foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
            if (baseCache->isVisible()) {
                BarSeriesRenderCache *cache = static_cast<BarSeriesRenderCache *>(baseCache);
                float seriesPos = m_seriesStart + m_seriesStep * cache->visualIndex() + 0.5f;
                ObjectHelper *barObj = cache->object();
                QQuaternion seriesRotation(cache->meshRotation());
                const BarRenderItemArray &renderArray = cache->renderArray();
                for (int row = startRow; row != stopRow; row += stepRow) {
                    const BarRenderItemRow &renderRow = renderArray.at(row);
                    for (int bar = startBar; bar != stopBar; bar += stepBar) {
                        const BarRenderItem &item = renderRow.at(bar);
                        if (!item.value())
                            continue;
                        GLfloat shadowOffset = 0.0f;
                        // Set front face culling for negative valued bars and back face culling
                        // for positive valued bars to remove peter-panning issues
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

                        colPos = (bar + seriesPos) * (m_cachedBarSpacing.width());
                        rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());

                        // Draw shadows for bars "on the other side" a bit off ground to avoid
                        // seeing shadows through the ground
                        modelMatrix.translate((colPos - m_rowWidth) / m_scaleFactor,
                                              item.height() + shadowOffset,
                                              (m_columnDepth - rowPos) / m_scaleFactor);
                        // Scale the bars down in X and Z to reduce self-shadowing issues
                        shadowScaler.setY(item.height());
                        if (!seriesRotation.isIdentity() || !item.rotation().isIdentity())
                            modelMatrix.rotate(seriesRotation * item.rotation());
                        modelMatrix.scale(shadowScaler);

                        MVPMatrix = depthProjectionViewMatrix * modelMatrix;

                        m_depthShader->setUniformValue(m_depthShader->MVP(), MVPMatrix);

                        // 1st attribute buffer : vertices
                        glEnableVertexAttribArray(m_depthShader->posAtt());
                        glBindBuffer(GL_ARRAY_BUFFER, barObj->vertexBuf());
                        glVertexAttribPointer(m_depthShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0,
                                              (void *)0);

                        // Index buffer
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, barObj->elementBuf());

                        // Draw the triangles
                        glDrawElements(GL_TRIANGLES, barObj->indexCount(), GL_UNSIGNED_SHORT,
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

        // Disable drawing to depth framebuffer (= enable drawing to screen)
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);

        // Reset culling to normal
        glCullFace(GL_BACK);

        // Revert to original viewport
        glViewport(m_primarySubViewport.x(),
                   m_primarySubViewport.y(),
                   m_primarySubViewport.width(),
                   m_primarySubViewport.height());
    }
#endif

    // Skip selection mode drawing if we're slicing or have no selection mode
    if (!m_cachedIsSlicingActivated && m_cachedSelectionMode > QAbstract3DGraph::SelectionNone
            && m_selectionState == SelectOnScene && m_visibleSeriesCount > 0) {
        // Bind selection shader
        m_selectionShader->bind();

        // Draw bars to selection buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
        glViewport(0, 0,
                   m_primarySubViewport.width(),
                   m_primarySubViewport.height());

        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (= selectionSkipColor)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled
        foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
            if (baseCache->isVisible()) {
                BarSeriesRenderCache *cache = static_cast<BarSeriesRenderCache *>(baseCache);
                float seriesPos = m_seriesStart + m_seriesStep * cache->visualIndex() + 0.5f;
                ObjectHelper *barObj = cache->object();
                QQuaternion seriesRotation(cache->meshRotation());
                const BarRenderItemArray &renderArray = cache->renderArray();
                for (int row = startRow; row != stopRow; row += stepRow) {
                    const BarRenderItemRow &renderRow = renderArray.at(row);
                    for (int bar = startBar; bar != stopBar; bar += stepBar) {
                        const BarRenderItem &item = renderRow.at(bar);
                        if (!item.value())
                            continue;

                        if (item.height() < 0)
                            glCullFace(GL_FRONT);
                        else
                            glCullFace(GL_BACK);

                        QMatrix4x4 modelMatrix;
                        QMatrix4x4 MVPMatrix;

                        colPos = (bar + seriesPos) * (m_cachedBarSpacing.width());
                        rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());

                        modelMatrix.translate((colPos - m_rowWidth) / m_scaleFactor,
                                              item.height(),
                                              (m_columnDepth - rowPos) / m_scaleFactor);
                        if (!seriesRotation.isIdentity() || !item.rotation().isIdentity())
                            modelMatrix.rotate(seriesRotation * item.rotation());
                        modelMatrix.scale(QVector3D(m_scaleX * m_seriesScaleX,
                                                    item.height(),
                                                    m_scaleZ * m_seriesScaleZ));

                        MVPMatrix = projectionViewMatrix * modelMatrix;

                        QVector4D barColor = QVector4D(GLfloat(row) / 255.0f,
                                                       GLfloat(bar) / 255.0f,
                                                       GLfloat(cache->visualIndex()) / 255.0f,
                                                       itemAlpha);

                        m_selectionShader->setUniformValue(m_selectionShader->MVP(), MVPMatrix);
                        m_selectionShader->setUniformValue(m_selectionShader->color(), barColor);

                        m_drawer->drawSelectionObject(m_selectionShader, barObj);
                    }
                }
            }
        }
        glCullFace(GL_BACK);
        drawCustomItems(RenderingSelection, m_selectionShader, activeCamera, projectionMatrix,
                        depthProjectionMatrix);
        drawLabels(true, activeCamera, viewMatrix, projectionMatrix, rowScaleFactor,
                   columnScaleFactor);
        glEnable(GL_DITHER);

        // Read color under cursor
        QVector4D clickedColor = Utils::getSelection(m_inputPosition,
                                                     m_viewport.height());
        m_clickedPosition = selectionColorToArrayPosition(clickedColor);
        m_clickedSeries = selectionColorToSeries(clickedColor);

        emit needRender();

        // Revert to original render target and viewport
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);
        glViewport(m_primarySubViewport.x(),
                   m_primarySubViewport.y(),
                   m_primarySubViewport.width(),
                   m_primarySubViewport.height());
    }

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    ShaderHelper *barShader = 0;
    GLuint gradientTexture = 0;
    Q3DTheme::ColorStyle previousColorStyle = Q3DTheme::ColorStyleUniform;

    // Set unchanging shader bindings
    if (m_haveGradientSeries) {
        m_barGradientShader->bind();
        m_barGradientShader->setUniformValue(m_barGradientShader->lightP(), lightPos);
        m_barGradientShader->setUniformValue(m_barGradientShader->view(), viewMatrix);
        m_barGradientShader->setUniformValue(m_barGradientShader->ambientS(),
                                             m_cachedTheme->ambientLightStrength());
        m_barGradientShader->setUniformValue(m_barGradientShader->gradientMin(), 0.0f);
        m_barGradientShader->setUniformValue(m_barGradientShader->lightColor(), lightColor);
    }

    if (m_haveUniformColorSeries) {
        m_barShader->bind();
        m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
        m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
        m_barShader->setUniformValue(m_barShader->ambientS(),
                                     m_cachedTheme->ambientLightStrength());
        m_barShader->setUniformValue(m_barShader->lightColor(), lightColor);
        barShader = m_barShader;
    } else {
        barShader = m_barGradientShader;
        previousColorStyle = Q3DTheme::ColorStyleRangeGradient;
    }

    int sliceReserveAmount = 0;
    if (m_selectionDirty && m_cachedIsSlicingActivated) {
        // Slice doesn't own its items, no need to delete them - just clear
        if (rowMode)
            sliceReserveAmount = m_cachedColumnCount;
        else
            sliceReserveAmount = m_cachedRowCount;

        // Set slice cache, i.e. axis cache from where slice labels are taken
        if (rowMode)
            m_sliceCache = &m_axisCacheX;
        else
            m_sliceCache = &m_axisCacheZ;
        m_sliceTitleItem = 0;
    }

    // Draw bars
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.5f, 1.0f);

    GLfloat adjustedLightStrength = m_cachedTheme->lightStrength() / 10.0f;
    GLfloat adjustedHighlightStrength = m_cachedTheme->highlightLightStrength() / 10.0f;

    bool barSelectionFound = false;

    QVector3D baseColor;
    QVector3D barColor;
    QVector3D modelScaler(m_scaleX * m_seriesScaleX, 0.0f, m_scaleZ * m_seriesScaleZ);
    bool somethingSelected = (m_visualSelectedBarPos != Bars3DController::invalidSelectionPosition());
    foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
        if (baseCache->isVisible()) {
            BarSeriesRenderCache *cache = static_cast<BarSeriesRenderCache *>(baseCache);
            float seriesPos = m_seriesStart + m_seriesStep * cache->visualIndex() + 0.5f;
            ObjectHelper *barObj = cache->object();
            QQuaternion seriesRotation(cache->meshRotation());
            Q3DTheme::ColorStyle colorStyle = cache->colorStyle();
            BarRenderItemArray &renderArray = cache->renderArray();
            bool colorStyleIsUniform = (colorStyle == Q3DTheme::ColorStyleUniform);
            if (sliceReserveAmount)
                cache->sliceArray().resize(sliceReserveAmount);

            // Rebind shader if it has changed
            if (colorStyleIsUniform != (previousColorStyle == Q3DTheme::ColorStyleUniform)) {
                if (colorStyleIsUniform)
                    barShader = m_barShader;
                else
                    barShader = m_barGradientShader;
                barShader->bind();
            }

            if (colorStyleIsUniform) {
                baseColor = cache->baseColor();
            } else if ((previousColorStyle != colorStyle)
                       && (colorStyle == Q3DTheme::ColorStyleObjectGradient)) {
                m_barGradientShader->setUniformValue(m_barGradientShader->gradientHeight(), 0.5f);
            }

            // Always use base color when no selection mode
            if (m_cachedSelectionMode == QAbstract3DGraph::SelectionNone) {
                if (colorStyleIsUniform)
                    barColor = baseColor;
                else
                    gradientTexture = cache->baseGradientTexture();
            }

            previousColorStyle = colorStyle;

            for (int row = startRow; row != stopRow; row += stepRow) {
                BarRenderItemRow &renderRow = renderArray[row];
                for (int bar = startBar; bar != stopBar; bar += stepBar) {
                    BarRenderItem &item = renderRow[bar];
                    if (item.height() < 0)
                        glCullFace(GL_FRONT);
                    else
                        glCullFace(GL_BACK);

                    QMatrix4x4 modelMatrix;
                    QMatrix4x4 itModelMatrix;
                    QMatrix4x4 MVPMatrix;

                    colPos = (bar + seriesPos) * (m_cachedBarSpacing.width());
                    rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());

                    modelMatrix.translate((colPos - m_rowWidth) / m_scaleFactor,
                                          item.height(),
                                          (m_columnDepth - rowPos) / m_scaleFactor);
                    modelScaler.setY(item.height());
                    if (!seriesRotation.isIdentity() || !item.rotation().isIdentity()) {
                        QQuaternion totalRotation = seriesRotation * item.rotation();
                        modelMatrix.rotate(totalRotation);
                        itModelMatrix.rotate(totalRotation);
                    }
                    modelMatrix.scale(modelScaler);
                    itModelMatrix.scale(modelScaler);
#ifdef SHOW_DEPTH_TEXTURE_SCENE
                    MVPMatrix = depthProjectionViewMatrix * modelMatrix;
#else
                    MVPMatrix = projectionViewMatrix * modelMatrix;
#endif
                    GLfloat lightStrength = m_cachedTheme->lightStrength();
                    GLfloat shadowLightStrength = adjustedLightStrength;

                    if (m_cachedSelectionMode > QAbstract3DGraph::SelectionNone) {
                        Bars3DController::SelectionType selectionType =
                                Bars3DController::SelectionNone;
                        if (somethingSelected)
                            selectionType = isSelected(row, bar, cache);

                        switch (selectionType) {
                        case Bars3DController::SelectionItem: {
                            if (colorStyleIsUniform)
                                barColor = cache->singleHighlightColor();
                            else
                                gradientTexture = cache->singleHighlightGradientTexture();

                            lightStrength = m_cachedTheme->highlightLightStrength();
                            shadowLightStrength = adjustedHighlightStrength;
                            // Insert position data into render item
                            // We have no ownership, don't delete the previous one
                            if (!m_cachedIsSlicingActivated
                                    && m_selectedSeriesCache == cache) {
                                selectedBar = &item;
                                selectedBar->setPosition(QPoint(row, bar));
                                item.setTranslation(modelMatrix.column(3).toVector3D());
                                barSelectionFound = true;
                            }
                            if (m_selectionDirty && m_cachedIsSlicingActivated) {
                                QVector3D translation = modelMatrix.column(3).toVector3D();
                                if (m_cachedSelectionMode & QAbstract3DGraph::SelectionColumn
                                        && m_visibleSeriesCount > 1) {
                                    translation.setZ((m_columnDepth
                                                      - ((row + seriesPos)
                                                         * (m_cachedBarSpacing.height())))
                                                     / m_scaleFactor);
                                }
                                item.setTranslation(translation);
                                item.setPosition(QPoint(row, bar));
                                if (rowMode)
                                    cache->sliceArray()[bar].setItem(item);
                                else
                                    cache->sliceArray()[row].setItem(item);
                            }
                            break;
                        }
                        case Bars3DController::SelectionRow: {
                            // Current bar is on the same row as the selected bar
                            if (colorStyleIsUniform)
                                barColor = cache->multiHighlightColor();
                            else
                                gradientTexture = cache->multiHighlightGradientTexture();

                            lightStrength = m_cachedTheme->highlightLightStrength();
                            shadowLightStrength = adjustedHighlightStrength;
                            if (m_cachedIsSlicingActivated) {
                                item.setTranslation(modelMatrix.column(3).toVector3D());
                                item.setPosition(QPoint(row, bar));
                                if (m_selectionDirty) {
                                    if (!m_sliceTitleItem && m_axisCacheZ.labelItems().size() > row)
                                        m_sliceTitleItem = m_axisCacheZ.labelItems().at(row);
                                    cache->sliceArray()[bar].setItem(item);
                                }
                            }
                            break;
                        }
                        case Bars3DController::SelectionColumn: {
                            // Current bar is on the same column as the selected bar
                            if (colorStyleIsUniform)
                                barColor = cache->multiHighlightColor();
                            else
                                gradientTexture = cache->multiHighlightGradientTexture();

                            lightStrength = m_cachedTheme->highlightLightStrength();
                            shadowLightStrength = adjustedHighlightStrength;
                            if (m_cachedIsSlicingActivated) {
                                QVector3D translation = modelMatrix.column(3).toVector3D();
                                if (m_visibleSeriesCount > 1) {
                                    translation.setZ((m_columnDepth - ((row + seriesPos)
                                                                       * (m_cachedBarSpacing.height())))
                                                     / m_scaleFactor);
                                }
                                item.setTranslation(translation);
                                item.setPosition(QPoint(row, bar));
                                if (m_selectionDirty) {
                                    if (!m_sliceTitleItem && m_axisCacheX.labelItems().size() > bar)
                                        m_sliceTitleItem = m_axisCacheX.labelItems().at(bar);
                                    cache->sliceArray()[row].setItem(item);
                                }
                            }
                            break;
                        }
                        case Bars3DController::SelectionNone: {
                            // Current bar is not selected, nor on a row or column
                            if (colorStyleIsUniform)
                                barColor = baseColor;
                            else
                                gradientTexture = cache->baseGradientTexture();
                            break;
                        }
                        }
                    }

                    // Skip drawing of 0-height bars
                    if (item.height() != 0) {
                        // Set shader bindings
                        barShader->setUniformValue(barShader->model(), modelMatrix);
                        barShader->setUniformValue(barShader->nModel(),
                                                   itModelMatrix.transposed().inverted());
                        barShader->setUniformValue(barShader->MVP(), MVPMatrix);
                        if (colorStyleIsUniform) {
                            barShader->setUniformValue(barShader->color(), barColor);
                        } else if (colorStyle == Q3DTheme::ColorStyleRangeGradient) {
                            barShader->setUniformValue(barShader->gradientHeight(),
                                                       qAbs(item.height()) / m_gradientFraction);
                        }

#if !defined(QT_OPENGL_ES_2)
                        if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                            // Set shadow shader bindings
                            QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                            barShader->setUniformValue(barShader->shadowQ(), m_shadowQualityToShader);
                            barShader->setUniformValue(barShader->depth(), depthMVPMatrix);
                            barShader->setUniformValue(barShader->lightS(), shadowLightStrength);
                            barShader->setUniformValue(barShader->lightColor(), lightColor);

                            // Draw the object
                            m_drawer->drawObject(barShader, barObj, gradientTexture, m_depthTexture);
                        } else
#else
                        Q_UNUSED(shadowLightStrength);
#endif
                        {
                            // Set shadowless shader bindings
                            barShader->setUniformValue(barShader->lightS(), lightStrength);

                            // Draw the object
                            m_drawer->drawObject(barShader, barObj, gradientTexture);
                        }
                    }
                }
            }
        }
    }

    glDisable(GL_POLYGON_OFFSET_FILL);

    drawCustomItems(RenderingNormal, m_customItemShader, activeCamera, projectionMatrix,
                    depthProjectionMatrix);

    // Bind background shader
    m_backgroundShader->bind();

    // Reset culling
    glCullFace(GL_BACK);

    // Draw background
    if (m_cachedTheme->isBackgroundEnabled() && m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 itModelMatrix;

        QVector3D backgroundScaler(rowScaleFactor, 1.0f, columnScaleFactor);
        modelMatrix.translate(0.0f, m_negativeBackgroundAdjustment, 0.0f);

        modelMatrix.scale(backgroundScaler);
        itModelMatrix.scale(backgroundScaler);
        modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);
        itModelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);

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
                                                adjustedLightStrength);

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

        // Draw floor for graph with negatives
        if (m_hasNegativeValues) {
            modelMatrix = QMatrix4x4();
            itModelMatrix = QMatrix4x4();

            modelMatrix.scale(backgroundScaler);

            if (m_yFlipped)
                modelMatrix.rotate(90.0f, 1.0f, 0.0f, 0.0f);
            else
                modelMatrix.rotate(-90.0f, 1.0f, 0.0f, 0.0f);

            itModelMatrix = modelMatrix;

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
            if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
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

    // Disable textures
    glDisable(GL_TEXTURE_2D);

    // Draw grid lines
    if (m_cachedTheme->isGridEnabled()) {
#if !(defined QT_OPENGL_ES_2)
        ShaderHelper *lineShader = m_backgroundShader;
#else
        ShaderHelper *lineShader = m_selectionShader; // Plain color shader for GL_LINES
#endif
        QQuaternion lineRotation;

        // Bind bar shader
        lineShader->bind();

        // Set unchanging shader bindings
        QVector3D barColor = Utils::vectorFromColor(m_cachedTheme->gridLineColor());
        lineShader->setUniformValue(lineShader->lightP(), lightPos);
        lineShader->setUniformValue(lineShader->view(), viewMatrix);
        lineShader->setUniformValue(lineShader->color(), barColor);
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

        GLfloat yFloorLinePosition = gridLineOffset;
        if (m_yFlipped)
            yFloorLinePosition = -yFloorLinePosition;

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

        // Floor lines: columns
#if defined(QT_OPENGL_ES_2)
        lineRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 90.0f);
#endif
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

        if (m_axisCacheY.segmentCount() > 0) {
            // Wall lines: back wall
            int gridLineCount = m_axisCacheY.gridLineCount();

            GLfloat zWallLinePosition = -columnScaleFactor + gridLineOffset;
            if (m_zFlipped)
                zWallLinePosition = -zWallLinePosition;

            gridLineScaler = QVector3D(rowScaleFactor, gridLineWidth, gridLineWidth);
            for (int line = 0; line < gridLineCount; line++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(0.0f,
                                      m_axisCacheY.gridLinePosition(line),
                                      zWallLinePosition);
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

            // Wall lines: side wall
            GLfloat xWallLinePosition = -rowScaleFactor + gridLineOffset;
            if (m_xFlipped)
                xWallLinePosition = -xWallLinePosition;

            if (m_xFlipped)
                lineRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, -90.0f);
            else
                lineRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 90.0f);

            gridLineScaler = QVector3D(gridLineWidth, gridLineWidth, columnScaleFactor);
            for (int line = 0; line < gridLineCount; line++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(xWallLinePosition,
                                      m_axisCacheY.gridLinePosition(line),
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
    drawLabels(false, activeCamera, viewMatrix, projectionMatrix, rowScaleFactor,
               columnScaleFactor);

    // Handle selected bar label generation
    if (barSelectionFound) {
        // Print value of selected bar
        glDisable(GL_DEPTH_TEST);
        // Draw the selection label
        LabelItem &labelItem = selectionLabelItem();
        if (m_selectedBar != selectedBar || m_updateLabels || !labelItem.textureId()
                || m_selectionLabelDirty) {
            QString labelText = selectionLabel();
            if (labelText.isNull() || m_selectionLabelDirty) {
                labelText = m_selectedSeriesCache->itemLabel();
                setSelectionLabel(labelText);
                m_selectionLabelDirty = false;
            }
            m_drawer->generateLabelItem(labelItem, labelText);
            m_selectedBar = selectedBar;
        }

        m_drawer->drawLabel(*selectedBar, labelItem, viewMatrix, projectionMatrix,
                            zeroVector, zeroVector, selectedBar->height(),
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

    // Release shader
    glUseProgram(0);
    m_selectionDirty = false;
}

void Bars3DRenderer::drawCustomItems(RenderingState state, ShaderHelper *shader,
                                     const Q3DCamera *activeCamera,
                                     const QMatrix4x4 &projectionMatrix,
                                     const QMatrix4x4 &depthProjectionMatrix)
{
    // TODO
}

void Bars3DRenderer::drawLabels(bool drawSelection, const Q3DCamera *activeCamera,
                                const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix,
                                GLfloat rowScaleFactor, GLfloat columnScaleFactor) {
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

    // Y Labels
    int labelNbr = 0;
    int labelCount = m_axisCacheY.labelCount();
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
            backLabelTrans.setY(m_axisCacheY.labelPosition(i));
            sideLabelTrans.setY(backLabelTrans.y());

            glPolygonOffset(GLfloat(i) / -10.0f, 1.0f);

            const LabelItem &axisLabelItem = *m_axisCacheY.labelItems().at(labelNbr);

            if (drawSelection) {
                QVector4D labelColor = QVector4D(0.0f, 0.0f, i / 255.0f,
                                                 alphaForValueSelection);
                shader->setUniformValue(shader->color(), labelColor);
            }

            // Back wall
            m_dummyBarRenderItem.setTranslation(backLabelTrans);
            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                zeroVector, backLabelRotation, 0, m_cachedSelectionMode,
                                shader, m_labelObj, activeCamera,
                                true, true, Drawer::LabelMid, backAlignment);

            // Side wall
            m_dummyBarRenderItem.setTranslation(sideLabelTrans);
            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                zeroVector, sideLabelRotation, 0, m_cachedSelectionMode,
                                shader, m_labelObj, activeCamera,
                                true, true, Drawer::LabelMid, sideAlignment);
        }
        labelNbr++;
    }

    // Z labels
    // Calculate the positions for row and column labels and store them
    GLfloat labelYAdjustment = 0.005f;
    GLfloat scaledRowWidth = rowScaleFactor;
    GLfloat scaledColumnDepth = columnScaleFactor;
    GLfloat colPosValue = scaledRowWidth + labelMargin;
    GLfloat rowPosValue = scaledColumnDepth + labelMargin;
    GLfloat rowPos = 0.0f;
    GLfloat colPos = 0.0f;
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
        if (m_axisCacheZ.labelItems().size() > row) {
            // Go through all rows and get position of max+1 or min-1 column, depending on x flip
            // We need only positions for them, labels have already been generated
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
            const LabelItem &axisLabelItem = *m_axisCacheZ.labelItems().at(row);

            if (drawSelection) {
                QVector4D labelColor = QVector4D(row / 255.0f, 0.0f, 0.0f, alphaForRowSelection);
                shader->setUniformValue(shader->color(), labelColor);
            }

            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                zeroVector, labelRotation, 0, m_cachedSelectionMode,
                                shader, m_labelObj, activeCamera,
                                true, true, Drawer::LabelMid, alignment,
                                false, drawSelection);
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

    // X labels
    alignment = m_zFlipped ? Qt::AlignRight : Qt::AlignLeft;
    for (int column = 0; column != m_cachedColumnCount; column++) {
        if (m_axisCacheX.labelItems().size() > column) {
            // Go through all columns and get position of max+1 or min-1 row, depending on z flip
            // We need only positions for them, labels have already been generated
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
            const LabelItem &axisLabelItem = *m_axisCacheX.labelItems().at(column);

            if (drawSelection) {
                QVector4D labelColor = QVector4D(0.0f, column / 255.0f, 0.0f,
                                                 alphaForColumnSelection);
                shader->setUniformValue(shader->color(), labelColor);
            }

            m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                zeroVector, labelRotation, 0, m_cachedSelectionMode,
                                shader, m_labelObj, activeCamera,
                                true, true, Drawer::LabelMid, alignment, false, drawSelection);
        }
    }

    glDisable(GL_POLYGON_OFFSET_FILL);
}

void Bars3DRenderer::updateMultiSeriesScaling(bool uniform)
{
    m_keepSeriesUniform = uniform;

    // Recalculate scale factors
    m_seriesScaleX = 1.0f / float(m_visibleSeriesCount);
    if (m_keepSeriesUniform)
        m_seriesScaleZ = m_seriesScaleX;
    else
        m_seriesScaleZ = 1.0f;
}

void Bars3DRenderer::updateBarSpecs(GLfloat thicknessRatio, const QSizeF &spacing, bool relative)
{
    // Convert ratio to QSizeF, as we need it in that format for autoscaling calculations
    m_cachedBarThickness.setWidth(1.0f);
    m_cachedBarThickness.setHeight(1.0f / thicknessRatio);

    if (relative) {
        m_cachedBarSpacing.setWidth((m_cachedBarThickness.width() * 2)
                                    * (spacing.width() + 1.0f));
        m_cachedBarSpacing.setHeight((m_cachedBarThickness.height() * 2)
                                     * (spacing.height() + 1.0f));
    } else {
        m_cachedBarSpacing = m_cachedBarThickness * 2 + spacing * 2;
    }

    // Slice mode doesn't update correctly without this
    if (m_cachedIsSlicingActivated)
        m_selectionDirty = true;

    // Calculate here and at setting sample space
    calculateSceneScalingFactors();
}

void Bars3DRenderer::updateAxisRange(QAbstract3DAxis::AxisOrientation orientation, float min,
                                     float max)
{
    Abstract3DRenderer::updateAxisRange(orientation, min, max);

    if (orientation == QAbstract3DAxis::AxisOrientationY) {
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
        calculateHeightAdjustment();
    }
}

void Bars3DRenderer::updateSelectedBar(const QPoint &position, QBar3DSeries *series)
{
    m_selectedBarPos = position;
    m_selectedSeriesCache = static_cast<BarSeriesRenderCache *>(m_renderCacheList.value(series, 0));
    m_selectionDirty = true;
    m_selectionLabelDirty = true;

    if (!m_selectedSeriesCache
            || !m_selectedSeriesCache->isVisible()
            || m_selectedSeriesCache->renderArray().isEmpty()) {
        m_visualSelectedBarPos = Bars3DController::invalidSelectionPosition();
        return;
    }

    int adjustedZ = m_selectedBarPos.x() - int(m_axisCacheZ.min());
    int adjustedX = m_selectedBarPos.y() - int(m_axisCacheX.min());
    int maxZ = m_selectedSeriesCache->renderArray().size() - 1;
    int maxX = maxZ >= 0 ? m_selectedSeriesCache->renderArray().at(0).size() - 1 : -1;

    if (m_selectedBarPos == Bars3DController::invalidSelectionPosition()
            || adjustedZ < 0 || adjustedZ > maxZ
            || adjustedX < 0 || adjustedX > maxX) {
        m_visualSelectedBarPos = Bars3DController::invalidSelectionPosition();
    } else {
        m_visualSelectedBarPos = QPoint(adjustedZ, adjustedX);
    }
}

void Bars3DRenderer::resetClickedStatus()
{
    m_clickedPosition = Bars3DController::invalidSelectionPosition();
    m_clickedSeries = 0;
}

void Bars3DRenderer::updateShadowQuality(QAbstract3DGraph::ShadowQuality quality)
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
        m_shadowQualityToShader = 7.5f;
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

void Bars3DRenderer::fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh mesh)
{
    if (!m_cachedTheme->isBackgroundEnabled()) {
        // Load full version of meshes that have it available
        // Note: Minimal, Point, and Arrow not supported in bar charts
        if (mesh != QAbstract3DSeries::MeshSphere)
            fileName.append(QStringLiteral("Full"));
    }
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
    GLfloat newAdjustment = 1.0f;
    GLfloat maxAbs = qFabs(m_axisCacheY.max());

    if (m_axisCacheY.max() < 0.0f) {
        m_heightNormalizer = GLfloat(qFabs(m_axisCacheY.min()) - qFabs(m_axisCacheY.max()));
        maxAbs = qFabs(m_axisCacheY.max()) - qFabs(m_axisCacheY.min());
    } else {
        m_heightNormalizer = GLfloat(m_axisCacheY.max() - m_axisCacheY.min());
    }

    // Height fractions are used in gradient calculations and are therefore doubled
    // Note that if max or min is exactly zero, we still consider it outside the range
    if (m_axisCacheY.max() <= 0.0f || m_axisCacheY.min() >= 0.0f) {
        m_noZeroInRange = true;
        m_gradientFraction = 2.0f;
    } else {
        m_noZeroInRange = false;
        GLfloat minAbs = qFabs(m_axisCacheY.min());
        m_gradientFraction = qMax(minAbs, maxAbs) / m_heightNormalizer * 2.0f;
    }

    // Calculate translation adjustment for negative background
    if (m_hasNegativeValues)
        newAdjustment = (qBound(0.0f, (maxAbs / m_heightNormalizer), 1.0f) - 0.5f) * 2.0f;

    if (newAdjustment != m_negativeBackgroundAdjustment) {
        m_negativeBackgroundAdjustment = newAdjustment;
        m_axisCacheY.setTranslate(m_negativeBackgroundAdjustment - 1.0f);
    }
}

Bars3DController::SelectionType Bars3DRenderer::isSelected(int row, int bar,
                                                           const BarSeriesRenderCache *cache)
{
    Bars3DController::SelectionType isSelectedType = Bars3DController::SelectionNone;

    if ((m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionMultiSeries)
         && m_selectedSeriesCache) || cache == m_selectedSeriesCache) {
        if (row == m_visualSelectedBarPos.x() && bar == m_visualSelectedBarPos.y()
                && (m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionItem))) {
            isSelectedType = Bars3DController::SelectionItem;
        } else if (row == m_visualSelectedBarPos.x()
                   && (m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionRow))) {
            isSelectedType = Bars3DController::SelectionRow;
        } else if (bar == m_visualSelectedBarPos.y()
                   && (m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionColumn))) {
            isSelectedType = Bars3DController::SelectionColumn;
        }
    }

    return isSelectedType;
}

QPoint Bars3DRenderer::selectionColorToArrayPosition(const QVector4D &selectionColor)
{
    QPoint position = Bars3DController::invalidSelectionPosition();
    m_clickedType = QAbstract3DGraph::ElementNone;
    if (selectionColor.w() == itemAlpha) {
        // Normal selection item
        position = QPoint(int(selectionColor.x() + int(m_axisCacheZ.min())),
                          int(selectionColor.y()) + int(m_axisCacheX.min()));
        // Pass item clicked info to input handler
        m_clickedType = QAbstract3DGraph::ElementSeries;
    } else if (selectionColor.w() == labelRowAlpha) {
        // Row selection
        if (m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionRow)) {
            // Use column from previous selection in case we have row + column mode
            GLint previousCol = qMax(0, m_selectedBarPos.y()); // Use 0 if previous is invalid
            position = QPoint(int(selectionColor.x() + int(m_axisCacheZ.min())), previousCol);
        }
        // Pass label clicked info to input handler
        m_clickedType = QAbstract3DGraph::ElementAxisZLabel;
    } else if (selectionColor.w() == labelColumnAlpha) {
        // Column selection
        if (m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionColumn)) {
            // Use row from previous selection in case we have row + column mode
            GLint previousRow = qMax(0, m_selectedBarPos.x()); // Use 0 if previous is invalid
            position = QPoint(previousRow, int(selectionColor.y()) + int(m_axisCacheX.min()));
        }
        // Pass label clicked info to input handler
        m_clickedType = QAbstract3DGraph::ElementAxisXLabel;
    } else if (selectionColor.w() == labelValueAlpha) {
        // Value selection
        position = Bars3DController::invalidSelectionPosition();
        // Pass label clicked info to input handler
        m_clickedType = QAbstract3DGraph::ElementAxisYLabel;
    } else if (selectionColor.w() == customItemAlpha) {
        // Custom item selection
        position = Bars3DController::invalidSelectionPosition();
        m_clickedType = QAbstract3DGraph::ElementCustomItem;
    }
    return position;
}

QBar3DSeries *Bars3DRenderer::selectionColorToSeries(const QVector4D &selectionColor)
{
    if (selectionColor == selectionSkipColor) {
        return 0;
    } else {
        int seriesIndexFromColor(selectionColor.z());
        foreach (SeriesRenderCache *baseCache, m_renderCacheList) {
            BarSeriesRenderCache *cache = static_cast<BarSeriesRenderCache *>(baseCache);
            if (cache->visualIndex() == seriesIndexFromColor)
                return cache->series();
        }
    }
    return 0;
}

void Bars3DRenderer::updateSlicingActive(bool isSlicing)
{
    if (isSlicing == m_cachedIsSlicingActivated)
        return;

    m_cachedIsSlicingActivated = isSlicing;

    if (!m_cachedIsSlicingActivated)
        initSelectionBuffer(); // We need to re-init selection buffer in case there has been a resize

#if !defined(QT_OPENGL_ES_2)
    updateDepthBuffer(); // Re-init depth buffer as well
#endif

    m_selectionDirty = true;
}

void Bars3DRenderer::initShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_barShader)
        delete m_barShader;
    m_barShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_barShader->initialize();
}

void Bars3DRenderer::initGradientShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_barGradientShader)
        delete m_barGradientShader;
    m_barGradientShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_barGradientShader->initialize();
}

void Bars3DRenderer::initSelectionShader()
{
    if (m_selectionShader)
        delete m_selectionShader;
    m_selectionShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexPlainColor"),
                                         QStringLiteral(":/shaders/fragmentPlainColor"));
    m_selectionShader->initialize();
}

void Bars3DRenderer::initSelectionBuffer()
{
    if (m_selectionTexture) {
        m_textureHelper->deleteTexture(&m_selectionTexture);
        m_selectionTexture = 0;
    }

    if (m_cachedIsSlicingActivated || m_primarySubViewport.size().isEmpty())
        return;

    m_selectionTexture = m_textureHelper->createSelectionTexture(m_primarySubViewport.size(),
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

QT_END_NAMESPACE_DATAVISUALIZATION
