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

#include "abstract3drenderer_p.h"
#include "texturehelper_p.h"
#include "q3dcamera_p.h"
#include "q3dtheme_p.h"
#include "qvalue3daxisformatter_p.h"
#include "shaderhelper_p.h"
#include "qcustom3ditem_p.h"
#include "qcustom3dlabel_p.h"

#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

const qreal doublePi(M_PI * 2.0);
const int polarGridRoundness(64);
const qreal polarGridAngle(doublePi / qreal(polarGridRoundness));
const float polarGridAngleDegrees(float(360.0 / qreal(polarGridRoundness)));
const qreal polarGridHalfAngle(polarGridAngle / 2.0);

Abstract3DRenderer::Abstract3DRenderer(Abstract3DController *controller)
    : QObject(0),
      m_hasNegativeValues(false),
      m_cachedTheme(new Q3DTheme()),
      m_drawer(new Drawer(m_cachedTheme)),
      m_cachedShadowQuality(QAbstract3DGraph::ShadowQualityMedium),
      m_autoScaleAdjustment(1.0f),
      m_cachedSelectionMode(QAbstract3DGraph::SelectionNone),
      m_cachedOptimizationHint(QAbstract3DGraph::OptimizationDefault),
      m_textureHelper(0),
      m_depthTexture(0),
      m_cachedScene(new Q3DScene()),
      m_selectionDirty(true),
      m_selectionState(SelectNone),
      m_devicePixelRatio(1.0f),
      m_selectionLabelDirty(true),
      m_clickPending(false),
      m_clickedSeries(0),
      m_clickedType(QAbstract3DGraph::ElementNone),
      m_selectionLabelItem(0),
      m_visibleSeriesCount(0),
      m_customItemShader(0),
      m_useOrthoProjection(false),
      m_xFlipped(false),
      m_yFlipped(false),
      m_zFlipped(false),
      m_yFlippedForGrid(false),
      m_backgroundObj(0),
      m_gridLineObj(0),
      m_labelObj(0),
      m_graphAspectRatio(2.0f),
      m_polarGraph(false),
      m_radialLabelOffset(1.0f),
      m_xRightAngleRotation(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f)),
      m_yRightAngleRotation(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 90.0f)),
      m_zRightAngleRotation(QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, 90.0f)),
      m_xRightAngleRotationNeg(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -90.0f)),
      m_yRightAngleRotationNeg(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, -90.0f)),
      m_zRightAngleRotationNeg(QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, -90.0f)),
      m_xFlipRotation(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -180.0f)),
      m_zFlipRotation(QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, -180.0f))
{
    QObject::connect(m_drawer, &Drawer::drawerChanged, this, &Abstract3DRenderer::updateTextures);
    QObject::connect(this, &Abstract3DRenderer::needRender, controller,
                     &Abstract3DController::needRender, Qt::QueuedConnection);
    QObject::connect(this, &Abstract3DRenderer::requestShadowQuality, controller,
                     &Abstract3DController::handleRequestShadowQuality, Qt::QueuedConnection);
}

Abstract3DRenderer::~Abstract3DRenderer()
{
    delete m_drawer;
    delete m_cachedScene;
    delete m_cachedTheme;
    delete m_selectionLabelItem;
    delete m_customItemShader;

    foreach (SeriesRenderCache *cache, m_renderCacheList) {
        cache->cleanup(m_textureHelper);
        delete cache;
    }
    m_renderCacheList.clear();

    foreach (CustomRenderItem *item, m_customRenderCache) {
        GLuint texture = item->texture();
        m_textureHelper->deleteTexture(&texture);
        delete item;
    }
    m_customRenderCache.clear();

    ObjectHelper::releaseObjectHelper(this, m_backgroundObj);
    ObjectHelper::releaseObjectHelper(this, m_gridLineObj);
    ObjectHelper::releaseObjectHelper(this, m_labelObj);

    if (m_textureHelper) {
        m_textureHelper->deleteTexture(&m_depthTexture);
        delete m_textureHelper;
    }
}

void Abstract3DRenderer::initializeOpenGL()
{
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

    m_textureHelper = new TextureHelper();
    m_drawer->initializeOpenGL();

    axisCacheForOrientation(QAbstract3DAxis::AxisOrientationX).setDrawer(m_drawer);
    axisCacheForOrientation(QAbstract3DAxis::AxisOrientationY).setDrawer(m_drawer);
    axisCacheForOrientation(QAbstract3DAxis::AxisOrientationZ).setDrawer(m_drawer);
}

void Abstract3DRenderer::render(const GLuint defaultFboHandle)
{
    if (defaultFboHandle) {
        glDepthMask(true);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDisable(GL_BLEND); // For QtQuick2 blending is enabled by default, but we don't want it to be
    }

    // Clear the graph background to the theme color
    glViewport(m_viewport.x(),
               m_viewport.y(),
               m_viewport.width(),
               m_viewport.height());
    glScissor(m_viewport.x(),
              m_viewport.y(),
              m_viewport.width(),
              m_viewport.height());
    glEnable(GL_SCISSOR_TEST);
    QVector4D clearColor = Utils::vectorFromColor(m_cachedTheme->windowColor());
    glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

void Abstract3DRenderer::updateSelectionState(SelectionState state)
{
    m_selectionState = state;
}

void Abstract3DRenderer::updateInputPosition(const QPoint &position)
{
    m_inputPosition = position;
}

void Abstract3DRenderer::initGradientShaders(const QString &vertexShader,
                                             const QString &fragmentShader)
{
    // Do nothing by default
    Q_UNUSED(vertexShader)
    Q_UNUSED(fragmentShader)
}

void Abstract3DRenderer::initCustomItemShaders(const QString &vertexShader,
                                               const QString &fragmentShader)
{
    if (m_customItemShader)
        delete m_customItemShader;
    m_customItemShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_customItemShader->initialize();
}

void Abstract3DRenderer::updateTheme(Q3DTheme *theme)
{
    // Synchronize the controller theme with renderer
    bool updateDrawer = theme->d_ptr->sync(*m_cachedTheme->d_ptr);

    if (updateDrawer)
        m_drawer->setTheme(m_cachedTheme);
}

void Abstract3DRenderer::updateScene(Q3DScene *scene)
{
    m_viewport = scene->d_ptr->glViewport();
    m_secondarySubViewport = scene->d_ptr->glSecondarySubViewport();

    if (m_primarySubViewport != scene->d_ptr->glPrimarySubViewport()) {
        // Resize of primary subviewport means resizing shadow and selection buffers
        m_primarySubViewport = scene->d_ptr->glPrimarySubViewport();
        handleResize();
    }

    if (m_devicePixelRatio != scene->devicePixelRatio()) {
        m_devicePixelRatio = scene->devicePixelRatio();
        handleResize();
    }

    scene->activeCamera()->d_ptr->updateViewMatrix(m_autoScaleAdjustment);
    // Set light position (rotate light with activeCamera, a bit above it (as set in defaultLightPos))
    scene->d_ptr->setLightPositionRelativeToCamera(defaultLightPos);

    QPoint logicalPixelPosition = scene->selectionQueryPosition();
    updateInputPosition(QPoint(logicalPixelPosition.x() * m_devicePixelRatio,
                               logicalPixelPosition.y() * m_devicePixelRatio));

    // Synchronize the renderer scene to controller scene
    scene->d_ptr->sync(*m_cachedScene->d_ptr);

    if (Q3DScene::invalidSelectionPoint() == logicalPixelPosition) {
        updateSelectionState(SelectNone);
    } else {
        // Selections are one-shot, reset selection active to false before processing
        scene->setSelectionQueryPosition(Q3DScene::invalidSelectionPoint());
        m_clickPending = true;

        if (scene->isSlicingActive()) {
            if (scene->isPointInPrimarySubView(logicalPixelPosition))
                updateSelectionState(SelectOnOverview);
            else if (scene->isPointInSecondarySubView(logicalPixelPosition))
                updateSelectionState(SelectOnSlice);
            else
                updateSelectionState(SelectNone);
        } else {
            updateSelectionState(SelectOnScene);
        }
    }
}

void Abstract3DRenderer::reInitShaders()
{
#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
        initGradientShaders(QStringLiteral(":/shaders/vertexShadow"),
                            QStringLiteral(":/shaders/fragmentShadowNoTexColorOnY"));
        initShaders(QStringLiteral(":/shaders/vertexShadow"),
                    QStringLiteral(":/shaders/fragmentShadowNoTex"));
        initBackgroundShaders(QStringLiteral(":/shaders/vertexShadow"),
                              QStringLiteral(":/shaders/fragmentShadowNoTex"));
        initCustomItemShaders(QStringLiteral(":/shaders/vertexShadow"),
                              QStringLiteral(":/shaders/fragmentShadow"));
    } else {
        initGradientShaders(QStringLiteral(":/shaders/vertex"),
                            QStringLiteral(":/shaders/fragmentColorOnY"));
        initShaders(QStringLiteral(":/shaders/vertex"),
                    QStringLiteral(":/shaders/fragment"));
        initBackgroundShaders(QStringLiteral(":/shaders/vertex"),
                              QStringLiteral(":/shaders/fragment"));
        initCustomItemShaders(QStringLiteral(":/shaders/vertexTexture"),
                              QStringLiteral(":/shaders/fragmentTexture"));
    }
#else
    initGradientShaders(QStringLiteral(":/shaders/vertex"),
                        QStringLiteral(":/shaders/fragmentColorOnYES2"));
    initShaders(QStringLiteral(":/shaders/vertex"),
                QStringLiteral(":/shaders/fragmentES2"));
    initBackgroundShaders(QStringLiteral(":/shaders/vertex"),
                          QStringLiteral(":/shaders/fragmentES2"));
    initCustomItemShaders(QStringLiteral(":/shaders/vertexTexture"),
                          QStringLiteral(":/shaders/fragmentTextureES2"));
#endif
}

void Abstract3DRenderer::handleShadowQualityChange()
{
    reInitShaders();

#if defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality != QAbstract3DGraph::ShadowQualityNone) {
        emit requestShadowQuality(QAbstract3DGraph::ShadowQualityNone);
        qWarning("Shadows are not yet supported for OpenGL ES2");
        m_cachedShadowQuality = QAbstract3DGraph::ShadowQualityNone;
    }
#endif
}

void Abstract3DRenderer::updateSelectionMode(QAbstract3DGraph::SelectionFlags mode)
{
    m_cachedSelectionMode = mode;
    m_selectionDirty = true;
}

void Abstract3DRenderer::updateAspectRatio(float ratio)
{
    m_graphAspectRatio = ratio;
    calculateZoomLevel();
    m_cachedScene->activeCamera()->d_ptr->updateViewMatrix(m_autoScaleAdjustment);
    foreach (SeriesRenderCache *cache, m_renderCacheList)
        cache->setDataDirty(true);
    updateCustomItemPositions();
}

void Abstract3DRenderer::updatePolar(bool enable)
{
    m_polarGraph = enable;
    foreach (SeriesRenderCache *cache, m_renderCacheList)
        cache->setDataDirty(true);
    updateCustomItemPositions();
}

void Abstract3DRenderer::updateRadialLabelOffset(float offset)
{
    m_radialLabelOffset = offset;
}

void Abstract3DRenderer::updateOptimizationHint(QAbstract3DGraph::OptimizationHints hint)
{
    m_cachedOptimizationHint = hint;
}

void Abstract3DRenderer::handleResize()
{
    if (m_primarySubViewport.width() == 0 || m_primarySubViewport.height() == 0)
        return;

    // Recalculate zoom
    calculateZoomLevel();

    // Re-init selection buffer
    initSelectionBuffer();

#if !defined(QT_OPENGL_ES_2)
    // Re-init depth buffer
    updateDepthBuffer();
#endif
}

void Abstract3DRenderer::calculateZoomLevel()
{
    // Calculate zoom level based on aspect ratio
    GLfloat div;
    GLfloat zoomAdjustment;
    div = qMin(m_primarySubViewport.width(), m_primarySubViewport.height());
    zoomAdjustment = 2.0f * defaultRatio
            * ((m_primarySubViewport.width() / div)
               / (m_primarySubViewport.height() / div)) / m_graphAspectRatio;
    m_autoScaleAdjustment = qMin(zoomAdjustment, 1.0f); // clamp to 1.0f
}

void Abstract3DRenderer::updateAxisType(QAbstract3DAxis::AxisOrientation orientation,
                                        QAbstract3DAxis::AxisType type)
{
    axisCacheForOrientation(orientation).setType(type);
}

void Abstract3DRenderer::updateAxisTitle(QAbstract3DAxis::AxisOrientation orientation,
                                         const QString &title)
{
    axisCacheForOrientation(orientation).setTitle(title);
}

void Abstract3DRenderer::updateAxisLabels(QAbstract3DAxis::AxisOrientation orientation,
                                          const QStringList &labels)
{
    axisCacheForOrientation(orientation).setLabels(labels);
}

void Abstract3DRenderer::updateAxisRange(QAbstract3DAxis::AxisOrientation orientation,
                                         float min, float max)
{
    AxisRenderCache &cache = axisCacheForOrientation(orientation);
    cache.setMin(min);
    cache.setMax(max);

    foreach (SeriesRenderCache *cache, m_renderCacheList)
        cache->setDataDirty(true);

    updateCustomItemPositions();
}

void Abstract3DRenderer::updateAxisSegmentCount(QAbstract3DAxis::AxisOrientation orientation,
                                                int count)
{
    AxisRenderCache &cache = axisCacheForOrientation(orientation);
    cache.setSegmentCount(count);
}

void Abstract3DRenderer::updateAxisSubSegmentCount(QAbstract3DAxis::AxisOrientation orientation,
                                                   int count)
{
    AxisRenderCache &cache = axisCacheForOrientation(orientation);
    cache.setSubSegmentCount(count);
}

void Abstract3DRenderer::updateAxisLabelFormat(QAbstract3DAxis::AxisOrientation orientation,
                                               const QString &format)
{
    axisCacheForOrientation(orientation).setLabelFormat(format);
}

void Abstract3DRenderer::updateAxisReversed(QAbstract3DAxis::AxisOrientation orientation,
                                            bool enable)
{
    axisCacheForOrientation(orientation).setReversed(enable);
    foreach (SeriesRenderCache *cache, m_renderCacheList)
        cache->setDataDirty(true);

    updateCustomItemPositions();
}

void Abstract3DRenderer::updateAxisFormatter(QAbstract3DAxis::AxisOrientation orientation,
                                             QValue3DAxisFormatter *formatter)
{
    AxisRenderCache &cache = axisCacheForOrientation(orientation);
    if (cache.ctrlFormatter() != formatter) {
        delete cache.formatter();
        cache.setFormatter(formatter->createNewInstance());
        cache.setCtrlFormatter(formatter);
    }
    formatter->d_ptr->populateCopy(*(cache.formatter()));
    cache.markPositionsDirty();

    foreach (SeriesRenderCache *cache, m_renderCacheList)
        cache->setDataDirty(true);

    updateCustomItemPositions();
}

void Abstract3DRenderer::updateAxisLabelAutoRotation(QAbstract3DAxis::AxisOrientation orientation,
                                                     float angle)
{
    AxisRenderCache &cache = axisCacheForOrientation(orientation);
    if (cache.labelAutoRotation() != angle)
        cache.setLabelAutoRotation(angle);
}

void Abstract3DRenderer::updateAxisTitleVisibility(QAbstract3DAxis::AxisOrientation orientation,
                                                   bool visible)
{
    AxisRenderCache &cache = axisCacheForOrientation(orientation);
    if (cache.isTitleVisible() != visible)
        cache.setTitleVisible(visible);
}

void Abstract3DRenderer::updateAxisTitleFixed(QAbstract3DAxis::AxisOrientation orientation,
                                              bool fixed)
{
    AxisRenderCache &cache = axisCacheForOrientation(orientation);
    if (cache.isTitleFixed() != fixed)
        cache.setTitleFixed(fixed);
}

void Abstract3DRenderer::modifiedSeriesList(const QVector<QAbstract3DSeries *> &seriesList)
{
    foreach (QAbstract3DSeries *series, seriesList) {
        SeriesRenderCache *cache = m_renderCacheList.value(series, 0);
        if (cache)
            cache->setDataDirty(true);
    }
}

void Abstract3DRenderer::fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh mesh)
{
    // Default implementation does nothing.
    Q_UNUSED(fileName)
    Q_UNUSED(mesh)
}

void Abstract3DRenderer::updateSeries(const QList<QAbstract3DSeries *> &seriesList)
{
    foreach (SeriesRenderCache *cache, m_renderCacheList)
        cache->setValid(false);

    m_visibleSeriesCount = 0;
    int seriesCount = seriesList.size();
    for (int i = 0; i < seriesCount; i++) {
        QAbstract3DSeries *series = seriesList.at(i);
        SeriesRenderCache *cache = m_renderCacheList.value(series);
        bool newSeries = false;
        if (!cache) {
            cache = createNewCache(series);
            m_renderCacheList[series] = cache;
            newSeries = true;
        }
        cache->setValid(true);
        cache->populate(newSeries);
        if (cache->isVisible())
            m_visibleSeriesCount++;
    }

    // Remove non-valid objects from the cache list
    foreach (SeriesRenderCache *cache, m_renderCacheList) {
        if (!cache->isValid())
            cleanCache(cache);
    }
}

void Abstract3DRenderer::updateCustomData(const QList<QCustom3DItem *> &customItems)
{
    if (customItems.isEmpty() && m_customRenderCache.isEmpty())
        return;

    foreach (CustomRenderItem *item, m_customRenderCache)
        item->setValid(false);

    int itemCount = customItems.size();
    // Check custom item list for items that are not yet in render item cache
    for (int i = 0; i < itemCount; i++) {
        QCustom3DItem *item = customItems.at(i);
        CustomRenderItem *renderItem = m_customRenderCache.value(item);
        if (!renderItem)
            renderItem = addCustomItem(item);
        renderItem->setValid(true);
        renderItem->setIndex(i); // always update index, as it must match the custom item index
    }

    // Check render item cache and remove items that are not in customItems list anymore
    foreach (CustomRenderItem *renderItem, m_customRenderCache) {
        if (!renderItem->isValid()) {
            m_customRenderCache.remove(renderItem->itemPointer());
            GLuint texture = renderItem->texture();
            m_textureHelper->deleteTexture(&texture);
            delete renderItem;
        }
    }
}

void Abstract3DRenderer::updateCustomItems()
{
    // Check all items
    foreach (CustomRenderItem *item, m_customRenderCache)
        updateCustomItem(item);
}

SeriesRenderCache *Abstract3DRenderer::createNewCache(QAbstract3DSeries *series)
{
    return new SeriesRenderCache(series, this);
}

void Abstract3DRenderer::cleanCache(SeriesRenderCache *cache)
{
    m_renderCacheList.remove(cache->series());
    cache->cleanup(m_textureHelper);
    delete cache;
}

AxisRenderCache &Abstract3DRenderer::axisCacheForOrientation(
        QAbstract3DAxis::AxisOrientation orientation)
{
    switch (orientation) {
    case QAbstract3DAxis::AxisOrientationX:
        return m_axisCacheX;
    case QAbstract3DAxis::AxisOrientationY:
        return m_axisCacheY;
    case QAbstract3DAxis::AxisOrientationZ:
        return m_axisCacheZ;
    default:
        qFatal("Abstract3DRenderer::axisCacheForOrientation");
        return m_axisCacheX;
    }
}

void Abstract3DRenderer::lowerShadowQuality()
{
    QAbstract3DGraph::ShadowQuality newQuality = QAbstract3DGraph::ShadowQualityNone;

    switch (m_cachedShadowQuality) {
    case QAbstract3DGraph::ShadowQualityHigh:
        qWarning("Creating high quality shadows failed. Changing to medium quality.");
        newQuality = QAbstract3DGraph::ShadowQualityMedium;
        break;
    case QAbstract3DGraph::ShadowQualityMedium:
        qWarning("Creating medium quality shadows failed. Changing to low quality.");
        newQuality = QAbstract3DGraph::ShadowQualityLow;
        break;
    case QAbstract3DGraph::ShadowQualityLow:
        qWarning("Creating low quality shadows failed. Switching shadows off.");
        newQuality = QAbstract3DGraph::ShadowQualityNone;
        break;
    case QAbstract3DGraph::ShadowQualitySoftHigh:
        qWarning("Creating soft high quality shadows failed. Changing to soft medium quality.");
        newQuality = QAbstract3DGraph::ShadowQualitySoftMedium;
        break;
    case QAbstract3DGraph::ShadowQualitySoftMedium:
        qWarning("Creating soft medium quality shadows failed. Changing to soft low quality.");
        newQuality = QAbstract3DGraph::ShadowQualitySoftLow;
        break;
    case QAbstract3DGraph::ShadowQualitySoftLow:
        qWarning("Creating soft low quality shadows failed. Switching shadows off.");
        newQuality = QAbstract3DGraph::ShadowQualityNone;
        break;
    default:
        // You'll never get here
        break;
    }

    emit requestShadowQuality(newQuality);
    updateShadowQuality(newQuality);
}

void Abstract3DRenderer::drawAxisTitleY(const QVector3D &sideLabelRotation,
                                        const QVector3D &backLabelRotation,
                                        const QVector3D &sideLabelTrans,
                                        const QVector3D &backLabelTrans,
                                        const QQuaternion &totalSideRotation,
                                        const QQuaternion &totalBackRotation,
                                        AbstractRenderItem &dummyItem,
                                        const Q3DCamera *activeCamera,
                                        float labelsMaxWidth,
                                        const QMatrix4x4 &viewMatrix,
                                        const QMatrix4x4 &projectionMatrix,
                                        ShaderHelper *shader)
{
    float scaleFactor = m_drawer->scaledFontSize() / m_axisCacheY.titleItem().size().height();
    float titleOffset = 2.0f * (labelMargin + (labelsMaxWidth * scaleFactor));
    float yRotation;
    QVector3D titleTrans;
    QQuaternion totalRotation;
    if (m_xFlipped == m_zFlipped) {
        yRotation = backLabelRotation.y();
        titleTrans = backLabelTrans;
        totalRotation = totalBackRotation;
    } else {
        yRotation = sideLabelRotation.y();
        titleTrans = sideLabelTrans;
        totalRotation = totalSideRotation;
    }

    QQuaternion offsetRotator = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, yRotation);
    QVector3D titleOffsetVector =
            offsetRotator.rotatedVector(QVector3D(-titleOffset, 0.0f, 0.0f));

    QQuaternion titleRotation;
    if (m_axisCacheY.isTitleFixed()) {
        titleRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, yRotation)
                * m_zRightAngleRotation;
    } else {
        titleRotation = totalRotation * m_zRightAngleRotation;
    }
    dummyItem.setTranslation(titleTrans + titleOffsetVector);

    m_drawer->drawLabel(dummyItem, m_axisCacheY.titleItem(), viewMatrix,
                        projectionMatrix, zeroVector, titleRotation, 0,
                        m_cachedSelectionMode, shader, m_labelObj, activeCamera,
                        true, true, Drawer::LabelMid, Qt::AlignBottom);
}

void Abstract3DRenderer::drawAxisTitleX(const QVector3D &labelRotation,
                                        const QVector3D &labelTrans,
                                        const QQuaternion &totalRotation,
                                        AbstractRenderItem &dummyItem,
                                        const Q3DCamera *activeCamera,
                                        float labelsMaxWidth,
                                        const QMatrix4x4 &viewMatrix,
                                        const QMatrix4x4 &projectionMatrix,
                                        ShaderHelper *shader,
                                        bool radial)
{
    float scaleFactor = m_drawer->scaledFontSize() / m_axisCacheX.titleItem().size().height();
    float titleOffset;
    if (radial)
        titleOffset = -2.0f * (labelMargin + m_drawer->scaledFontSize());
    else
        titleOffset = 2.0f * (labelMargin + (labelsMaxWidth * scaleFactor));
    float zRotation = 0.0f;
    float yRotation = 0.0f;
    float xRotation = -90.0f + labelRotation.z();
    float offsetRotation = labelRotation.z();
    float extraRotation = -90.0f;
    Qt::AlignmentFlag alignment = Qt::AlignTop;
    if (m_yFlippedForGrid) {
        alignment = Qt::AlignBottom;
        zRotation = 180.0f;
        if (m_zFlipped) {
            titleOffset = -titleOffset;
            if (m_xFlipped) {
                offsetRotation = -offsetRotation;
                extraRotation = -extraRotation;
            } else {
                xRotation = -90.0f - labelRotation.z();
            }
        } else {
            yRotation = 180.0f;
            if (m_xFlipped) {
                offsetRotation = -offsetRotation;
                xRotation = -90.0f - labelRotation.z();
            } else {
                extraRotation = -extraRotation;
            }
        }
    } else {
        if (m_zFlipped) {
            titleOffset = -titleOffset;
            if (m_xFlipped) {
                yRotation = 180.0f;
                offsetRotation = -offsetRotation;
            } else {
                yRotation = 180.0f;
                xRotation = -90.0f - labelRotation.z();
                extraRotation = -extraRotation;
            }
        } else {
            if (m_xFlipped) {
                offsetRotation = -offsetRotation;
                xRotation = -90.0f - labelRotation.z();
                extraRotation = -extraRotation;
            }
        }
    }

    if (radial) {
        if (m_zFlipped) {
            titleOffset = -titleOffset;
        } else {
            if (m_yFlippedForGrid)
                alignment = Qt::AlignTop;
            else
                alignment = Qt::AlignBottom;
        }
    }

    if (offsetRotation == 180.0f || offsetRotation == -180.0f)
        offsetRotation = 0.0f;
    QQuaternion offsetRotator = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, offsetRotation);
    QVector3D titleOffsetVector =
            offsetRotator.rotatedVector(QVector3D(0.0f, 0.0f, titleOffset));

    QQuaternion titleRotation;
    if (m_axisCacheX.isTitleFixed()) {
        titleRotation = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, zRotation)
                * QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, yRotation)
                * QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, xRotation);
    } else {
        titleRotation = totalRotation
                * QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, extraRotation);
    }
    dummyItem.setTranslation(labelTrans + titleOffsetVector);

    m_drawer->drawLabel(dummyItem, m_axisCacheX.titleItem(), viewMatrix,
                        projectionMatrix, zeroVector, titleRotation, 0,
                        m_cachedSelectionMode, shader, m_labelObj, activeCamera,
                        true, true, Drawer::LabelMid, alignment);
}

void Abstract3DRenderer::drawAxisTitleZ(const QVector3D &labelRotation,
                                        const QVector3D &labelTrans,
                                        const QQuaternion &totalRotation,
                                        AbstractRenderItem &dummyItem,
                                        const Q3DCamera *activeCamera,
                                        float labelsMaxWidth,
                                        const QMatrix4x4 &viewMatrix,
                                        const QMatrix4x4 &projectionMatrix,
                                        ShaderHelper *shader)
{
    float scaleFactor = m_drawer->scaledFontSize() / m_axisCacheZ.titleItem().size().height();
    float titleOffset = 2.0f * (labelMargin + (labelsMaxWidth * scaleFactor));
    float zRotation = labelRotation.z();
    float yRotation = -90.0f;
    float xRotation = -90.0f;
    float extraRotation = 90.0f;
    Qt::AlignmentFlag alignment = Qt::AlignTop;
    if (m_yFlippedForGrid) {
        alignment = Qt::AlignBottom;
        xRotation = -xRotation;
        if (m_zFlipped) {
            if (m_xFlipped) {
                titleOffset = -titleOffset;
                zRotation = -zRotation;
                extraRotation = -extraRotation;
            } else {
                zRotation = -zRotation;
                yRotation = -yRotation;
            }
        } else {
            if (m_xFlipped) {
                titleOffset = -titleOffset;
            } else {
                extraRotation = -extraRotation;
                yRotation = -yRotation;
            }
        }
    } else {
        if (m_zFlipped) {
            zRotation = -zRotation;
            if (m_xFlipped) {
                titleOffset = -titleOffset;
            } else {
                extraRotation = -extraRotation;
                yRotation = -yRotation;
            }
        } else {
            if (m_xFlipped) {
                titleOffset = -titleOffset;
                extraRotation = -extraRotation;
            } else {
                yRotation = -yRotation;
            }
        }
    }

    float offsetRotation = zRotation;
    if (offsetRotation == 180.0f || offsetRotation == -180.0f)
        offsetRotation = 0.0f;
    QQuaternion offsetRotator = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, offsetRotation);
    QVector3D titleOffsetVector =
            offsetRotator.rotatedVector(QVector3D(titleOffset, 0.0f, 0.0f));

    QQuaternion titleRotation;
    if (m_axisCacheZ.isTitleFixed()) {
        titleRotation = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, zRotation)
                * QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, yRotation)
                * QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, xRotation);
    } else {
        titleRotation = totalRotation
                * QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, extraRotation);
    }
    dummyItem.setTranslation(labelTrans + titleOffsetVector);

    m_drawer->drawLabel(dummyItem, m_axisCacheZ.titleItem(), viewMatrix,
                        projectionMatrix, zeroVector, titleRotation, 0,
                        m_cachedSelectionMode, shader, m_labelObj, activeCamera,
                        true, true, Drawer::LabelMid, alignment);
}

void Abstract3DRenderer::loadGridLineMesh()
{
    ObjectHelper::resetObjectHelper(this, m_gridLineObj,
                                    QStringLiteral(":/defaultMeshes/plane"));
}

void Abstract3DRenderer::loadLabelMesh()
{
    ObjectHelper::resetObjectHelper(this, m_labelObj,
                                    QStringLiteral(":/defaultMeshes/plane"));
}

void Abstract3DRenderer::generateBaseColorTexture(const QColor &color, GLuint *texture)
{
    m_textureHelper->deleteTexture(texture);
    *texture = m_textureHelper->createUniformTexture(color);
}

void Abstract3DRenderer::fixGradientAndGenerateTexture(QLinearGradient *gradient,
                                                       GLuint *gradientTexture)
{
    // Readjust start/stop to match gradient texture size
    gradient->setStart(qreal(gradientTextureWidth), qreal(gradientTextureHeight));
    gradient->setFinalStop(0.0, 0.0);

    m_textureHelper->deleteTexture(gradientTexture);

    *gradientTexture = m_textureHelper->createGradientTexture(*gradient);
}

LabelItem &Abstract3DRenderer::selectionLabelItem()
{
    if (!m_selectionLabelItem)
        m_selectionLabelItem = new LabelItem;
    return *m_selectionLabelItem;
}

void Abstract3DRenderer::setSelectionLabel(const QString &label)
{
    if (m_selectionLabelItem)
        m_selectionLabelItem->clear();
    m_selectionLabel = label;
}

QString &Abstract3DRenderer::selectionLabel()
{
    return m_selectionLabel;
}

QVector4D Abstract3DRenderer::indexToSelectionColor(GLint index)
{
    GLubyte idxRed = index & 0xff;
    GLubyte idxGreen = (index & 0xff00) >> 8;
    GLubyte idxBlue = (index & 0xff0000) >> 16;

    return QVector4D(idxRed, idxGreen, idxBlue, 0);
}

CustomRenderItem *Abstract3DRenderer::addCustomItem(QCustom3DItem *item)
{
    CustomRenderItem *newItem = new CustomRenderItem();
    newItem->setRenderer(this);
    newItem->setItemPointer(item); // Store pointer for render item updates
    newItem->setMesh(item->meshFile());
    QVector3D scaling = item->scaling();
    QImage textureImage = item->d_ptr->textureImage();
    bool facingCamera = false;
    if (item->d_ptr->m_isLabelItem) {
        QCustom3DLabel *labelItem = static_cast<QCustom3DLabel *>(item);
        float pointSize = labelItem->font().pointSizeF();
        // Check do we have custom visuals or need to use theme
        if (!labelItem->dptr()->m_customVisuals) {
            // Recreate texture using theme
            labelItem->dptr()->createTextureImage(m_cachedTheme->labelBackgroundColor(),
                                                  m_cachedTheme->labelTextColor(),
                                                  m_cachedTheme->isLabelBackgroundEnabled(),
                                                  m_cachedTheme->isLabelBorderEnabled());
            pointSize = m_cachedTheme->font().pointSizeF();
            textureImage = item->d_ptr->textureImage();
        }
        // Calculate scaling based on text (texture size), font size and asked scaling
        float scaledFontSize = (0.05f + pointSize / 500.0f) / float(textureImage.height());
        scaling.setX(scaling.x() * textureImage.width() * scaledFontSize);
        scaling.setY(scaling.y() * textureImage.height() * scaledFontSize);
        // Check if facing camera
        facingCamera = labelItem->isFacingCamera();
    }
    newItem->setScaling(scaling);
    newItem->setRotation(item->rotation());
    newItem->setPosition(item->position());
    newItem->setPositionAbsolute(item->isPositionAbsolute());
    newItem->setBlendNeeded(textureImage.hasAlphaChannel());
    GLuint texture = m_textureHelper->create2DTexture(textureImage, true, true, true);
    newItem->setTexture(texture);
    item->d_ptr->clearTextureImage();
    QVector3D translation = convertPositionToTranslation(item->position(),
                                                         item->isPositionAbsolute());
    newItem->setTranslation(translation);
    newItem->setVisible(item->isVisible());
    newItem->setShadowCasting(item->isShadowCasting());
    newItem->setFacingCamera(facingCamera);
    m_customRenderCache.insert(item, newItem);
    return newItem;
}

void Abstract3DRenderer::updateCustomItem(CustomRenderItem *renderItem)
{
    QCustom3DItem *item = renderItem->itemPointer();
    if (item->d_ptr->m_dirtyBits.meshDirty) {
        renderItem->setMesh(item->meshFile());
        item->d_ptr->m_dirtyBits.meshDirty = false;
    }
    if (item->d_ptr->m_dirtyBits.scalingDirty) {
        QVector3D scaling = item->scaling();
        // In case we have label item, we need to recreate texture for scaling adjustment
        if (item->d_ptr->m_isLabelItem) {
            QCustom3DLabel *labelItem = static_cast<QCustom3DLabel *>(item);
            float pointSize = labelItem->font().pointSizeF();
            // Check do we have custom visuals or need to use theme
            if (labelItem->dptr()->m_customVisuals) {
                // Recreate texture
                labelItem->dptr()->createTextureImage();
            } else {
                // Recreate texture using theme
                labelItem->dptr()->createTextureImage(m_cachedTheme->labelBackgroundColor(),
                                                      m_cachedTheme->labelTextColor(),
                                                      m_cachedTheme->isLabelBackgroundEnabled(),
                                                      m_cachedTheme->isLabelBorderEnabled());
                pointSize = m_cachedTheme->font().pointSizeF();
            }
            QImage textureImage = item->d_ptr->textureImage();
            // Calculate scaling based on text (texture size), font size and asked scaling
            float scaledFontSize = (0.05f + pointSize / 500.0f) / float(textureImage.height());
            scaling.setX(scaling.x() * textureImage.width() * scaledFontSize);
            scaling.setY(scaling.y() * textureImage.height() * scaledFontSize);
            item->d_ptr->clearTextureImage();
        }
        renderItem->setScaling(scaling);
        item->d_ptr->m_dirtyBits.scalingDirty = false;
    }
    if (item->d_ptr->m_dirtyBits.rotationDirty) {
        renderItem->setRotation(item->rotation());
        item->d_ptr->m_dirtyBits.rotationDirty = false;
    }
    if (item->d_ptr->m_dirtyBits.textureDirty) {
        QImage textureImage = item->d_ptr->textureImage();
        if (item->d_ptr->m_isLabelItem) {
            QCustom3DLabel *labelItem = static_cast<QCustom3DLabel *>(item);
            // Check do we have custom visuals or need to use theme
            if (!labelItem->dptr()->m_customVisuals) {
                // Recreate texture using theme
                labelItem->dptr()->createTextureImage(m_cachedTheme->labelBackgroundColor(),
                                                      m_cachedTheme->labelTextColor(),
                                                      m_cachedTheme->isLabelBackgroundEnabled(),
                                                      m_cachedTheme->isLabelBorderEnabled());
                textureImage = item->d_ptr->textureImage();
            }
        }
        renderItem->setBlendNeeded(textureImage.hasAlphaChannel());
        GLuint oldTexture = renderItem->texture();
        m_textureHelper->deleteTexture(&oldTexture);
        GLuint texture = m_textureHelper->create2DTexture(textureImage, true, true, true);
        renderItem->setTexture(texture);
        item->d_ptr->clearTextureImage();
        item->d_ptr->m_dirtyBits.textureDirty = false;
    }
    if (item->d_ptr->m_dirtyBits.positionDirty || item->d_ptr->m_dirtyBits.positionAbsoluteDirty) {
        renderItem->setPosition(item->position());
        renderItem->setPositionAbsolute(item->isPositionAbsolute());
        QVector3D translation = convertPositionToTranslation(item->position(),
                                                             item->isPositionAbsolute());
        renderItem->setTranslation(translation);
        item->d_ptr->m_dirtyBits.positionDirty = false;
        item->d_ptr->m_dirtyBits.positionAbsoluteDirty = false;
    }
    if (item->d_ptr->m_dirtyBits.visibleDirty) {
        renderItem->setVisible(item->isVisible());
        item->d_ptr->m_dirtyBits.visibleDirty = false;
    }
    if (item->d_ptr->m_dirtyBits.shadowCastingDirty) {
        renderItem->setShadowCasting(item->isShadowCasting());
        item->d_ptr->m_dirtyBits.shadowCastingDirty = false;
    }
    if (item->d_ptr->m_isLabelItem) {
        QCustom3DLabel *labelItem = static_cast<QCustom3DLabel *>(item);
        if (labelItem->dptr()->m_facingCameraDirty) {
            renderItem->setFacingCamera(labelItem->isFacingCamera());
            labelItem->dptr()->m_facingCameraDirty = false;
        }
    }
}

void Abstract3DRenderer::updateCustomItemPositions()
{
    foreach (CustomRenderItem *renderItem, m_customRenderCache) {
        QVector3D translation = convertPositionToTranslation(renderItem->position(),
                                                             renderItem->isPositionAbsolute());
        renderItem->setTranslation(translation);
    }
}

#ifdef USE_REFLECTIONS
void Abstract3DRenderer::drawCustomItems(RenderingState state,
                                         ShaderHelper *shader,
                                         const QMatrix4x4 &viewMatrix,
                                         const QMatrix4x4 &projectionViewMatrix,
                                         const QMatrix4x4 &depthProjectionViewMatrix,
                                         GLuint depthTexture,
                                         GLfloat shadowQuality,
                                         GLfloat reflection)
#else
void Abstract3DRenderer::drawCustomItems(RenderingState state,
                                         ShaderHelper *shader,
                                         const QMatrix4x4 &viewMatrix,
                                         const QMatrix4x4 &projectionViewMatrix,
                                         const QMatrix4x4 &depthProjectionViewMatrix,
                                         GLuint depthTexture,
                                         GLfloat shadowQuality)
#endif
{
    if (m_customRenderCache.isEmpty())
        return;

    if (RenderingNormal == state) {
        shader->bind();
        shader->setUniformValue(shader->lightP(), m_cachedScene->activeLight()->position());
        shader->setUniformValue(shader->ambientS(), m_cachedTheme->ambientLightStrength());
        shader->setUniformValue(shader->lightColor(),
                                Utils::vectorFromColor(m_cachedTheme->lightColor()));
        shader->setUniformValue(shader->view(), viewMatrix);
    }

    // Draw custom items
    foreach (CustomRenderItem *item, m_customRenderCache) {
        // Check that the render item is visible, and skip drawing if not
        if (!item->isVisible())
            continue;

        // Check if the render item is in data coordinates and not within axis ranges, and skip drawing if it is
        if (!item->isPositionAbsolute()
                && (item->position().x() < m_axisCacheX.min()
                    || item->position().x() > m_axisCacheX.max()
                    || item->position().z() < m_axisCacheZ.min()
                    || item->position().z() > m_axisCacheZ.max()
                    || item->position().y() < m_axisCacheY.min()
                    || item->position().y() > m_axisCacheY.max())) {
            continue;
        }

        QMatrix4x4 modelMatrix;
        QMatrix4x4 itModelMatrix;
        QMatrix4x4 MVPMatrix;

        QQuaternion rotation = item->rotation();
        // Check if the (label) item should be facing camera, and adjust rotation accordingly
        if (item->isFacingCamera()) {
            float camRotationX = m_cachedScene->activeCamera()->xRotation();
            float camRotationY = m_cachedScene->activeCamera()->yRotation();
            rotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, -camRotationX)
                    * QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -camRotationY);
        }

#ifdef USE_REFLECTIONS
        if (reflection < 0.0f) {
            if (item->itemPointer()->d_ptr->m_isLabelItem)
                continue;
            else
                glCullFace(GL_FRONT);
        } else {
            glCullFace(GL_BACK);
        }
        QVector3D trans = item->translation();
        trans.setY(reflection * trans.y());
        modelMatrix.translate(trans);
        if (reflection < 0.0f) {
            QQuaternion mirror = QQuaternion(rotation.scalar(),
                                             -rotation.x(), rotation.y(), -rotation.z());
            modelMatrix.rotate(mirror);
            itModelMatrix.rotate(mirror);
        } else {
            modelMatrix.rotate(rotation);
            itModelMatrix.rotate(rotation);
        }
        QVector3D scale = item->scaling();
        scale.setY(reflection * scale.y());
        modelMatrix.scale(scale);
#else
        modelMatrix.translate(item->translation());
        modelMatrix.rotate(rotation);
        modelMatrix.scale(item->scaling());
        itModelMatrix.rotate(rotation);
#endif
        if (!item->isFacingCamera())
            itModelMatrix.scale(item->scaling());
        MVPMatrix = projectionViewMatrix * modelMatrix;

        if (RenderingNormal == state) {
            // Normal render
            shader->setUniformValue(shader->model(), modelMatrix);
            shader->setUniformValue(shader->MVP(), MVPMatrix);
            shader->setUniformValue(shader->nModel(), itModelMatrix.inverted().transposed());

            if (item->isBlendNeeded()) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDisable(GL_CULL_FACE);
            } else {
                glDisable(GL_BLEND);
                glEnable(GL_CULL_FACE);
            }

#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                // Set shadow shader bindings
                shader->setUniformValue(shader->shadowQ(), shadowQuality);
                shader->setUniformValue(shader->depth(), depthProjectionViewMatrix * modelMatrix);
                shader->setUniformValue(shader->lightS(), m_cachedTheme->lightStrength() / 10.0f);
                m_drawer->drawObject(shader, item->mesh(), item->texture(), depthTexture);
            } else
#else
            Q_UNUSED(depthTexture)
            Q_UNUSED(shadowQuality)
#endif
            {
                // Set shadowless shader bindings
                shader->setUniformValue(shader->lightS(), m_cachedTheme->lightStrength());
                m_drawer->drawObject(shader, item->mesh(), item->texture());
            }
        } else if (RenderingSelection == state) {
            // Selection render
            shader->setUniformValue(shader->MVP(), MVPMatrix);
            QVector4D itemColor = indexToSelectionColor(item->index());
            itemColor.setW(customItemAlpha);
            itemColor /= 255.0f;
            shader->setUniformValue(shader->color(), itemColor);
            m_drawer->drawObject(shader, item->mesh());
        } else if (item->isShadowCasting()) {
            // Depth render
            shader->setUniformValue(shader->MVP(), depthProjectionViewMatrix * modelMatrix);
            m_drawer->drawObject(shader, item->mesh());
        }
    }

    if (RenderingNormal == state) {
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }
}

void Abstract3DRenderer::calculatePolarXZ(const QVector3D &dataPos, float &x, float &z) const
{
    // x is angular, z is radial
    qreal angle = m_axisCacheX.formatter()->positionAt(dataPos.x()) * doublePi;
    qreal radius = m_axisCacheZ.formatter()->positionAt(dataPos.z());

    // Convert angle & radius to X and Z coords
    x = float(radius * qSin(angle)) * m_graphAspectRatio;
    z = -float(radius * qCos(angle)) * m_graphAspectRatio;
}

void Abstract3DRenderer::drawRadialGrid(ShaderHelper *shader, float yFloorLinePos,
                                        const QMatrix4x4 &projectionViewMatrix,
                                        const QMatrix4x4 &depthMatrix)
{
    static QVector<QQuaternion> lineRotations;
    if (!lineRotations.size()) {
        lineRotations.resize(polarGridRoundness);
        for (int j = 0; j < polarGridRoundness; j++) {
            lineRotations[j] = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f,
                                                             polarGridAngleDegrees * float(j));
        }
    }
    int gridLineCount = m_axisCacheZ.gridLineCount();
    const QVector<float> &gridPositions = m_axisCacheZ.formatter()->gridPositions();
    const QVector<float> &subGridPositions = m_axisCacheZ.formatter()->subGridPositions();
    int mainSize = gridPositions.size();
    QVector3D translateVector(0.0f, yFloorLinePos, 0.0f);
    QQuaternion finalRotation = m_xRightAngleRotationNeg;
    if (m_yFlippedForGrid)
        finalRotation *= m_xFlipRotation;

    for (int i = 0; i < gridLineCount; i++) {
        float gridPosition = (i >= mainSize)
                ? subGridPositions.at(i - mainSize) : gridPositions.at(i);
        float radiusFraction = m_graphAspectRatio * gridPosition;
        QVector3D gridLineScaler(radiusFraction * float(qSin(polarGridHalfAngle)),
                                 gridLineWidth, gridLineWidth);
        translateVector.setZ(gridPosition * m_graphAspectRatio);
        for (int j = 0; j < polarGridRoundness; j++) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 itModelMatrix;
            modelMatrix.rotate(lineRotations.at(j));
            itModelMatrix.rotate(lineRotations.at(j));
            modelMatrix.translate(translateVector);
            modelMatrix.scale(gridLineScaler);
            itModelMatrix.scale(gridLineScaler);
            modelMatrix.rotate(finalRotation);
            itModelMatrix.rotate(finalRotation);
            QMatrix4x4 MVPMatrix = projectionViewMatrix * modelMatrix;

            shader->setUniformValue(shader->model(), modelMatrix);
            shader->setUniformValue(shader->nModel(), itModelMatrix.inverted().transposed());
            shader->setUniformValue(shader->MVP(), MVPMatrix);
#if !defined(QT_OPENGL_ES_2)
            if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
                // Set shadow shader bindings
                QMatrix4x4 depthMVPMatrix = depthMatrix * modelMatrix;
                shader->setUniformValue(shader->depth(), depthMVPMatrix);
                // Draw the object
                m_drawer->drawObject(shader, m_gridLineObj, 0, m_depthTexture);
            } else {
                // Draw the object
                m_drawer->drawObject(shader, m_gridLineObj);
            }
#else
            m_drawer->drawLine(shader);
#endif
        }
    }
}

void Abstract3DRenderer::drawAngularGrid(ShaderHelper *shader, float yFloorLinePos,
                                         const QMatrix4x4 &projectionViewMatrix,
                                         const QMatrix4x4 &depthMatrix)
{
    float halfRatio((m_graphAspectRatio + (labelMargin / 2.0f)) / 2.0f);
    QVector3D gridLineScaler(gridLineWidth, gridLineWidth, halfRatio);
    int gridLineCount = m_axisCacheX.gridLineCount();
    const QVector<float> &gridPositions = m_axisCacheX.formatter()->gridPositions();
    const QVector<float> &subGridPositions = m_axisCacheX.formatter()->subGridPositions();
    int mainSize = gridPositions.size();
    QVector3D translateVector(0.0f, yFloorLinePos, -halfRatio);
    QQuaternion finalRotation = m_xRightAngleRotationNeg;
    if (m_yFlippedForGrid)
        finalRotation *= m_xFlipRotation;
    for (int i = 0; i < gridLineCount; i++) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 itModelMatrix;
        float gridPosition = (i >= mainSize)
                ? subGridPositions.at(i - mainSize) : gridPositions.at(i);
        QQuaternion lineRotation = QQuaternion::fromAxisAndAngle(upVector, gridPosition * 360.0f);
        modelMatrix.rotate(lineRotation);
        itModelMatrix.rotate(lineRotation);
        modelMatrix.translate(translateVector);
        modelMatrix.scale(gridLineScaler);
        itModelMatrix.scale(gridLineScaler);
        modelMatrix.rotate(finalRotation);
        itModelMatrix.rotate(finalRotation);
        QMatrix4x4 MVPMatrix = projectionViewMatrix * modelMatrix;

        shader->setUniformValue(shader->model(), modelMatrix);
        shader->setUniformValue(shader->nModel(), itModelMatrix.inverted().transposed());
        shader->setUniformValue(shader->MVP(), MVPMatrix);
#if !defined(QT_OPENGL_ES_2)
        if (m_cachedShadowQuality > QAbstract3DGraph::ShadowQualityNone) {
            // Set shadow shader bindings
            QMatrix4x4 depthMVPMatrix = depthMatrix * modelMatrix;
            shader->setUniformValue(shader->depth(), depthMVPMatrix);
            // Draw the object
            m_drawer->drawObject(shader, m_gridLineObj, 0, m_depthTexture);
        } else {
            // Draw the object
            m_drawer->drawObject(shader, m_gridLineObj);
        }
#else
        m_drawer->drawLine(shader);
#endif
    }
}

QT_END_NAMESPACE_DATAVISUALIZATION
