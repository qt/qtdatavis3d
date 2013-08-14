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

#include "surface3dcontroller_p.h"
#include "surface3drenderer_p.h"
#include "camerahelper_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "surfaceobject_p.h"
#include "texturehelper_p.h"
#include "theme_p.h"
#include "utils_p.h"
#include "drawer_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <qmath.h>

#include <QLinearGradient>
#include <QPainter>

#include <QDebug>

static const int ID_TO_RGBA_MASK = 0xff;

QT_DATAVIS3D_BEGIN_NAMESPACE

Surface3dRenderer::Surface3dRenderer(Surface3dController *controller)
    : QObject(controller),
      m_controller(controller),
      m_mousePressed(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_isGridEnabled(true),
      m_isBackgroundEnabled(true),
      m_shadowQuality(QDataVis::ShadowLow),
      m_labelTransparency(QDataVis::TransparencyFromTheme),
      m_font(QFont(QStringLiteral("Arial"))),
      m_hasNegativeValues(false),
      m_segmentYCount(0),
      m_segmentYStep(0.0f),
      m_segmentXCount(0),
      m_segmentZCount(0),
      m_backgroundShader(0),
      m_surfaceShader(0),
      m_surfaceGridShader(0),
      m_selectionShader(0),
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
      m_surfaceObj(0),
      m_depthTexture(0),
      m_depthFrameBuffer(0),
      m_selectionFrameBuffer(0),
      m_selectionDepthBuffer(0),
      m_gradientTexture(0),
      m_selectionTexture(0),
      m_selectionResultTexture(0),
      m_shadowQualityToShader(33.3f),
      m_querySelection(false),
      m_drawer(new Drawer(m_cachedTheme, m_font, m_labelTransparency))
{
    // Listen to changes in the controller
    QObject::connect(m_controller, &Surface3dController::smoothStatusChanged, this,
                     &Surface3dRenderer::updateSmoothStatus);
    QObject::connect(m_controller, &Surface3dController::surfaceGridChanged, this,
                     &Surface3dRenderer::updateSurfaceGridStatus);
    QObject::connect(m_controller, &Surface3dController::segmentCountChanged, this,
                     &Surface3dRenderer::updateSegmentCount);
    QObject::connect(m_controller, &Surface3dController::themeChanged, this,
                     &Surface3dRenderer::updateTheme);
    QObject::connect(m_controller, &Surface3dController::leftMousePressed, this,
                     &Surface3dRenderer::getSelection);

    m_cachedSmoothSurface =  m_controller->smoothSurface();
    updateSurfaceGridStatus(m_controller->surfaceGrid());
    updateTheme(m_controller->theme());

    initializeOpenGL();
}

Surface3dRenderer::~Surface3dRenderer()
{
    qDebug() << "Surface3dRenderer::~Surface3dRenderer()";
    m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
    m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
    m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);

    m_textureHelper->deleteTexture(&m_depthTexture);
    m_textureHelper->deleteTexture(&m_gradientTexture);
    m_textureHelper->deleteTexture(&m_selectionTexture);
    m_textureHelper->deleteTexture(&m_selectionResultTexture);

    delete m_backgroundShader;
    delete m_selectionShader;
    delete m_surfaceShader;
    delete m_surfaceGridShader;

    delete m_backgroundObj;
    delete m_surfaceObj;
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
    if (m_shadowQuality > QDataVis::ShadowNone) {
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

    initSurfaceShaders();

    // Init selection shader
    initSelectionShaders();

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

    //loadSurfaceObj();
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

    QVector3D clearColor = Utils::vectorFromColor(m_cachedTheme.m_windowColor);
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

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    //
    //  Do the surface drawing
    //

    if (m_querySelection && m_surfaceObj) {
        m_selectionShader->bind();
        glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled

        glDisable(GL_CULL_FACE);

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;

        modelMatrix.translate(0.0f, 1.0f - m_yAdjustment, zComp);
        modelMatrix.scale(QVector3D( m_xLength / m_scaleFactor,
                                    1.0f,
                                    m_zLength / m_scaleFactor));

        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

        m_selectionShader->setUniformValue(m_selectionShader->MVP(), MVPMatrix);

        // Activate texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_selectionTexture);
        m_selectionShader->setUniformValue(m_selectionShader->texture(), 0);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(m_selectionShader->posAtt());
        glBindBuffer(GL_ARRAY_BUFFER, m_surfaceObj->vertexBuf());
        glVertexAttribPointer(m_selectionShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        // 3rd attribute buffer : UVs
        glEnableVertexAttribArray(m_selectionShader->uvAtt());
        glBindBuffer(GL_ARRAY_BUFFER, m_surfaceObj->uvBuf());
        glVertexAttribPointer(m_selectionShader->uvAtt(), 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_surfaceObj->elementBuf());

        // Draw the triangles
        glDrawElements(GL_TRIANGLES, m_surfaceObj->indexCount(), m_surfaceObj->indicesType(), (void *)0);
        //m_drawer->drawObject(m_selectionShader, m_surfaceObj, m_selectionTexture, 0);

        // Free buffers
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(m_selectionShader->uvAtt());
        glDisableVertexAttribArray(m_selectionShader->posAtt());

        glEnable(GL_DITHER);

        m_querySelection = false;

        QPoint point = m_controller->mousePosition();
        GLubyte pixel[4] = {0};
        glReadPixels(point.x(), height() - point.y(), 1, 1,
                     GL_RGBA, GL_UNSIGNED_BYTE, (void *)pixel);
        //uint id = pixel[0] + pixel[1] * 256 + pixel[2] * 65536 + pixel[3] * 16777216;

        qDebug() << "pixel = " << pixel[0] << ", " << pixel[1] << ", " << pixel[2] << ", " << pixel[3];

        glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);

        // Release selection shader
        m_selectionShader->release();
    }

    if (m_surfaceObj) {
        m_surfaceShader->bind();
        // m_selectionShader->bind(); // IFDEF print selection

        // For surface we can see climpses from underneath
        glDisable(GL_CULL_FACE);

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 depthMVPMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(0.0f, 1.0f - m_yAdjustment, zComp);
        modelMatrix.scale(QVector3D( m_xLength / m_scaleFactor,
                                    1.0f,
                                    m_zLength / m_scaleFactor));
        itModelMatrix.scale(QVector3D( m_xLength / m_scaleFactor,
                                      1.0f,
                                      m_zLength / m_scaleFactor));

#ifdef SHOW_DEPTH_TEXTURE_SCENE
        MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;
#else
        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
#endif
        // TODO Check the usage?
        depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

        // Set shader bindings
        m_surfaceShader->setUniformValue(m_surfaceShader->lightP(), lightPos);
        m_surfaceShader->setUniformValue(m_surfaceShader->view(), viewMatrix);
        m_surfaceShader->setUniformValue(m_surfaceShader->model(), modelMatrix);
        m_surfaceShader->setUniformValue(m_surfaceShader->nModel(), itModelMatrix.inverted().transposed());
        m_surfaceShader->setUniformValue(m_surfaceShader->MVP(), MVPMatrix);
        //m_selectionShader->setUniformValue(m_selectionShader->MVP(), MVPMatrix); // IFDEF print selection
        m_surfaceShader->setUniformValue(m_surfaceShader->ambientS(), m_cachedTheme.m_ambientStrength);

        //IF QT_OPENGL_ES_2 TODO
        // Shadow quality etc.
        //m_backgroundShader->setUniformValue(m_backgroundShader->shadowQ(), m_shadowQualityToShader);
        //m_backgroundShader->setUniformValue(m_backgroundShader->depth(), depthMVPMatrix);
        m_surfaceShader->setUniformValue(m_surfaceShader->lightS(),
                                            m_cachedTheme.m_lightStrength * 2.0f);

        m_drawer->drawObject(m_surfaceShader, m_surfaceObj, m_gradientTexture, m_depthTexture);
        //m_drawer->drawObject(m_selectionShader, m_surfaceObj, m_selectionTexture, 0); // IFDEF print selection

        m_surfaceShader->release();
        //m_selectionShader->release(); // IFDEF print selection

        if (m_cachedSurfaceGridOn) {
            // Draw the grid over the surface
            glPolygonOffset(1.0f, 1.0f);
            glEnable(GL_POLYGON_OFFSET_FILL);

            m_surfaceGridShader->bind();

            QVector3D gridColor = Utils::vectorFromColor(QColor(Qt::white));
            // Set shader bindings
            m_surfaceGridShader->setUniformValue(m_surfaceGridShader->view(), viewMatrix);
            m_surfaceGridShader->setUniformValue(m_surfaceGridShader->model(), modelMatrix);
            m_surfaceGridShader->setUniformValue(m_surfaceGridShader->nModel(), itModelMatrix.inverted().transposed());
            m_surfaceGridShader->setUniformValue(m_surfaceGridShader->MVP(), MVPMatrix);
            m_surfaceGridShader->setUniformValue(m_surfaceGridShader->color(), gridColor);
            //m_surfaceGridShader->setUniformValue(m_surfaceGridShader->ambientS(), m_theme->m_ambientStrength);
            m_drawer->drawSurfaceGrid(m_surfaceGridShader, m_surfaceObj);

            m_surfaceGridShader->release();

            glPolygonOffset(0.0f, 0.0f);
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }

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
        modelMatrix.scale(QVector3D( m_xLength / m_scaleFactor,
                                    1.0f,
                                    m_zLength / m_scaleFactor));
        modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);
        itModelMatrix.scale(QVector3D( m_xLength / m_scaleFactor,
                                      1.0f,
                                      m_zLength / m_scaleFactor));

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
        if (m_shadowQuality > QDataVis::ShadowNone) {
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
}

void Surface3dRenderer::updateSegmentCount(GLint segmentCount, GLfloat step, GLfloat minimum)
{
    m_segmentYCount = segmentCount;
    m_segmentYStep = step;
    if (segmentCount > 0 && step > 0.0) {
        m_yRange = m_segmentYCount * m_segmentYStep;
        m_yAdjustment = 2.0f - ((m_yRange - minimum) / m_yRange); // TODO: to function
    }

    qDebug() << "m_yAdjustment = " << m_yAdjustment;
}

void Surface3dRenderer::setXZStuff(GLint segmentXCount, GLint segmentZCount)
{
    m_segmentXCount = segmentXCount;
    m_segmentZCount = segmentZCount;

    // TODO: Invent "idiotproof" max scene size formula..
    // This seems to work ok if spacing is not negative (and row/column or column/row ratio is not too high)
    m_maxSceneSize = 2 * qSqrt(segmentXCount * segmentZCount);

    calculateSceneScalingFactors();
}

void Surface3dRenderer::updateTheme(Theme theme)
{
    m_cachedTheme.setFromTheme(theme);

    // Update things depending from the theme
    updateSurfaceGradient();
}

void Surface3dRenderer::updateSurfaceGradient()
{
    QImage image(QSize(4, 100), QImage::Format_RGB32);
    QPainter pmp(&image);
    pmp.setBrush(QBrush(m_cachedTheme.m_surfaceGradient));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 4, 100);

//    QImage image(QStringLiteral("C:\\Users\\misalmel\\Work\\gerrit\\qtdatavis3d_2\\grid.png"));

    if (m_gradientTexture) {
        m_textureHelper->deleteTexture(&m_gradientTexture);
        m_gradientTexture = 0;
    }

    m_gradientTexture = m_textureHelper->create2DTexture(image, false, true);
}

void Surface3dRenderer::updateSelectionTexture()
{
    // Create the selection ID image. Each grid corner gets 2x2 pixel area of
    // ID color so that each vertex (data point) has 4x4 pixel area of ID color
    // TODO: power of two thing for ES
    int idImageWidth = (m_segmentXCount - 1) * 4;
    int idImageHeight = (m_segmentZCount - 1) * 4;
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

            idToRGBA(id + m_segmentXCount, &r, &g, &b, &a);
            fillIdCorner(&bits[p + 2 * stride], r, g, b, a, stride);

            idToRGBA(id + m_segmentXCount + 1, &r, &g, &b, &a);
            fillIdCorner(&bits[p + 2 * stride + 8], r, g, b, a, stride);

            id++;
        }
        id++;
    }

    // Use this to save the ID image to file
    //QImage image(bits, idImageWidth, idImageHeight, QImage::Format_ARGB32);
    //image.save("C:\\Users\\misalmel\\Work\\gerrit\\qtdatavis3d_2\\selection.png");

    // If old texture exists, delete it
    if (m_selectionTexture) {
        m_textureHelper->deleteTexture(&m_selectionTexture);
        m_selectionTexture = 0;
    }

    // Move the ID image (bits) to the texture
    m_selectionTexture = m_textureHelper->create2DTexture(bits, idImageWidth, idImageHeight);

    // Release the temp bits allocation
    delete bits;

    // Create the result selection texture and buffers
    if (m_selectionResultTexture) {
        m_textureHelper->deleteTexture(&m_selectionResultTexture);
        m_selectionResultTexture = 0;
    }

    m_selectionResultTexture = m_textureHelper->createSelectionTexture(m_mainViewPort.size(),
                                                                       m_selectionFrameBuffer,
                                                                       m_selectionDepthBuffer);
}

void Surface3dRenderer::fillIdCorner(uchar *p, uchar r, uchar g, uchar b, uchar a, int stride)
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

void Surface3dRenderer::idToRGBA(uint id, uchar *r, uchar *g, uchar *b, uchar *a)
{
    *r = id & ID_TO_RGBA_MASK;
    *g = (id >> 8) & ID_TO_RGBA_MASK;
    *b = (id >> 16) & ID_TO_RGBA_MASK;
    *a = (id >> 24) & ID_TO_RGBA_MASK;
}

void Surface3dRenderer::getSelection()
{
    qDebug() << "Surface3dRenderer::getSelection";
    m_querySelection = true;
}

void Surface3dRenderer::setSeries(QList<qreal> series)
{
    m_series = series;

    // TODO temp solution
    if (!m_surfaceObj)
        loadSurfaceObj();

    if (m_cachedSmoothSurface)
        m_surfaceObj->setUpSmoothData(series, m_segmentXCount, m_segmentZCount, m_yRange, true);
    else
        m_surfaceObj->setUpData(series, m_segmentXCount, m_segmentZCount, m_yRange, true);

    updateSelectionTexture();
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

    m_xLength = m_segmentXCount;
    m_zLength = m_segmentZCount;
    m_maxDimension = qMax(m_xLength, m_zLength);
    m_scaleFactor = qMin((m_segmentXCount * (m_maxDimension / m_maxSceneSize)),
                         (m_segmentZCount * (m_maxDimension / m_maxSceneSize)));
    m_scaleX = 1.0f / m_scaleFactor; // TODO: correspondance to m_barThickness
    m_scaleZ = 1.0f / m_scaleFactor; // TODO: correspondance to m_barThickness

    //qDebug() << "m_scaleX" << m_scaleX << "m_scaleFactor" << m_scaleFactor;
    //qDebug() << "m_scaleZ" << m_scaleZ << "m_scaleFactor" << m_scaleFactor;
    //qDebug() << "m_rowWidth:" << m_rowWidth << "m_columnDepth:" << m_columnDepth << "m_maxDimension:" << m_maxDimension;
}

void Surface3dRenderer::updateSmoothStatus(bool enable)
{
    m_cachedSmoothSurface = enable;

    if (!m_surfaceObj)
        return;

    if (m_cachedSmoothSurface)
        m_surfaceObj->setUpSmoothData(m_series, m_segmentXCount, m_segmentZCount, m_yRange, true);
    else
        m_surfaceObj->setUpData(m_series, m_segmentXCount, m_segmentZCount, m_yRange, true);

    initSurfaceShaders();
}

void Surface3dRenderer::updateSurfaceGridStatus(bool enable)
{
    m_cachedSurfaceGridOn = enable;
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

void Surface3dRenderer::loadSurfaceObj()
{
    if (!m_isInitialized)
        return;

    if (m_surfaceObj)
        delete m_surfaceObj;
    m_surfaceObj = new SurfaceObject();
    //m_surfaceObj->setUpData();
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
    if (m_shadowQuality > QDataVis::ShadowNone && !m_mainViewPort.size().isEmpty()) {
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

void Surface3dRenderer::initSelectionShaders()
{
    if (m_selectionShader)
        delete m_selectionShader;
    m_selectionShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexLabel"),
                                         QStringLiteral(":/shaders/fragmentLabel"));
    m_selectionShader->initialize();
}

void Surface3dRenderer::initSurfaceShaders()
{
    if (m_surfaceShader)
        delete m_surfaceShader;
    if (m_cachedSmoothSurface) {
        m_surfaceShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexSurface"),
                                           QStringLiteral(":/shaders/fragmentSurface"));
    } else {
        m_surfaceShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexSurfaceFlat"),
                                           QStringLiteral(":/shaders/fragmentSurfaceFlat"));
    }
    m_surfaceShader->initialize();

    if (m_surfaceGridShader)
        delete m_surfaceGridShader;
    m_surfaceGridShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexSurfaceGrid"),
                                           QStringLiteral(":/shaders/fragmentSurfaceGrid"));
    m_surfaceGridShader->initialize();
}

QT_DATAVIS3D_END_NAMESPACE


//p = 90;
//qDebug() << "rgba = " << bits[p + 0] << ", " << bits[p + 1] << ", " <<
//            bits[p + 2] << ", " << bits[p + 3];
//p += 4;
//qDebug() << "rgba = " << bits[p + 0] << ", " << bits[p + 1] << ", " <<
//            bits[p + 2] << ", " << bits[p + 3];
//p += 4;
//qDebug() << "rgba = " << bits[p + 0] << ", " << bits[p + 1] << ", " <<
//            bits[p + 2] << ", " << bits[p + 3];
//p += 4;
//qDebug() << "rgba = " << bits[p + 0] << ", " << bits[p + 1] << ", " <<
//            bits[p + 2] << ", " << bits[p + 3];
//p += 4;
//qDebug() << "rgba = " << bits[p + 0] << ", " << bits[p + 1] << ", " <<
//            bits[p + 2] << ", " << bits[p + 3];
//p += 4;

