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

#include "surface3dcontroller_p.h"
#include "surface3drenderer_p.h"
#include "q3dcamera.h"
#include "q3dcamera_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "surfaceobject_p.h"
#include "texturehelper_p.h"
#include "selectionpointer_p.h"
#include "utils_p.h"
#include "drawer_p.h"
#include "q3dlight.h"
#include "qsurface3dseries_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <qmath.h>

#include <QDebug>

static const int ID_TO_RGBA_MASK = 0xff;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

//#define SHOW_DEPTH_TEXTURE_SCENE

const GLfloat aspectRatio = 2.0f; // Forced ratio of x and z to y. Dynamic will make it look odd.
const GLfloat backgroundMargin = 1.1f; // Margin for background (1.1f = make it 10% larger to avoid items being drawn inside background)
const GLfloat labelMargin = 0.05f;
const GLfloat backgroundBottom = 1.0f;
const GLfloat gridLineWidth = 0.005f;
const GLfloat sliceZScale = 0.1f;
const GLfloat sliceUnits = 2.5f;
const int subViewDivider = 5;
const uint invalidSelectionId = uint(-1);

Surface3DRenderer::Surface3DRenderer(Surface3DController *controller)
    : Abstract3DRenderer(controller),
      m_labelBackground(false),
      m_font(QFont(QStringLiteral("Arial"))),
      m_isGridEnabled(true),
      m_cachedIsSlicingActivated(false),
      m_depthShader(0),
      m_backgroundShader(0),
      m_surfaceShader(0),
      m_surfaceGridShader(0),
      m_selectionShader(0),
      m_labelShader(0),
      m_heightNormalizer(0.0f),
      m_scaleFactor(0.0f),
      m_scaleX(0.0f),
      m_scaleZ(0.0f),
      m_scaleXWithBackground(0.0f),
      m_scaleZWithBackground(0.0f),
      m_surfaceScaleX(0.0f),
      m_surfaceScaleZ(0.0f),
      m_surfaceOffsetX(0.0f),
      m_surfaceOffsetZ(0.0f),
      m_minVisibleColumnValue(0.0f),
      m_maxVisibleColumnValue(0.0f),
      m_minVisibleRowValue(0.0f),
      m_maxVisibleRowValue(0.0f),
      m_visibleColumnRange(0.0f),
      m_visibleRowRange(0.0f),
      m_backgroundObj(0),
      m_gridLineObj(0),
      m_labelObj(0),
      m_surfaceObj(0),
      m_sliceSurfaceObj(0),
      m_depthTexture(0),
      m_depthModelTexture(0),
      m_depthFrameBuffer(0),
      m_selectionFrameBuffer(0),
      m_selectionDepthBuffer(0),
      m_selectionTexture(0),
      m_selectionResultTexture(0),
      m_shadowQualityToShader(33.3f),
      m_cachedFlatShading(false),
      m_flatSupported(true),
      m_cachedSurfaceVisible(true),
      m_cachedSurfaceGridOn(true),
      m_selectionPointer(0),
      m_selectionActive(false),
      m_xFlipped(false),
      m_zFlipped(false),
      m_yFlipped(false),
      m_sampleSpace(QRect(0, 0, 0, 0)),
      m_shadowQualityMultiplier(3),
      m_clickedPointId(invalidSelectionId),
      m_hasHeightAdjustmentChanged(true),
      m_selectedPoint(Surface3DController::invalidSelectionPosition()),
      m_selectedSeries(0),
      m_uniformGradientTexture(0),
      m_clickedPosition(Surface3DController::invalidSelectionPosition())
{
    // Check if flat feature is supported
    ShaderHelper tester(this, QStringLiteral(":/shaders/vertexSurfaceFlat"),
                        QStringLiteral(":/shaders/fragmentSurfaceFlat"));
    if (!tester.testCompile()) {
        m_flatSupported = false;
        connect(this, &Surface3DRenderer::flatShadingSupportedChanged,
                controller, &Surface3DController::handleFlatShadingSupportedChange);
        emit flatShadingSupportedChanged(m_flatSupported);
        qWarning() << "Warning: Flat qualifier not supported on your platform's GLSL language."
                      " Requires at least GLSL version 1.2 with GL_EXT_gpu_shader4 extension.";
    }

    initializeOpenGLFunctions();
    initializeOpenGL();

    // Create initial uniform gradient
    generateUniformGradient(m_uniformGradientTextureColor);
}

Surface3DRenderer::~Surface3DRenderer()
{
    if (QOpenGLContext::currentContext()) {
        m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
        m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
        m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);

        m_textureHelper->deleteTexture(&m_depthTexture);
        m_textureHelper->deleteTexture(&m_depthModelTexture);
        m_textureHelper->deleteTexture(&m_selectionTexture);
        m_textureHelper->deleteTexture(&m_selectionResultTexture);
        m_textureHelper->deleteTexture(&m_uniformGradientTexture);
    }
    delete m_depthShader;
    delete m_backgroundShader;
    delete m_selectionShader;
    delete m_surfaceShader;
    delete m_surfaceGridShader;
    delete m_labelShader;

    delete m_backgroundObj;
    delete m_surfaceObj;
    delete m_sliceSurfaceObj;
    delete m_gridLineObj;
    delete m_labelObj;

    delete m_selectionPointer;

    for (int i = 0; i < m_dataArray.size(); i++)
        delete m_dataArray.at(i);
    m_dataArray.clear();

    for (int i = 0; i < m_sliceDataArray.size(); i++)
        delete m_sliceDataArray.at(i);
    m_sliceDataArray.clear();
}

void Surface3DRenderer::initializeOpenGL()
{
    Abstract3DRenderer::initializeOpenGL();

    // Initialize shaders
    initSurfaceShaders();
    initLabelShaders(QStringLiteral(":/shaders/vertexLabel"),
                     QStringLiteral(":/shaders/fragmentLabel"));

#if !defined(QT_OPENGL_ES_2)
    // Init depth shader (for shadows). Init in any case, easier to handle shadow activation if done via api.
    initDepthShader();
#endif

    // Init selection shader
    initSelectionShaders();

    // Load grid line mesh
    loadGridLineMesh();

    // Load label mesh
    loadLabelMesh();

    // Resize in case we've missed resize events
    // Resize calls initSelectionBuffer and initDepthBuffer, so they don't need to be called here
    handleResize();

    // Load background mesh (we need to be initialized first)
    loadBackgroundMesh();
}

void Surface3DRenderer::updateData()
{
    // Surface only supports single series for now, so we are only interested in the first series
    const QSurfaceDataArray *array = 0;
    if (m_visibleSeriesList.size()) {
        QSurface3DSeries *firstSeries = static_cast<QSurface3DSeries *>(m_visibleSeriesList.at(0).series());
        QSurfaceDataProxy *dataProxy = firstSeries->dataProxy();
        if (dataProxy)
            array = dataProxy->array();
    }

    calculateSceneScalingFactors();

    // Need minimum of 2x2 array to draw a surface
    if (array && array->size() >= 2 && array->at(0)->size() >= 2) {
        QRect sampleSpace = calculateSampleRect(*array);

        bool dimensionChanged = false;
        if (m_sampleSpace != sampleSpace) {
            dimensionChanged = true;
            m_sampleSpace = sampleSpace;

            for (int i = 0; i < m_dataArray.size(); i++)
                delete m_dataArray.at(i);
            m_dataArray.clear();
        }

        if (sampleSpace.width() >= 2 && sampleSpace.height() >= 2) {
            if (dimensionChanged) {
                m_dataArray.reserve(sampleSpace.height());
                for (int i = 0; i < sampleSpace.height(); i++)
                    m_dataArray << new QSurfaceDataRow(sampleSpace.width());
            }
            for (int i = 0; i < sampleSpace.height(); i++) {
                for (int j = 0; j < sampleSpace.width(); j++)
                    (*(m_dataArray.at(i)))[j] = array->at(i + sampleSpace.y())->at(
                            j + sampleSpace.x());
            }

            if (m_dataArray.size() > 0) {
                if (!m_surfaceObj)
                    loadSurfaceObj();

                // Note: Data setup can change sample space (as min width/height is 1)
                if (!m_cachedFlatShading) {
                    m_surfaceObj->setUpSmoothData(m_dataArray, m_sampleSpace, m_heightNormalizer,
                                                  m_axisCacheY.min(), dimensionChanged);
                } else {
                    m_surfaceObj->setUpData(m_dataArray, m_sampleSpace, m_heightNormalizer,
                                            m_axisCacheY.min(), dimensionChanged);
                }

                if (dimensionChanged)
                    updateSelectionTexture();
            }
        }
    } else {
        for (int i = 0; i < m_dataArray.size(); i++)
            delete m_dataArray.at(i);
        m_dataArray.clear();
        m_sampleSpace = QRect();

        delete m_surfaceObj;
        m_surfaceObj = 0;
#if !defined(QT_OPENGL_ES_2)
        m_textureHelper->fillDepthTexture(m_depthTexture, m_primarySubViewport.size(),
                                          m_shadowQualityMultiplier, 1.0f);
#endif
    }

    for (int i = 0; i < m_sliceDataArray.size(); i++)
        delete m_sliceDataArray.at(i);
    m_sliceDataArray.clear();

    updateSelectedPoint(m_selectedPoint, m_selectedSeries);
}

void Surface3DRenderer::updateSeries(const QList<QAbstract3DSeries *> &seriesList,
                                     bool updateVisibility)
{
    Abstract3DRenderer::updateSeries(seriesList, updateVisibility);

    if (m_visibleSeriesList.size()) {
        QSurface3DSeries *series = static_cast<QSurface3DSeries *>(m_visibleSeriesList.at(0).series());
        updateFlatStatus(series->isFlatShadingEnabled());

        QSurface3DSeries::DrawFlags drawMode = series->drawMode();
        m_cachedSurfaceVisible = drawMode.testFlag(QSurface3DSeries::DrawSurface);
#if !defined(QT_OPENGL_ES_2)
        if (!m_cachedSurfaceVisible) {
            m_textureHelper->fillDepthTexture(m_depthTexture, m_primarySubViewport.size(),
                                              m_shadowQualityMultiplier, 1.0f);
        }
#endif
        m_cachedSurfaceGridOn = drawMode.testFlag(QSurface3DSeries::DrawWireframe);

        QVector3D seriesColor = Utils::vectorFromColor(series->baseColor());
        if (m_uniformGradientTextureColor != seriesColor)
            generateUniformGradient(seriesColor);
        if (m_selectionPointer) {
            m_selectionPointer->setHighlightColor(
                        Utils::vectorFromColor(series->singleHighlightColor()));
            // Make sure selection pointer object reference is still good
            m_selectionPointer->setPointerObject(m_visibleSeriesList.at(0).object());
            m_selectionPointer->setRotation(m_visibleSeriesList.at(0).meshRotation());
        }
    }
}

void Surface3DRenderer::updateRows(const QVector<int> &rows)
{
    // Surface only supports single series for now, so we are only interested in the first series
    const QSurfaceDataArray *array = 0;
    if (m_visibleSeriesList.size()) {
        QSurface3DSeries *firstSeries = static_cast<QSurface3DSeries *>(m_visibleSeriesList.at(0).series());
        QSurfaceDataProxy *dataProxy = firstSeries->dataProxy();
        if (dataProxy)
            array = dataProxy->array();
    }

    if (array && array->size() >= 2 && array->at(0)->size() >= 2
            && m_sampleSpace.width() >= 2 && m_sampleSpace.height() >= 2) {
        bool updateBuffers = false;
        int sampleSpaceTop = m_sampleSpace.y() + m_sampleSpace.height();
        foreach (int row, rows) {
            if (row >= m_sampleSpace.y() && row <= sampleSpaceTop) {
                updateBuffers = true;
                for (int j = 0; j < m_sampleSpace.width(); j++)
                    (*(m_dataArray.at(row - m_sampleSpace.y())))[j] =
                        array->at(row)->at(j + m_sampleSpace.x());

                if (m_cachedFlatShading) {
                    m_surfaceObj->updateCoarseRow(m_dataArray, row - m_sampleSpace.y(),
                                                  m_heightNormalizer,
                                                  m_axisCacheY.min());
                } else {
                    m_surfaceObj->updateSmoothRow(m_dataArray, row - m_sampleSpace.y(),
                                                  m_heightNormalizer,
                                                  m_axisCacheY.min());
                }
            }
        }
        if (updateBuffers)
            m_surfaceObj->uploadBuffers();
    }

    updateSelectedPoint(m_selectedPoint, m_selectedSeries);
}

void Surface3DRenderer::updateItem(const QVector<QPoint> &points)
{
    // Surface only supports single series for now, so we are only interested in the first series
    const QSurfaceDataArray *array = 0;
    if (m_visibleSeriesList.size()) {
        QSurface3DSeries *firstSeries = static_cast<QSurface3DSeries *>(m_visibleSeriesList.at(0).series());
        QSurfaceDataProxy *dataProxy = firstSeries->dataProxy();
        if (dataProxy)
            array = dataProxy->array();
    }

    if (array && array->size() >= 2 && array->at(0)->size() >= 2
            && m_sampleSpace.width() >= 2 && m_sampleSpace.height() >= 2) {
        int sampleSpaceTop = m_sampleSpace.y() + m_sampleSpace.height();
        int sampleSpaceRight = m_sampleSpace.x() + m_sampleSpace.width();
        bool updateBuffers = false;
        foreach (QPoint item, points) {
            if (item.y() <= sampleSpaceTop && item.y() >= m_sampleSpace.y() &&
                    item.x() <= sampleSpaceRight && item.x() >= m_sampleSpace.x()) {
                updateBuffers = true;
                int x = item.x() - m_sampleSpace.x();
                int y = item.y() - m_sampleSpace.y();
                (*(m_dataArray.at(y)))[x] = array->at(item.y())->at(item.x());

                if (m_cachedFlatShading) {
                    m_surfaceObj->updateCoarseItem(m_dataArray, y, x, m_heightNormalizer,
                                                   m_axisCacheY.min());
                } else {
                    m_surfaceObj->updateSmoothItem(m_dataArray, y, x, m_heightNormalizer,
                                                   m_axisCacheY.min());
                }
            }
        }
        if (updateBuffers)
            m_surfaceObj->uploadBuffers();
    }

    updateSelectedPoint(m_selectedPoint, m_selectedSeries);
}

void Surface3DRenderer::updateSliceDataModel(const QPoint &point)
{
    int column = point.y();
    int row = point.x();

    for (int i = 0; i < m_sliceDataArray.size(); i++)
        delete m_sliceDataArray.at(i);
    m_sliceDataArray.clear();

    m_sliceDataArray.reserve(2);
    QSurfaceDataRow *sliceRow;

    float adjust = (0.025f * m_heightNormalizer) / 2.0f;
    float stepDown = 2.0f * adjust;
    if (m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionRow)) {
        QSurfaceDataRow *src = m_dataArray.at(row);
        sliceRow = new QSurfaceDataRow(src->size());
        for (int i = 0; i < sliceRow->size(); i++)
            (*sliceRow)[i].setPosition(QVector3D(src->at(i).x(), src->at(i).y() + adjust, -1.0f));
    } else {
        sliceRow = new QSurfaceDataRow(m_sampleSpace.height());
        for (int i = 0; i < m_sampleSpace.height(); i++) {
            (*sliceRow)[i].setPosition(QVector3D(m_dataArray.at(i)->at(column).z(),
                                                 m_dataArray.at(i)->at(column).y() + adjust,
                                                 -1.0f));
        }
    }

    m_sliceDataArray << sliceRow;

    // Make a duplicate, so that we get a little bit depth
    QSurfaceDataRow *duplicateRow = new QSurfaceDataRow(*sliceRow);
    for (int i = 0; i < sliceRow->size(); i++) {
        (*sliceRow)[i].setPosition(QVector3D(sliceRow->at(i).x(), sliceRow->at(i).y() - stepDown,
                                             1.0f));
    }

    m_sliceDataArray << duplicateRow;

    QRect sliceRect(0, 0, sliceRow->size(), 2);

    if (sliceRow->size() > 0) {
        if (!m_sliceSurfaceObj)
            loadSliceSurfaceObj();

        if (!m_cachedFlatShading) {
            m_sliceSurfaceObj->setUpSmoothData(m_sliceDataArray, sliceRect, m_heightNormalizer,
                                               m_axisCacheY.min(), true);
        } else {
            m_sliceSurfaceObj->setUpData(m_sliceDataArray, sliceRect, m_heightNormalizer,
                                         m_axisCacheY.min(), true);
        }
    }
}

QRect Surface3DRenderer::calculateSampleRect(const QSurfaceDataArray &array)
{
    QRect sampleSpace;

    int rowCount = array.size();
    int columnCount = array.at(0)->size();

    int i;
    bool found;
    float axisMinX = m_axisCacheX.min();
    float axisMaxX = m_axisCacheX.max();
    float axisMinZ = m_axisCacheZ.min();
    float axisMaxZ = m_axisCacheZ.max();

    // m_minVisibleColumnValue
    for (i = 0, found = false; i < columnCount; i++) {
        if (array.at(0)->at(i).x() >= axisMinX) {
            found = true;
            break;
        }
    }
    if (found) {
        m_minVisibleColumnValue = array.at(0)->at(i).x();
        sampleSpace.setLeft(i);
    } else {
        sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
        return sampleSpace;
    }

    // m_maxVisibleColumnValue
    for (i = columnCount - 1, found = false; i >= 0; i--) {
        if (array.at(0)->at(i).x() <= axisMaxX) {
            found = true;
            break;
        }
    }
    if (found) {
        m_maxVisibleColumnValue = array.at(0)->at(i).x();
        sampleSpace.setRight(i);
    } else {
        sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
        return sampleSpace;
    }

    // m_minVisibleRowValue
    for (i = 0, found = false; i < rowCount; i++) {
        if (array.at(i)->at(0).z() >= axisMinZ) {
            found = true;
            break;
        }
    }
    if (found) {
        m_minVisibleRowValue = array.at(i)->at(0).z();
        sampleSpace.setTop(i);
    } else {
        sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
        return sampleSpace;
    }

    // m_maxVisibleRowValue
    for (i = rowCount - 1, found = false; i >= 0; i--) {
        if (array.at(i)->at(0).z() <= axisMaxZ) {
            found = true;
            break;
        }
    }
    if (found) {
        m_maxVisibleRowValue = array.at(i)->at(0).z();
        sampleSpace.setBottom(i);
    } else {
        sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
        return sampleSpace;
    }

    m_visibleColumnRange = m_maxVisibleColumnValue - m_minVisibleColumnValue;
    m_visibleRowRange = m_maxVisibleRowValue - m_minVisibleRowValue;
    m_surfaceScaleX = m_scaleX * m_visibleColumnRange / m_areaSize.width();
    m_surfaceScaleZ = m_scaleZ * m_visibleRowRange / m_areaSize.height();
    GLfloat axis2XCenterX = axisMinX + axisMaxX;
    GLfloat axis2XCenterZ = axisMinZ + axisMaxZ;
    GLfloat data2XCenterX = GLfloat(m_minVisibleColumnValue + m_maxVisibleColumnValue);
    GLfloat data2XCenterZ = GLfloat(m_minVisibleRowValue + m_maxVisibleRowValue);
    m_surfaceOffsetX = m_scaleX * (data2XCenterX - axis2XCenterX) / m_areaSize.width();
    m_surfaceOffsetZ = -m_scaleZ * (data2XCenterZ - axis2XCenterZ) / m_areaSize.height();

    return sampleSpace;
}

void Surface3DRenderer::updateScene(Q3DScene *scene)
{
    // Set initial camera position
    // X must be 0 for rotation to work - we can use "setCameraRotation" for setting it later
    if (m_hasHeightAdjustmentChanged) {
        scene->activeCamera()->d_ptr->setBaseOrientation(cameraDistanceVector, zeroVector, upVector);
        // For now this is used just to make things once. Proper use will come
        m_hasHeightAdjustmentChanged = false;
    }

    Abstract3DRenderer::updateScene(scene);

    if (m_selectionPointer && m_selectionActive
            && m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionItem)) {
        m_selectionDirty = true; // Ball may need repositioning if scene changes
    }

    updateSlicingActive(scene->isSlicingActive());
}

void Surface3DRenderer::render(GLuint defaultFboHandle)
{
    // Handle GL state setup for FBO buffers and clearing of the render surface
    Abstract3DRenderer::render(defaultFboHandle);

    drawScene(defaultFboHandle);
    if (m_cachedIsSlicingActivated)
        drawSlicedScene();

    // Render selection ball
    if (m_selectionPointer && m_selectionActive
            && m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionItem)) {
        m_selectionPointer->render(defaultFboHandle);
    }
}

void Surface3DRenderer::drawSlicedScene()
{
    QVector3D lightPos;

    QVector3D lightColor = Utils::vectorFromColor(m_cachedTheme->lightColor());

    // Specify viewport
    glViewport(m_secondarySubViewport.x(),
               m_secondarySubViewport.y(),
               m_secondarySubViewport.width(),
               m_secondarySubViewport.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;

    GLfloat aspect = (GLfloat)m_secondarySubViewport.width()
            / (GLfloat)m_secondarySubViewport.height();
    GLfloat sliceUnitsScaled = sliceUnits / m_autoScaleAdjustment;
    projectionMatrix.ortho(-sliceUnitsScaled * aspect, sliceUnitsScaled * aspect,
                           -sliceUnitsScaled, sliceUnitsScaled,
                           -1.0f, 4.0f);

    // Set view matrix
    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, 1.0f), zeroVector, upVector);

    // Set light position
    lightPos = QVector3D(0.0f, 0.0f, 2.0f);

    QMatrix4x4 projectionViewMatrix = projectionMatrix * viewMatrix;

    bool rowMode = m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionRow);

    GLfloat scaleX = 0.0f;
    GLfloat scaleXBackground = 0.0f;
    GLfloat offset = 0.0f;
    if (rowMode) {
        scaleX = m_surfaceScaleX;
        scaleXBackground = m_scaleXWithBackground;
        offset = m_surfaceOffsetX;
    } else {
        scaleX = m_surfaceScaleZ;
        scaleXBackground = m_scaleZWithBackground;
        offset = -m_surfaceOffsetZ;
    }

    if (m_surfaceObj) {
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 modelMatrix;
        QMatrix4x4 itModelMatrix;

        const SeriesRenderCache &series = m_visibleSeriesList.at(0);

        modelMatrix.translate(offset, 0.0f, 0.0f);
        QVector3D scaling(scaleX, 1.0f, sliceZScale);
        modelMatrix.scale(scaling);
        itModelMatrix.scale(scaling);

        MVPMatrix = projectionViewMatrix * modelMatrix;

        if (m_cachedSurfaceVisible) {
            if (m_cachedSurfaceGridOn) {
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(0.5f, 1.0f);
            }

            ShaderHelper *surfaceShader = m_surfaceShader;
            surfaceShader->bind();

            GLuint baseGradientTexture = m_uniformGradientTexture;
            if (series.colorStyle() != Q3DTheme::ColorStyleUniform)
                baseGradientTexture = series.baseGradientTexture();

            // Set shader bindings
            surfaceShader->setUniformValue(surfaceShader->lightP(), lightPos);
            surfaceShader->setUniformValue(surfaceShader->view(), viewMatrix);
            surfaceShader->setUniformValue(surfaceShader->model(), modelMatrix);
            surfaceShader->setUniformValue(surfaceShader->nModel(),
                                           itModelMatrix.inverted().transposed());
            surfaceShader->setUniformValue(surfaceShader->MVP(), MVPMatrix);
            surfaceShader->setUniformValue(surfaceShader->lightS(), 0.15f);
            surfaceShader->setUniformValue(surfaceShader->ambientS(),
                                           m_cachedTheme->ambientLightStrength() * 2.3f);
            surfaceShader->setUniformValue(surfaceShader->lightColor(), lightColor);

            m_drawer->drawObject(surfaceShader, m_sliceSurfaceObj, baseGradientTexture);
        }

        // Draw surface grid
        if (m_cachedSurfaceGridOn) {
            m_surfaceGridShader->bind();
            m_surfaceGridShader->setUniformValue(m_surfaceGridShader->color(),
                                                 Utils::vectorFromColor(m_cachedTheme->gridLineColor()));
            m_surfaceGridShader->setUniformValue(m_surfaceGridShader->MVP(), MVPMatrix);
            m_drawer->drawSurfaceGrid(m_surfaceGridShader, m_sliceSurfaceObj);

            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }

    // Disable textures
    glDisable(GL_TEXTURE_2D);

    // Grid lines
    if (m_cachedTheme->isGridEnabled() && m_heightNormalizer) {
        ShaderHelper *lineShader = m_backgroundShader;
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
            QVector3D gridLineScaleX(scaleXBackground, gridLineWidth, gridLineWidth);

            GLfloat lineStep = 2.0f * m_axisCacheY.subSegmentStep() / m_heightNormalizer;
            GLfloat linePos = -1.0f;
            int lastSegment = m_axisCacheY.subSegmentCount() * m_axisCacheY.segmentCount();

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(0.0f, linePos, -sliceZScale);

                modelMatrix.scale(gridLineScaleX);
                itModelMatrix.scale(gridLineScaleX);

                MVPMatrix = projectionViewMatrix * modelMatrix;

                // Set the rest of the shader bindings
                lineShader->setUniformValue(lineShader->model(), modelMatrix);
                lineShader->setUniformValue(lineShader->nModel(),
                                            itModelMatrix.inverted().transposed());
                lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

                // Draw the object
                m_drawer->drawObject(lineShader, m_gridLineObj);

                linePos += lineStep;
            }
        }

        // Vertical lines
        QVector3D gridLineScaleY(gridLineWidth, backgroundMargin, gridLineWidth);

        int lastSegment;
        GLfloat lineStep;
        GLfloat linePos;
        if (rowMode) {
            lineStep = -2.0f * aspectRatio * m_axisCacheX.subSegmentStep() / m_scaleFactor;
            lastSegment = m_axisCacheX.subSegmentCount() * m_axisCacheX.segmentCount();
            linePos = m_scaleX;
        } else {
            lineStep = -2.0f * aspectRatio * m_axisCacheZ.subSegmentStep() / m_scaleFactor;
            lastSegment = m_axisCacheZ.subSegmentCount() * m_axisCacheZ.segmentCount();
            linePos = m_scaleZ;
        }

        for (int segment = 0; segment <= lastSegment; segment++) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 itModelMatrix;

            modelMatrix.translate(linePos, 0.0f, -sliceZScale);
            modelMatrix.scale(gridLineScaleY);
            itModelMatrix.scale(gridLineScaleY);

            MVPMatrix = projectionViewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            lineShader->setUniformValue(lineShader->model(), modelMatrix);
            lineShader->setUniformValue(lineShader->nModel(),
                                        itModelMatrix.inverted().transposed());
            lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

            // Draw the object
            m_drawer->drawObject(lineShader, m_gridLineObj);

            linePos += lineStep;
        }
    }

    // Draw axis labels
    m_labelShader->bind();
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Y Labels to back wall
    GLfloat posStep = 2.0f * m_axisCacheY.segmentStep() / m_heightNormalizer;
    GLfloat labelPos = -1.0f;
    int labelNbr = 0;

    QVector3D positionComp(0.0f, 0.0f, 0.0f);
    QVector3D rotation(0.0f, 0.0f, 0.0f);
    QVector3D labelTrans = QVector3D(scaleXBackground + labelMargin, labelPos, 0.0f);
    for (int segment = 0; segment <= m_axisCacheY.segmentCount(); segment++) {
        if (m_axisCacheY.labelItems().size() > labelNbr) {
            labelTrans.setY(labelPos);
            const LabelItem &axisLabelItem = *m_axisCacheY.labelItems().at(labelNbr);

            // Draw the label here
            m_dummyRenderItem.setTranslation(labelTrans);
            m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                positionComp, rotation, 0, m_cachedSelectionMode, m_labelShader,
                                m_labelObj, m_cachedScene->activeCamera(),
                                true, true, Drawer::LabelMid, Qt::AlignRight, true);
        }
        labelNbr++;
        labelPos += posStep;
    }

    // X Labels to ground
    int countLabelItems;
    int lastSegment;
    if (rowMode) {
        posStep = 2.0f * aspectRatio * m_axisCacheX.segmentStep() / m_scaleFactor;
        labelPos = -m_scaleX;
        lastSegment = m_axisCacheX.segmentCount();
        countLabelItems = m_axisCacheX.labelItems().size();
    } else {
        posStep = 2.0f * aspectRatio * m_axisCacheZ.segmentStep() / m_scaleFactor;
        labelPos = -m_scaleZ;
        lastSegment = m_axisCacheZ.segmentCount();
        countLabelItems = m_axisCacheZ.labelItems().size();
    }

    labelNbr = 0;
    positionComp.setY(-0.1f);
    rotation.setZ(-45.0f);
    labelTrans.setY(-backgroundMargin);
    for (int segment = 0; segment <= lastSegment; segment++) {
        if (countLabelItems > labelNbr) {
            // Draw the label here
            labelTrans.setX(labelPos);

            m_dummyRenderItem.setTranslation(labelTrans);

            LabelItem *axisLabelItem;
            if (rowMode)
                axisLabelItem = m_axisCacheX.labelItems().at(labelNbr);
            else
                axisLabelItem = m_axisCacheZ.labelItems().at(labelNbr);

            m_drawer->drawLabel(m_dummyRenderItem, *axisLabelItem, viewMatrix, projectionMatrix,
                                positionComp, rotation, 0, QAbstract3DGraph::SelectionRow,
                                m_labelShader, m_labelObj, m_cachedScene->activeCamera(),
                                false, false, Drawer::LabelBelow, Qt::AlignBottom, true);
        }
        labelNbr++;
        labelPos += posStep;
    }

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // Release shader
    glUseProgram(0);
}

void Surface3DRenderer::drawScene(GLuint defaultFboHandle)
{
    GLfloat backgroundRotation = 0;
    QVector3D lightColor = Utils::vectorFromColor(m_cachedTheme->lightColor());

    glViewport(m_primarySubViewport.x(),
               m_primarySubViewport.y(),
               m_primarySubViewport.width(),
               m_primarySubViewport.height());

    // Specify viewport

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, (GLfloat)m_primarySubViewport.width()
                                 / (GLfloat)m_primarySubViewport.height(), 0.1f, 100.0f);

    // Calculate view matrix
    QMatrix4x4 viewMatrix = m_cachedScene->activeCamera()->d_ptr->viewMatrix();

    QMatrix4x4 projectionViewMatrix = projectionMatrix * viewMatrix;

    // Calculate flipping indicators
    if (viewMatrix.row(0).x() > 0)
        m_zFlipped = false;
    else
        m_zFlipped = true;
    if (viewMatrix.row(0).z() <= 0)
        m_xFlipped = false;
    else
        m_xFlipped = true;

    // calculate background rotation based on view matrix rotation
    if (viewMatrix.row(0).x() > 0 && viewMatrix.row(0).z() <= 0)
        backgroundRotation = 270.0f;
    else if (viewMatrix.row(0).x() > 0 && viewMatrix.row(0).z() > 0)
        backgroundRotation = 180.0f;
    else if (viewMatrix.row(0).x() <= 0 && viewMatrix.row(0).z() > 0)
        backgroundRotation = 90.0f;
    else if (viewMatrix.row(0).x() <= 0 && viewMatrix.row(0).z() <= 0)
        backgroundRotation = 0.0f;

    QVector3D lightPos = m_cachedScene->activeLight()->position();

    QMatrix4x4 depthViewMatrix;
    QMatrix4x4 depthProjectionMatrix;
    QMatrix4x4 depthProjectionViewMatrix;

    QVector3D surfaceScaler(m_surfaceScaleX, 1.0f, m_surfaceScaleZ);
    QVector3D surfaceOffset(m_surfaceOffsetX, 0.0f, m_surfaceOffsetZ);

    // Draw depth buffer
#if !defined(QT_OPENGL_ES_2)
    GLfloat adjustedLightStrength =  m_cachedTheme->lightStrength() / 10.0f;
    if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone && m_surfaceObj
            && m_cachedSurfaceVisible) {
        // Render scene into a depth texture for using with shadow mapping
        // Enable drawing to depth framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthFrameBuffer);

        // Attach texture to depth attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                               m_depthTexture, 0);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Bind depth shader
        m_depthShader->bind();

        // Set viewport for depth map rendering. Must match texture size. Larger values give smoother shadows.
        glViewport(0, 0,
                   m_primarySubViewport.width() * m_shadowQualityMultiplier,
                   m_primarySubViewport.height() * m_shadowQualityMultiplier);

        // Get the depth view matrix
        // It may be possible to hack lightPos here if we want to make some tweaks to shadow
        QVector3D depthLightPos = m_cachedScene->activeCamera()->d_ptr->calculatePositionRelativeToCamera(
                    zeroVector, 0.0f, 3.5f / m_autoScaleAdjustment);
        depthViewMatrix.lookAt(depthLightPos, zeroVector, upVector);

        // Set the depth projection matrix
#ifndef USE_WIDER_SHADOWS
        // Use this for perspective shadows
        depthProjectionMatrix.perspective(10.0f, (GLfloat)m_primarySubViewport.width()
                                          / (GLfloat)m_primarySubViewport.height(), 3.0f, 100.0f);
#else
        // Use these for orthographic shadows
        depthProjectionMatrix.ortho(-2.0f * 2.0f, 2.0f * 2.0f,
                                    -2.0f, 2.0f,
                                    0.0f, 100.0f);
#endif
        depthProjectionViewMatrix = depthProjectionMatrix * depthViewMatrix;

        glDisable(GL_CULL_FACE);

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;

        modelMatrix.translate(surfaceOffset);
        modelMatrix.scale(surfaceScaler);

        MVPMatrix = depthProjectionViewMatrix * modelMatrix;

        m_depthShader->setUniformValue(m_depthShader->MVP(), MVPMatrix);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(m_depthShader->posAtt());
        glBindBuffer(GL_ARRAY_BUFFER, m_surfaceObj->vertexBuf());
        glVertexAttribPointer(m_depthShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0,
                              (void *)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_surfaceObj->elementBuf());

        // Draw the triangles
        glDrawElements(GL_TRIANGLES, m_surfaceObj->indexCount(), m_surfaceObj->indicesType(),
                       (void *)0);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                               m_depthModelTexture, 0);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw the triangles
        glDrawElements(GL_TRIANGLES, m_surfaceObj->indexCount(), m_surfaceObj->indicesType(),
                       (void *)0);

        // Free buffers
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(m_depthShader->posAtt());

        // Disable drawing to depth framebuffer (= enable drawing to screen)
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);

        // Revert to original viewport
        glViewport(m_primarySubViewport.x(),
                   m_primarySubViewport.y(),
                   m_primarySubViewport.width(),
                   m_primarySubViewport.height());

        // Reset culling to normal
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
#endif
    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    // Draw selection buffer
    if (!m_cachedIsSlicingActivated && m_surfaceObj && m_selectionState == SelectOnScene
            && m_cachedSelectionMode > QAbstract3DGraph::SelectionNone
            && (m_cachedSurfaceVisible || m_cachedSurfaceGridOn)
            && m_visibleSeriesList.size() > 0) {
        m_selectionShader->bind();
        glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
        glViewport(0,
                   0,
                   m_primarySubViewport.width(),
                   m_primarySubViewport.height());

        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled

        glDisable(GL_CULL_FACE);

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;

        modelMatrix.translate(surfaceOffset);
        modelMatrix.scale(surfaceScaler);

        MVPMatrix = projectionViewMatrix * modelMatrix;

        m_selectionShader->setUniformValue(m_selectionShader->MVP(), MVPMatrix);

        m_drawer->drawObject(m_selectionShader, m_surfaceObj, m_selectionTexture);

        glEnable(GL_DITHER);

        GLubyte pixel[4] = {0, 0, 0, 0};
        glReadPixels(m_inputPosition.x(), m_viewport.height() - m_inputPosition.y(),
                     1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pixel);

        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);

        // Put the RGBA value back to uint
#if !defined(QT_OPENGL_ES_2)
        uint selectionId = pixel[0] + pixel[1] * 256 + pixel[2] * 65536 + pixel[3] * 16777216;
#else
        uint selectionId = pixel[0] + pixel[1] * 256 + pixel[2] * 65536;
#endif

        m_clickedPosition = selectionIdToSurfacePoint(selectionId);
        m_clickedSeries = m_visibleSeriesList.at(0).series();

        emit needRender();

        // Revert to original viewport
        glViewport(m_primarySubViewport.x(),
                   m_primarySubViewport.y(),
                   m_primarySubViewport.width(),
                   m_primarySubViewport.height());
    }

    // Draw the surface
    if (m_surfaceObj && m_sampleSpace.width() >= 2 && m_sampleSpace.height() >= 2) {
        m_surfaceShader->bind();
        // For surface we can see climpses from underneath
        glDisable(GL_CULL_FACE);
        if (m_cachedSurfaceGridOn) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0.5f, 1.0f);
        }

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(surfaceOffset);
        modelMatrix.scale(surfaceScaler);
        itModelMatrix.scale(surfaceScaler);

#ifdef SHOW_DEPTH_TEXTURE_SCENE
        MVPMatrix = depthProjectionViewMatrix * modelMatrix;
#else
        MVPMatrix = projectionViewMatrix * modelMatrix;
#endif

        if (m_cachedSurfaceVisible) {
            // Set shader bindings
            m_surfaceShader->setUniformValue(m_surfaceShader->lightP(), lightPos);
            m_surfaceShader->setUniformValue(m_surfaceShader->view(), viewMatrix);
            m_surfaceShader->setUniformValue(m_surfaceShader->model(), modelMatrix);
            m_surfaceShader->setUniformValue(m_surfaceShader->nModel(),
                                             itModelMatrix.inverted().transposed());
            m_surfaceShader->setUniformValue(m_surfaceShader->MVP(), MVPMatrix);
            m_surfaceShader->setUniformValue(m_surfaceShader->ambientS(),
                                             m_cachedTheme->ambientLightStrength());
            m_surfaceShader->setUniformValue(m_surfaceShader->lightColor(), lightColor);

            GLuint gradientTexture;
            if (m_visibleSeriesList.at(0).colorStyle() == Q3DTheme::ColorStyleUniform)
                gradientTexture = m_uniformGradientTexture;
            else
                gradientTexture = m_visibleSeriesList.at(0).baseGradientTexture();

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                // Set shadow shader bindings
                QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
                m_surfaceShader->setUniformValue(m_surfaceShader->shadowQ(),
                                                 m_shadowQualityToShader);
                m_surfaceShader->setUniformValue(m_surfaceShader->depth(), depthMVPMatrix);
                m_surfaceShader->setUniformValue(m_surfaceShader->lightS(), adjustedLightStrength);

                // Draw the object
                m_drawer->drawObject(m_surfaceShader, m_surfaceObj, gradientTexture,
                                     m_depthModelTexture);
            } else
#endif
            {
                // Set shadowless shader bindings
                m_surfaceShader->setUniformValue(m_surfaceShader->lightS(),
                                                 m_cachedTheme->lightStrength());

                // Draw the object
                m_drawer->drawObject(m_surfaceShader, m_surfaceObj, gradientTexture);
            }
            glEnable(GL_CULL_FACE);
        }

        // Draw surface grid
        if (m_cachedSurfaceGridOn) {
            m_surfaceGridShader->bind();
            m_surfaceGridShader->setUniformValue(m_surfaceGridShader->color(),
                                                 Utils::vectorFromColor(m_cachedTheme->gridLineColor()));
            m_surfaceGridShader->setUniformValue(m_surfaceGridShader->MVP(), MVPMatrix);
            m_drawer->drawSurfaceGrid(m_surfaceGridShader, m_surfaceObj);

            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }

    // Bind background shader
    m_backgroundShader->bind();
    glCullFace(GL_BACK);

    // Draw background
    if (m_cachedTheme->isBackgroundEnabled() && m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 itModelMatrix;

        QVector3D bgScale(m_scaleXWithBackground, backgroundMargin, m_scaleZWithBackground);
        modelMatrix.scale(bgScale);

        // If we're viewing from below, background object must be flipped
        if (m_yFlipped) {
            modelMatrix.rotate(180.0f, 1.0f, 0.0f, 0.0f);
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
    }

    // Draw grid lines
    QVector3D gridLineScaleX(m_scaleXWithBackground, gridLineWidth, gridLineWidth);
    QVector3D gridLineScaleZ(gridLineWidth, gridLineWidth, m_scaleZWithBackground);
    QVector3D gridLineScaleY(gridLineWidth, backgroundMargin, gridLineWidth);

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

        GLfloat yFloorLinePosition = -backgroundMargin + gridLineOffset;
        if (m_yFlipped)
            yFloorLinePosition = -yFloorLinePosition;

        // Rows (= Z)
        if (m_axisCacheZ.segmentCount() > 0) {
            // Floor lines
            GLfloat lineStep = 2.0f * aspectRatio * m_axisCacheZ.subSegmentStep() / m_scaleFactor;
            GLfloat linePos = m_scaleZ; // Start line
            int lastSegment = m_axisCacheZ.subSegmentCount() * m_axisCacheZ.segmentCount();

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(0.0f, yFloorLinePosition, linePos);

                modelMatrix.scale(gridLineScaleX);
                itModelMatrix.scale(gridLineScaleX);

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
                linePos -= lineStep;
            }

            // Side wall lines
            GLfloat lineXTrans = m_scaleXWithBackground - gridLineOffset;
            linePos = m_scaleZ; // Start line

            if (!m_xFlipped)
                lineXTrans = -lineXTrans;

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(lineXTrans, 0.0f, linePos);

                modelMatrix.scale(gridLineScaleY);
                itModelMatrix.scale(gridLineScaleY);

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
            GLfloat lineStep = -2.0f * aspectRatio * m_axisCacheX.subSegmentStep() / m_scaleFactor;
            GLfloat linePos = m_scaleX;
            int lastSegment = m_axisCacheX.subSegmentCount() * m_axisCacheX.segmentCount();

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(linePos, yFloorLinePosition, 0.0f);

                modelMatrix.scale(gridLineScaleZ);
                itModelMatrix.scale(gridLineScaleZ);

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
            GLfloat lineZTrans = m_scaleZWithBackground - gridLineOffset;
            linePos = m_scaleX;

            if (!m_zFlipped)
                lineZTrans = -lineZTrans;

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(linePos, 0.0f, lineZTrans);

                modelMatrix.scale(gridLineScaleY);
                itModelMatrix.scale(gridLineScaleY);

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
            GLfloat lineStep = 2.0f * m_axisCacheY.subSegmentStep() / m_heightNormalizer;
            GLfloat linePos = -1.0f;
            int lastSegment = m_axisCacheY.subSegmentCount() * m_axisCacheY.segmentCount();

            GLfloat lineZTrans = m_scaleZWithBackground - gridLineOffset;

            if (!m_zFlipped)
                lineZTrans = -lineZTrans;

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(0.0f, linePos, lineZTrans);

                modelMatrix.scale(gridLineScaleX);
                itModelMatrix.scale(gridLineScaleX);

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
            linePos = -1.0f;
            lastSegment = m_axisCacheY.subSegmentCount() * m_axisCacheY.segmentCount();
            GLfloat lineXTrans = m_scaleXWithBackground - gridLineOffset;

            if (!m_xFlipped)
                lineXTrans = -lineXTrans;

            for (int segment = 0; segment <= lastSegment; segment++) {
                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;
                QMatrix4x4 itModelMatrix;

                modelMatrix.translate(lineXTrans, linePos, 0.0f);

                modelMatrix.scale(gridLineScaleZ);
                itModelMatrix.scale(gridLineScaleZ);

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
    m_labelShader->bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_OFFSET_FILL);

    // Z Labels
    QVector3D positionZComp(0.0f, 0.0f, 0.0f);
    if (m_axisCacheZ.segmentCount() > 0) {
        GLfloat posStep = 2.0f * aspectRatio * m_axisCacheZ.segmentStep() / m_scaleFactor;
        GLfloat labelPos = m_scaleZ;
        int lastSegment = m_axisCacheZ.segmentCount();
        int labelNbr = 0;
        GLfloat labelXTrans = m_scaleXWithBackground + labelMargin;
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
                                         labelPos);
        QVector3D rotation(rotLabelX, rotLabelY, rotLabelZ);

        for (int segment = 0; segment <= lastSegment; segment++) {
            if (m_axisCacheZ.labelItems().size() > labelNbr) {
                glPolygonOffset(GLfloat(segment) / -10.0f, 1.0f);
                // Draw the label here
                labelTrans.setZ(labelPos);
                m_dummyRenderItem.setTranslation(labelTrans);
                const LabelItem &axisLabelItem = *m_axisCacheZ.labelItems().at(labelNbr);

                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    positionZComp, rotation, 0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, m_cachedScene->activeCamera(),
                                    true, true, Drawer::LabelMid, alignment);
            }
            labelNbr++;
            labelPos -= posStep;
        }
    }
    // X Labels
    if (m_axisCacheX.segmentCount() > 0) {
        GLfloat posStep = 2.0f * aspectRatio * m_axisCacheX.segmentStep() / m_scaleFactor;
        GLfloat labelPos = -m_scaleX;
        int lastSegment = m_axisCacheX.segmentCount();

        int labelNbr = 0;
        GLfloat labelZTrans = m_scaleZWithBackground + labelMargin;
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
        QVector3D labelTrans = QVector3D(labelPos,
                                         labelYTrans,
                                         labelZTrans);
        QVector3D rotation(rotLabelX, rotLabelY, rotLabelZ);

        for (int segment = 0; segment <= lastSegment; segment++) {
            if (m_axisCacheX.labelItems().size() > labelNbr) {
                glPolygonOffset(GLfloat(segment) / -10.0f, 1.0f);
                // Draw the label here
                labelTrans.setX(labelPos);
                m_dummyRenderItem.setTranslation(labelTrans);
                const LabelItem &axisLabelItem = *m_axisCacheX.labelItems().at(labelNbr);

                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    positionZComp, rotation, 0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, m_cachedScene->activeCamera(),
                                    true, true, Drawer::LabelMid, alignment);
            }
            labelNbr++;
            labelPos += posStep;
        }
    }
    // Y Labels
    if (m_axisCacheY.segmentCount() > 0) {
        GLfloat posStep = 2.0f * m_axisCacheY.segmentStep() / m_heightNormalizer;
        GLfloat labelPos = -1.0f;
        int labelNbr = 0;
        GLfloat labelXTrans = m_scaleXWithBackground;
        GLfloat labelZTrans = m_scaleZWithBackground;

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

                glPolygonOffset(GLfloat(segment) / -10.0f, 1.0f);

                // Back wall
                labelTransBack.setY(labelPos);
                m_dummyRenderItem.setTranslation(labelTransBack);
                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    positionZComp, labelRotateVectorBack, 0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, m_cachedScene->activeCamera(),
                                    true, true, Drawer::LabelMid, alignmentBack);

                // Side wall
                labelTransSide.setY(labelPos);
                m_dummyRenderItem.setTranslation(labelTransSide);
                m_drawer->drawLabel(m_dummyRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
                                    positionZComp, labelRotateVectorSide, 0, m_cachedSelectionMode,
                                    m_labelShader, m_labelObj, m_cachedScene->activeCamera(),
                                    true, true, Drawer::LabelMid, alignmentSide);
            }
            labelNbr++;
            labelPos += posStep;
        }
    }
    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    // Release shader
    glUseProgram(0);

    // Selection handling
    if (m_selectionDirty || m_selectionLabelDirty) {
        QPoint visiblePoint = Surface3DController::invalidSelectionPosition();
        if (m_selectedPoint != Surface3DController::invalidSelectionPosition()) {
            int x = m_selectedPoint.x() - m_sampleSpace.y();
            int y = m_selectedPoint.y() - m_sampleSpace.x();
            if (x >= 0 && y >= 0 && x < m_sampleSpace.height() && y < m_sampleSpace.width()
                    && m_dataArray.size()) {
                visiblePoint = QPoint(x, y);
            }
        }

        if (m_cachedSelectionMode == QAbstract3DGraph::SelectionNone
                || visiblePoint == Surface3DController::invalidSelectionPosition()) {
            m_selectionActive = false;
        } else {
            if (m_cachedIsSlicingActivated)
                updateSliceDataModel(visiblePoint);
            if (m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionItem))
                surfacePointSelected(visiblePoint);
            m_selectionActive = true;
        }

        m_selectionDirty = false;
    }
}

// This one needs to be called when the data size changes
void Surface3DRenderer::updateSelectionTexture()
{
    // Create the selection ID image. Each grid corner gets 2x2 pixel area of
    // ID color so that each vertex (data point) has 4x4 pixel area of ID color
    int idImageWidth = (m_sampleSpace.width() - 1) * 4;
    int idImageHeight = (m_sampleSpace.height() - 1) * 4;
    int stride = idImageWidth * 4 * sizeof(uchar); // 4 = number of color components (rgba)

    uchar *bits = new uchar[idImageWidth * idImageHeight * 4 * sizeof(uchar)];
    uint id = 1;
    for (int i = 0; i < idImageHeight; i += 4) {
        for (int j = 0; j < idImageWidth; j += 4) {
            int p = (i * idImageWidth + j) * 4;
            uchar r, g, b, a;
            idToRGBA(id, &r, &g, &b, &a);
            fillIdCorner(&bits[p], r, g, b, a, stride);

            idToRGBA(id + 1, &r, &g, &b, &a);
            fillIdCorner(&bits[p + 8], r, g, b, a, stride);

            idToRGBA(id + m_sampleSpace.width(), &r, &g, &b, &a);
            fillIdCorner(&bits[p + 2 * stride], r, g, b, a, stride);

            idToRGBA(id + m_sampleSpace.width() + 1, &r, &g, &b, &a);
            fillIdCorner(&bits[p + 2 * stride + 8], r, g, b, a, stride);

            id++;
        }
        id++;
    }

    // If old texture exists, delete it
    if (m_selectionTexture) {
        m_textureHelper->deleteTexture(&m_selectionTexture);
        m_selectionTexture = 0;
    }

    // Move the ID image (bits) to the texture
    QImage image = QImage(bits, idImageWidth, idImageHeight, QImage::Format_RGB32);
    m_selectionTexture = m_textureHelper->create2DTexture(image, false, false, false);

    // Release the temp bits allocation
    delete[] bits;
}

void Surface3DRenderer::initSelectionBuffer()
{
    // Create the result selection texture and buffers
    if (m_selectionResultTexture) {
        m_textureHelper->deleteTexture(&m_selectionResultTexture);
        m_selectionResultTexture = 0;
    }

    m_selectionResultTexture = m_textureHelper->createSelectionTexture(m_primarySubViewport.size(),
                                                                       m_selectionFrameBuffer,
                                                                       m_selectionDepthBuffer);
}

void Surface3DRenderer::fillIdCorner(uchar *p, uchar r, uchar g, uchar b, uchar a, int stride)
{
    p[0] = r;
    p[1] = g;
    p[2] = b;
    p[3] = a;
    p[4] = r;
    p[5] = g;
    p[6] = b;
    p[7] = a;
    p[stride + 0] = r;
    p[stride + 1] = g;
    p[stride + 2] = b;
    p[stride + 3] = a;
    p[stride + 4] = r;
    p[stride + 5] = g;
    p[stride + 6] = b;
    p[stride + 7] = a;
}

void Surface3DRenderer::idToRGBA(uint id, uchar *r, uchar *g, uchar *b, uchar *a)
{
    *r = id & ID_TO_RGBA_MASK;
    *g = (id >> 8) & ID_TO_RGBA_MASK;
    *b = (id >> 16) & ID_TO_RGBA_MASK;
    *a = (id >> 24) & ID_TO_RGBA_MASK;
}

void Surface3DRenderer::calculateSceneScalingFactors()
{
    // Calculate scene scaling and translation factors
    m_heightNormalizer = GLfloat(m_axisCacheY.max() - m_axisCacheY.min());
    m_areaSize.setHeight(m_axisCacheZ.max() -  m_axisCacheZ.min());
    m_areaSize.setWidth(m_axisCacheX.max() - m_axisCacheX.min());
    m_scaleFactor = qMax(m_areaSize.width(), m_areaSize.height());
    m_scaleX = aspectRatio * m_areaSize.width() / m_scaleFactor;
    m_scaleZ = aspectRatio * m_areaSize.height() / m_scaleFactor;
    m_scaleXWithBackground = m_scaleX * backgroundMargin;
    m_scaleZWithBackground = m_scaleZ * backgroundMargin;
}

bool Surface3DRenderer::updateFlatStatus(bool enable)
{
    if (enable && !m_flatSupported) {
        qWarning() << "Warning: Flat qualifier not supported on your platform's GLSL language."
                      " Requires at least GLSL version 1.2 with GL_EXT_gpu_shader4 extension.";
        enable = false;
    }

    bool changed = false;
    if (enable != m_cachedFlatShading) {
        m_cachedFlatShading = enable;
        changed = true;
        initSurfaceShaders();
    }

    // If no surface object created yet, don't try to update the object
    if (m_surfaceObj && changed && m_sampleSpace.width() >= 2 && m_sampleSpace.height() >= 2) {
        if (!m_cachedFlatShading) {
            m_surfaceObj->setUpSmoothData(m_dataArray, m_sampleSpace, m_heightNormalizer,
                                          m_axisCacheY.min(), true);
        } else {
            m_surfaceObj->setUpData(m_dataArray, m_sampleSpace, m_heightNormalizer,
                                    m_axisCacheY.min(), true);
        }
    }

    return m_cachedFlatShading;
}

void Surface3DRenderer::updateSelectedPoint(const QPoint &position, const QSurface3DSeries *series)
{
    m_selectedPoint = position;
    m_selectedSeries = series;
    m_selectionDirty = true;
}

void Surface3DRenderer::resetClickedStatus()
{
    m_clickedPosition = Surface3DController::invalidSelectionPosition();
    m_clickedSeries = 0;
}

void Surface3DRenderer::updateSurfaceGridStatus(bool enable)
{
    m_cachedSurfaceGridOn = enable;
}

void Surface3DRenderer::loadBackgroundMesh()
{
    if (m_backgroundObj)
        delete m_backgroundObj;
    m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/background"));
    m_backgroundObj->load();
}

void Surface3DRenderer::loadSurfaceObj()
{
    if (m_surfaceObj)
        delete m_surfaceObj;
    m_surfaceObj = new SurfaceObject();
}

void Surface3DRenderer::loadSliceSurfaceObj()
{
    if (m_sliceSurfaceObj)
        delete m_sliceSurfaceObj;
    m_sliceSurfaceObj = new SurfaceObject();
}

void Surface3DRenderer::loadGridLineMesh()
{
    if (m_gridLineObj)
        delete m_gridLineObj;
    m_gridLineObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/plane"));
    m_gridLineObj->load();
}

void Surface3DRenderer::surfacePointSelected(const QPoint &point)
{
    int row = point.x();
    int column = point.y();

    float value = m_dataArray.at(row)->at(column).y();

    if (!m_selectionPointer)
        m_selectionPointer = new SelectionPointer(m_drawer);

    QVector3D pos;
    if (m_cachedIsSlicingActivated) {
        if (m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionRow)) {
            pos = m_sliceSurfaceObj->vertexAt(column, 0);
            pos *= QVector3D(m_surfaceScaleX, 1.0f, 0.0f);
            pos += QVector3D(m_surfaceOffsetX, 0.0f, 0.0f);
            m_selectionPointer->updateBoundingRect(m_secondarySubViewport);
            m_selectionPointer->updateSliceData(true, m_autoScaleAdjustment);
        } else if (m_cachedSelectionMode.testFlag(QAbstract3DGraph::SelectionColumn)) {
            pos = m_sliceSurfaceObj->vertexAt(row, 0);
            pos *= QVector3D(m_surfaceScaleZ, 1.0f, 0.0f);
            pos += QVector3D(-m_surfaceOffsetZ, 0.0f, 0.0f);
            m_selectionPointer->updateBoundingRect(m_secondarySubViewport);
            m_selectionPointer->updateSliceData(true, m_autoScaleAdjustment);
        }
    } else {
        pos = m_surfaceObj->vertexAt(column, row);
        pos *= QVector3D(m_surfaceScaleX, 1.0f, m_surfaceScaleZ);;
        pos += QVector3D(m_surfaceOffsetX, 0.0f, m_surfaceOffsetZ);
        m_selectionPointer->updateBoundingRect(m_primarySubViewport);
        m_selectionPointer->updateSliceData(false, m_autoScaleAdjustment);
    }

    m_selectionPointer->setPosition(pos);
    m_selectionPointer->setLabel(createSelectionLabel(value, column, row));
    m_selectionPointer->setPointerObject(m_visibleSeriesList.at(0).object());
    m_selectionPointer->setHighlightColor(m_visibleSeriesList.at(0).singleHighlightColor());
    m_selectionPointer->updateScene(m_cachedScene);
    m_selectionPointer->setRotation(m_visibleSeriesList.at(0).meshRotation());
}

// Maps selection Id to surface point in data array
QPoint Surface3DRenderer::selectionIdToSurfacePoint(uint id)
{
    int column = ((id - 1) % m_sampleSpace.width()) + m_sampleSpace.x();
    int row = ((id - 1) / m_sampleSpace.width()) +  m_sampleSpace.y();
    return QPoint(row, column);
}

QString Surface3DRenderer::createSelectionLabel(float value, int column, int row)
{
    QString labelText = m_visibleSeriesList[0].itemLabelFormat();
    static const QString xTitleTag(QStringLiteral("@xTitle"));
    static const QString yTitleTag(QStringLiteral("@yTitle"));
    static const QString zTitleTag(QStringLiteral("@zTitle"));
    static const QString xLabelTag(QStringLiteral("@xLabel"));
    static const QString yLabelTag(QStringLiteral("@yLabel"));
    static const QString zLabelTag(QStringLiteral("@zLabel"));
    static const QString seriesNameTag(QStringLiteral("@seriesName"));

    labelText.replace(xTitleTag, m_axisCacheX.title());
    labelText.replace(yTitleTag, m_axisCacheY.title());
    labelText.replace(zTitleTag, m_axisCacheZ.title());

    if (labelText.contains(xLabelTag)) {
        QString labelFormat = m_axisCacheX.labelFormat();
        if (labelFormat.isEmpty())
            labelFormat = Utils::defaultLabelFormat();
        QString valueLabelText = generateValueLabel(labelFormat,
                                                    m_dataArray.at(row)->at(column).x());
        labelText.replace(xLabelTag, valueLabelText);
    }
    if (labelText.contains(yLabelTag)) {
        QString labelFormat = m_axisCacheY.labelFormat();
        if (labelFormat.isEmpty())
            labelFormat = Utils::defaultLabelFormat();
        QString valueLabelText = generateValueLabel(labelFormat, value);
        labelText.replace(yLabelTag, valueLabelText);
    }
    if (labelText.contains(zLabelTag)) {
        QString labelFormat = m_axisCacheZ.labelFormat();
        if (labelFormat.isEmpty())
            labelFormat = Utils::defaultLabelFormat();
        QString valueLabelText = generateValueLabel(labelFormat,
                                                    m_dataArray.at(row)->at(column).z());
        labelText.replace(zLabelTag, valueLabelText);
    }

    labelText.replace(seriesNameTag, m_visibleSeriesList[0].name());

    m_selectionLabelDirty = false;

    return labelText;
}

void Surface3DRenderer::updateShadowQuality(QAbstract3DGraph::ShadowQuality quality)
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
    updateDepthBuffer();
#endif
}

void Surface3DRenderer::updateTextures()
{
    // Do nothing, but required as it is pure virtual on parent
}

void Surface3DRenderer::updateSlicingActive(bool isSlicing)
{
    if (m_cachedIsSlicingActivated == isSlicing)
        return;

    m_cachedIsSlicingActivated = isSlicing;

    if (!m_cachedIsSlicingActivated)
        initSelectionBuffer(); // We need to re-init selection buffer in case there has been a resize

#if !defined(QT_OPENGL_ES_2)
    updateDepthBuffer(); // Re-init depth buffer as well
#endif

    m_selectionDirty = true;
}

void Surface3DRenderer::loadLabelMesh()
{
    if (m_labelObj)
        delete m_labelObj;
    m_labelObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/plane"));
    m_labelObj->load();
}

void Surface3DRenderer::initShaders(const QString &vertexShader, const QString &fragmentShader)
{
    Q_UNUSED(vertexShader);
    Q_UNUSED(fragmentShader);

    // draw the shader for the surface according to smooth status, shadow and uniform color
    if (m_surfaceShader)
        delete m_surfaceShader;
#if !defined(QT_OPENGL_ES_2)
    if (!m_cachedFlatShading) {
        if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
            m_surfaceShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexShadow"),
                                               QStringLiteral(":/shaders/fragmentSurfaceShadowNoTex"));
        } else {
            m_surfaceShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertex"),
                                               QStringLiteral(":/shaders/fragmentSurface"));
        }
    } else {
        if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
            m_surfaceShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexSurfaceShadowFlat"),
                                               QStringLiteral(":/shaders/fragmentSurfaceShadowFlat"));
        } else {
            m_surfaceShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexSurfaceFlat"),
                                               QStringLiteral(":/shaders/fragmentSurfaceFlat"));
        }
    }
#else
    m_surfaceShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertex"),
                                       QStringLiteral(":/shaders/fragmentSurfaceES2"));
#endif
    m_surfaceShader->initialize();
}

void Surface3DRenderer::initBackgroundShaders(const QString &vertexShader,
                                              const QString &fragmentShader)
{
    if (m_backgroundShader)
        delete m_backgroundShader;
    m_backgroundShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_backgroundShader->initialize();
}

void Surface3DRenderer::initSelectionShaders()
{
    if (m_selectionShader)
        delete m_selectionShader;
    m_selectionShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexLabel"),
                                         QStringLiteral(":/shaders/fragmentLabel"));
    m_selectionShader->initialize();
}

void Surface3DRenderer::initSurfaceShaders()
{
    // Gridline shader
    if (m_surfaceGridShader)
        delete m_surfaceGridShader;
    m_surfaceGridShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexPlainColor"),
                                           QStringLiteral(":/shaders/fragmentPlainColor"));
    m_surfaceGridShader->initialize();

    // Triggers surface shader selection by shadow setting
    handleShadowQualityChange();
}

void Surface3DRenderer::initLabelShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_labelShader)
        delete m_labelShader;
    m_labelShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_labelShader->initialize();
}

#if !defined(QT_OPENGL_ES_2)
void Surface3DRenderer::initDepthShader()
{
    if (m_depthShader)
        delete m_depthShader;
    m_depthShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexDepth"),
                                     QStringLiteral(":/shaders/fragmentDepth"));
    m_depthShader->initialize();
}

void Surface3DRenderer::updateDepthBuffer()
{
    if (m_depthTexture) {
        m_textureHelper->deleteTexture(&m_depthTexture);
        m_depthTexture = 0;
    }
    if (m_depthModelTexture) {
        m_textureHelper->deleteTexture(&m_depthModelTexture);
        m_depthModelTexture = 0;
    }

    if (m_primarySubViewport.size().isEmpty())
        return;

    if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
        m_depthTexture = m_textureHelper->createDepthTextureFrameBuffer(m_primarySubViewport.size(),
                                                                        m_depthFrameBuffer,
                                                                        m_shadowQualityMultiplier);
        m_textureHelper->fillDepthTexture(m_depthTexture, m_primarySubViewport.size(),
                                          m_shadowQualityMultiplier, 1.0f);
        m_depthModelTexture = m_textureHelper->createDepthTexture(m_primarySubViewport.size(),
                                                                  m_shadowQualityMultiplier);
        if (!m_depthTexture || !m_depthModelTexture)
            lowerShadowQuality();
    }
}
#endif

void Surface3DRenderer::generateUniformGradient(const QVector3D newColor)
{
    if (m_visibleSeriesList.size()) {
        QColor newQColor = Utils::colorFromVector(newColor);
        m_uniformGradientTextureColor = newColor;
        QLinearGradient newGradient;
        newGradient.setColorAt(0.0, newQColor);
        newGradient.setColorAt(1.0, newQColor);
        fixGradientAndGenerateTexture(&newGradient, &m_uniformGradientTexture);
    }
}

QT_END_NAMESPACE_DATAVISUALIZATION
