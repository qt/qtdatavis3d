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

#include "abstract3drenderer_p.h"
#include "q3dvalueaxis.h"
#include "texturehelper_p.h"
#include "utils_p.h"
#include "q3dscene_p.h"
#include "q3dcamera_p.h"
#include "q3dlight_p.h"
#include "qabstract3dseries_p.h"

Q_DECLARE_METATYPE(QtDataVisualization::QDataVis::ShadowQuality)

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Abstract3DRenderer::Abstract3DRenderer(Abstract3DController *controller)
    : QObject(0),
      m_hasNegativeValues(false),
      m_cachedTheme(),
      m_cachedFont(QFont(QStringLiteral("Arial"))),
      m_cachedLabelStyle(QDataVis::LabelStyleFromTheme),
      m_drawer(new Drawer(m_cachedTheme, m_cachedFont, m_cachedLabelStyle)),
      m_cachedBoundingRect(QRect(0,0,0,0)),
      m_cachedShadowQuality(QDataVis::ShadowQualityMedium),
      m_autoScaleAdjustment(1.0f),
      m_cachedSelectionMode(QDataVis::SelectionNone),
      m_cachedIsGridEnabled(false),
      m_cachedIsBackgroundEnabled(false),
      m_cachedColorStyle(QDataVis::ColorStyleUniform),
      m_objectGradientTexture(0),
      m_singleHighlightGradientTexture(0),
      m_multiHighlightGradientTexture(0),
      m_textureHelper(0),
      m_cachedScene(new Q3DScene()),
      m_selectionDirty(true),
      m_inputState(QDataVis::InputStateNone)
    #ifdef DISPLAY_RENDER_SPEED
    , m_isFirstFrame(true),
      m_numFrames(0)
    #endif

{
    QObject::connect(m_drawer, &Drawer::drawerChanged, this, &Abstract3DRenderer::updateTextures);
    QObject::connect(this, &Abstract3DRenderer::needRender, controller,
                     &Abstract3DController::needRender, Qt::QueuedConnection);
    QObject::connect(this, &Abstract3DRenderer::requestShadowQuality, controller,
                     &Abstract3DController::handleRequestShadowQuality, Qt::QueuedConnection);
}

Abstract3DRenderer::~Abstract3DRenderer()
{
    if (m_textureHelper) {
        m_textureHelper->deleteTexture(&m_objectGradientTexture);
        m_textureHelper->deleteTexture(&m_singleHighlightGradientTexture);
        m_textureHelper->deleteTexture(&m_multiHighlightGradientTexture);
    }

    delete m_drawer;
    delete m_textureHelper;
    delete m_cachedScene;
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

    axisCacheForOrientation(Q3DAbstractAxis::AxisOrientationX).setDrawer(m_drawer);
    axisCacheForOrientation(Q3DAbstractAxis::AxisOrientationY).setDrawer(m_drawer);
    axisCacheForOrientation(Q3DAbstractAxis::AxisOrientationZ).setDrawer(m_drawer);
}

void Abstract3DRenderer::render(const GLuint defaultFboHandle)
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

    if (defaultFboHandle) {
        glDepthMask(true);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDisable(GL_BLEND); // For QtQuick2 blending is enabled by default, but we don't want it to be
    }

    glViewport(m_cachedScene->viewport().x(),
               m_cachedScene->viewport().y(),
               m_cachedScene->viewport().width(),
               m_cachedScene->viewport().height());

    QVector3D clearColor = Utils::vectorFromColor(m_cachedTheme.m_windowColor);
    glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

QString Abstract3DRenderer::generateValueLabel(const QString &format, qreal value)
{
    QString valueLabelFormat = format;
    Utils::ParamType valueParamType = Utils::findFormatParamType(valueLabelFormat);
    QByteArray valueFormatArray = valueLabelFormat.toUtf8();
    return Utils::formatLabel(valueFormatArray, valueParamType, value);
}

QString Abstract3DRenderer::itemLabelFormat() const
{
    return m_cachedItemLabelFormat;
}

void Abstract3DRenderer::updateInputState(QDataVis::InputState state)
{
    m_inputState = state;
}

void Abstract3DRenderer::updateInputPosition(const QPoint &position)
{
    m_inputPosition = position;
}

void Abstract3DRenderer::updateBoundingRect(const QRect &boundingRect)
{
    m_cachedBoundingRect = boundingRect;
    handleResize();
}

void Abstract3DRenderer::updatePosition(const QRect &boundingRect)
{
    m_cachedBoundingRect = boundingRect;
}

void Abstract3DRenderer::updateTheme(Theme theme)
{
    m_cachedTheme.setFromTheme(theme);

    m_drawer->setTheme(m_cachedTheme);

    // Re-initialize shaders
    reInitShaders();
}

void Abstract3DRenderer::updateScene(Q3DScene *scene)
{
    // Synchronize the controller scene to that of the renderer, and vice versa.
    // Controller scene had priority if both have changed same values.
    scene->d_ptr->sync(*m_cachedScene->d_ptr);
    m_cachedScene->d_ptr->sync(*scene->d_ptr);
}

void Abstract3DRenderer::reInitShaders()
{
#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > QDataVis::ShadowQualityNone) {
        if (m_cachedColorStyle != QDataVis::ColorStyleUniform) {
            initShaders(QStringLiteral(":/shaders/vertexShadow"),
                        QStringLiteral(":/shaders/fragmentShadowNoTexColorOnY"));
        } else {
            initShaders(QStringLiteral(":/shaders/vertexShadow"),
                        QStringLiteral(":/shaders/fragmentShadowNoTex"));
        }
        initBackgroundShaders(QStringLiteral(":/shaders/vertexShadow"),
                              QStringLiteral(":/shaders/fragmentShadowNoTex"));
    } else {
        if (m_cachedColorStyle != QDataVis::ColorStyleUniform) {
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
    if (m_cachedColorStyle != QDataVis::ColorStyleUniform) {
        initShaders(QStringLiteral(":/shaders/vertex"),
                    QStringLiteral(":/shaders/fragmentColorOnYES2"));
    } else {
        initShaders(QStringLiteral(":/shaders/vertex"),
                    QStringLiteral(":/shaders/fragmentES2"));
    }
    initBackgroundShaders(QStringLiteral(":/shaders/vertex"),
                          QStringLiteral(":/shaders/fragmentES2"));
#endif
}

void Abstract3DRenderer::handleShadowQualityChange()
{
    reInitShaders();

#if defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality != QDataVis::ShadowQualityNone) {
        emit requestShadowQuality(QDataVis::ShadowQualityNone);
        qWarning("Shadows are not yet supported for OpenGL ES2");
        m_cachedShadowQuality = QDataVis::ShadowQualityNone;
    }
#endif
}

void Abstract3DRenderer::updateFont(const QFont &font)
{
    m_cachedFont = font;
    m_drawer->setFont(font);
}

void Abstract3DRenderer::updateLabelStyle(QDataVis::LabelStyle style)
{
    m_cachedLabelStyle = style;
    m_drawer->setStyle(style);
}

void Abstract3DRenderer::updateMeshFileName(const QString &objFileName)
{
    if (objFileName != m_cachedObjFile) {
        m_cachedObjFile = objFileName;
        loadMeshFile();
    }
}

void Abstract3DRenderer::updateSelectionMode(QDataVis::SelectionFlags mode)
{
    m_cachedSelectionMode = mode;
    m_selectionDirty = true;
}

void Abstract3DRenderer::updateGridEnabled(bool enable)
{
    m_cachedIsGridEnabled = enable;
}

void Abstract3DRenderer::updateBackgroundEnabled(bool enable)
{
    m_cachedIsBackgroundEnabled = enable;
}

void Abstract3DRenderer::handleResize()
{
    if (m_cachedBoundingRect.width() == 0 || m_cachedBoundingRect.height() == 0)
        return;
    // Calculate zoom level based on aspect ratio
    GLfloat div;
    GLfloat zoomAdjustment;
    div = qMin(m_cachedBoundingRect.width(), m_cachedBoundingRect.height());
    zoomAdjustment = defaultRatio * ((m_cachedBoundingRect.width() / div)
                                     / (m_cachedBoundingRect.height() / div));
    m_autoScaleAdjustment = qMin(zoomAdjustment, 1.0f); // clamp to 1.0f

    // Re-init selection buffer
    initSelectionBuffer();

#if !defined(QT_OPENGL_ES_2)
    // Re-init depth buffer
    updateDepthBuffer();
#endif
}

void Abstract3DRenderer::updateAxisType(Q3DAbstractAxis::AxisOrientation orientation, Q3DAbstractAxis::AxisType type)
{
    axisCacheForOrientation(orientation).setType(type);
}

void Abstract3DRenderer::updateAxisTitle(Q3DAbstractAxis::AxisOrientation orientation, const QString &title)
{
    axisCacheForOrientation(orientation).setTitle(title);
}

void Abstract3DRenderer::updateAxisLabels(Q3DAbstractAxis::AxisOrientation orientation, const QStringList &labels)
{
    axisCacheForOrientation(orientation).setLabels(labels);
}

void Abstract3DRenderer::updateAxisRange(Q3DAbstractAxis::AxisOrientation orientation, qreal min, qreal max)
{
    AxisRenderCache &cache = axisCacheForOrientation(orientation);
    cache.setMin(min);
    cache.setMax(max);
}

void Abstract3DRenderer::updateAxisSegmentCount(Q3DAbstractAxis::AxisOrientation orientation, int count)
{
    axisCacheForOrientation(orientation).setSegmentCount(count);
}

void Abstract3DRenderer::updateAxisSubSegmentCount(Q3DAbstractAxis::AxisOrientation orientation, int count)
{
    axisCacheForOrientation(orientation).setSubSegmentCount(count);
}

void Abstract3DRenderer::updateAxisLabelFormat(Q3DAbstractAxis::AxisOrientation orientation, const QString &format)
{
    axisCacheForOrientation(orientation).setLabelFormat(format);
}

void Abstract3DRenderer::updateColorStyle(QDataVis::ColorStyle style)
{
    bool changed = (m_cachedColorStyle != style);

    m_cachedColorStyle = style;

    if (changed)
        reInitShaders();
}

void Abstract3DRenderer::updateObjectColor(const QColor &color)
{
    m_cachedObjectColor = color;
}

void Abstract3DRenderer::updateObjectGradient(const QLinearGradient &gradient)
{
    m_cachedObjectGradient = gradient;
    fixGradient(&m_cachedObjectGradient, &m_objectGradientTexture);
}

void Abstract3DRenderer::updateSingleHighlightColor(const QColor &color)
{
    m_cachedSingleHighlightColor = color;
}

void Abstract3DRenderer::updateSingleHighlightGradient(const QLinearGradient &gradient)
{
    m_cachedSingleHighlightGradient = gradient;
    fixGradient(&m_cachedSingleHighlightGradient, &m_singleHighlightGradientTexture);
}

void Abstract3DRenderer::updateMultiHighlightColor(const QColor &color)
{
    m_cachedMultiHighlightColor = color;
}

void Abstract3DRenderer::updateMultiHighlightGradient(const QLinearGradient &gradient)
{
    m_cachedMultiHighlightGradient = gradient;
    fixGradient(&m_cachedMultiHighlightGradient, &m_multiHighlightGradientTexture);
}

void Abstract3DRenderer::updateSeriesData(const QList<QAbstract3DSeries *> &seriesList)
{
    // TODO: To series visuals update - just use first series format for now
    if (seriesList.size())
        m_cachedItemLabelFormat = seriesList.at(0)->itemLabelFormat();
}

AxisRenderCache &Abstract3DRenderer::axisCacheForOrientation(Q3DAbstractAxis::AxisOrientation orientation)
{
    switch (orientation) {
    case Q3DAbstractAxis::AxisOrientationX:
        return m_axisCacheX;
    case Q3DAbstractAxis::AxisOrientationY:
        return m_axisCacheY;
    case Q3DAbstractAxis::AxisOrientationZ:
        return m_axisCacheZ;
    default:
        qFatal("Abstract3DRenderer::axisCacheForOrientation");
        return m_axisCacheX;
    }
}

void Abstract3DRenderer::lowerShadowQuality()
{
    QDataVis::ShadowQuality newQuality = QDataVis::ShadowQualityNone;

    switch (m_cachedShadowQuality) {
    case QDataVis::ShadowQualityHigh:
        qWarning("Creating high quality shadows failed. Changing to medium quality.");
        newQuality = QDataVis::ShadowQualityMedium;
        break;
    case QDataVis::ShadowQualityMedium:
        qWarning("Creating medium quality shadows failed. Changing to low quality.");
        newQuality = QDataVis::ShadowQualityLow;
        break;
    case QDataVis::ShadowQualityLow:
        qWarning("Creating low quality shadows failed. Switching shadows off.");
        newQuality = QDataVis::ShadowQualityNone;
        break;
    case QDataVis::ShadowQualitySoftHigh:
        qWarning("Creating soft high quality shadows failed. Changing to soft medium quality.");
        newQuality = QDataVis::ShadowQualitySoftMedium;
        break;
    case QDataVis::ShadowQualitySoftMedium:
        qWarning("Creating soft medium quality shadows failed. Changing to soft low quality.");
        newQuality = QDataVis::ShadowQualitySoftLow;
        break;
    case QDataVis::ShadowQualitySoftLow:
        qWarning("Creating soft low quality shadows failed. Switching shadows off.");
        newQuality = QDataVis::ShadowQualityNone;
        break;
    default:
        // You'll never get here
        break;
    }

    emit requestShadowQuality(newQuality);
    updateShadowQuality(newQuality);
}

void Abstract3DRenderer::fixGradient(QLinearGradient *gradient, GLuint *gradientTexture)
{
    // Readjust start/stop to match gradient texture size
    gradient->setStart(qreal(gradientTextureWidth), qreal(gradientTextureHeight));
    gradient->setFinalStop(0.0, 0.0);

    if (*gradientTexture) {
        m_textureHelper->deleteTexture(gradientTexture);
        *gradientTexture = 0;
    }

    *gradientTexture = m_textureHelper->createGradientTexture(*gradient);
}

QT_DATAVISUALIZATION_END_NAMESPACE
