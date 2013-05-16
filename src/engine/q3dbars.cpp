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

#include "q3dbars.h"
#include "q3dbars_p.h"
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

// Uncommenting this draws the shadow map with wider FOV than scene itself, making the light
// seem to be closer to scene than it actually is. This way shadows look slightly better (to me anyway)
#define USE_WIDER_SHADOWS

// You can verify that depth buffer drawing works correctly by uncommenting this.
// You should see the scene from  where the light is
//#define SHOW_DEPTH_TEXTURE_SCENE

//#define DISPLAY_RENDER_SPEED

#ifdef DISPLAY_RENDER_SPEED
#include <QTime>
#endif

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

//#define USE_HAX0R_SELECTION // keep this defined until the "real" method works
#define DISPLAY_FULL_DATA_ON_SELECTION // Append selection value text with row and column labels

const GLfloat gridLineWidth = 0.005f;

Q3DBars::Q3DBars()
    : d_ptr(new Q3DBarsPrivate(this))
{
}

Q3DBars::~Q3DBars()
{
}

void Q3DBars::initialize()
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
                                     QStringLiteral(":/shaders/fragmentShadowNoTex"));
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
        d_ptr->initBackgroundShaders(QStringLiteral(":/shaders/vertex"),
                                     QStringLiteral(":/shaders/fragment"));
    }
    d_ptr->initLabelShaders(QStringLiteral(":/shaders/vertexLabel"),
                            QStringLiteral(":/shaders/fragmentLabel"));

    // Init depth shader (for shadows). Init in any case, easier to handle shadow activation if done via api.
    d_ptr->initDepthShader();

    // Init selection shader
    d_ptr->initSelectionShader();

#ifndef USE_HAX0R_SELECTION
    // Init the selection buffer
    d_ptr->initSelectionBuffer();
#endif

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
    CameraHelper::setDefaultCameraOrientation(QVector3D(0.0f, 0.0f, 6.0f + zComp),
                                              QVector3D(0.0f, 0.0f, zComp),
                                              QVector3D(0.0f, 1.0f, 0.0f));

    // Set view port
    glViewport(0, 0, width(), height());

    // Set initialized -flag
    d_ptr->m_isInitialized = true;
}

void Q3DBars::render()
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

    // If zoom selection is on, draw zoom scene
    drawZoomScene();
    // Draw bars scene
    drawScene();
}

void Q3DBars::drawZoomScene()
{
    // Set clear color
    QVector3D clearColor = Utils::vectorFromColor(d_ptr->m_theme->m_windowColor);
    glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If no zoom, return
    if (!d_ptr->m_zoomActivated)
        return;

    GLfloat barPosX = 0;
    GLint startBar = 0;
    GLint stopBar = d_ptr->m_zoomSelection->d_ptr->row().size();
    GLint stepBar = 1;
    QVector3D lightPos;

    // Specify viewport
    glViewport(d_ptr->m_zoomViewPort.x(), d_ptr->m_zoomViewPort.y(),
               d_ptr->m_zoomViewPort.width(), d_ptr->m_zoomViewPort.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, (GLfloat)d_ptr->m_zoomViewPort.width()
                                 / (GLfloat)d_ptr->m_zoomViewPort.height(), 0.1f, 100.0f);

#ifdef ROTATE_ZOOM_SELECTION
    // Calculate view matrix
    QMatrix4x4 viewMatrix = CameraHelper::calculateViewMatrix(d_ptr->m_mousePos,
                                                              d_ptr->m_zoomLevel
                                                              * d_ptr->m_zoomAdjustment,
                                                              d_ptr->m_zoomViewPort.width(),
                                                              d_ptr->m_zoomViewPort.height());

    // Get light position (rotate light with camera, a bit above it (as set in defaultLightPos))
    lightPos = CameraHelper::calculateLightPosition(defaultLightPos);

    if (viewMatrix.row(0).z() <= 0) {
        startBar = d_ptr->m_zoomSelection->d_ptr->row().size() - 1;
        stopBar = -1;
        stepBar = -1;
    }
#else
    // Set view matrix
    QMatrix4x4 viewMatrix;

    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, 5.0f + zComp),
                      QVector3D(0.0f, 0.0f, zComp),
                      QVector3D(0.0f, 1.0f, 0.0f));

    // Adjust scaling (zoom rate based on aspect ratio)
    viewMatrix.scale(d_ptr->m_zoomAdjustment);

    // Set light position a bit above the camera (depends on do we have row or column zoom)
    if (ModeZoomColumn == d_ptr->m_selectionMode)
        lightPos = CameraHelper::calculateLightPosition(defaultLightPos, -85.0f);
    else
        lightPos = CameraHelper::calculateLightPosition(defaultLightPos, 5.0f);
#endif

    // Bind bar shader
    d_ptr->m_barShader->bind();

    // Draw bars
    // Draw the selected row / column
    for (int bar = startBar; bar != stopBar; bar += stepBar) {
        QDataItem *item = d_ptr->m_zoomSelection->d_ptr->getItem(bar);
        if (!item)
            continue;

        GLfloat barHeight = item->d_ptr->value() / d_ptr->m_heightNormalizer;

        if (barHeight < 0)
            glCullFace(GL_FRONT);
        else
            glCullFace(GL_BACK);

        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 itModelMatrix;

        GLfloat barPosY = item->d_ptr->translation().y() - d_ptr->m_yAdjustment / 2.0f + 0.2f; // we need some room for labels underneath; add +0.2f
        if (ModeZoomRow == d_ptr->m_selectionMode)
            barPosX = item->d_ptr->translation().x();
        else
            barPosX = -(item->d_ptr->translation().z() - zComp); // flip z; frontmost bar to the left
        modelMatrix.translate(barPosX, barPosY, zComp);
        modelMatrix.scale(QVector3D(d_ptr->m_scaleX, barHeight, d_ptr->m_scaleZ));
        itModelMatrix.scale(QVector3D(d_ptr->m_scaleX, barHeight, d_ptr->m_scaleZ));

        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

        QVector3D baseColor = Utils::vectorFromColor(d_ptr->m_theme->m_baseColor);
        QVector3D heightColor = Utils::vectorFromColor(d_ptr->m_theme->m_heightColor) * barHeight;

        QVector3D barColor = baseColor + heightColor;

        GLfloat lightStrength = d_ptr->m_theme->m_lightStrength;

        if (barHeight != 0) {
            // Set shader bindings
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightP(), lightPos);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->view(), viewMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->model(), modelMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->nModel(),
                                                itModelMatrix.inverted().transposed());
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->MVP(), MVPMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->color(), barColor);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightS(), lightStrength);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->ambientS(),
                                                d_ptr->m_theme->m_ambientStrength);

            // Draw the object
            d_ptr->m_drawer->drawObject(d_ptr->m_barShader, d_ptr->m_barObj);
        }
    }

    // Release bar shader
    d_ptr->m_barShader->release();

    // Draw labels
    d_ptr->m_labelShader->bind();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glCullFace(GL_BACK);
    if (d_ptr->m_labelTransparency > TransparencyNone) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Draw labels for axes
    QDataItem *dummyItem = NULL;
    LabelItem x;
    LabelItem z;
    LabelItem y;
    d_ptr->m_dataSet->d_ptr->axisLabelItems(&x, &z, &y);
    LabelItem zoomSelectionLabel = d_ptr->m_zoomSelection->d_ptr->labelItem();
    if (ModeZoomRow == d_ptr->m_selectionMode) {
        d_ptr->m_drawer->drawLabel(*dummyItem, zoomSelectionLabel, viewMatrix, projectionMatrix,
                                   QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                                   QVector3D(0.0f, 0.0f, 0.0f), d_ptr->m_heightNormalizer,
                                   d_ptr->m_selectionMode, d_ptr->m_labelShader,
                                   d_ptr->m_labelObj, false, false, LabelTop);
        d_ptr->m_drawer->drawLabel(*dummyItem, z, viewMatrix, projectionMatrix,
                                   QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                                   QVector3D(0.0f, 0.0f, 0.0f), d_ptr->m_heightNormalizer,
                                   d_ptr->m_selectionMode, d_ptr->m_labelShader,
                                   d_ptr->m_labelObj, false, false, LabelBottom);
    } else {
        d_ptr->m_drawer->drawLabel(*dummyItem, x, viewMatrix, projectionMatrix,
                                   QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                                   QVector3D(0.0f, 0.0f, 0.0f), d_ptr->m_heightNormalizer,
                                   d_ptr->m_selectionMode, d_ptr->m_labelShader,
                                   d_ptr->m_labelObj, false, false, LabelBottom);
        d_ptr->m_drawer->drawLabel(*dummyItem, zoomSelectionLabel, viewMatrix, projectionMatrix,
                                   QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                                   QVector3D(0.0f, 0.0f, 0.0f), d_ptr->m_heightNormalizer,
                                   d_ptr->m_selectionMode, d_ptr->m_labelShader,
                                   d_ptr->m_labelObj, false, false, LabelTop);
    }
    d_ptr->m_drawer->drawLabel(*dummyItem, y, viewMatrix, projectionMatrix,
                               QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                               QVector3D(0.0f, 0.0f, 90.0f), d_ptr->m_heightNormalizer,
                               d_ptr->m_selectionMode, d_ptr->m_labelShader,
                               d_ptr->m_labelObj, false, false, LabelLeft);

    // Draw labels for bars
    for (int col = 0; col < d_ptr->m_zoomSelection->d_ptr->row().size(); col++) {
        QDataItem *item = d_ptr->m_zoomSelection->d_ptr->getItem(col);
        // Draw values
        d_ptr->m_drawer->drawLabel(*item, item->d_ptr->label(), viewMatrix, projectionMatrix,
                                   QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                                   QVector3D(0.0f, 0.0f, 0.0f), d_ptr->m_heightNormalizer,
                                   d_ptr->m_selectionMode, d_ptr->m_labelShader,
                                   d_ptr->m_labelObj);
        // Draw labels
        LabelItem labelItem;
        if (ModeZoomRow == d_ptr->m_selectionMode) {
            if (d_ptr->m_dataSet->d_ptr->columnLabelItems().size() > col) {
                // If draw order of bars is flipped, label draw order should be too
                if (d_ptr->m_xFlipped) {
                    labelItem = d_ptr->m_dataSet->d_ptr->columnLabelItems().at(
                                d_ptr->m_dataSet->d_ptr->columnLabelItems().size() - col - 1);
                } else {
                    labelItem = d_ptr->m_dataSet->d_ptr->columnLabelItems().at(col);
                }
            }
        } else {
            if (d_ptr->m_dataSet->d_ptr->rowLabelItems().size() > col) {
                // If draw order of bars is flipped, label draw order should be too
                if (d_ptr->m_zFlipped) {
                    labelItem = d_ptr->m_dataSet->d_ptr->rowLabelItems().at(
                                d_ptr->m_dataSet->d_ptr->rowLabelItems().size() - col - 1);
                } else {
                    labelItem = d_ptr->m_dataSet->d_ptr->rowLabelItems().at(col);
                }
            }
        }
        d_ptr->m_drawer->drawLabel(*item, labelItem, viewMatrix, projectionMatrix,
                                   QVector3D(0.0f, d_ptr->m_yAdjustment, zComp),
                                   QVector3D(0.0f, 0.0f, -45.0f), d_ptr->m_heightNormalizer,
                                   d_ptr->m_selectionMode, d_ptr->m_labelShader,
                                   d_ptr->m_labelObj, false, false, LabelBelow);
    }

    glDisable(GL_TEXTURE_2D);
    if (d_ptr->m_labelTransparency > TransparencyNone)
        glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // Release label shader
    d_ptr->m_labelShader->release();
}

void Q3DBars::drawScene()
{
    GLint startBar = 0;
    GLint stopBar = 0;
    GLint stepBar = 0;

    GLint startRow = 0;
    GLint stopRow = 0;
    GLint stepRow = 0;

    GLfloat backgroundRotation = 0;

    GLfloat barPos = 0;
    GLfloat rowPos = 0;

    static QVector3D selection = QVector3D(0, 0, 0);

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

    // Calculate drawing order
    // Draw order is reversed to optimize amount of drawing (ie. draw front objects first, depth test handles not needing to draw objects behind them)
    if (viewMatrix.row(0).x() >= 0) {
        startRow = 0;
        stopRow = d_ptr->m_sampleCount.second;
        stepRow = 1;
        d_ptr->m_zFlipped = false;
    } else {
        startRow = d_ptr->m_sampleCount.second - 1;
        stopRow = -1;
        stepRow = -1;
        d_ptr->m_zFlipped = true;
    }
    if (viewMatrix.row(0).z() <= 0) {
        startBar = 0;
        stopBar = d_ptr->m_sampleCount.first;
        stepBar = 1;
        d_ptr->m_xFlipped = false;
    } else {
        startBar = d_ptr->m_sampleCount.first - 1;
        stopBar = -1;
        stepBar = -1;
        d_ptr->m_xFlipped = true;
    }

    // calculate background rotation based on view matrix rotation
    if (viewMatrix.row(0).x() >= 0 && viewMatrix.row(0).z() <= 0)
        backgroundRotation = 270.0f;
    else if (viewMatrix.row(0).x() > 0 && viewMatrix.row(0).z() > 0)
        backgroundRotation = 180.0f;
    else if (viewMatrix.row(0).x() <= 0 && viewMatrix.row(0).z() >= 0)
        backgroundRotation = 90.0f;
    else if (viewMatrix.row(0).x() < 0 && viewMatrix.row(0).z() < 0)
        backgroundRotation = 0.0f;

    // Get light position (rotate light with camera, a bit above it (as set in defaultLightPos))
    QVector3D lightPos = CameraHelper::calculateLightPosition(defaultLightPos);
    //lightPos = QVector3D(0.0f, 4.0f, zComp); // center of bars, 4.0f above - for testing

    // Skip depth rendering if we're in zoom mode
    // TODO: Fix this, causes problems if depth rendering is off in zoom mode
    // Introduce regardless of shadow quality to simplify logic
    QMatrix4x4 depthViewMatrix;
    QMatrix4x4 depthProjectionMatrix;

    if (d_ptr->m_shadowQuality > ShadowNone/*!d_ptr->m_zoomActivated*/) {
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
        depthViewMatrix.lookAt(lightPos, QVector3D(0.0f, 1.0f - d_ptr->m_yAdjustment, zComp),
                               QVector3D(0.0f, 1.0f, 0.0f));
        // TODO: Why does depthViewMatrix.column(3).y() goes to zero when we're directly above? That causes the scene to be not drawn from above -> must be fixed
        //qDebug() << lightPos << depthViewMatrix << depthViewMatrix.column(3);
        // Set the depth projection matrix
#ifdef USE_WIDER_SHADOWS
        // Use this for a bit exaggerated shadows
        depthProjectionMatrix.perspective(20.0f, (GLfloat)d_ptr->m_sceneViewPort.width()
                                          / (GLfloat)d_ptr->m_sceneViewPort.height(), 3.0f, 100.0f);
#else
        // Use these for normal shadows, with the light further away
        depthProjectionMatrix = projectionMatrix;
#endif
        // Draw bars to depth buffer
        for (int row = startRow; row != stopRow; row += stepRow) {
            for (int bar = startBar; bar != stopBar; bar += stepBar) {
                if (!d_ptr->m_dataSet->d_ptr->getRow(row))
                    continue;
                QDataItem *item = d_ptr->m_dataSet->d_ptr->getRow(row)->d_ptr->getItem(bar);
                if (!item)
                    continue;

                GLfloat barHeight = item->d_ptr->value() / d_ptr->m_heightNormalizer;
                // skip shadows for barHeight < 0 (for now)
                // skip shadows for 0 -height bars
                if (barHeight <= 0)
                    continue;

                //if (barHeight < 0)
                //    glCullFace(GL_FRONT);
                //else
                //    glCullFace(GL_BACK);

                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;

                barPos = (bar + 1) * (d_ptr->m_barSpacing.width());
                rowPos = (row + 1) * (d_ptr->m_barSpacing.height());

                modelMatrix.translate((d_ptr->m_rowWidth - barPos) / d_ptr->m_scaleFactor,
                                      barHeight - d_ptr->m_yAdjustment,
                                      (d_ptr->m_columnDepth - rowPos) / d_ptr->m_scaleFactor
                                      + zComp);
                modelMatrix.scale(QVector3D(d_ptr->m_scaleX, barHeight, d_ptr->m_scaleZ));

                MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

                d_ptr->m_depthShader->setUniformValue(d_ptr->m_depthShader->MVP(), MVPMatrix);

                // 1st attribute buffer : vertices
                glEnableVertexAttribArray(d_ptr->m_depthShader->posAtt());
                glBindBuffer(GL_ARRAY_BUFFER, d_ptr->m_barObj->vertexBuf());
                glVertexAttribPointer(d_ptr->m_depthShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0,
                                      (void *)0);

                // Index buffer
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_ptr->m_barObj->elementBuf());

                // Draw the triangles
                glDrawElements(GL_TRIANGLES, d_ptr->m_barObj->indexCount(), GL_UNSIGNED_SHORT,
                               (void *)0);

                // Free buffers
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDisableVertexAttribArray(d_ptr->m_depthShader->posAtt());
            }
        }

        // Disable drawing to framebuffer (= enable drawing to screen)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Release depth shader
        d_ptr->m_depthShader->release();

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
        // Reset culling to normal
        glCullFace(GL_BACK);

        // Revert to original viewport
        glViewport(d_ptr->m_sceneViewPort.x(), d_ptr->m_sceneViewPort.y(),
                   d_ptr->m_sceneViewPort.width(), d_ptr->m_sceneViewPort.height());
    }

#if 1
    // Skip selection mode drawing if we're zoomed or have no selection mode
    if (!d_ptr->m_zoomActivated && d_ptr->m_selectionMode > ModeNone) {
        // Bind selection shader
        d_ptr->m_selectionShader->bind();

        // Draw bars to selection buffer
#ifndef USE_HAX0R_SELECTION
        glBindFramebuffer(GL_FRAMEBUFFER, d_ptr->m_selectionFrameBuffer);
        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
#endif
        glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled
        for (int row = startRow; row != stopRow; row += stepRow) {
            for (int bar = startBar; bar != stopBar; bar += stepBar) {
                if (!d_ptr->m_dataSet->d_ptr->getRow(row))
                    continue;
                QDataItem *item = d_ptr->m_dataSet->d_ptr->getRow(row)->d_ptr->getItem(bar);
                if (!item)
                    continue;

                GLfloat barHeight = item->d_ptr->value() / d_ptr->m_heightNormalizer;

                if (barHeight < 0)
                    glCullFace(GL_FRONT);
                else
                    glCullFace(GL_BACK);

                QMatrix4x4 modelMatrix;
                QMatrix4x4 MVPMatrix;

                barPos = (bar + 1) * (d_ptr->m_barSpacing.width());
                rowPos = (row + 1) * (d_ptr->m_barSpacing.height());

                modelMatrix.translate((d_ptr->m_rowWidth - barPos) / d_ptr->m_scaleFactor,
                                      barHeight - d_ptr->m_yAdjustment,
                                      (d_ptr->m_columnDepth - rowPos) / d_ptr->m_scaleFactor
                                      + zComp);
                modelMatrix.scale(QVector3D(d_ptr->m_scaleX, barHeight, d_ptr->m_scaleZ));

                MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

                // TODO: Save position to qdataitem, so that we don't need to calculate it each time?

                // add +2 to avoid black
                QVector3D barColor = QVector3D((GLdouble)(row + 2)
                                               / (GLdouble)(d_ptr->m_sampleCount.second + 2),
                                               (GLdouble)(bar + 2)
                                               / (GLdouble)(d_ptr->m_sampleCount.first + 2),
                                               0.0);

                d_ptr->m_selectionShader->setUniformValue(d_ptr->m_selectionShader->MVP(),
                                                          MVPMatrix);
                d_ptr->m_selectionShader->setUniformValue(d_ptr->m_selectionShader->color(),
                                                          barColor);

#ifdef USE_HAX0R_SELECTION
                // 1st attribute buffer : vertices
                glEnableVertexAttribArray(d_ptr->m_selectionShader->posAtt());
                glBindBuffer(GL_ARRAY_BUFFER, d_ptr->m_barObj->vertexBuf());
                glVertexAttribPointer(d_ptr->m_selectionShader->posAtt(),
                                      3, GL_FLOAT, GL_FALSE, 0, (void *)0);

                // Index buffer
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_ptr->m_barObj->elementBuf());

                // Draw the triangles
                glDrawElements(GL_TRIANGLES, d_ptr->m_barObj->indexCount(),
                               GL_UNSIGNED_SHORT, (void *)0);

                // Free buffers
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                glDisableVertexAttribArray(d_ptr->m_selectionShader->posAtt());
#else
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
#endif
            }
        }
        glEnable(GL_DITHER);

        // Read color under cursor
        if (Q3DBarsPrivate::MouseOnScene == d_ptr->m_mousePressed)
            selection = Utils::getSelection(d_ptr->m_mousePos, height());

#ifndef USE_HAX0R_SELECTION
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif

        // Release selection shader
        d_ptr->m_selectionShader->release();

#if 0 // Use this if you want to see what is being drawn to the framebuffer
        glCullFace(GL_BACK);
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
        d_ptr->m_drawer->drawObject(d_ptr->m_labelShader, d_ptr->m_labelObj, true,
                                    d_ptr->m_selectionTexture);
        glDisable(GL_TEXTURE_2D);
        d_ptr->m_labelShader->release();
#endif

#ifdef USE_HAX0R_SELECTION
        // Set clear color
        QVector3D clearColor = Utils::vectorFromColor(d_ptr->m_theme->m_windowColor);
        glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), 1.0f);
        // Clear after selection
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
    }

    // Bind bar shader
    d_ptr->m_barShader->bind();

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    // Draw bars
    if (!d_ptr->m_zoomActivated && d_ptr->m_zoomSelection)
        d_ptr->m_zoomSelection->d_ptr->clear();
    bool barSelectionFound = false;
    for (int row = startRow; row != stopRow; row += stepRow) {
        for (int bar = startBar; bar != stopBar; bar += stepBar) {
            if (!d_ptr->m_dataSet->d_ptr->getRow(row))
                continue;
            QDataItem *item = d_ptr->m_dataSet->d_ptr->getRow(row)->d_ptr->getItem(bar);
            if (!item)
                continue;

            GLfloat barHeight = item->d_ptr->value() / d_ptr->m_heightNormalizer;

            if (barHeight < 0)
                glCullFace(GL_FRONT);
            else
                glCullFace(GL_BACK);

            QMatrix4x4 modelMatrix;
            QMatrix4x4 itModelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 depthMVPMatrix;

            barPos = (bar + 1) * (d_ptr->m_barSpacing.width());
            rowPos = (row + 1) * (d_ptr->m_barSpacing.height());
            modelMatrix.translate((d_ptr->m_rowWidth - barPos) / d_ptr->m_scaleFactor,
                                  barHeight - d_ptr->m_yAdjustment,
                                  (d_ptr->m_columnDepth - rowPos) / d_ptr->m_scaleFactor + zComp);
            modelMatrix.scale(QVector3D(d_ptr->m_scaleX, barHeight, d_ptr->m_scaleZ));
            itModelMatrix.scale(QVector3D(d_ptr->m_scaleX, barHeight, d_ptr->m_scaleZ));
#ifdef SHOW_DEPTH_TEXTURE_SCENE
            MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;
#else
            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
#endif
            depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;

            QVector3D baseColor = Utils::vectorFromColor(d_ptr->m_theme->m_baseColor);
            QVector3D heightColor = Utils::vectorFromColor(d_ptr->m_theme->m_heightColor)
                    * barHeight;
            QVector3D depthColor = Utils::vectorFromColor(d_ptr->m_theme->m_depthColor)
                    * (float(row) / GLfloat(d_ptr->m_sampleCount.second));

            QVector3D barColor = baseColor + heightColor + depthColor;

            GLfloat lightStrength = d_ptr->m_theme->m_lightStrength;
            if (d_ptr->m_selectionMode > ModeNone) {
                Q3DBarsPrivate::SelectionType selectionType = d_ptr->isSelected(row, bar,
                                                                                selection);
                switch (selectionType) {
                case Q3DBarsPrivate::SelectionBar: {
                    barColor = Utils::vectorFromColor(d_ptr->m_theme->m_highlightBarColor);
                    lightStrength = d_ptr->m_theme->m_highlightLightStrength;
                    //if (d_ptr->m_mousePressed) {
                    //    qDebug() << "selected object:" << barIndex << "( row:" << row + 1 << ", column:" << bar + 1 << ")";
                    //    qDebug() << "object position:" << modelMatrix.column(3).toVector3D();
                    //}
                    // Insert data to QDataItem. We have no ownership, don't delete the previous one
                    if (!d_ptr->m_zoomActivated) {
                        d_ptr->m_selectedBar = item;
                        if (d_ptr->m_dataSet->d_ptr->rowLabelItems().size() > row
                                && d_ptr->m_dataSet->d_ptr->columnLabelItems().size() > bar) {
                            d_ptr->m_selectedBar->setPosition(
                                        QPoint(d_ptr->m_dataSet->d_ptr->rowLabelItems().size()
                                               - row - 1,
                                               d_ptr->m_dataSet->d_ptr->columnLabelItems().size()
                                               - bar - 1));
                        }
                        item->d_ptr->setTranslation(modelMatrix.column(3).toVector3D());
                        barSelectionFound = true;
                        if (d_ptr->m_selectionMode >= ModeZoomRow) {
                            item->d_ptr->setTranslation(modelMatrix.column(3).toVector3D());
                            d_ptr->m_zoomSelection->addItem(item);
                        }
                    }
                    break;
                }
                case Q3DBarsPrivate::SelectionRow: {
                    // Current bar is on the same row as the selected bar
                    barColor = Utils::vectorFromColor(d_ptr->m_theme->m_highlightRowColor);
                    lightStrength = d_ptr->m_theme->m_highlightLightStrength;
                    if (!d_ptr->m_zoomActivated && ModeZoomRow == d_ptr->m_selectionMode) {
                        item->d_ptr->setTranslation(modelMatrix.column(3).toVector3D());
                        d_ptr->m_zoomSelection->addItem(item);
                        if (d_ptr->m_dataSet->d_ptr->rowLabelItems().size() > row) {
                            d_ptr->m_zoomSelection->d_ptr->setLabelItem(
                                        d_ptr->m_dataSet->d_ptr->rowLabelItems().at(
                                            d_ptr->m_dataSet->d_ptr->rowLabelItems().size()
                                            - row - 1));
                        }
                    }
                    break;
                }
                case Q3DBarsPrivate::SelectionColumn: {
                    // Current bar is on the same column as the selected bar
                    barColor = Utils::vectorFromColor(d_ptr->m_theme->m_highlightColumnColor);
                    lightStrength = d_ptr->m_theme->m_highlightLightStrength;
                    if (!d_ptr->m_zoomActivated && ModeZoomColumn == d_ptr->m_selectionMode) {
                        item->d_ptr->setTranslation(modelMatrix.column(3).toVector3D());
                        d_ptr->m_zoomSelection->addItem(item);
                        if (d_ptr->m_dataSet->d_ptr->columnLabelItems().size() > bar) {
                            d_ptr->m_zoomSelection->d_ptr->setLabelItem(
                                        d_ptr->m_dataSet->d_ptr->columnLabelItems().at(
                                            d_ptr->m_dataSet->d_ptr->columnLabelItems().size()
                                            - bar - 1));
                        }
                    }
                    break;
                }
                case Q3DBarsPrivate::SelectionNone: {
                    // Current bar is not selected, nor on a row or column
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
                                                    itModelMatrix.transposed().inverted());
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
                    d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightS(),
                                                        lightStrength);

                    // Draw the object
                    d_ptr->m_drawer->drawObject(d_ptr->m_barShader, d_ptr->m_barObj);
                }
            }
        }
    }

    // Release bar shader
    d_ptr->m_barShader->release();

    // Bind background shader
    d_ptr->m_backgroundShader->bind();

    glCullFace(GL_BACK);

    // Draw background
    if (d_ptr->m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        QMatrix4x4 depthMVPMatrix;
        QMatrix4x4 itModelMatrix;

        modelMatrix.translate(0.0f, 1.0f - d_ptr->m_yAdjustment, zComp);
        modelMatrix.scale(QVector3D(d_ptr->m_rowWidth / d_ptr->m_scaleFactor,
                                    1.0f,
                                    d_ptr->m_columnDepth / d_ptr->m_scaleFactor));
        modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);
        itModelMatrix.scale(QVector3D(d_ptr->m_rowWidth / d_ptr->m_scaleFactor,
                                      1.0f,
                                      d_ptr->m_columnDepth / d_ptr->m_scaleFactor));

#ifdef SHOW_DEPTH_TEXTURE_SCENE
        MVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;
#else
        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;
#endif
        depthMVPMatrix = depthProjectionMatrix * depthViewMatrix * modelMatrix;


        QVector3D backgroundColor = Utils::vectorFromColor(d_ptr->m_theme->m_backgroundColor);

        // Set shader bindings
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->lightP(),
                                                   lightPos);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->view(),
                                                   viewMatrix);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->model(),
                                                   modelMatrix);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->nModel(),
                                                   itModelMatrix.inverted().transposed());
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->MVP(),
                                                   MVPMatrix);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->color(),
                                                   backgroundColor);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->ambientS(),
                                                   d_ptr->m_theme->m_ambientStrength);

        if (d_ptr->m_shadowQuality > ShadowNone) {
            // Set shadow shader bindings
            d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->shadowQ(),
                                                       d_ptr->m_shadowQualityToShader);
            d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->depth(),
                                                       depthMVPMatrix);
            d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->lightS(),
                                                       d_ptr->m_theme->m_lightStrength / 10.0f);

            // Draw the object
            d_ptr->m_drawer->drawObject(d_ptr->m_backgroundShader, d_ptr->m_backgroundObj,
                                        0, d_ptr->m_depthTexture);
        } else {
            // Set shadowless shader bindings
            d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->lightS(),
                                                       d_ptr->m_theme->m_lightStrength);

            // Draw the object
            d_ptr->m_drawer->drawObject(d_ptr->m_backgroundShader, d_ptr->m_backgroundObj);
        }
    }

    // Disable textures
    glDisable(GL_TEXTURE_2D);

    // Release background shader
    d_ptr->m_backgroundShader->release();

    // Draw grid lines
    if (d_ptr->m_gridEnabled) {
        // Bind bar shader
        d_ptr->m_barShader->bind();

        // Set unchanging shader bindings
        QVector3D barColor = Utils::vectorFromColor(Qt::black);
        d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightP(), lightPos);
        d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->view(), viewMatrix);
        d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->color(), barColor);
        d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightS(),
                                            d_ptr->m_theme->m_lightStrength);
        d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->ambientS(),
                                            d_ptr->m_theme->m_ambientStrength);

        // Floor lines: rows
        for (GLfloat row = 0.0f; row <= d_ptr->m_sampleCount.second; row++) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 itModelMatrix;

            rowPos = (row + 0.5f) * (d_ptr->m_barSpacing.height());
            modelMatrix.translate(0.0f, -d_ptr->m_yAdjustment,
                                  (d_ptr->m_columnDepth - rowPos) / d_ptr->m_scaleFactor + zComp);
            modelMatrix.scale(QVector3D(d_ptr->m_rowWidth / d_ptr->m_scaleFactor, gridLineWidth,
                                        gridLineWidth));
            itModelMatrix.scale(QVector3D(d_ptr->m_rowWidth / d_ptr->m_scaleFactor, gridLineWidth,
                                          gridLineWidth));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->model(), modelMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->nModel(),
                                                itModelMatrix.inverted().transposed());
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->MVP(), MVPMatrix);

            // Draw the object
            d_ptr->m_drawer->drawObject(d_ptr->m_barShader, d_ptr->m_gridLineObj);
        }

        // Floor lines: columns
        for (GLfloat bar = 0.0f; bar <= d_ptr->m_sampleCount.first; bar++) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 itModelMatrix;

            barPos = (bar + 0.5f) * (d_ptr->m_barSpacing.width());
            modelMatrix.translate((d_ptr->m_rowWidth - barPos) / d_ptr->m_scaleFactor,
                                  -d_ptr->m_yAdjustment, zComp);
            modelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                        d_ptr->m_columnDepth / d_ptr->m_scaleFactor));
            itModelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                          d_ptr->m_columnDepth / d_ptr->m_scaleFactor));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->model(), modelMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->nModel(),
                                                itModelMatrix.inverted().transposed());
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->MVP(), MVPMatrix);

            // Draw the object
            d_ptr->m_drawer->drawObject(d_ptr->m_barShader, d_ptr->m_gridLineObj);
        }

        // Wall lines: back wall
        GLfloat heightStep = d_ptr->m_heightNormalizer / 2.5; // TODO: Replace 2.5 with a dynamic number deduced from scene?
        for (GLfloat barHeight = heightStep; barHeight <= d_ptr->m_heightNormalizer * 2.0f;
             barHeight += heightStep) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 itModelMatrix;

            if (d_ptr->m_zFlipped) {
                modelMatrix.translate(0.0f,
                                      barHeight / d_ptr->m_heightNormalizer - d_ptr->m_yAdjustment,
                                      d_ptr->m_columnDepth / d_ptr->m_scaleFactor + zComp);
            } else {
                modelMatrix.translate(0.0f,
                                      barHeight / d_ptr->m_heightNormalizer - d_ptr->m_yAdjustment,
                                      -d_ptr->m_columnDepth / d_ptr->m_scaleFactor + zComp);
            }
            modelMatrix.scale(QVector3D(d_ptr->m_rowWidth / d_ptr->m_scaleFactor, gridLineWidth,
                                        gridLineWidth));
            itModelMatrix.scale(QVector3D(d_ptr->m_rowWidth / d_ptr->m_scaleFactor, gridLineWidth,
                                          gridLineWidth));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->model(), modelMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->nModel(),
                                                itModelMatrix.inverted().transposed());
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->MVP(), MVPMatrix);

            // Draw the object
            d_ptr->m_drawer->drawObject(d_ptr->m_barShader, d_ptr->m_gridLineObj);
        }

        // Wall lines: side wall
        for (GLfloat barHeight = heightStep; barHeight <= d_ptr->m_heightNormalizer * 2.0f;
             barHeight += heightStep) {
            QMatrix4x4 modelMatrix;
            QMatrix4x4 MVPMatrix;
            QMatrix4x4 itModelMatrix;

            if (d_ptr->m_xFlipped) {
                modelMatrix.translate(d_ptr->m_rowWidth / d_ptr->m_scaleFactor,
                                      barHeight / d_ptr->m_heightNormalizer - d_ptr->m_yAdjustment,
                                      zComp);
            } else {
                modelMatrix.translate(-d_ptr->m_rowWidth / d_ptr->m_scaleFactor,
                                      barHeight / d_ptr->m_heightNormalizer - d_ptr->m_yAdjustment,
                                      zComp);
            }
            modelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                        d_ptr->m_columnDepth / d_ptr->m_scaleFactor));
            itModelMatrix.scale(QVector3D(gridLineWidth, gridLineWidth,
                                          d_ptr->m_columnDepth / d_ptr->m_scaleFactor));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

            // Set the rest of the shader bindings
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->model(), modelMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->nModel(),
                                                itModelMatrix.inverted().transposed());
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->MVP(), MVPMatrix);

            // Draw the object
            d_ptr->m_drawer->drawObject(d_ptr->m_barShader, d_ptr->m_gridLineObj);
        }

        // Release bar shader
        d_ptr->m_barShader->release();
    }

    // TODO: Draw y labels

    // Generate label textures for zoom selection if m_updateLabels is set
    if (d_ptr->m_zoomActivated && d_ptr->m_updateLabels) {
        // Create label textures
        for (int col = 0; col < d_ptr->m_zoomSelection->d_ptr->row().size(); col++) {
            QDataItem *item = d_ptr->m_zoomSelection->d_ptr->getItem(col);
            d_ptr->m_drawer->generateLabelTexture(item);
        }
    }

    // Handle zoom activation and label drawing
    if (!barSelectionFound) {
        // We have no ownership, don't delete. Just NULL the pointer.
        d_ptr->m_selectedBar = NULL;
        if (d_ptr->m_zoomActivated && Q3DBarsPrivate::MouseOnOverview == d_ptr->m_mousePressed) {
            d_ptr->m_sceneViewPort = QRect(0, 0, width(), height());
            d_ptr->m_zoomActivated = false;
        }
    } else if (d_ptr->m_selectionMode >= ModeZoomRow
               && Q3DBarsPrivate::MouseOnScene == d_ptr->m_mousePressed) {
        // Activate zoom mode
        d_ptr->m_zoomActivated = true;
        d_ptr->m_sceneViewPort = QRect(0, height() - height() / 5, width() / 5, height() / 5);

        // Create label textures
        for (int col = 0; col < d_ptr->m_zoomSelection->d_ptr->row().size(); col++) {
            QDataItem *item = d_ptr->m_zoomSelection->d_ptr->getItem(col);
            d_ptr->m_drawer->generateLabelTexture(item);
        }
    } else {
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
            if ((d_ptr->m_dataSet->d_ptr->columnLabels().size()
                 > d_ptr->m_selectedBar->d_ptr->position().y())
                    && (d_ptr->m_dataSet->d_ptr->rowLabels().size()
                        > d_ptr->m_selectedBar->d_ptr->position().x())) {
                labelText.append(QStringLiteral(" ("));
                labelText.append(d_ptr->m_dataSet->d_ptr->rowLabels().at(
                                     d_ptr->m_selectedBar->d_ptr->position().x()));
                labelText.append(QStringLiteral(", "));
                labelText.append(d_ptr->m_dataSet->d_ptr->columnLabels().at(
                                     d_ptr->m_selectedBar->d_ptr->position().y()));
                labelText.append(QStringLiteral(")"));
                //qDebug() << labelText;
            }
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
        if (d_ptr->m_dataSet->d_ptr->rowLabelItems().size() > row) {
            label->d_ptr->setLabel(d_ptr->m_dataSet->d_ptr->rowLabelItems().at(
                                       d_ptr->m_dataSet->d_ptr->rowLabelItems().size() - row - 1));
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
        if (d_ptr->m_dataSet->d_ptr->columnLabelItems().size() > bar) {
            label->d_ptr->setLabel(d_ptr->m_dataSet->d_ptr->columnLabelItems().at(
                                       d_ptr->m_dataSet->d_ptr->columnLabelItems().size()
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
}

void Q3DBars::mousePressEvent(QMouseEvent *event)
{
    // TODO: for testing shaders
    //static GLint shaderNo = 1;
    //qDebug() << "mouse button pressed" << event->button();
    if (Qt::LeftButton == event->button()) {
        if (d_ptr->m_zoomActivated) {
            //qDebug() << event->pos().x() << event->pos().y() << d_ptr->m_sceneViewPort << d_ptr->m_zoomViewPort;
            if (event->pos().x() <= d_ptr->m_sceneViewPort.width()
                    && event->pos().y() <= d_ptr->m_sceneViewPort.height()) {
                d_ptr->m_mousePressed = Q3DBarsPrivate::MouseOnOverview;
                //qDebug() << "Mouse pressed on overview";
            } else {
                d_ptr->m_mousePressed = Q3DBarsPrivate::MouseOnZoom;
                //qDebug() << "Mouse pressed on zoom";
            }
        } else {
            d_ptr->m_mousePressed = Q3DBarsPrivate::MouseOnScene;
            // update mouse positions to prevent jumping when releasing or repressing a button
            d_ptr->m_mousePos = event->pos();
            //qDebug() << "Mouse pressed on scene";
        }
    } else if (Qt::MiddleButton == event->button()) {
        // reset rotations
        d_ptr->m_mousePos = QPoint(0, 0);
    } else if (Qt::RightButton == event->button()) {
        d_ptr->m_mousePressed = Q3DBarsPrivate::MouseRotating;
        // update mouse positions to prevent jumping when releasing or repressing a button
        d_ptr->m_mousePos = event->pos();

        // TODO: testing shaders
        //        if (++shaderNo > 3)
        //            shaderNo = 1;
        //        switch (shaderNo) {
        //        case 1:
        //            d_ptr->initShaders(QStringLiteral(":/shaders/vertex")
        //                               , QStringLiteral(":/shaders/fragment"));
        //            break;
        //        case 2:
        //            d_ptr->initShaders(QStringLiteral(":/shaders/vertex")
        //                               , QStringLiteral(":/shaders/fragmentColorOnY"));
        //            break;
        //        case 3:
        //            d_ptr->initShaders(QStringLiteral(":/shaders/vertex")
        //                               , QStringLiteral(":/shaders/fragmentAmbient"));
        //            break;
        //        }
    }
    CameraHelper::updateMousePos(d_ptr->m_mousePos);
}

void Q3DBars::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "mouse button released" << event->button();
    if (Q3DBarsPrivate::MouseRotating == d_ptr->m_mousePressed) {
        // update mouse positions to prevent jumping when releasing or repressing a button
        d_ptr->m_mousePos = event->pos();
        CameraHelper::updateMousePos(event->pos());
    }
    d_ptr->m_mousePressed = Q3DBarsPrivate::MouseNone;
}

void Q3DBars::mouseMoveEvent(QMouseEvent *event)
{
    if (Q3DBarsPrivate::MouseRotating == d_ptr->m_mousePressed) {
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

void Q3DBars::wheelEvent(QWheelEvent *event)
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

void Q3DBars::resizeEvent(QResizeEvent *event)
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
    d_ptr->initDepthBuffer();
}

void Q3DBars::setBarSpecs(QSizeF thickness, QSizeF spacing, bool relative)
{
    d_ptr->m_barThickness = thickness;
    if (relative) {
        d_ptr->m_barSpacing.setWidth((thickness.width() * 2) * (spacing.width() + 1.0f));
        d_ptr->m_barSpacing.setHeight((thickness.height() * 2) * (spacing.height() + 1.0f));
    } else {
        d_ptr->m_barSpacing = thickness * 2 + spacing * 2;
    }
    // Calculate here and at setting sample space
    d_ptr->calculateSceneScalingFactors();
}

void Q3DBars::setBarType(BarStyle style, bool smooth)
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
    }
    // Reload mesh data
    if (d_ptr->m_isInitialized)
        d_ptr->loadBarMesh();
}

void Q3DBars::setMeshFileName(const QString &objFileName)
{
    d_ptr->m_objFile = objFileName;
}

void Q3DBars::setupSampleSpace(int samplesRow, int samplesColumn, const QString &labelRow,
                               const QString &labelColumn, const QString &labelHeight)
{
    // Delete previous data set
    delete d_ptr->m_dataSet;
    d_ptr->m_dataSet = new QDataSet();
    d_ptr->m_sampleCount = qMakePair(samplesRow, samplesColumn);
    d_ptr->m_dataSet->setLabels(labelRow, labelColumn, labelHeight);
    // TODO: Invent "idiotproof" max scene size formula..
    // This seems to work ok if spacing is not negative (and row/column or column/row ratio is not too high)
    d_ptr->m_maxSceneSize = 2 * qSqrt(samplesRow * samplesColumn);
    //qDebug() << "maxSceneSize" << d_ptr->m_maxSceneSize;
    // Calculate here and at setting bar specs
    d_ptr->calculateSceneScalingFactors();
    d_ptr->m_axisLabelX = labelRow;
    d_ptr->m_axisLabelZ = labelColumn;
    d_ptr->m_axisLabelY = labelHeight;
}

void Q3DBars::setCameraPreset(CameraPreset preset)
{
    CameraHelper::setCameraPreset(preset);
}

void Q3DBars::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    d_ptr->m_horizontalRotation = qBound(-180.0f, horizontal, 180.0f);
    d_ptr->m_verticalRotation = qBound(0.0f, vertical, 90.0f);
    d_ptr->m_zoomLevel = qBound(10, distance, 500);
    CameraHelper::setCameraRotation(QPointF(d_ptr->m_horizontalRotation,
                                            d_ptr->m_verticalRotation));
    //qDebug() << "camera rotation set to" << d_ptr->m_horizontalRotation << d_ptr->m_verticalRotation;
}

void Q3DBars::setTheme(ColorTheme theme)
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
}

void Q3DBars::setBarColor(QColor baseColor, QColor heightColor, QColor depthColor, bool uniform)
{
    d_ptr->m_theme->m_baseColor = baseColor;
    d_ptr->m_theme->m_heightColor = heightColor;
    d_ptr->m_theme->m_depthColor = depthColor;
    //qDebug() << "colors:" << d_ptr->m_baseColor << d_ptr->m_heightColor << d_ptr->m_depthColor;
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

void Q3DBars::setSelectionMode(SelectionMode mode)
{
    d_ptr->m_selectionMode = mode;
    // Disable zoom if mode changes
    d_ptr->m_zoomActivated = false;
    d_ptr->m_sceneViewPort = QRect(0, 0, width(), height());
    // Create zoom selection if there isn't one
    if (mode >= ModeZoomRow && !d_ptr->m_zoomSelection)
        d_ptr->m_zoomSelection = new QDataRow();
}

void Q3DBars::setWindowTitle(const QString &title)
{
    setTitle(title);
}

void Q3DBars::setFontSize(float fontsize)
{
    d_ptr->m_font.setPointSizeF(fontsize);
    d_ptr->m_drawer->setFont(d_ptr->m_font);
}

void Q3DBars::setFont(const QFont &font)
{
    d_ptr->m_font = font;
    d_ptr->m_drawer->setFont(font);
}

void Q3DBars::setLabelTransparency(LabelTransparency transparency)
{
    d_ptr->m_labelTransparency = transparency;
    d_ptr->m_drawer->setTransparency(transparency);
}

void Q3DBars::setGridEnabled(bool enable)
{
    d_ptr->m_gridEnabled = enable;
}


void Q3DBars::setShadowQuality(ShadowQuality quality)
{
    d_ptr->m_shadowQuality = quality;
    switch (quality) {
    case ShadowLow:
        d_ptr->m_shadowQualityToShader = 33.3f;
        break;
    case ShadowMedium:
        d_ptr->m_shadowQualityToShader = 66.7f;
        break;
    case ShadowHigh:
        d_ptr->m_shadowQualityToShader = 100.0f;
        break;
    default:
        d_ptr->m_shadowQualityToShader = 0.0f;
        break;
    }
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
                                     QStringLiteral(":/shaders/fragmentShadowNoTex"));
    } else {
        // Re-init shaders
        if (!d_ptr->m_theme->m_uniformColor) {
            d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                               QStringLiteral(":/shaders/fragmentColorOnY"));
        } else {
            d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                               QStringLiteral(":/shaders/fragment"));
        }
        d_ptr->initBackgroundShaders(QStringLiteral(":/shaders/vertex"),
                                     QStringLiteral(":/shaders/fragment"));
    }
}

void Q3DBars::addDataRow(const QVector<float> &dataRow, const QString &labelRow,
                         const QVector<QString> &labelsColumn)
{
    // Convert to QDataRow and add to QDataSet
    QDataRow *row = new QDataRow(labelRow);
    for (int i = 0; i < dataRow.size(); i++)
        row->addItem(new QDataItem(dataRow.at(i)));
    row->d_ptr->verifySize(d_ptr->m_sampleCount.first);
    d_ptr->m_dataSet->addRow(row);
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_dataSet->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
    d_ptr->m_dataSet->setLabels(d_ptr->m_axisLabelX, d_ptr->m_axisLabelZ, d_ptr->m_axisLabelY,
                                QVector<QString>(), labelsColumn);
    d_ptr->m_dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.second);
}

void Q3DBars::addDataRow(const QVector<QDataItem*> &dataRow, const QString &labelRow,
                         const QVector<QString> &labelsColumn)
{
    // Convert to QDataRow and add to QDataSet
    QDataRow *row = new QDataRow(labelRow);
    for (int i = 0; i < dataRow.size(); i++)
        row->addItem(dataRow.at(i));
    row->d_ptr->verifySize(d_ptr->m_sampleCount.first);
    d_ptr->m_dataSet->addRow(row);
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_dataSet->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
    d_ptr->m_dataSet->setLabels(d_ptr->m_axisLabelX, d_ptr->m_axisLabelZ, d_ptr->m_axisLabelY,
                                QVector<QString>(), labelsColumn);
    d_ptr->m_dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.second);
}

void Q3DBars::addDataRow(QDataRow *dataRow)
{
    QDataRow *row = dataRow;
    // Check that the input data fits into sample space, and resize if it doesn't
    row->d_ptr->verifySize(d_ptr->m_sampleCount.first);
    // With each new row, the previous data row must be moved back
    // ie. we need as many vectors as we have rows in the sample space
    d_ptr->m_dataSet->addRow(row);
    // if the added data pushed us over sample space, remove the oldest data set
    d_ptr->m_dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.second);
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_dataSet->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
}

void Q3DBars::addDataSet(const QVector< QVector<float> > &data, const QVector<QString> &labelsRow,
                         const QVector<QString> &labelsColumn)
{
    // Copy axis labels
    QString xAxis;
    QString zAxis;
    QString yAxis;
    d_ptr->m_dataSet->d_ptr->axisLabels(&xAxis, &zAxis, &yAxis);
    // Delete old data set
    delete d_ptr->m_dataSet;
    d_ptr->m_dataSet = new QDataSet();
    // Give drawer to data set
    d_ptr->m_dataSet->d_ptr->setDrawer(d_ptr->m_drawer);
    // Convert to QDataRow and add to QDataSet
    QDataRow *row;
    for (int rowNr = 0; rowNr < data.size(); rowNr++) {
        if (labelsRow.size() >= (rowNr + 1))
            row = new QDataRow(labelsRow.at(rowNr));
        else
            row = new QDataRow();
        for (int colNr = 0; colNr < data.at(rowNr).size(); colNr++)
            row->addItem(new QDataItem(data.at(rowNr).at(colNr)));
        row->d_ptr->verifySize(d_ptr->m_sampleCount.first);
        d_ptr->m_dataSet->addRow(row);
        row++;
    }
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_dataSet->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
    d_ptr->m_dataSet->setLabels(xAxis, zAxis, yAxis, labelsRow, labelsColumn);
    d_ptr->m_dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.second);
}

void Q3DBars::addDataSet(const QVector< QVector<QDataItem*> > &data,
                         const QVector<QString> &labelsRow,
                         const QVector<QString> &labelsColumn)
{
    // Copy axis labels
    QString xAxis;
    QString zAxis;
    QString yAxis;
    d_ptr->m_dataSet->d_ptr->axisLabels(&xAxis, &zAxis, &yAxis);
    // Delete old data set
    delete d_ptr->m_dataSet;
    d_ptr->m_dataSet = new QDataSet();
    // Give drawer to data set
    d_ptr->m_dataSet->d_ptr->setDrawer(d_ptr->m_drawer);
    // Convert to QDataRow and add to QDataSet
    QDataRow *row;
    for (int rowNr = 0; rowNr < data.size(); rowNr++) {
        if (labelsRow.size() >= (rowNr + 1))
            row = new QDataRow(labelsRow.at(rowNr));
        else
            row = new QDataRow();
        for (int colNr = 0; colNr < data.at(rowNr).size(); colNr++)
            row->addItem(data.at(rowNr).at(colNr));
        row->d_ptr->verifySize(d_ptr->m_sampleCount.first);
        d_ptr->m_dataSet->addRow(row);
        row++;
    }
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_dataSet->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
    d_ptr->m_dataSet->setLabels(xAxis, zAxis, yAxis, labelsRow, labelsColumn);
    d_ptr->m_dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.second);
}

void Q3DBars::addDataSet(QDataSet* dataSet)
{
    delete d_ptr->m_dataSet;
    // Check sizes
    dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.second, d_ptr->m_sampleCount.first);
    // Take ownership of given set
    d_ptr->m_dataSet = dataSet;
    // Find highest value
    // Get the limits
    QPair<GLfloat, GLfloat> limits = d_ptr->m_dataSet->d_ptr->limitValues();
    d_ptr->m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    d_ptr->calculateHeightAdjustment(limits);
    // Give drawer to data set
    d_ptr->m_dataSet->d_ptr->setDrawer(d_ptr->m_drawer);
}

Q3DBarsPrivate::Q3DBarsPrivate(Q3DBars *q)
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
      m_sampleCount(0, 0),
      m_objFile(QStringLiteral(":/defaultMeshes/bar")),
      m_mousePressed(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_zoomLevel(100),
      m_zoomAdjustment(1.0f),
      m_horizontalRotation(-45.0f),
      m_verticalRotation(15.0f),
      m_barThickness(QSizeF(0.75f, 0.75f)),
      m_barSpacing(m_barThickness * 3.0f),
      m_heightNormalizer(0.0f),
      m_yAdjustment(0.0f),
      m_rowWidth(0),
      m_columnDepth(0),
      m_maxDimension(0),
      m_scaleX(0),
      m_scaleZ(0),
      m_scaleFactor(0),
      m_maxSceneSize(40.0),
      m_theme(new Theme()),
      m_isInitialized(false),
      m_selectionMode(ModeBar),
      m_selectedBar(0),
      m_zoomSelection(0),
      m_dataSet(new QDataSet()),
      m_axisLabelX(QStringLiteral("X")),
      m_axisLabelZ(QStringLiteral("Z")),
      m_axisLabelY(QStringLiteral("Y")),
      m_sceneViewPort(0, 0, q->width(), q->height()),
      m_zoomViewPort(0, 0, q->width(), q->height()),
      m_zoomActivated(false),
      m_textureHelper(new TextureHelper()),
      m_labelTransparency(TransparencyNone),
      m_font(QFont(QStringLiteral("Arial"))),
      m_drawer(new Drawer(*m_theme, m_font, m_labelTransparency)),
      m_xFlipped(false),
      m_zFlipped(false),
      m_bgrTexture(0),
      m_depthTexture(0),
      m_selectionTexture(0),
      m_depthFrameBuffer(0),
      m_selectionFrameBuffer(0),
      m_selectionDepthBuffer(0),
      m_updateLabels(false),
      m_gridEnabled(true),
      m_shadowQuality(ShadowLow),
      m_shadowQualityToShader(33.3f)
{
    m_dataSet->d_ptr->setDrawer(m_drawer);
    QObject::connect(m_drawer, &Drawer::drawerChanged, this, &Q3DBarsPrivate::updateTextures);
}

Q3DBarsPrivate::~Q3DBarsPrivate()
{
    qDebug() << "Destroying Q3DBarsPrivate";
#ifndef USE_HAX0R_SELECTION
    m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);
    m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
    m_textureHelper->deleteTexture(&m_selectionTexture);
#endif
    m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
    m_textureHelper->deleteTexture(&m_bgrTexture);
    delete m_dataSet;
    if (m_zoomSelection) {
        m_zoomSelection->d_ptr->clear();
        delete m_zoomSelection;
    }
    delete m_barShader;
    delete m_depthShader;
    delete m_selectionShader;
    delete m_backgroundShader;
    delete m_barObj;
    delete m_backgroundObj;
    delete m_gridLineObj;
    delete m_textureHelper;
    delete m_drawer;
}

void Q3DBarsPrivate::loadBarMesh()
{
    if (m_barObj)
        delete m_barObj;
    m_barObj = new ObjectHelper(m_objFile);
    m_barObj->load();
}

void Q3DBarsPrivate::loadBackgroundMesh()
{
    if (m_backgroundObj)
        delete m_backgroundObj;
    m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/background"));
    m_backgroundObj->load();
}

void Q3DBarsPrivate::loadGridLineMesh()
{
    if (m_gridLineObj)
        delete m_gridLineObj;
    m_gridLineObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/bar"));
    m_gridLineObj->load();
}

void Q3DBarsPrivate::loadLabelMesh()
{
    if (m_labelObj)
        delete m_labelObj;
    m_labelObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/label"));
    m_labelObj->load();
}

void Q3DBarsPrivate::initShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_barShader)
        delete m_barShader;
    m_barShader = new ShaderHelper(q_ptr, vertexShader, fragmentShader);
    m_barShader->initialize();
}

void Q3DBarsPrivate::initSelectionShader()
{
    if (m_selectionShader)
        delete m_selectionShader;
    m_selectionShader = new ShaderHelper(q_ptr, QStringLiteral(":/shaders/vertexSelection"),
                                         QStringLiteral(":/shaders/fragmentSelection"));
    m_selectionShader->initialize();
}

void Q3DBarsPrivate::initDepthShader()
{
    if (m_depthShader)
        delete m_depthShader;
    m_depthShader = new ShaderHelper(q_ptr, QStringLiteral(":/shaders/vertexDepth"),
                                     QStringLiteral(":/shaders/fragmentDepth"));
    m_depthShader->initialize();
}

void Q3DBarsPrivate::initSelectionBuffer()
{
#ifndef USE_HAX0R_SELECTION
    if (m_selectionTexture) {
        m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);
        m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
        m_textureHelper->deleteTexture(&m_selectionTexture);
    }
    m_selectionTexture = m_textureHelper->createSelectionTexture(q_ptr->size(),
                                                                 m_selectionFrameBuffer,
                                                                 m_selectionDepthBuffer);
#endif
}

void Q3DBarsPrivate::initDepthBuffer()
{
    if (m_depthTexture) {
        m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
        m_textureHelper->deleteTexture(&m_depthTexture);
    }
    m_depthTexture = m_textureHelper->createDepthTexture(q_ptr->size(), m_depthFrameBuffer,
                                                         m_shadowQuality);
}

void Q3DBarsPrivate::initBackgroundShaders(const QString &vertexShader,
                                           const QString &fragmentShader)
{
    if (m_backgroundShader)
        delete m_backgroundShader;
    m_backgroundShader = new ShaderHelper(q_ptr, vertexShader, fragmentShader);
    m_backgroundShader->initialize();
}

void Q3DBarsPrivate::initLabelShaders(const QString &vertexShader, const QString &fragmentShader)
{
    if (m_labelShader)
        delete m_labelShader;
    m_labelShader = new ShaderHelper(q_ptr, vertexShader, fragmentShader);
    m_labelShader->initialize();
}

void Q3DBarsPrivate::updateTextures()
{
    // Drawer has changed; this flag needs to be checked when checking if we need to update labels
    m_updateLabels = true;
}

void Q3DBarsPrivate::calculateSceneScalingFactors()
{
    // Calculate scene scaling and translation factors
    m_rowWidth = ((m_sampleCount.first + 1) * m_barSpacing.width()) / 2.0f;
    m_columnDepth = ((m_sampleCount.second + 1) * m_barSpacing.height()) / 2.0f;
    m_maxDimension = qMax(m_rowWidth, m_columnDepth);
    m_scaleFactor = qMin((m_sampleCount.first * (m_maxDimension / m_maxSceneSize)),
                         (m_sampleCount.second * (m_maxDimension / m_maxSceneSize)));
    m_scaleX = m_barThickness.width() / m_scaleFactor;
    m_scaleZ = m_barThickness.height() / m_scaleFactor;
    //qDebug() << "m_scaleX" << m_scaleX << "m_scaleFactor" << m_scaleFactor;
    //qDebug() << "m_scaleZ" << m_scaleZ << "m_scaleFactor" << m_scaleFactor;
    //qDebug() << "m_rowWidth:" << m_rowWidth << "m_columnDepth:" << m_columnDepth << "m_maxDimension:" << m_maxDimension;
}

void Q3DBarsPrivate::calculateHeightAdjustment(const QPair<GLfloat, GLfloat> &limits)
{
    // 2.0f = max difference between minimum and maximum value after scaling with m_heightNormalizer
    m_yAdjustment = 2.0f - ((limits.second - limits.first) / m_heightNormalizer);
    //qDebug() << m_yAdjustment;
}

Q3DBarsPrivate::SelectionType Q3DBarsPrivate::isSelected(GLint row, GLint bar,
                                                         const QVector3D &selection)
{
    //static QVector3D prevSel = selection; // TODO: For debugging
    SelectionType isSelectedType = SelectionNone;
#ifdef USE_HAX0R_SELECTION
    if (selection == Utils::vectorFromColor(m_theme->m_windowColor))
#else
    if (selection == Utils::vectorFromColor(Qt::white))
#endif
        return isSelectedType; // skip window
    QVector3D current = QVector3D((GLubyte)(((GLdouble)(row + 2) / (GLdouble)(m_sampleCount.second + 2))
                                            * 255.0 + 0.49), // +0.49 to fix rounding (there are conversions from unsigned short to GLdouble and back)
                                  (GLubyte)(((GLdouble)(bar + 2) / (GLdouble)(m_sampleCount.first + 2))
                                            * 255.0 + 0.49), // +0.49 to fix rounding (there are conversions from unsigned short to GLdouble and back)
                                  0);
    // TODO: For debugging
    //if (selection != prevSel) {
    //    qDebug() << selection.x() << selection .y() << selection.z();
    //    prevSel = selection;
    //}
    if (current == selection)
        isSelectedType = SelectionBar;
    else if (current.y() == selection.y() && (m_selectionMode == ModeBarAndColumn
                                              || m_selectionMode == ModeBarRowAndColumn
                                              || m_selectionMode == ModeZoomColumn))
        isSelectedType = SelectionColumn;
    else if (current.x() == selection.x() && (m_selectionMode == ModeBarAndRow
                                              || m_selectionMode == ModeBarRowAndColumn
                                              || m_selectionMode == ModeZoomRow))
        isSelectedType = SelectionRow;
    return isSelectedType;
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE
