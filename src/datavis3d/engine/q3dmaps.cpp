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

#include "q3dmaps.h"
#include "q3dmaps_p.h"
#include "camerahelper_p.h"
#include "qdataitem_p.h"
#include "qdatarow_p.h"
#include "qdataset_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"
#include "theme_p.h"
#include "utils_p.h"
#include "drawer_p.h"

#include <QMatrix4x4>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QScreen>
#include <QMouseEvent>

#include <qmath.h>

#include <QDebug>

//#define DISPLAY_RENDER_SPEED

// Uncommenting this draws the shadow map with wider FOV than scene itself, making the light
// seem to be closer to scene than it actually is. This way shadows look slightly better (to me anyway)
#define USE_WIDER_SHADOWS

// You can verify that depth buffer drawing works correctly by uncommenting this.
// You should see the scene from  where the light is
//#define SHOW_DEPTH_TEXTURE_SCENE

#ifdef DISPLAY_RENDER_SPEED
#include <QTime>
#endif

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

//#define DISPLAY_FULL_DATA_ON_SELECTION // Append selection value text with row and column labels

const GLfloat gridLineWidth = 0.005f;
GLfloat distanceMod = 0.0f;
static QVector3D skipColor = QVector3D(255, 255, 255); // Selection texture's background color

Q3DMaps::Q3DMaps()
    : d_ptr(new Q3DMapsPrivate(this))
{
}

Q3DMaps::~Q3DMaps()
{
}

void Q3DMaps::initialize()
{
    // Initialize shaders
    if (d_ptr->m_shadowQuality > ShadowNone) {
        if (!d_ptr->m_theme->m_uniformColor) {
            d_ptr->initShaders(QStringLiteral(":/shaders/vertexShadow"),
                               QStringLiteral(":/shaders/fragmentShadowNoTexColorOnY"));
        } else {
            d_ptr->initShaders(QStringLiteral(":/shaders/vertexShadow"),
                               QStringLiteral(":/shaders/fragmentShadowNoTex"));
        }
        d_ptr->initBackgroundShaders(QStringLiteral(":/shaders/vertexShadow"),
                                     QStringLiteral(":/shaders/fragmentShadow"));
        // Init the depth buffer (for shadows)
        d_ptr->initDepthBuffer();
    } else {
        if (!d_ptr->m_theme->m_uniformColor) {
            d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                               QStringLiteral(":/shaders/fragmentColorOnY"));
        } else {
            d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                               QStringLiteral(":/shaders/fragment"));
        }
        d_ptr->initBackgroundShaders(QStringLiteral(":/shaders/vertexTexture"),
                                     QStringLiteral(":/shaders/fragmentTexture"));
    }
    d_ptr->initLabelShaders(QStringLiteral(":/shaders/vertexLabel"),
                            QStringLiteral(":/shaders/fragmentLabel"));

    // Init depth shader (for shadows). Init in any case, easier to handle shadow activation if done via api.
    d_ptr->initDepthShader();

    // Init selection shader
    d_ptr->initSelectionShader();

    // Init the selection buffer
    d_ptr->initSelectionBuffer();

    // Load default mesh
    d_ptr->loadBarMesh();

    // Load background mesh
    d_ptr->loadBackgroundMesh();

    // Load grid line mesh
    d_ptr->loadGridLineMesh();

    // Load label mesh
    d_ptr->loadLabelMesh();

    // Set OpenGL features
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    // Set initial camera position
    // X must be 0 for rotation to work - we can use "setCameraRotation" for setting it later
    CameraHelper::setDefaultCameraOrientation(QVector3D(0.0f, 0.0f, 1.0f + 2.9f * zComp),
                                              QVector3D(0.0f, 0.0f, zComp),
                                              QVector3D(0.0f, 1.0f, 0.0f));

    // Adjust to default rotation
    setCameraPosition(d_ptr->m_horizontalRotation, d_ptr->m_verticalRotation, d_ptr->m_zoomLevel);

    // Set view port
    glViewport(0, 0, width(), height());

    // Set initialized -flag
    d_ptr->m_isInitialized = true;

    // Update default light position
#ifndef USE_WIDER_SHADOWS
    distanceMod = 1.0f;
#endif
}

void Q3DMaps::render()
{
    if (!d_ptr->m_isInitialized)
        return;

#ifdef DISPLAY_RENDER_SPEED
    // For speed computation
    static bool firstRender = true;
    static QTime lastTime;
    static GLint nbFrames = 0;
    if (firstRender) {
        lastTime.start();
        firstRender = false;
    }

    // Measure speed (as milliseconds per frame)
    nbFrames++;
    if (lastTime.elapsed() >= 1000) { // print only if last measurement was more than 1s ago
        qDebug() << qreal(lastTime.elapsed()) / qreal(nbFrames) << "ms/frame (=" << qreal(nbFrames) << "fps)";
        nbFrames = 0;
        lastTime.restart();
    }
#endif

    // Draw scene
    drawScene();
}

void Q3DMaps::drawScene()
{
    // Set clear color
    QVector3D clearColor = Utils::vectorFromColor(d_ptr->m_theme->m_windowColor);
    glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static QVector3D selection = skipColor;

    // Specify viewport
    glViewport(d_ptr->m_sceneViewPort.x(), d_ptr->m_sceneViewPort.y(),
               d_ptr->m_sceneViewPort.width(), d_ptr->m_sceneViewPort.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, (GLfloat)d_ptr->m_sceneViewPort.width()
                                 / (GLfloat)d_ptr->m_sceneViewPort.height(), 0.1f, 100.0f);

    // Calculate view matrix
    QMatrix4x4 viewMatrix = CameraHelper::calculateViewMatrix(d_ptr->m_mousePos,
                                                              d_ptr->m_zoomLevel
                                                              * d_ptr->m_zoomAdjustment,
                                                              d_ptr->m_sceneViewPort.width(),
                                                              d_ptr->m_sceneViewPort.height());

    // Get light position (rotate light with camera, a bit above it (as set in defaultLightPos))
    QVector3D lightPos = CameraHelper::calculateLightPosition(defaultLightPos, 0.0f, distanceMod);

    // Map adjustment direction to model matrix scaling
    GLfloat heightMultiplier = 0.0f;
    GLfloat widthMultiplier = 0.0f;
    GLfloat depthMultiplier = 0.0f;
    GLfloat heightScaler = 0.0f;
    GLfloat widthScaler = 0.0f;
    GLfloat depthScaler = 0.0f;
    switch (d_ptr->m_adjustDirection) {
    case AdjustHeight:
        widthMultiplier = 0.0f;
        heightMultiplier = 1.0f;
        depthMultiplier = 0.0f;
        widthScaler = d_ptr->m_barThickness.x() / d_ptr->m_scaleFactor;
        heightScaler = 0.0f;
        depthScaler = d_ptr->m_barThickness.z() / d_ptr->m_scaleFactor;
        break;
    case AdjustWidth:
        widthMultiplier = 1.0f;
        heightMultiplier = 0.0f;
        depthMultiplier = 0.0f;
        widthScaler = 0.0f;
        heightScaler = d_ptr->m_barThickness.y() / d_ptr->m_scaleFactor;
        depthScaler = d_ptr->m_barThickness.z() / d_ptr->m_scaleFactor;
        break;
    case AdjustDepth:
        widthMultiplier = 0.0f;
        heightMultiplier = 0.0f;
        depthMultiplier = 1.0f;
        widthScaler = d_ptr->m_barThickness.x() / d_ptr->m_scaleFactor;
        heightScaler = d_ptr->m_barThickness.y() / d_ptr->m_scaleFactor;
        depthScaler = 0.0f;
        break;
    case AdjustRadius:
        widthMultiplier = 1.0f;
        heightMultiplier = 0.0f;
        depthMultiplier = 1.0f;
        widthScaler = 0.0f;
        heightScaler = d_ptr->m_barThickness.y() / d_ptr->m_scaleFactor;
        depthScaler = 0.0f;
        break;
    case AdjustAll:
        widthMultiplier = 1.0f;
        heightMultiplier = 1.0f;
        depthMultiplier = 1.0f;
        widthScaler = 0.0f;
        heightScaler = 0.0f;
        depthScaler = 0.0f;
        break;
    }

    // Introduce regardless of shadow quality to simplify logic
    QMatrix4x4 depthViewMatrix;
    QMatrix4x4 depthProjectionMatrix;

    if (d_ptr->m_shadowQuality > ShadowNone) {
        // Render scene into a depth texture for using with shadow mapping
        // Bind depth shader
        d_ptr->m_depthShader->bind();

        // Set viewport for depth map rendering. Must match texture size. Larger values give smoother shadows.
        glViewport(d_ptr->m_sceneViewPort.x(), d_ptr->m_sceneViewPort.y(),
                   d_ptr->m_sceneViewPort.width() * d_ptr->m_shadowQuality,
                   d_ptr->m_sceneViewPort.height() * d_ptr->m_shadowQuality);

        // Enable drawing to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, d_ptr->m_depthFrameBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Set front face culling to reduce self-shadowing issues
        glCullFace(GL_FRONT);

        // Get the depth view matrix
        // It may be possible to hack lightPos here if we want to make some tweaks to shadow
        depthViewMatrix.lookAt(lightPos, QVector3D(0.0f, -d_ptr->m_yAdjustment, zComp),
                               QVector3D(0.0f, 1.0f, 0.0f));
        // TODO: Why does depthViewMatrix.column(3).y() goes to zero when we're directly above? That causes the scene to be not drawn from above -> must be fixed
        //qDebug() << lightPos << depthViewMatrix << depthViewMatrix.column(3);
        // Set the depth projection matrix
#ifdef USE_WIDER_SHADOWS
        // Use this for a bit exaggerated shadows
        depthProjectionMatrix.perspective(60.0f, (GLfloat)d_ptr->m_sceneViewPort.width()
                                          / (GLfloat)d_ptr->m_sceneViewPort.height(), 3.0f, 100.0f);
#else
        // Use these for normal shadows, with the light further away
        depthProjectionMatrix = projectionMatrix;
#endif
#if 0
        // Draw background to depth buffer (You don't want to do this, except maybe for debugging purposes)
        if (d_ptr->m_backgroundObj) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;

            modelMatrix.translate(0.0f, -d_ptr->m_yAdjustment, zComp);
            modelMatrix.scale(QVector3D(d_ptr->m_areaSize.width() / d_ptr->m_scaleFactor,
                                        1.0f,
                                        d_ptr->m_areaSize.height() / d_ptr->m_scaleFactor));
            modelMatrix.rotate(-90.0f, 1.0f, 0.0f, 0.0f);

            MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            d_ptr->m_depthShader->setUniformValue(d_ptr->m_depthShader->MVP(), MVPMatrix);

            // 1st attribute buffer : vertices
            glEnableVertexAttribArray(d_ptr->m_depthShader->posAtt());
            glBindBuffer(GL_ARRAY_BUFFER, d_ptr->m_backgroundObj->vertexBuf());
            glVertexAttribPointer(d_ptr->m_depthShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_ptr->m_backgroundObj->elementBuf());

            // Draw the triangles
            glDrawElements(GL_TRIANGLES, d_ptr->m_backgroundObj->indexCount(), GL_UNSIGNED_SHORT,
                           (void *)0);

            // Free buffers
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDisableVertexAttribArray(d_ptr->m_depthShader->posAtt());
        }
#endif
        // Draw bars to depth buffer
        for (int bar = 0; bar < d_ptr->m_data->d_ptr->row().size(); bar++) {
            QDataItem *item = d_ptr->m_data->d_ptr->getItem(bar);
            if (!item)
                continue;

            GLfloat barHeight = item->d_ptr->value() / d_ptr->m_heightNormalizer;

            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;

            modelMatrix.translate(item->d_ptr->translation().x(),
                                  heightMultiplier * barHeight + heightScaler
                                  - d_ptr->m_yAdjustment,
                                  item->d_ptr->translation().z());
            modelMatrix.scale(QVector3D(widthMultiplier * barHeight + widthScaler,
                                        heightMultiplier * barHeight + heightScaler,
                                        depthMultiplier * barHeight + depthScaler));

            MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            d_ptr->m_depthShader->setUniformValue(d_ptr->m_depthShader->MVP(), MVPMatrix);

            // 1st attribute buffer : vertices
            glEnableVertexAttribArray(d_ptr->m_depthShader->posAtt());
            glBindBuffer(GL_ARRAY_BUFFER, d_ptr->m_barObj->vertexBuf());
            glVertexAttribPointer(d_ptr->m_depthShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_ptr->m_barObj->elementBuf());

            // Draw the triangles
            glDrawElements(GL_TRIANGLES, d_ptr->m_barObj->indexCount(), GL_UNSIGNED_SHORT, (void *)0);

            // Free buffers
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDisableVertexAttribArray(d_ptr->m_depthShader->posAtt());
        }

        // Disable drawing to framebuffer (= enable drawing to screen)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Reset culling to normal
        glCullFace(GL_BACK);

        // Release depth shader
        d_ptr->m_depthShader->release();

        // Revert to original viewport
        glViewport(d_ptr->m_sceneViewPort.x(), d_ptr->m_sceneViewPort.y(),
                   d_ptr->m_sceneViewPort.width(), d_ptr->m_sceneViewPort.height());

#if 0 // Use this if you want to see what is being drawn to the framebuffer
        // You'll also have to comment out GL_COMPARE_R_TO_TEXTURE -line in texturehelper (if using it)
        d_ptr->m_labelShader->bind();
        glEnable(GL_TEXTURE_2D);
        QMatrix4x4 modelMatrix;
        QMatrix4x4 viewmatrix;
        viewmatrix.lookAt(QVector3D(0.0f, 0.0f, 2.5f + zComp),
                          QVector3D(0.0f, 0.0f, zComp),
                          QVector3D(0.0f, 1.0f, 0.0f));
        modelMatrix.translate(0.0, 0.0, zComp);
        QMatrix4x4 MVPMatrix = projectionMatrix * viewmatrix * modelMatrix;
        d_ptr->m_labelShader->setUniformValue(d_ptr->m_labelShader->MVP(), MVPMatrix);
        d_ptr->m_drawer->drawObject(d_ptr->m_labelShader, d_ptr->m_labelObj,
                                    d_ptr->m_depthTexture);
        glDisable(GL_TEXTURE_2D);
        d_ptr->m_labelShader->release();
#endif
    }
#if 1
    // Skip selection mode drawing if we're zoomed or have no selection mode
    if (!d_ptr->m_zoomActivated && d_ptr->m_selectionMode > ModeNone) {
        // Bind selection shader
        d_ptr->m_selectionShader->bind();

        // Draw bars to selection buffer
        glBindFramebuffer(GL_FRAMEBUFFER, d_ptr->m_selectionFrameBuffer);
        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(skipColor.x() / 255, skipColor.y() / 255, skipColor.z() / 255, 1.0f); // Set clear color to white (= skipColor)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled
        GLint barIdxRed = 0;
        GLint barIdxGreen = 0;
        GLint barIdxBlue = 0;
        for (int bar = 0; bar < d_ptr->m_data->d_ptr->row().size(); bar++, barIdxRed++) {
            QDataItem *item = d_ptr->m_data->d_ptr->getItem(bar);
            if (!item)
                continue;

            GLfloat barHeight = item->d_ptr->value() / d_ptr->m_heightNormalizer;

            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;

            modelMatrix.translate(item->d_ptr->translation().x(),
                                  heightMultiplier * barHeight + heightScaler
                                  - d_ptr->m_yAdjustment,
                                  item->d_ptr->translation().z());
            modelMatrix.scale(QVector3D(widthMultiplier * barHeight + widthScaler,
                                        heightMultiplier * barHeight + heightScaler,
                                        depthMultiplier * barHeight + depthScaler));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

            if (barIdxRed > 0 && barIdxRed % 256 == 0) {
                barIdxRed = 0;
                barIdxGreen++;
            }
            if (barIdxGreen > 0 && barIdxGreen % 256 == 0) {
                barIdxGreen = 0;
                barIdxBlue++;
            }
            if (barIdxBlue > 255)
                qFatal("Too many objects");

            QVector3D barColor = QVector3D((GLfloat)barIdxRed / 255.0f,
                                           (GLfloat)barIdxGreen / 255.0f,
                                           (GLfloat)barIdxBlue / 255.0f);

            d_ptr->m_selectionShader->setUniformValue(d_ptr->m_selectionShader->MVP(),
                                                      MVPMatrix);
            d_ptr->m_selectionShader->setUniformValue(d_ptr->m_selectionShader->color(),
                                                      barColor);

            // 1st attribute buffer : vertices
            glEnableVertexAttribArray(d_ptr->m_selectionShader->posAtt());
            glBindBuffer(GL_ARRAY_BUFFER, d_ptr->m_barObj->vertexBuf());
            glVertexAttribPointer(d_ptr->m_selectionShader->posAtt(),
                                  3, GL_FLOAT, GL_FALSE, 0, (void *)0);

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_ptr->m_barObj->elementBuf());

            // Draw the triangles
            glDrawElements(GL_TRIANGLES, d_ptr->m_barObj->indexCount(), GL_UNSIGNED_SHORT,
                           (void *)0);

            // Free buffers
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDisableVertexAttribArray(d_ptr->m_selectionShader->posAtt());
        }
        glEnable(GL_DITHER);

        // Read color under cursor
        if (Q3DMapsPrivate::MouseOnScene == d_ptr->m_mousePressed)
            selection = Utils::getSelection(d_ptr->m_mousePos, height());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Release selection shader
        d_ptr->m_selectionShader->release();

#if 0 // Use this if you want to see what is being drawn to the framebuffer
        d_ptr->m_labelShader->bind();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        QMatrix4x4 modelMatrix;
        QMatrix4x4 viewmatrix;
        viewmatrix.lookAt(QVector3D(0.0f, 0.0f, 2.0f + zComp),
                          QVector3D(0.0f, 0.0f, zComp),
                          QVector3D(0.0f, 1.0f, 0.0f));
        modelMatrix.translate(0.0, 0.0, zComp);
        QMatrix4x4 MVPMatrix = projectionMatrix * viewmatrix * modelMatrix;
        d_ptr->m_labelShader->setUniformValue(d_ptr->m_labelShader->MVP(), MVPMatrix);
        d_ptr->m_drawer->drawObject(d_ptr->m_labelShader, d_ptr->m_labelObj,
                                    d_ptr->m_selectionTexture);
        glDisable(GL_TEXTURE_2D);
        d_ptr->m_labelShader->release();
#endif
    }
#if 1
    // Bind bar shader
    d_ptr->m_barShader->bind();

    // Enable texture
    glEnable(GL_TEXTURE_2D);

    // Draw bars
    // TODO: Handle zoom by camera transformations
    //if (!d_ptr->m_zoomActivated)

    bool barSelectionFound = false;
    for (int bar = 0; bar < d_ptr->m_data->d_ptr->row().size(); bar++) {
        QDataItem *item = d_ptr->m_data->d_ptr->getItem(bar);
        if (!item)
            continue;

        GLfloat barHeight = item->d_ptr->value() / d_ptr->m_heightNormalizer;

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 depthMVPMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(item->d_ptr->translation().x(),
                              heightMultiplier * barHeight + heightScaler - d_ptr->m_yAdjustment,
                              item->d_ptr->translation().z());
        modelMatrix.scale(QVector3D(widthMultiplier * barHeight + widthScaler,
                                    heightMultiplier * barHeight + heightScaler,
                                    depthMultiplier * barHeight + depthScaler));
        itModelMatrix.scale(QVector3D(widthMultiplier * barHeight + widthScaler,
                                      heightMultiplier * barHeight + heightScaler,
                                      depthMultiplier * barHeight + depthScaler));

#ifdef SHOW_DEPTH_TEXTURE_SCENE
        MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;
#else
        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
#endif
        depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

        QVector3D baseColor = Utils::vectorFromColor(d_ptr->m_theme->m_baseColor);
        QVector3D heightColor = Utils::vectorFromColor(d_ptr->m_theme->m_heightColor)
                * barHeight;

        QVector3D barColor = baseColor + heightColor;

        GLfloat lightStrength = d_ptr->m_theme->m_lightStrength;
        if (d_ptr->m_selectionMode > ModeNone) {
            Q3DMapsPrivate::SelectionType selectionType = d_ptr->isSelected(bar, selection);
            switch (selectionType) {
            case Q3DMapsPrivate::SelectionBar: {
                barColor = Utils::vectorFromColor(d_ptr->m_theme->m_highlightBarColor);
                lightStrength = d_ptr->m_theme->m_highlightLightStrength;
                // Insert data to QDataItem. We have no ownership, don't delete the previous one
                if (!d_ptr->m_zoomActivated) {
                    d_ptr->m_selectedBar = item;
                    barSelectionFound = true;
                }
                break;
            }
            case Q3DMapsPrivate::SelectionNone: {
                // Current bar is not selected, nor on a row or column
                // do nothing
                break;
            }
            default: {
                // Unsupported selection mode
                // do nothing
                break;
            }
            }
        }

        if (barHeight != 0) {
            // Set shader bindings
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightP(), lightPos);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->view(), viewMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->model(), modelMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->nModel(),
                                                itModelMatrix.inverted().transposed());
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->MVP(), MVPMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->color(), barColor);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->ambientS(),
                                                d_ptr->m_theme->m_ambientStrength);

            if (d_ptr->m_shadowQuality > ShadowNone) {
                // Set shadow shader bindings
                d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->shadowQ(),
                                                    d_ptr->m_shadowQualityToShader);
                d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->depth(),
                                                    depthMVPMatrix);
                d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightS(),
                                                    lightStrength / 10.0f);

                // Draw the object
                d_ptr->m_drawer->drawObject(d_ptr->m_barShader, d_ptr->m_barObj,
                                            0, d_ptr->m_depthTexture);
            } else {
                // Set shadowless shader bindings
                d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightS(), lightStrength);

                // Draw the object
                d_ptr->m_drawer->drawObject(d_ptr->m_barShader, d_ptr->m_barObj);
            }
        }
    }

    // Release bar shader
    d_ptr->m_barShader->release();
#if 1
    // Bind background shader
    d_ptr->m_backgroundShader->bind();
    if (d_ptr->m_bgrHasAlpha) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Draw background
    if (d_ptr->m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 depthMVPMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(0.0f, -d_ptr->m_yAdjustment, zComp);
        modelMatrix.scale(QVector3D(d_ptr->m_areaSize.width() / d_ptr->m_scaleFactor,
                                    1.0f,
                                    d_ptr->m_areaSize.height() / d_ptr->m_scaleFactor));
        modelMatrix.rotate(-90.0f, 1.0f, 0.0f, 0.0f);
        itModelMatrix.scale(QVector3D(d_ptr->m_areaSize.width() / d_ptr->m_scaleFactor,
                                      1.0f,
                                      d_ptr->m_areaSize.height() / d_ptr->m_scaleFactor));

#ifdef SHOW_DEPTH_TEXTURE_SCENE
        MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;
#else
        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
#endif
        depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

        // Set shader bindings
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->lightP(), lightPos);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->view(), viewMatrix);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->model(), modelMatrix);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->nModel(),
                                                   itModelMatrix.inverted().transposed());
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->MVP(), MVPMatrix);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->ambientS(),
                                                   d_ptr->m_theme->m_ambientStrength * 3.0f);

        if (d_ptr->m_shadowQuality > ShadowNone) {
            // Set shadow shader bindings
            d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->shadowQ(),
                                                       d_ptr->m_shadowQualityToShader);
            d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->depth(),
                                                       depthMVPMatrix);
            d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->lightS(),
                                                       d_ptr->m_theme->m_lightStrength / 25.0f);

            // Draw the object
            d_ptr->m_drawer->drawObject(d_ptr->m_backgroundShader, d_ptr->m_backgroundObj,
                                        d_ptr->m_bgrTexture, d_ptr->m_depthTexture);
        } else {
            // Set shadowless shader bindings
            d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->lightS(),
                                                       d_ptr->m_theme->m_lightStrength);

            // Draw the object
            d_ptr->m_drawer->drawObject(d_ptr->m_backgroundShader, d_ptr->m_backgroundObj,
                                        d_ptr->m_bgrTexture);
        }
    }

    // Disable textures
    glDisable(GL_TEXTURE_2D);
    if (d_ptr->m_bgrHasAlpha)
        glDisable(GL_BLEND);

    // Release background shader
    d_ptr->m_backgroundShader->release();
#endif

    // Handle zoom activation and label drawing
    if (!barSelectionFound) {
        // We have no ownership, don't delete. Just NULL the pointer.
        d_ptr->m_selectedBar = NULL;
        //if (d_ptr->m_zoomActivated && Q3DMapsPrivate::MouseOnOverview == d_ptr->m_mousePressed) {
        //d_ptr->m_sceneViewPort = QRect(0, 0, width(), height());
        //d_ptr->m_zoomActivated = false;
        //}
    } /*else if (d_ptr->m_selectionMode >= ModeZoomRow
               && Q3DMapsPrivate::MouseOnScene == d_ptr->m_mousePressed) {
        // Activate zoom mode
        d_ptr->m_zoomActivated = true;
        d_ptr->m_sceneViewPort = QRect(0, height() - height() / 5, width() / 5, height() / 5);

        // Create label textures
        for (int col = 0; col < d_ptr->m_zoomSelection->d_ptr->row().size(); col++) {
            QDataItem *item = d_ptr->m_zoomSelection->d_ptr->getItem(col);
            d_ptr->m_drawer->generateLabelTexture(item);
        }
    }*/ else {
        // Print value of selected bar
        static QDataItem *prevItem = d_ptr->m_selectedBar;
        d_ptr->m_labelShader->bind();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        if (d_ptr->m_labelTransparency > TransparencyNone) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
#ifndef DISPLAY_FULL_DATA_ON_SELECTION
        // Draw just the value string of the selected bar
        if (prevItem != d_ptr->m_selectedBar || d_ptr->m_updateLabels) {
            d_ptr->m_drawer->generateLabelTexture(d_ptr->m_selectedBar);
            prevItem = d_ptr->m_selectedBar;
        }

        d_ptr->m_drawer->drawLabel(*d_ptr->m_selectedBar, d_ptr->m_selectedBar->d_ptr->label(),
                                   viewMatrix, projectionMatrix,
                                   QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                                   QVector3D(0.0f, 0.0f, 0.0f), d_ptr->m_heightNormalizer,
                                   d_ptr->m_selectionMode, d_ptr->m_labelShader,
                                   d_ptr->m_labelObj, true);
#else
        static bool firstSelection = true;
        // Draw the value string followed by row label and column label
        LabelItem labelItem = d_ptr->m_selectedBar->d_ptr->selectionLabel();
        if (firstSelection || prevItem != d_ptr->m_selectedBar || d_ptr->m_updateLabels) {
            QString labelText = d_ptr->m_selectedBar->d_ptr->valueStr();
            //            if ((d_ptr->m_data->d_ptr->columnLabels().size()
            //                 > d_ptr->m_selectedBar->d_ptr->position().y())
            //                    && (d_ptr->m_data->d_ptr->rowLabels().size()
            //                        > d_ptr->m_selectedBar->d_ptr->position().x())) {
            //                labelText.append(QStringLiteral(" ("));
            //                labelText.append(d_ptr->m_data->d_ptr->rowLabels().at(
            //                                     d_ptr->m_selectedBar->d_ptr->position().x()));
            //                labelText.append(QStringLiteral(", "));
            //                labelText.append(d_ptr->m_data->d_ptr->columnLabels().at(
            //                                     d_ptr->m_selectedBar->d_ptr->position().y()));
            //                labelText.append(QStringLiteral(")"));
            //                //qDebug() << labelText;
            //            }
            d_ptr->m_drawer->generateLabelItem(&labelItem, labelText);
            d_ptr->m_selectedBar->d_ptr->setSelectionLabel(labelItem);
            prevItem = d_ptr->m_selectedBar;
            firstSelection = false;
        }

        d_ptr->m_drawer->drawLabel(*d_ptr->m_selectedBar, labelItem, viewMatrix, projectionMatrix,
                                   QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                                   QVector3D(0.0f, 0.0f, 0.0f), d_ptr->m_heightNormalizer,
                                   d_ptr->m_selectionMode, d_ptr->m_labelShader,
                                   d_ptr->m_labelObj, true, false);
#endif
        glDisable(GL_TEXTURE_2D);
        if (d_ptr->m_labelTransparency > TransparencyNone)
            glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // Release label shader
        d_ptr->m_labelShader->release();

        // Reset label update flag; they should have been updated when we get here
        d_ptr->m_updateLabels = false;
    }
#if 0
    // TODO: Calculations done temporarily here. When optimizing, move to after data set addition? Keep drawing of the labels here.
    // Bind label shader
    d_ptr->m_labelShader->bind();

    glEnable(GL_TEXTURE_2D);
    if (d_ptr->m_labelTransparency > TransparencyNone) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Calculate the positions for row and column labels and store them into QDataItems (and QDataRows?)
    for (int row = 0; row != d_ptr->m_sampleCount.second; row += 1) {
        // Go through all rows and get position of max+1 or min-1 column, depending on x flip
        // We need only positions for them, labels have already been generated at QDataSet. Just add LabelItems
        rowPos = (row + 1) * (d_ptr->m_barSpacing.height());
        barPos = 0;
        GLfloat rotLabelX = -90.0f;
        GLfloat rotLabelY = 0.0f;
        Qt::AlignmentFlag alignment = Qt::AlignRight;
        if (d_ptr->m_zFlipped)
            rotLabelY = 180.0f;
        if (d_ptr->m_xFlipped) {
            barPos = (d_ptr->m_sampleCount.first + 1) * (d_ptr->m_barSpacing.width());
            alignment = Qt::AlignLeft;
        }
        QVector3D labelPos = QVector3D((d_ptr->m_rowWidth - barPos) / d_ptr->m_scaleFactor,
                                       -d_ptr->m_yAdjustment + 0.005f, // raise a bit over background to avoid depth "glimmering"
                                       (d_ptr->m_columnDepth - rowPos) / d_ptr->m_scaleFactor
                                       + zComp);

        // TODO: Try it; draw the label here

        // Create a data item
        QDataItem *label = new QDataItem();
        label->d_ptr->setTranslation(labelPos);
        if (d_ptr->m_data->d_ptr->rowLabelItems().size() > row) {
            label->d_ptr->setLabel(d_ptr->m_data->d_ptr->rowLabelItems().at(
                                       d_ptr->m_data->d_ptr->rowLabelItems().size() - row - 1));
        }

        //qDebug() << "labelPos, row" << row + 1 << ":" << labelPos << d_ptr->m_dataSet->d_ptr->rowLabels().at(row);

        d_ptr->m_drawer->drawLabel(*label, label->d_ptr->label(), viewMatrix, projectionMatrix,
                                   QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                                   QVector3D(rotLabelX, rotLabelY, 0.0f), d_ptr->m_heightNormalizer,
                                   d_ptr->m_selectionMode, d_ptr->m_labelShader,
                                   d_ptr->m_labelObj, true, true, LabelMid, alignment);

        delete label;
    }
    for (int bar = 0; bar != d_ptr->m_sampleCount.first; bar += 1) {
        // Go through all columns and get position of max+1 or min-1 row, depending on z flip
        // We need only positions for them, labels have already been generated at QDataSet. Just add LabelItems
        barPos = (bar + 1) * (d_ptr->m_barSpacing.width());
        rowPos = 0;
        GLfloat rotLabelX = -90.0f;
        GLfloat rotLabelY = 90.0f;
        Qt::AlignmentFlag alignment = Qt::AlignLeft;
        if (d_ptr->m_xFlipped)
            rotLabelY = -90.0f;
        if (d_ptr->m_zFlipped) {
            rowPos = (d_ptr->m_sampleCount.second + 1) * (d_ptr->m_barSpacing.height());
            alignment = Qt::AlignRight;
        }
        QVector3D labelPos = QVector3D((d_ptr->m_rowWidth - barPos) / d_ptr->m_scaleFactor,
                                       -d_ptr->m_yAdjustment + 0.005f, // raise a bit over background to avoid depth "glimmering"
                                       (d_ptr->m_columnDepth - rowPos) / d_ptr->m_scaleFactor
                                       + zComp);

        // TODO: Try it; draw the label here

        // Create a data item
        QDataItem *label = new QDataItem();
        label->d_ptr->setTranslation(labelPos);
        if (d_ptr->m_data->d_ptr->columnLabelItems().size() > bar) {
            label->d_ptr->setLabel(d_ptr->m_data->d_ptr->columnLabelItems().at(
                                       d_ptr->m_data->d_ptr->columnLabelItems().size()
                                       - bar - 1));
        }

        //qDebug() << "labelPos, col" << bar + 1 << ":" << labelPos << d_ptr->m_dataSet->d_ptr->columnLabels().at(bar);

        d_ptr->m_drawer->drawLabel(*label, label->d_ptr->label(), viewMatrix, projectionMatrix,
                                   QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                                   QVector3D(rotLabelX, rotLabelY, 0.0f), d_ptr->m_heightNormalizer,
                                   d_ptr->m_selectionMode, d_ptr->m_labelShader,
                                   d_ptr->m_labelObj, true, true, LabelMid, alignment);

        delete label;
    }
    glDisable(GL_TEXTURE_2D);
    if (d_ptr->m_labelTransparency > TransparencyNone)
        glDisable(GL_BLEND);

    // Release label shader
    d_ptr->m_labelShader->release();
#endif
#endif
#endif
}

void Q3DMaps::mousePressEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button()) {
        if (d_ptr->m_zoomActivated) {
            //qDebug() << event->pos().x() << event->pos().y() << d_ptr->m_sceneViewPort << d_ptr->m_zoomViewPort;
            if (event->pos().x() <= d_ptr->m_sceneViewPort.width()
                    && event->pos().y() <= d_ptr->m_sceneViewPort.height()) {
                d_ptr->m_mousePressed = Q3DMapsPrivate::MouseOnOverview;
                //qDebug() << "Mouse pressed on overview";
            } else {
                d_ptr->m_mousePressed = Q3DMapsPrivate::MouseOnZoom;
                //qDebug() << "Mouse pressed on zoom";
            }
        } else {
            d_ptr->m_mousePressed = Q3DMapsPrivate::MouseOnScene;
            // update mouse positions to prevent jumping when releasing or repressing a button
            d_ptr->m_mousePos = event->pos();
            //qDebug() << "Mouse pressed on scene";
        }
    } else if (Qt::MiddleButton == event->button()) {
        // reset rotations
        d_ptr->m_mousePos = QPoint(0, 0);
    } else if (Qt::RightButton == event->button()) {
        d_ptr->m_mousePressed = Q3DMapsPrivate::MouseRotating;
        // update mouse positions to prevent jumping when releasing or repressing a button
        d_ptr->m_mousePos = event->pos();
    }
    CameraHelper::updateMousePos(d_ptr->m_mousePos);
}

void Q3DMaps::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "mouse button released" << event->button();
    if (Q3DMapsPrivate::MouseRotating == d_ptr->m_mousePressed) {
        // update mouse positions to prevent jumping when releasing or repressing a button
        d_ptr->m_mousePos = event->pos();
        CameraHelper::updateMousePos(event->pos());
    }
    d_ptr->m_mousePressed = Q3DMapsPrivate::MouseNone;
}

void Q3DMaps::mouseMoveEvent(QMouseEvent *event)
{
    if (Q3DMapsPrivate::MouseRotating == d_ptr->m_mousePressed) {
        //qDebug() << "mouse moved while pressed" << event->pos();
        d_ptr->m_mousePos = event->pos();
    }
#if 0
    // TODO: Testi - laske kursorin sijainti scenessä
    QPointF mouse3D((2.0f * event->pos().x() - width()) / height(),
                    1.0f - (2.0f * event->pos().y()) / height());
    //qDebug() << "mouse position in scene" << mouse3D;

    // TODO: Testi laske focal point
    GLfloat focalPoint = tan(45.0f / 2.0f);

    // TODO: Testi - laske viewmatriisin kerroin
    QVector3D worldRay = QVector3D(0.0f, 0.0f, 0.0f)
            - QVector3D(mouse3D.x(), mouse3D.y(), -focalPoint);
    //qDebug() << "worldRay" << worldRay;
    // multiply viewmatrix with this to get something?
#endif
}

void Q3DMaps::wheelEvent(QWheelEvent *event)
{
    if (d_ptr->m_zoomLevel > 100)
        d_ptr->m_zoomLevel += event->angleDelta().y() / 12;
    else if (d_ptr->m_zoomLevel > 50)
        d_ptr->m_zoomLevel += event->angleDelta().y() / 60;
    else
        d_ptr->m_zoomLevel += event->angleDelta().y() / 120;
    if (d_ptr->m_zoomLevel > 500)
        d_ptr->m_zoomLevel = 500;
    else if (d_ptr->m_zoomLevel < 10)
        d_ptr->m_zoomLevel = 10;
}

void Q3DMaps::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    // Set view port
    if (d_ptr->m_zoomActivated)
        d_ptr->m_sceneViewPort = QRect(0, height() - height() / 5, width() / 5, height() / 5);
    else
        d_ptr->m_sceneViewPort = QRect(0, 0, width(), height());
    d_ptr->m_zoomViewPort = QRect(0, 0, width(), height());

    // Calculate zoom level based on aspect ratio
    GLfloat div;
    GLfloat zoomAdjustment;
    div = qMin(width(), height());
    zoomAdjustment = defaultRatio * ((width() / div) / (height() / div));
    //qDebug() << "zoom adjustment" << zoomAdjustment;
    d_ptr->m_zoomAdjustment = qMin(zoomAdjustment, 1.0f); // clamp to 1.0f

    // Re-init selection buffer
    d_ptr->initSelectionBuffer();

    // Re-init depth buffer
    if (d_ptr->m_isInitialized && d_ptr->m_shadowQuality > ShadowNone)
        d_ptr->initDepthBuffer();
}

void Q3DMaps::setBarSpecs(const QVector3D &thickness, AdjustmentDirection direction)
{
    d_ptr->m_barThickness = thickness;
    d_ptr->m_adjustDirection = direction;
}

void Q3DMaps::setBarType(BarStyle style, bool smooth)
{
    if (style == Bars) {
        if (smooth)
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/barSmooth");
        else
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/bar");
    } else if (style == Pyramids) {
        if (smooth)
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/pyramidSmooth");
        else
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/pyramid");
    } else if (style == Cones) {
        if (smooth)
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/coneSmooth");
        else
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/cone");
    } else if (style == Cylinders) {
        if (smooth)
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/cylinderSmooth");
        else
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/cylinder");
    } else if (style == BevelBars) {
        if (smooth)
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/bevelbarSmooth");
        else
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/bevelbar");
    } else if (style == Spheres) {
        if (smooth)
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/sphereSmooth");
        else
            d_ptr->m_objFile = QStringLiteral(":/defaultMeshes/sphere");
    }
    // Reload mesh data
    if (d_ptr->m_isInitialized)
        d_ptr->loadBarMesh();
}

void Q3DMaps::setMeshFileName(const QString &objFileName)
{
    d_ptr->m_objFile = objFileName;
}

void Q3DMaps::setCameraPreset(CameraPreset preset)
{
    CameraHelper::setCameraPreset(preset);
}

void Q3DMaps::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    d_ptr->m_horizontalRotation = qBound(-180.0f, horizontal, 180.0f);
    d_ptr->m_verticalRotation = qBound(0.0f, vertical, 90.0f);
    d_ptr->m_zoomLevel = qBound(10, distance, 500);
    CameraHelper::setCameraRotation(QPointF(d_ptr->m_horizontalRotation,
                                            d_ptr->m_verticalRotation));
    //qDebug() << "camera rotation set to" << d_ptr->m_horizontalRotation << d_ptr->m_verticalRotation;
}

void Q3DMaps::setTheme(ColorTheme theme)
{
    d_ptr->m_theme->useTheme(theme);
    d_ptr->m_drawer->setTheme(*d_ptr->m_theme);
    // Re-initialize shaders
    if (!d_ptr->m_theme->m_uniformColor) {
        d_ptr->initShaders(QStringLiteral(":/shaders/vertexShadow"),
                           QStringLiteral(":/shaders/fragmentShadowNoTexColorOnY"));
    } else {
        d_ptr->initShaders(QStringLiteral(":/shaders/vertexShadow"),
                           QStringLiteral(":/shaders/fragmentShadowNoTex"));
    }
    d_ptr->m_updateLabels = true;
}

void Q3DMaps::setBarColor(QColor baseColor, QColor heightColor, bool uniform)
{
    d_ptr->m_theme->m_baseColor = baseColor;
    d_ptr->m_theme->m_heightColor = heightColor;
    if (d_ptr->m_theme->m_uniformColor != uniform) {
        // Re-initialize shaders
        if (!d_ptr->m_theme->m_uniformColor) {
            d_ptr->initShaders(QStringLiteral(":/shaders/vertexShadow"),
                               QStringLiteral(":/shaders/fragmentShadowNoTexColorOnY"));
        } else {
            d_ptr->initShaders(QStringLiteral(":/shaders/vertexShadow"),
                               QStringLiteral(":/shaders/fragmentShadowNoTex"));
        }
    }
    d_ptr->m_theme->m_uniformColor = uniform;
}

void Q3DMaps::setSelectionMode(SelectionMode mode)
{
    d_ptr->m_selectionMode = mode;
    // Disable zoom if mode changes
    //d_ptr->m_zoomActivated = false;
    //d_ptr->m_sceneViewPort = QRect(0, 0, width(), height());
}

void Q3DMaps::setWindowTitle(const QString &title)
{
    setTitle(title);
}

void Q3DMaps::setFontSize(float fontsize)
{
    d_ptr->m_font.setPointSizeF(fontsize);
    d_ptr->m_drawer->setFont(d_ptr->m_font);
    d_ptr->m_updateLabels = true;
}

void Q3DMaps::setFont(const QFont &font)
{
    d_ptr->m_font = font;
    d_ptr->m_drawer->setFont(font);
    d_ptr->m_updateLabels = true;
}

void Q3DMaps::setLabelTransparency(LabelTransparency transparency)
{
    d_ptr->m_labelTransparency = transparency;
    d_ptr->m_drawer->setTransparency(transparency);
    d_ptr->m_updateLabels = true;
}

void Q3DMaps::setGridEnabled(bool enable)
{
    d_ptr->m_gridEnabled = enable;
}

bool Q3DMaps::addDataItem(QDataItem* dataItem)
{
    // Check validity
    if (!d_ptr->isValid(*dataItem))
        return false;
    // Convert position to translation
    d_ptr->calculateTranslation(dataItem);
    // Add item
    d_ptr->m_data->addItem(dataItem);
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_data->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
    return true;
}

bool Q3DMaps::addData(const QVector<QDataItem*> &data)
{
    // Convert to QDataRow
    for (int i = 0; i < data.size(); i++) {
        QDataItem *item = data.at(i);
        // Check validity
        if (!d_ptr->isValid(*item)) {
            return false;
        } else {
            // Convert position to translation
            d_ptr->calculateTranslation(item);
            // Add item
            d_ptr->m_data->addItem(item);
        }
    }
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_data->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
    return true;
}

bool Q3DMaps::addData(const QDataRow &dataRow)
{
    for (int itemIdx = 0; itemIdx < dataRow.d_ptr->row().size(); itemIdx++) {
        QDataItem *item = dataRow.d_ptr->getItem(itemIdx);
        // Check validity
        if (!d_ptr->isValid(*item)) {
            return false;
        } else {
            // Convert position to translation
            d_ptr->calculateTranslation(item);
            // Add item
            d_ptr->m_data->addItem(item);
        }
    }
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_data->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
    return true;
}

bool Q3DMaps::setData(const QVector<QDataItem*> &dataRow)
{
    // Delete previous data
    delete d_ptr->m_data;
    // Convert to QDataRow
    d_ptr->m_data = new QDataRow();
    for (int i = 0; i < dataRow.size(); i++) {
        QDataItem *item = dataRow.at(i);
        // Check validity
        if (!d_ptr->isValid(*item)) {
            return false;
        } else {
            // Convert position to translation
            d_ptr->calculateTranslation(item);
            // Add item
            d_ptr->m_data->addItem(item);
        }
    }
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_data->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
    return true;
}

bool Q3DMaps::setData(QDataRow *dataRow)
{
    // Check validity
    for (int i = 0; i < dataRow->d_ptr->row().size(); i++) {
        if (!d_ptr->isValid(*dataRow->d_ptr->row().at(i)))
            return false;
        else
            d_ptr->calculateTranslation(dataRow->d_ptr->row()[i]);
    }
    // Delete previous data
    delete d_ptr->m_data;
    // Set give data as new data
    d_ptr->m_data = dataRow;
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_data->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
    return true;
}

void Q3DMaps::setAreaSpecs(const QRect &areaRect, const QImage &image)
{
    d_ptr->calculateSceneScalingFactors(areaRect);
    setImage(image);
}

void Q3DMaps::setImage(const QImage &image)
{
    d_ptr->m_bgrHasAlpha = image.hasAlphaChannel();
    if (d_ptr->m_bgrTexture)
        glDeleteTextures(1, &d_ptr->m_bgrTexture);
    d_ptr->m_bgrTexture = d_ptr->m_textureHelper->create2DTexture(image, true, true);
}

void Q3DMaps::setShadowQuality(ShadowQuality quality)
{
    d_ptr->m_shadowQuality = quality;
    switch (quality) {
    case ShadowLow:
        //qDebug() << "ShadowLow";
        d_ptr->m_shadowQualityToShader = 33.3f;
        break;
    case ShadowMedium:
        //qDebug() << "ShadowMedium";
        d_ptr->m_shadowQualityToShader = 100.0f;
        break;
    case ShadowHigh:
        //qDebug() << "ShadowHigh";
        d_ptr->m_shadowQualityToShader = 200.0f;
        break;
    default:
        d_ptr->m_shadowQualityToShader = 0.0f;
        break;
    }
    if (d_ptr->m_isInitialized) {
        if (d_ptr->m_shadowQuality > ShadowNone) {
            // Re-init depth buffer
            d_ptr->initDepthBuffer();
            // Re-init shaders
            if (!d_ptr->m_theme->m_uniformColor) {
                d_ptr->initShaders(QStringLiteral(":/shaders/vertexShadow"),
                                   QStringLiteral(":/shaders/fragmentShadowNoTexColorOnY"));
            } else {
                d_ptr->initShaders(QStringLiteral(":/shaders/vertexShadow"),
                                   QStringLiteral(":/shaders/fragmentShadowNoTex"));
            }
            d_ptr->initBackgroundShaders(QStringLiteral(":/shaders/vertexShadow"),
                                         QStringLiteral(":/shaders/fragmentShadow"));
        } else {
            // Re-init shaders
            if (!d_ptr->m_theme->m_uniformColor) {
                d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                                   QStringLiteral(":/shaders/fragmentColorOnY"));
            } else {
                d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                                   QStringLiteral(":/shaders/fragment"));
            }
            d_ptr->initBackgroundShaders(QStringLiteral(":/shaders/vertexTexture"),
                                         QStringLiteral(":/shaders/fragmentTexture"));
        }
    }
}

Q3DMapsPrivate::Q3DMapsPrivate(Q3DMaps *q)
    : q_ptr(q),
      m_paintDevice(0),
      m_barShader(0),
      m_depthShader(0),
      m_selectionShader(0),
      m_backgroundShader(0),
      m_labelShader(0),
      m_barObj(0),
      m_backgroundObj(0),
      m_gridLineObj(0),
      m_labelObj(0),
      m_objFile(QStringLiteral(":/defaultMeshes/bar")),
      m_mousePressed(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_zoomLevel(100),
      m_zoomAdjustment(1.0f),
      m_horizontalRotation(0.0f),
      m_verticalRotation(45.0f),
      m_barThickness(QVector3D(1.0f, 0.0f, 1.0f)),
      m_heightNormalizer(0.0f),
      m_yAdjustment(0.0f),
      m_scaleFactor(1.0f),
      m_theme(new Theme()),
      m_isInitialized(false),
      m_selectionMode(ModeBar),
      m_selectedBar(0),
      m_data(new QDataRow()),
      m_axisLabelX(QStringLiteral("X")),
      m_axisLabelZ(QStringLiteral("Z")),
      m_axisLabelY(QStringLiteral("Y")),
      m_sceneViewPort(0, 0, q->width(), q->height()),
      m_zoomViewPort(0, 0, q->width(), q->height()),
      m_zoomActivated(false),
      m_textureHelper(new TextureHelper()),
      m_labelTransparency(TransparencyFromTheme),
      m_font(QFont(QStringLiteral("Arial"))),
      m_drawer(new Drawer(*m_theme, m_font, m_labelTransparency)),
      m_bgrTexture(0),
      m_depthTexture(0),
      m_selectionTexture(0),
      m_depthFrameBuffer(0),
      m_selectionFrameBuffer(0),
      m_selectionDepthBuffer(0),
      m_areaSize(QSizeF(1.0f, 1.0f)),
      m_updateLabels(true),
      m_gridEnabled(true),
      m_shadowQuality(ShadowLow),
      m_shadowQualityToShader(33.3f),
      m_bgrHasAlpha(false)
{
    //m_data->d_ptr->setDrawer(m_drawer);
    //QObject::connect(m_drawer, &Drawer::drawerChanged, this, &Q3DMapsPrivate::updateTextures);
}

Q3DMapsPrivate::~Q3DMapsPrivate()
{
    m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
    m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);
    m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
    m_textureHelper->deleteTexture(&m_selectionTexture);
    m_textureHelper->deleteTexture(&m_bgrTexture);
    delete m_data;
    delete m_barShader;
    delete m_selectionShader;
    delete m_backgroundShader;
    delete m_barObj;
    delete m_backgroundObj;
    delete m_gridLineObj;
    delete m_textureHelper;
    delete m_drawer;
}

void Q3DMapsPrivate::loadBarMesh()
{
    if (m_barObj)
        delete m_barObj;
    m_barObj = new ObjectHelper(m_objFile);
    m_barObj->load();
}

void Q3DMapsPrivate::loadBackgroundMesh()
{
    if (m_backgroundObj)
        delete m_backgroundObj;
    m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/label"));
    m_backgroundObj->load();
}

void Q3DMapsPrivate::loadGridLineMesh()
{
    if (m_gridLineObj)
        delete m_gridLineObj;
    m_gridLineObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/bar"));
    m_gridLineObj->load();
}

void Q3DMapsPrivate::loadLabelMesh()
{
    if (m_labelObj)
        delete m_labelObj;
    m_labelObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/label"));
    m_labelObj->load();
}

void Q3DMapsPrivate::initShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_barShader)
        delete m_barShader;
    m_barShader = new ShaderHelper(q_ptr, vertexShader, fragmentShader);
    m_barShader->initialize();
}

void Q3DMapsPrivate::initSelectionShader()
{
    if (m_selectionShader)
        delete m_selectionShader;
    m_selectionShader = new ShaderHelper(q_ptr, QStringLiteral(":/shaders/vertexSelection"),
                                         QStringLiteral(":/shaders/fragmentSelection"));
    m_selectionShader->initialize();
}

void Q3DMapsPrivate::initDepthShader()
{
    if (m_depthShader)
        delete m_depthShader;
    m_depthShader = new ShaderHelper(q_ptr, QStringLiteral(":/shaders/vertexDepth"),
                                     QStringLiteral(":/shaders/fragmentDepth"));
    m_depthShader->initialize();
}

void Q3DMapsPrivate::initSelectionBuffer()
{
    if (m_selectionTexture) {
        m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);
        m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
        m_textureHelper->deleteTexture(&m_selectionTexture);
    }
    m_selectionTexture = m_textureHelper->createSelectionTexture(q_ptr->size(),
                                                                 m_selectionFrameBuffer,
                                                                 m_selectionDepthBuffer);
}

void Q3DMapsPrivate::initDepthBuffer()
{
    if (m_depthTexture) {
        m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
        m_textureHelper->deleteTexture(&m_depthTexture);
    }
    m_depthTexture = m_textureHelper->createDepthTexture(q_ptr->size(), m_depthFrameBuffer,
                                                         m_shadowQuality);
}

void Q3DMapsPrivate::initBackgroundShaders(const QString &vertexShader,
                                           const QString &fragmentShader)
{
    if (m_backgroundShader)
        delete m_backgroundShader;
    m_backgroundShader = new ShaderHelper(q_ptr, vertexShader, fragmentShader);
    m_backgroundShader->initialize();
}

void Q3DMapsPrivate::initLabelShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_labelShader)
        delete m_labelShader;
    m_labelShader = new ShaderHelper(q_ptr, vertexShader, fragmentShader);
    //    m_labelShader = new ShaderHelper(q_ptr, QStringLiteral(":/shaders/testDepthVert"),
    //                                     QStringLiteral(":/shaders/testDepthFrag"));
    m_labelShader->initialize();
}

void Q3DMapsPrivate::updateTextures()
{
    // Drawer has changed; this flag needs to be checked when checking if we need to update labels
    m_updateLabels = true;
}

void Q3DMapsPrivate::calculateSceneScalingFactors(const QRect &areaRect)
{
    m_areaSize = areaRect.size();
    // Calculate scaling factor so that we can be sure the whole area fits to positive z space
    if (zComp > 1.0f)
        m_scaleFactor = qMax(m_areaSize.width(), m_areaSize.height()) / zComp;
    else
        m_scaleFactor = qMax(m_areaSize.width(), m_areaSize.height());
    //qDebug() << "scaleFactor" << m_scaleFactor;
}

void Q3DMapsPrivate::calculateHeightAdjustment(const QPair<GLfloat, GLfloat> &limits)
{
    // 2.0f = max difference between minimum and maximum value after scaling with m_heightNormalizer
    m_yAdjustment = 2.0f - ((limits.second - limits.first) / m_heightNormalizer);
    //qDebug() << m_yAdjustment;
}

void Q3DMapsPrivate::calculateTranslation(QDataItem *item)
{
    // We need to convert position (which is in coordinates), to translation (which has origin in the center and is scaled)
    // -> move pos(center, center) to trans(0, 0) and pos(0, 0) to trans(left, top)
    GLfloat xTrans = 2.0f * (item->d_ptr->position().x() - (m_areaSize.width() / 2.0f))
            / m_scaleFactor;
    GLfloat zTrans = 2.0f * (item->d_ptr->position().y() - (m_areaSize.height() / 2.0f))
            / m_scaleFactor;
    //qDebug() << "x, y" << item->d_ptr->position().x() << item->d_ptr->position().y();
    item->d_ptr->setTranslation(QVector3D(xTrans, 0.0f, zTrans + zComp));
    //qDebug() << item->d_ptr->translation();
}

Q3DMapsPrivate::SelectionType Q3DMapsPrivate::isSelected(GLint bar, const QVector3D &selection)
{
    GLubyte barIdxRed = 0;
    GLubyte barIdxGreen = 0;
    GLubyte barIdxBlue = 0;
    //static QVector3D prevSel = selection; // TODO: For debugging
    SelectionType isSelectedType = SelectionNone;

    if (selection == skipColor)
        return isSelectedType; // skip window

    if (bar <= 255) {
        barIdxRed = bar;
    } else if (bar <= 65535) {
        barIdxGreen = bar / 256;
        barIdxRed = bar % 256;
    } else {
        barIdxBlue = bar / 65535;
        barIdxGreen = bar % 65535;
        barIdxRed = bar % 256;
    }

    QVector3D current = QVector3D(barIdxRed, barIdxGreen, barIdxBlue);

    // TODO: For debugging
    //if (selection != prevSel) {
    //    qDebug() << selection.x() << selection .y() << selection.z();
    //    prevSel = selection;
    //}

    if (current == selection)
        isSelectedType = SelectionBar;

    return isSelectedType;
}

bool Q3DMapsPrivate::isValid(const QDataItem &item)
{
    bool retval = true;
    if (item.d_ptr->value() < 0) {
        qCritical("Data item value out of range");
        retval = false;
    } else if (item.d_ptr->position().x() < 0 || item.d_ptr->position().x() > m_areaSize.width()) {
        qCritical("Data item x position out of range");
        retval = false;
    } else if (item.d_ptr->position().y() < 0 || item.d_ptr->position().y() > m_areaSize.height()) {
        qCritical("Data item y position out of range");
        retval = false;
    }
    return retval;
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE
