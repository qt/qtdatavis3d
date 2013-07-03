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

#include "surface3dcontroller_p.h"
#include "surface3drenderer_p.h"
#include "camerahelper_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"
#include "theme_p.h"
#include "utils_p.h"
#include "drawer_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <qmath.h>

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

Surface3dRenderer::Surface3dRenderer(Surface3dController *controller)
    : QObject(controller),
      m_controller(controller),
      m_mousePressed(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_theme(new Theme()),
      m_isGridEnabled(true),
      m_isBackgroundEnabled(true),
      m_shadowQuality(ShadowLow),
      m_labelTransparency(TransparencyFromTheme),
      m_font(QFont(QStringLiteral("Arial"))),
      m_hasNegativeValues(false),
      m_tickYCount(0),
      m_tickYStep(0.0f),
      m_tickXCount(0),
      m_tickZCount(0),
      m_backgroundShader(0),
      m_isInitialized(false),
      m_yRange(0.0f), // m_heightNormalizer
      m_yAdjustment(0.0f),
      m_xLength(0.0f),
      m_zLength(0.0f),
      m_maxDimension(0.0f),
      m_scaleFactor(0.0f),
      m_scaleX(0.0f),
      m_scaleZ(0.0f),
      m_maxSceneSize(40.0),
      m_backgroundObj(0),
      m_gridLineObj(0),
      m_depthTexture(0),
      m_depthFrameBuffer(0),
      m_shadowQualityToShader(33.3f),
      m_drawer(new Drawer(*m_theme, m_font, m_labelTransparency))
{
    initializeOpenGL();
}

Surface3dRenderer::~Surface3dRenderer()
{
    m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);

    if (m_backgroundShader)
        delete m_backgroundShader;

    if (m_depthTexture)
        m_textureHelper->deleteTexture(&m_depthTexture);

    delete m_backgroundObj;
    delete m_textureHelper;
    delete m_drawer;
}

void Surface3dRenderer::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (m_isInitialized)
        return;

    initializeOpenGLFunctions();

    m_textureHelper = new TextureHelper();
    m_drawer->initializeOpenGL();

    // Initialize shaders
#if !defined(QT_OPENGL_ES_2)
    if (m_shadowQuality > ShadowNone) {
        initBackgroundShaders(QStringLiteral(":/shaders/vertexShadow"),
                              QStringLiteral(":/shaders/fragmentShadowNoTex"));
    } else {
        initBackgroundShaders(QStringLiteral(":/shaders/vertex"),
                              QStringLiteral(":/shaders/fragment"));
    }
#else
    initBackgroundShaders(QStringLiteral(":/shaders/vertexES2"),
                          QStringLiteral(":/shaders/fragmentES2"));
#endif

    // Init selection shader
    //initSelectionShader();

    // Load grid line mesh
    loadGridLineMesh();

    // Load label mesh
    //loadLabelMesh();

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
    glViewport(m_sliceViewPort.x(), m_sliceViewPort.y(),
               m_sliceViewPort.width(), m_sliceViewPort.height());

    // Set initialized -flag
    m_isInitialized = true;

    // Resize in case we've missed resize events
    // Resize calls initSelectionBuffer and initDepthBuffer, so they don't need to be called here
    handleResize();

    // Load background mesh (we need to be initialized first)
    loadBackgroundMesh();
}

void Surface3dRenderer::render(CameraHelper *camera, const GLuint defaultFboHandle)
{
    if (!m_isInitialized)
        return;

    if (defaultFboHandle) {
        glDepthMask(true);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    QVector3D clearColor = Utils::vectorFromColor(m_theme->m_windowColor);
    glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: bars have m_hasHeightAdjustmentChanged, which is always true!
    // Set initial camera position
    // X must be 0 for rotation to work - we can use "setCameraRotation" for setting it later
    camera->setDefaultCameraOrientation(QVector3D(0.0f, 0.0f, 6.0f + zComp),
                                        QVector3D(0.0f, 0.0f, zComp),
                                        QVector3D(0.0f, 1.0f, 0.0f));

    drawScene(camera, defaultFboHandle);
}

void Surface3dRenderer::drawScene(CameraHelper *camera, const GLuint defaultFboHandle)
{
    Q_UNUSED(defaultFboHandle)

    //qDebug() << "Surface3dRenderer::drawScene";

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
                100.0f, //TODO: m_zoomLevel * m_autoScaleAdjustment
                m_mainViewPort.width(),
                m_mainViewPort.height(),
                m_hasNegativeValues);

    // calculate background rotation based on view matrix rotation
    if (viewMatrix.row(0).x() > 0 && viewMatrix.row(0).z() <= 0)
        backgroundRotation = 270.0f;
    else if (viewMatrix.row(0).x() > 0 && viewMatrix.row(0).z() > 0)
        backgroundRotation = 180.0f;
    else if (viewMatrix.row(0).x() <= 0 && viewMatrix.row(0).z() > 0)
        backgroundRotation = 90.0f;
    else if (viewMatrix.row(0).x() <= 0 && viewMatrix.row(0).z() <= 0)
        backgroundRotation = 0.0f;

    QVector3D lightPos = camera->calculateLightPosition(defaultLightPos);

    QMatrix4x4 depthViewMatrix;
    QMatrix4x4 depthProjectionMatrix;
    depthProjectionMatrix = projectionMatrix; // TODO
    depthViewMatrix.lookAt(lightPos, QVector3D(0.0f, -m_yAdjustment, zComp),
                           QVector3D(0.0f, 1.0f, 0.0f)); // TODO: Move

    //
    //  Do the surface drawing
    //

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    // Bind background shader
    m_backgroundShader->bind();

    if (m_hasNegativeValues)
        glDisable(GL_CULL_FACE);
    else
        glCullFace(GL_BACK);

    // Draw background
    if (m_isBackgroundEnabled && m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 depthMVPMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(0.0f, 1.0f - m_yAdjustment, zComp);
        modelMatrix.scale(QVector3D( 9.6f / 4.057f /*m_xLength / m_scaleFactor*/,
                                    1.0f,
                                    7.2f / 4.057f /*m_zLength / m_scaleFactor*/));
        modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);
        itModelMatrix.scale(QVector3D( 9.6f / 4.057f /*m_xLength / m_scaleFactor*/,
                                      1.0f,
                                      7.2f / 4.057f /*m_zLength / m_scaleFactor*/));

#ifdef SHOW_DEPTH_TEXTURE_SCENE
        MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;
#else
        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
#endif
        depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

        QVector3D backgroundColor = Utils::vectorFromColor(m_theme->m_backgroundColor);

        // Set shader bindings
        m_backgroundShader->setUniformValue(m_backgroundShader->lightP(), lightPos);
        m_backgroundShader->setUniformValue(m_backgroundShader->view(), viewMatrix);
        m_backgroundShader->setUniformValue(m_backgroundShader->model(), modelMatrix);
        m_backgroundShader->setUniformValue(m_backgroundShader->nModel(),
                                            itModelMatrix.inverted().transposed());
        m_backgroundShader->setUniformValue(m_backgroundShader->MVP(), MVPMatrix);
        m_backgroundShader->setUniformValue(m_backgroundShader->color(), backgroundColor);
        m_backgroundShader->setUniformValue(m_backgroundShader->ambientS(),
                                            m_theme->m_ambientStrength * 2.0f);

#if !defined(QT_OPENGL_ES_2)
        if (m_shadowQuality > ShadowNone) {
            // Set shadow shader bindings
            m_backgroundShader->setUniformValue(m_backgroundShader->shadowQ(),
                                                m_shadowQualityToShader);
            m_backgroundShader->setUniformValue(m_backgroundShader->depth(), depthMVPMatrix);
            m_backgroundShader->setUniformValue(m_backgroundShader->lightS(),
                                                m_theme->m_lightStrength / 10.0f);

            // Draw the object
            m_drawer->drawObject(m_backgroundShader, m_backgroundObj, 0, m_depthTexture);
        } else
#endif
        {
            // Set shadowless shader bindings
            m_backgroundShader->setUniformValue(m_backgroundShader->lightS(),
                                                m_theme->m_lightStrength);

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
}

void Surface3dRenderer::setYRangeStuff(GLint tickCount, GLfloat step, GLfloat minimum)
{
    m_tickYCount = tickCount;
    m_tickYStep = step;
    if (tickCount > 0 && step > 0.0) {
        m_yRange = m_tickYCount * m_tickYStep;
        m_yAdjustment = 2.0f - ((m_yRange - minimum) / m_yRange); // TODO: to function
    }

    qDebug() << "m_yAdjustment = " << m_yAdjustment;
}

void Surface3dRenderer::setXZStuff(GLint tickXCount, GLint tickZCount)
{
    m_tickXCount = tickXCount;
    m_tickZCount = tickZCount;

    // TODO: Invent "idiotproof" max scene size formula..
    // This seems to work ok if spacing is not negative (and row/column or column/row ratio is not too high)
    m_maxSceneSize = 2 * qSqrt(tickXCount * tickZCount);

    calculateSceneScalingFactors();
}

void Surface3dRenderer::calculateSceneScalingFactors()
{
    // Calculate scene scaling and translation factors
//    m_rowWidth = ((m_columnCount + 1) * m_barSpacing.width()) / 2.0f;
//    m_columnDepth = ((m_rowCount + 1) * m_barSpacing.height()) / 2.0f;
//    m_maxDimension = qMax(m_rowWidth, m_columnDepth);
//    m_scaleFactor = qMin((m_columnCount * (m_maxDimension / m_maxSceneSize)),
//                         (m_rowCount * (m_maxDimension / m_maxSceneSize)));
//    m_scaleX = m_barThickness.width() / m_scaleFactor;
//    m_scaleZ = m_barThickness.height() / m_scaleFactor;

    m_xLength = m_tickXCount;
    m_zLength = m_tickZCount;
    m_maxDimension = qMax(m_xLength, m_zLength);
    m_scaleFactor = qMin((m_tickXCount * (m_maxDimension / m_maxSceneSize)),
                         (m_tickZCount * (m_maxDimension / m_maxSceneSize)));
    m_scaleX = 1.0f / m_scaleFactor; // TODO: correspondance to m_barThickness
    m_scaleZ = 1.0f / m_scaleFactor; // TODO: correspondance to m_barThickness

    //qDebug() << "m_scaleX" << m_scaleX << "m_scaleFactor" << m_scaleFactor;
    //qDebug() << "m_scaleZ" << m_scaleZ << "m_scaleFactor" << m_scaleFactor;
    //qDebug() << "m_rowWidth:" << m_rowWidth << "m_columnDepth:" << m_columnDepth << "m_maxDimension:" << m_maxDimension;
}

void Surface3dRenderer::loadBackgroundMesh()
{
    if (!m_isInitialized)
        return;

    if (m_backgroundObj)
        delete m_backgroundObj;
    if (m_hasNegativeValues)
        m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/negativeBackground"));
    else
        m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/background"));
    m_backgroundObj->load();
}

void Surface3dRenderer::loadGridLineMesh()
{
    if (m_gridLineObj)
        delete m_gridLineObj;
    m_gridLineObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/bar"));
    m_gridLineObj->load();
}

const QSize Surface3dRenderer::size()
{
    return m_boundingRect.size();
}

const QRect Surface3dRenderer::boundingRect()
{
    return m_boundingRect;
}

void Surface3dRenderer::setBoundingRect(const QRect boundingRect)
{
    m_boundingRect = boundingRect;
    handleResize();
}

void Surface3dRenderer::setWidth(const int width)
{
    m_boundingRect.setWidth(width);
    handleResize();
}

int Surface3dRenderer::width()
{
    return m_boundingRect.width();
}

void Surface3dRenderer::setHeight(const int height)
{
    m_boundingRect.setHeight(height);
    handleResize();
}

int Surface3dRenderer::height()
{
    return m_boundingRect.height();
}

void Surface3dRenderer::setX(const int x)
{
    m_boundingRect.setX(x);
}

int Surface3dRenderer::x()
{
    return m_boundingRect.x();
}

void Surface3dRenderer::setY(const int y)
{
    m_boundingRect.setY(y);
}

int Surface3dRenderer::y()
{
    return m_boundingRect.y();
}

#if defined(Q_OS_ANDROID)
void Surface3dRenderer::mouseDoubleClickEvent(QMouseEvent *event)
{

}

void Surface3dRenderer::touchEvent(QTouchEvent *event)
{

}
#endif
void Surface3dRenderer::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event)
    Q_UNUSED(mousePos)
}

void Surface3dRenderer::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event)
    Q_UNUSED(mousePos)
}

void Surface3dRenderer::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event)
    Q_UNUSED(mousePos)
}

void Surface3dRenderer::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
}

void Surface3dRenderer::handleResize()
{
    if (!m_isInitialized)
        return;

    qDebug() << "Surface3dRenderer::handleResize " << width() << "x" << height();

    m_mainViewPort = QRect(0, 0, width(), height());
    m_sliceViewPort = QRect(0, 0, width(), height());

#if !defined(QT_OPENGL_ES_2)
    // Re-init depth buffer
    updateDepthBuffer();
#endif
}

#if !defined(QT_OPENGL_ES_2)
void Surface3dRenderer::updateDepthBuffer()
{
    if (m_depthTexture) {
        m_textureHelper->deleteTexture(&m_depthTexture);
        m_depthTexture = 0;
    }

    // TODO: bars uses some m_cachedShadowQuality
    if (m_shadowQuality > ShadowNone && !m_mainViewPort.size().isEmpty()) {
        m_depthTexture = m_textureHelper->createDepthTexture(m_mainViewPort.size(),
                                                             m_depthFrameBuffer,
                                                             m_shadowQuality);
        if (!m_depthTexture) {
            qDebug() << "Failed to create m_depthTexture";
//            switch (m_shadowQuality) {
//            case ShadowHigh:
//                qWarning("Creating high quality shadows failed. Changing to medium quality.");
//                (void)setShadowQuality(ShadowMedium);
//                break;
//            case ShadowMedium:
//                qWarning("Creating medium quality shadows failed. Changing to low quality.");
//                (void)setShadowQuality(ShadowLow);
//                break;
//            case ShadowLow:
//                qWarning("Creating low quality shadows failed. Switching shadows off.");
//                (void)setShadowQuality(ShadowNone);
//                break;
//            default:
//                // You'll never get here
//                break;
//            }
        }
    }
}
#endif

void Surface3dRenderer::initBackgroundShaders(const QString &vertexShader,
                                           const QString &fragmentShader)
{
    if (m_backgroundShader)
        delete m_backgroundShader;
    m_backgroundShader = new ShaderHelper(this, vertexShader, fragmentShader);
    m_backgroundShader->initialize();
}

QT_DATAVIS3D_END_NAMESPACE
