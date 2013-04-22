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

#define DISPLAY_RENDER_SPEED

#ifdef DISPLAY_RENDER_SPEED
#include <QTime>
#endif

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

//#define USE_HAX0R_SELECTION // keep this defined until the "real" method works
//#define USE_PAINTER_TEXT // Use QPainter labels or opengl labels
#define DISPLAY_FULL_DATA_ON_SELECTION // Append selection value text with row and column labels

const GLfloat zComp = 10.0f; // Compensation for z position; move all objects to positive z, as shader can't handle negative values correctly
const QVector3D defaultLightPos = QVector3D(0.0f, 3.0f, zComp);

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
    if (!d_ptr->m_theme->m_uniformColor) {
        d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                           QStringLiteral(":/shaders/fragmentColorOnY"));
    } else {
        d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                           QStringLiteral(":/shaders/fragment"));
    }
    //d_ptr->initBackgroundShaders(QStringLiteral(":/shaders/vertexTexture"),
    //                             QStringLiteral(":/shaders/fragmentTexture"));
    d_ptr->initBackgroundShaders(QStringLiteral(":/shaders/vertex"),
                                 QStringLiteral(":/shaders/fragment"));
#ifndef USE_PAINTER_TEXT
    d_ptr->initLabelShaders(QStringLiteral(":/shaders/vertexLabel"),
                            QStringLiteral(":/shaders/fragmentLabel"));
#endif
    d_ptr->initSelectionShader();

#ifndef USE_HAX0R_SELECTION
    // Init the selection buffer
    d_ptr->initSelectionBuffer();
#endif

    // Load default mesh
    d_ptr->loadBarMesh();

    // Load background mesh
    d_ptr->loadBackgroundMesh();

#ifndef USE_PAINTER_TEXT
    // Load label mesh
    d_ptr->loadLabelMesh();
#endif

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

#ifdef USE_PAINTER_TEXT
    if (d_ptr->m_paintDevice) {
        QPainter painter(d_ptr->m_paintDevice);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        render(&painter);
        painter.end();
    } else {
        d_ptr->m_paintDevice = getDevice();
    }
#else
    // If zoom selection is on, draw zoom scene
    drawZoomScene();
    // Draw bars scene
    drawScene();
#endif
}

void Q3DBars::render(QPainter *painter)
{
#ifdef USE_PAINTER_TEXT
    painter->beginNativePainting();
    // Set OpenGL features
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Do native OpenGL rendering
    // If zoom selection is on, draw zoom scene
    drawZoomScene();
    // Draw bars scene
    drawScene();
    painter->endNativePainting();

    // If a bar is selected, display it's value
    QDataItem *data = d_ptr->m_selectedBar;
    if (d_ptr->m_selectionMode < ZoomRow && data) {
        glDisable(GL_DEPTH_TEST);
        Utils::printText(painter, data->d_ptr->valueStr(), data->d_ptr->labelSize()); // use size for screen position; this way we don't need 2 member variables in qdataitem
    } else if (d_ptr->m_zoomActivated) {
        glDisable(GL_DEPTH_TEST);
        GLfloat scale = 1.0f;
        //painter->setWindow(d_ptr->m_zoomViewPort); // Using these causes unwanted transformations
        //painter->setViewport(d_ptr->m_zoomViewPort); // Using these causes unwanted transformations
        //painter->setViewTransformEnabled(false); // Using these causes unwanted transformations
        for (int col = 0; col < d_ptr->m_zoomSelection->d_ptr->row().size(); col++) {
            // print value of each column
            QDataItem *item = d_ptr->m_zoomSelection->d_ptr->getItem(col);
            // we need to convert 3D coordinates to screen coordinates for printing
            // posX = (2 * X - W) / H   ->   coordX = (posX * H + W) / 2
            // posY = 1 - (2 * Y) / H   ->   coordY = ((1 - posY) * H) / 2
            GLfloat coordX;
            if (ZoomColumn == d_ptr->m_selectionMode) {
                coordX = (-(item->d_ptr->translation().z() - zComp) // flip front to left
                          * d_ptr->m_zoomViewPort.height() / 2.0f
                          + d_ptr->m_zoomViewPort.width()) / 2.0f;
            } else {
                coordX = (item->d_ptr->translation().x()
                          * d_ptr->m_zoomViewPort.height() / 2.0f
                          + d_ptr->m_zoomViewPort.width()) / 2.0f;
            }
            if (d_ptr->m_zoomViewPort.height() > d_ptr->m_zoomViewPort.width()) {
                scale = (GLfloat)d_ptr->m_zoomViewPort.width()
                        / (GLfloat)d_ptr->m_zoomViewPort.height();
            }
            //float coordY = ((1.0f - item->d_ptr->translation().y())
            //                * d_ptr->m_zoomViewPort.height()) / 2.0f;
            // Use a fixed label distance from the bottom of the screen
            QSize screenCoords(coordX, 150.0f); // use coord Y for reducing from painter window height to avoid unwanted transformations
            Utils::printText(painter, item->d_ptr->valueStr(), screenCoords, false, 60.0f, scale);
            //QPoint screenCoords(coordX, d_ptr->m_zoomViewPort.height() - 100.0f);
            // Use a label distance from the bottom of the screen based on bar height
            //QPoint screenCoords(coordX, coordY - (d_ptr->m_zoomViewPort.height() / 4.0f)
            //                    * (item->d_ptr->value() / d_ptr->m_heightNormalizer));
            //qDebug() << "bar" << col << "position on screen" << screenCoords << item->d_ptr->valueStr()
            //         << "viewport:" << d_ptr->m_zoomViewPort.width() << d_ptr->m_zoomViewPort.height();
            //Utils::printText(painter, item->d_ptr->valueStr(), screenCoords, true, 60.0f);
        }
    }
#endif
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
                                                              d_ptr->m_zoomLevel,
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
#if 1
    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, 5.0f + zComp),
                      QVector3D(0.0f, 0.0f, zComp),
                      QVector3D(0.0f, 1.0f, 0.0f));
#else
    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, d_ptr->m_scaleFactorX + zComp),
                      QVector3D(0.0f, 0.0f, zComp),
                      QVector3D(0.0f, 1.0f, 0.0f));
    GLfloat zoomwidth;
    if (ZoomRow == d_ptr->m_selectionMode) {
        zoomwidth = d_ptr->m_zoomSelection->d_ptr->getItem(0)->d_ptr->translation().x()
                - d_ptr->m_zoomSelection->d_ptr->getItem(d_ptr->m_zoomSelection->d_ptr->row().size()
                                                         - 1)->d_ptr->translation().x();
        zoomwidth *= d_ptr->m_scaleX;
    } else {
        zoomwidth = d_ptr->m_zoomSelection->d_ptr->getItem(0)->d_ptr->translation().z()
                - d_ptr->m_zoomSelection->d_ptr->getItem(d_ptr->m_zoomSelection->d_ptr->row().size()
                                                         - 1)->d_ptr->translation().z();
        zoomwidth *= d_ptr->m_scaleZ;
    }
    qDebug() << "zoomwidth" << zoomwidth << "inverse / 2" << 0.5f / zoomwidth;
    viewMatrix.scale(0.5f / zoomwidth);
#endif
    if (d_ptr->m_zoomViewPort.height() > d_ptr->m_zoomViewPort.width()) {
        viewMatrix.scale((GLfloat)d_ptr->m_zoomViewPort.width()
                         / (GLfloat)d_ptr->m_zoomViewPort.height());
        // TODO: Center shrunk view
    }

    // Set light position a bit above the camera (depends on do we have row or column zoom)
    if (ZoomColumn == d_ptr->m_selectionMode)
        lightPos = CameraHelper::calculateLightPosition(defaultLightPos, -85.0f);
    else
        lightPos = CameraHelper::calculateLightPosition(defaultLightPos, 5.0f);
#endif

    // Bind bar shader
    d_ptr->m_barShader->bind();

    // Draw bars
    //    bool barSelectionFound = false;
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

        if (ZoomRow == d_ptr->m_selectionMode)
            barPosX = item->d_ptr->translation().x();
        else
            barPosX = -(item->d_ptr->translation().z() - zComp); // flip z; frontmost bar to the left
        modelMatrix.translate(barPosX,
                              item->d_ptr->translation().y(), // TODO: Needs a better system; calculate y position modifier somehow (based on highest bar height?)
                              zComp);
        modelMatrix.scale(QVector3D(d_ptr->m_scaleX, barHeight, d_ptr->m_scaleZ));

        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

        QVector3D baseColor = Utils::vectorFromColor(d_ptr->m_theme->m_baseColor);
        QVector3D heightColor = Utils::vectorFromColor(d_ptr->m_theme->m_heightColor) * barHeight;

        QVector3D barColor = baseColor + heightColor;

        GLfloat lightStrength = d_ptr->m_theme->m_lightStrength;
#if 0 // TODO: Implement selection in zoom
        if (d_ptr->m_selectionMode > None) {
            Q3DBarsPrivate::SelectionType selectionType = d_ptr->isSelected(row, bar, selection);
            switch (selectionType) {
            case Q3DBarsPrivate::Bar:
            {
                // highlight bar by inverting the color of the bar
                //barColor = QVector3D(1.0f, 1.0f, 1.0f) - barColor;
                barColor = Utils::vectorFromColor(d_ptr->m_highlightBarColor);
                lightStrength = d_ptr->m_highlightLightStrength;
                //if (d_ptr->m_mousePressed) {
                //    qDebug() << "selected object:" << barIndex << "( row:" << row + 1 << ", column:" << bar + 1 << ")";
                //    qDebug() /*<< barIndex*/ << "object position:" << modelMatrix.column(3).toVector3D();
                //}
                // Insert data to QDataItem. We have no ownership, don't delete the previous one
                d_ptr->m_selectedBar = item;
                d_ptr->m_selectedBar->d_ptr->setPosition(d_ptr->m_mousePos);
                barSelectionFound = true;
                break;
            }
            case Q3DBarsPrivate::Row:
            {
                // Current bar is on the same row as the selected bar
                barColor = Utils::vectorFromColor(d_ptr->m_highlightRowColor);
                lightStrength = d_ptr->m_highlightLightStrength;
                break;
            }
            case Q3DBarsPrivate::Column:
            {
                // Current bar is on the same column as the selected bar
                barColor = Utils::vectorFromColor(d_ptr->m_highlightColumnColor);
                lightStrength = d_ptr->m_highlightLightStrength;
                break;
            }
            case Q3DBarsPrivate::None:
            {
                // Current bar is not selected, nor on a row or column
                // do nothing
                break;
            }
            }
        }
#endif
        if (barHeight != 0) {
            // Set shader bindings
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightP(), lightPos);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->view(), viewMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->model(), modelMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->nModel(),
                                                modelMatrix.inverted().transposed());
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->MVP(), MVPMatrix);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->color(), barColor);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightS(), lightStrength);
            d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->ambientS(),
                                                d_ptr->m_theme->m_ambientStrength);

            // Draw the object
            d_ptr->m_drawer->drawObject(d_ptr->m_barShader, d_ptr->m_barObj);
        }
    }
#if 0
    if (!barSelectionFound) {
        // We have no ownership, don't delete. Just NULL the pointer.
        d_ptr->m_selectedBar = NULL;
        if (d_ptr->m_zoomActivated && Q3DBarsPrivate::MouseOnOverview == d_ptr->m_mousePressed) {
            d_ptr->m_sceneViewPort = QRect(0, 0, width(), height());
            d_ptr->m_zoomActivated = false;
        }
    } else if (d_ptr->m_selectionMode >= ZoomRow
               && Q3DBarsPrivate::MouseOnScene == d_ptr->m_mousePressed) {
        d_ptr->m_zoomActivated = true;
        d_ptr->m_sceneViewPort = QRect(0, height() - height() / 5,
                                       width() / 5, height() / 5);
    }
#endif
    // Release bar shader
    d_ptr->m_barShader->release();

#ifndef USE_PAINTER_TEXT
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
    if (ZoomRow == d_ptr->m_selectionMode) {
        drawLabel(*dummyItem, zoomSelectionLabel, viewMatrix, projectionMatrix, false, 0.0f,
                  LabelTop);
        drawLabel(*dummyItem, z, viewMatrix, projectionMatrix, false, 0.0f, LabelBottom);
    } else {
        drawLabel(*dummyItem, x, viewMatrix, projectionMatrix, false, 0.0f, LabelBottom);
        drawLabel(*dummyItem, zoomSelectionLabel, viewMatrix, projectionMatrix, false, 0.0f,
                  LabelTop);
    }
    drawLabel(*dummyItem, y, viewMatrix, projectionMatrix, false, 90.0f, LabelLeft);

    // Draw labels for bars
    for (int col = 0; col < d_ptr->m_zoomSelection->d_ptr->row().size(); col++) {
        QDataItem *item = d_ptr->m_zoomSelection->d_ptr->getItem(col);
        // Draw values
        drawLabel(*item, item->d_ptr->label(), viewMatrix, projectionMatrix, false, 0.0f,
                  LabelOver);
        // Draw labels
        LabelItem labelItem;
        if (ZoomRow == d_ptr->m_selectionMode) {
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
        drawLabel(*item, labelItem, viewMatrix, projectionMatrix, false, -45.0f, LabelBelow);
    }

    glDisable(GL_TEXTURE_2D);
    if (d_ptr->m_labelTransparency > TransparencyNone)
        glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // Release label shader
    d_ptr->m_labelShader->release();
#endif
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
                                                              d_ptr->m_zoomLevel,
                                                              d_ptr->m_sceneViewPort.width(),
                                                              d_ptr->m_sceneViewPort.height());
    if (d_ptr->m_sceneViewPort.height() > d_ptr->m_sceneViewPort.width()) {
        viewMatrix.scale((GLfloat)d_ptr->m_sceneViewPort.width()
                         / (GLfloat)d_ptr->m_sceneViewPort.height());
        // TODO: Center shrunk view
    }

    // Calculate drawing order
    //qDebug() << "viewMatrix z" << viewMatrix.row(0).z(); // jos negatiivinen, käännä bar -piirtojärjestys
    //qDebug() << "viewMatrix x" << viewMatrix.row(0).x(); // jos negatiivinen, käännä row -piirtojärjestys
    // Draw order is reversed to optimize amount of drawing (ie. draw front objects first, depth test handles not needing to draw objects behind them)
    if (viewMatrix.row(0).x() > 0) {
        startRow = 0;
        stopRow = d_ptr->m_sampleCount.y();
        stepRow = 1;
        d_ptr->m_zFlipped = false;
    } else {
        startRow = d_ptr->m_sampleCount.y() - 1;
        stopRow = -1;
        stepRow = -1;
        d_ptr->m_zFlipped = true;
    }
    if (viewMatrix.row(0).z() < 0) {
        startBar = 0;
        stopBar = d_ptr->m_sampleCount.x();
        stepBar = 1;
        d_ptr->m_xFlipped = false;
    } else {
        startBar = d_ptr->m_sampleCount.x() - 1;
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

    // Skip selection mode drawing if we're zoomed or have no selection mode
    if (!d_ptr->m_zoomActivated && d_ptr->m_selectionMode > None) {
        // Bind selection shader
        d_ptr->m_selectionShader->bind();

        // Draw bars to selection buffer
#ifndef USE_HAX0R_SELECTION
        glBindFramebuffer(GL_FRAMEBUFFER, d_ptr->m_selectionFrameBuffer);
        glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
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

                barPos = (bar + 1) * (d_ptr->m_barSpacing.x());
                rowPos = (row + 1) * (d_ptr->m_barSpacing.y());

                modelMatrix.translate((d_ptr->m_rowWidth - barPos) / d_ptr->m_scaleFactorX,
                                      barHeight - 1.0f, // TODO: Doesn't work right with negative values; calculate a variable based on bar heights instead of using -1.0f
                                      (d_ptr->m_columnDepth - rowPos) / d_ptr->m_scaleFactorZ
                                      + zComp);
                modelMatrix.scale(QVector3D(d_ptr->m_scaleX, barHeight, d_ptr->m_scaleZ));

                MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

                // TODO: Save position to qdataitem, so that we don't need to calculate it each time?

                // add +2 to avoid black
                QVector3D barColor = QVector3D((GLfloat)(row + 2)
                                               / (GLfloat)(d_ptr->m_sampleCount.y() + 2),
                                               (GLfloat)(bar + 2)
                                               / (GLfloat)(d_ptr->m_sampleCount.x() + 2),
                                               0.0f);

                d_ptr->m_selectionShader->setUniformValue(d_ptr->m_selectionShader->MVP(),
                                                          MVPMatrix);
                d_ptr->m_selectionShader->setUniformValue(d_ptr->m_selectionShader->color(),
                                                          barColor);

#ifdef USE_HAX0R_SELECTION
                // 1st attribute buffer : vertices
                glEnableVertexAttribArray(d_ptr->m_selectionShader->posAtt());
                glBindBuffer(GL_ARRAY_BUFFER, d_ptr->m_barObj->vertexBuf());
                glVertexAttribPointer(d_ptr->m_selectionShader->posAtt(),
                                      3, GL_FLOAT, GL_FALSE, 0, (void*)0);

                // Index buffer
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_ptr->m_barObj->elementBuf());

                // Draw the triangles
                glDrawElements(GL_TRIANGLES, d_ptr->m_barObj->indexCount(),
                               GL_UNSIGNED_SHORT, (void*)0);

                // Free buffers
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                glDisableVertexAttribArray(d_ptr->m_selectionShader->posAtt());
#else
                // 1st attribute buffer : vertices
                glEnableVertexAttribArray(d_ptr->m_selectionShader->posAtt());
                glBindBuffer(GL_ARRAY_BUFFER, d_ptr->m_barObj->vertexBuf());
                glVertexAttribPointer(d_ptr->m_selectionShader->posAtt(),
                                      3, GL_FLOAT, GL_FALSE, 0, (void*)0);

                // Index buffer
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_ptr->m_barObj->elementBuf());

                // Draw the triangles
                glDrawElements(GL_TRIANGLES, d_ptr->m_barObj->indexCount(), GL_UNSIGNED_SHORT,
                               (void*)0);

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
        // Clear after selection
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
    }

    // Bind bar shader
    d_ptr->m_barShader->bind();

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
            QMatrix4x4 MVPMatrix;
            // TODO: Laske rivi- ja sarakelabelien paikat (sijainnit: min-1 ja max+1) ja pistä johonki talteen?
            barPos = (bar + 1) * (d_ptr->m_barSpacing.x());
            rowPos = (row + 1) * (d_ptr->m_barSpacing.y());
            modelMatrix.translate((d_ptr->m_rowWidth - barPos) / d_ptr->m_scaleFactorX,
                                  barHeight - 1.0f, // TODO: Doesn't work right with negative values; calculate a variable based on bar heights instead of using -1.0f
                                  (d_ptr->m_columnDepth - rowPos) / d_ptr->m_scaleFactorZ + zComp);
            modelMatrix.scale(QVector3D(d_ptr->m_scaleX, barHeight, d_ptr->m_scaleZ));

            MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

            QVector3D baseColor = Utils::vectorFromColor(d_ptr->m_theme->m_baseColor);
            QVector3D heightColor = Utils::vectorFromColor(d_ptr->m_theme->m_heightColor)
                    * barHeight;
            QVector3D depthColor = Utils::vectorFromColor(d_ptr->m_theme->m_depthColor)
                    * (float(row) / GLfloat(d_ptr->m_sampleCount.y()));

            QVector3D barColor = baseColor + heightColor + depthColor;

            GLfloat lightStrength = d_ptr->m_theme->m_lightStrength;
            if (d_ptr->m_selectionMode > None) {
                Q3DBarsPrivate::SelectionType selectionType = d_ptr->isSelected(row, bar,
                                                                                selection);
                switch (selectionType) {
                case Q3DBarsPrivate::Bar:
                {
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
                            d_ptr->m_selectedBar->d_ptr->setPosition(
                                        QPoint(d_ptr->m_dataSet->d_ptr->rowLabelItems().size()
                                               - row - 1,
                                               d_ptr->m_dataSet->d_ptr->columnLabelItems().size()
                                               - bar - 1));
                        }
#ifdef USE_PAINTER_TEXT
                        QSize mousePositionAsSize = QSize(d_ptr->m_mousePos.x(),
                                                          d_ptr->m_mousePos.y());
                        d_ptr->m_selectedBar->d_ptr->setLabelSize(mousePositionAsSize);
#else
                        item->d_ptr->setTranslation(modelMatrix.column(3).toVector3D());
#endif
                        barSelectionFound = true;
                        if (d_ptr->m_selectionMode >= ZoomRow) {
                            item->d_ptr->setTranslation(modelMatrix.column(3).toVector3D());
                            d_ptr->m_zoomSelection->addItem(item);
                        }
                    }
                    break;
                }
                case Q3DBarsPrivate::Row:
                {
                    // Current bar is on the same row as the selected bar
                    barColor = Utils::vectorFromColor(d_ptr->m_theme->m_highlightRowColor);
                    lightStrength = d_ptr->m_theme->m_highlightLightStrength;
                    if (!d_ptr->m_zoomActivated && ZoomRow == d_ptr->m_selectionMode) {
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
                case Q3DBarsPrivate::Column:
                {
                    // Current bar is on the same column as the selected bar
                    barColor = Utils::vectorFromColor(d_ptr->m_theme->m_highlightColumnColor);
                    lightStrength = d_ptr->m_theme->m_highlightLightStrength;
                    if (!d_ptr->m_zoomActivated && ZoomColumn == d_ptr->m_selectionMode) {
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
                case Q3DBarsPrivate::None:
                {
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
                                                    modelMatrix.inverted().transposed());
                d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->MVP(), MVPMatrix);
                d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->color(), barColor);
                d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->lightS(), lightStrength);
                d_ptr->m_barShader->setUniformValue(d_ptr->m_barShader->ambientS(),
                                                    d_ptr->m_theme->m_ambientStrength);

                // Draw the object
                d_ptr->m_drawer->drawObject(d_ptr->m_barShader, d_ptr->m_barObj);
            }
        }
    }

    // Release bar shader
    d_ptr->m_barShader->release();

    // Bind background shader
    d_ptr->m_backgroundShader->bind();

    // TODO: If we want to use background texture, we should create it in initBackground instead of here and keep the texture id in d_ptr
    // Create texture
    //glEnable(GL_TEXTURE_2D);
    //GLuint bgrTexture = d_ptr->m_textureHelper->create2DTexture(
    //            QImage(QStringLiteral(":/textures/cubetex")), true);

    glCullFace(GL_BACK);

    // Draw background
    if (d_ptr->m_backgroundObj) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 MVPMatrix;
        if (zComp != 0)
            modelMatrix.translate(0.0f, 0.0f, zComp);
        modelMatrix.scale(QVector3D(d_ptr->m_rowWidth * d_ptr->m_sceneScale,
                                    1.0f,
                                    d_ptr->m_columnDepth * d_ptr->m_sceneScale));
        modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);

        MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

        QVector3D backgroundColor = Utils::vectorFromColor(d_ptr->m_theme->m_backgroundColor);

        // Set shader bindings
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->lightP(),
                                                   lightPos);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->view(),
                                                   viewMatrix);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->model(),
                                                   modelMatrix);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->nModel(),
                                                   modelMatrix.inverted().transposed());
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->MVP(),
                                                   MVPMatrix);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->color(),
                                                   backgroundColor);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->lightS(),
                                                   d_ptr->m_theme->m_lightStrength);
        d_ptr->m_backgroundShader->setUniformValue(d_ptr->m_backgroundShader->ambientS(),
                                                   d_ptr->m_theme->m_ambientStrength);

        // Draw the object
        d_ptr->m_drawer->drawObject(d_ptr->m_backgroundShader, d_ptr->m_backgroundObj);
        //d_ptr->m_drawer->drawObject(d_ptr->m_backgroundShader, d_ptr->m_backgroundObj, true, bgrTexture);
    }

    // Disable textures
    //glBindTexture(GL_TEXTURE_2D, 0);
    //glDeleteTextures(1, &bgrTexture);  // TODO: If we want to use background texture, we should create it in initBackground and delete on exit
    //glDisable(GL_TEXTURE_2D);

    // Release background shader
    d_ptr->m_backgroundShader->release();

    // Handle zoom activation and label drawing
    if (!barSelectionFound) {
        // We have no ownership, don't delete. Just NULL the pointer.
        d_ptr->m_selectedBar = NULL;
        if (d_ptr->m_zoomActivated && Q3DBarsPrivate::MouseOnOverview == d_ptr->m_mousePressed) {
            d_ptr->m_sceneViewPort = QRect(0, 0, width(), height());
            d_ptr->m_zoomActivated = false;
        }
    } else if (d_ptr->m_selectionMode >= ZoomRow
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
#ifndef USE_PAINTER_TEXT
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
        if (prevItem != d_ptr->m_selectedBar) {
            d_ptr->m_drawer->generateLabelTexture(d_ptr->m_selectedBar);
            prevItem = d_ptr->m_selectedBar;
        }

        drawLabel(*d_ptr->m_selectedBar, d_ptr->m_selectedBar->d_ptr->label(), viewMatrix,
                  projectionMatrix, true);
#else
        static bool firstSelection = true;
        // Draw the value string followed by row label and column label
        LabelItem labelItem = d_ptr->m_selectedBar->d_ptr->selectionLabel();
        if (firstSelection || prevItem != d_ptr->m_selectedBar) {
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

        drawLabel(*d_ptr->m_selectedBar, labelItem, viewMatrix, projectionMatrix, true);
#endif
        glDisable(GL_TEXTURE_2D);
        if (d_ptr->m_labelTransparency > TransparencyNone)
            glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // Release label shader
        d_ptr->m_labelShader->release();
#endif
    }
}

void Q3DBars::drawLabel(const QDataItem &item, const LabelItem &label,
                        const QMatrix4x4 &viewmatrix, const QMatrix4x4 &projectionmatrix,
                        bool useDepth, qreal rotation, Q3DBars::LabelPosition position)
{
    // Draw label
    LabelItem labelItem = label;
    if (!labelItem.textureId())
        return; // No texture, skip

    QSize textureSize = labelItem.size();
    QMatrix4x4 modelMatrix;
    QMatrix4x4 MVPMatrix;
    GLfloat xPosition;
    GLfloat yPosition;
    GLfloat zPosition = zComp;

    switch (position) {
    case Q3DBars::LabelBelow:
    {
        yPosition = -1.25f;
        //if (!useDepth)
        //  yPosition -= 0.25f;
        break;
    }
    case Q3DBars::LabelLow:
    {
        yPosition = 0.0f;
        break;
    }
    case Q3DBars::LabelMid:
    {
        yPosition = item.d_ptr->translation().y();
        break;
    }
    case Q3DBars::LabelHigh:
    {
        yPosition = item.d_ptr->translation().y() + (item.d_ptr->value()
                                                     / d_ptr->m_heightNormalizer) / 2.0f;
        break;
    }
    case Q3DBars::LabelOver:
    {
        yPosition = item.d_ptr->translation().y()
                + (item.d_ptr->value() / d_ptr->m_heightNormalizer) + 0.1f;
        //if (!useDepth)
        //  yPosition -= 0.25f;
        break;
    }
    case Q3DBars::LabelBottom:
    {
        yPosition = -1.9f; // TODO: Calculate from scene
        xPosition = 0.0f;
        break;
    }
    case Q3DBars::LabelTop:
    {
        yPosition = 1.5f; // TODO: Calculate from scene
        xPosition = 0.0f;
        break;
    }
    case Q3DBars::LabelLeft:
    {
        yPosition = -0.2f; // TODO: Calculate from scene
        xPosition = -2.5f; // TODO: Calculate from scene
        break;
    }
    case Q3DBars::LabelRight:
    {
        yPosition = -0.2f; // TODO: Calculate from scene
        xPosition = 2.5f; // TODO: Calculate from scene
        break;
    }
    }

    if (position < LabelBottom) {
        xPosition = item.d_ptr->translation().x();
        if (useDepth)
            zPosition = item.d_ptr->translation().z();
        else if (ZoomColumn == d_ptr->m_selectionMode)
            xPosition = -(item.d_ptr->translation().z()) + zComp; // flip first to left
    }

    // Position label
    modelMatrix.translate(xPosition, yPosition, zPosition);

    // Rotate
    modelMatrix.rotate(rotation, 0.0f, 0.0f, 1.0f);

    if (useDepth) {
        // Apply negative camera rotations to keep labels facing camera
        QPointF rotations = CameraHelper::getCameraRotations();
        modelMatrix.rotate(-rotations.x(), 0.0f, 1.0f, 0.0f);
        modelMatrix.rotate(-rotations.y(), 1.0f, 0.0f, 0.0f);
    }

    // Calculate scale factor to get uniform font size
    GLfloat scaledFontSize = 0.05f + d_ptr->m_fontSize / 500.0f;
    GLfloat scaleFactor = scaledFontSize / (GLfloat)textureSize.height();

    // Scale label based on text size
    modelMatrix.scale(QVector3D((GLfloat)textureSize.width() * scaleFactor
                                , scaledFontSize
                                , 0.0f));

    MVPMatrix = projectionmatrix * viewmatrix * modelMatrix;

    // Set shader bindings
    d_ptr->m_labelShader->setUniformValue(d_ptr->m_labelShader->MVP(), MVPMatrix);

    // Draw the object
    d_ptr->m_drawer->drawObject(d_ptr->m_labelShader, d_ptr->m_labelObj, true,
                                labelItem.textureId());
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
}

void Q3DBars::setBarSpecs(QPointF thickness, QPointF spacing, bool relative)
{
    d_ptr->m_barThickness = thickness;
    if (relative) {
        d_ptr->m_barSpacing.setX((thickness.x() * 2) * (spacing.x() + 1.0f));
        d_ptr->m_barSpacing.setY((thickness.y() * 2) * (spacing.y() + 1.0f));
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

void Q3DBars::setupSampleSpace(QPoint sampleCount, const QString &labelRow,
                               const QString &labelColumn, const QString &labelHeight)
{
    d_ptr->m_sampleCount = sampleCount;
    d_ptr->m_dataSet->setLabels(labelRow, labelColumn, labelHeight);
    // TODO: Invent "idiotproof" max scene size formula..
    // This seems to work ok if spacing is not negative
    d_ptr->m_maxSceneSize = 2 * qSqrt(sampleCount.x() * sampleCount.y());
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
    qDebug() << "camera rotation set to" << d_ptr->m_horizontalRotation << d_ptr->m_verticalRotation;
}

void Q3DBars::setTheme(ColorTheme theme)
{
    d_ptr->m_theme->useTheme(theme);
    d_ptr->m_drawer->setTheme(*d_ptr->m_theme);
    // Re-initialize shaders
    if (!d_ptr->m_theme->m_uniformColor) {
        d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                           QStringLiteral(":/shaders/fragmentColorOnY"));
    } else {
        d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                           QStringLiteral(":/shaders/fragment"));
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
            d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                               QStringLiteral(":/shaders/fragmentColorOnY"));
        } else {
            d_ptr->initShaders(QStringLiteral(":/shaders/vertex"),
                               QStringLiteral(":/shaders/fragment"));
        }
    }
    d_ptr->m_theme->m_uniformColor = uniform;
}

void Q3DBars::setSelectionMode(SelectionMode mode)
{
    d_ptr->m_selectionMode = mode;
    if (mode >= ZoomRow && !d_ptr->m_zoomSelection)
        d_ptr->m_zoomSelection = new QDataRow();
}

void Q3DBars::setWindowTitle(const QString &title)
{
    setTitle(title);
}

void Q3DBars::setFontSize(float fontsize)
{
    d_ptr->m_fontSize = fontsize;
}

void Q3DBars::setFont(const QFont &font)
{
    d_ptr->m_font = font;
    d_ptr->m_fontSize = font.pointSizeF();
    d_ptr->m_drawer->setFont(font);
}

void Q3DBars::setLabelTransparency(LabelTransparency transparency)
{
    d_ptr->m_labelTransparency = transparency;
    d_ptr->m_drawer->setTransparency(transparency);
}

void Q3DBars::addDataRow(const QVector<float> &dataRow, const QString &labelRow,
                         const QVector<QString> &labelsColumn)
{
    // Convert to QDataRow and add to QDataSet
    QDataRow *row = new QDataRow(labelRow);
    for (int i = 0; i < dataRow.size(); i++)
        row->addItem(new QDataItem(dataRow.at(i)));
    row->d_ptr->verifySize(d_ptr->m_sampleCount.x());
    d_ptr->m_dataSet->addRow(row);
    d_ptr->m_heightNormalizer = d_ptr->m_dataSet->d_ptr->highestValue();
    d_ptr->m_dataSet->setLabels(d_ptr->m_axisLabelX, d_ptr->m_axisLabelZ, d_ptr->m_axisLabelY,
                                QVector<QString>(), labelsColumn);
    d_ptr->m_dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.y());
}

void Q3DBars::addDataRow(const QVector<QDataItem*> &dataRow, const QString &labelRow,
                         const QVector<QString> &labelsColumn)
{
    // Convert to QDataRow and add to QDataSet
    QDataRow *row = new QDataRow(labelRow);
    for (int i = 0; i < dataRow.size(); i++)
        row->addItem(dataRow.at(i));
    row->d_ptr->verifySize(d_ptr->m_sampleCount.x());
    d_ptr->m_dataSet->addRow(row);
    d_ptr->m_heightNormalizer = d_ptr->m_dataSet->d_ptr->highestValue();
    d_ptr->m_dataSet->setLabels(d_ptr->m_axisLabelX, d_ptr->m_axisLabelZ, d_ptr->m_axisLabelY,
                                QVector<QString>(), labelsColumn);
    d_ptr->m_dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.y());
}

void Q3DBars::addDataRow(QDataRow *dataRow)
{
    QDataRow *row = dataRow;
    // Check that the input data fits into sample space, and resize if it doesn't
    row->d_ptr->verifySize(d_ptr->m_sampleCount.x());
    d_ptr->m_heightNormalizer = row->d_ptr->highestValue();
    // With each new row, the previous data row must be moved back
    // ie. we need as many vectors as we have rows in the sample space
    d_ptr->m_dataSet->addRow(row);
    // if the added data pushed us over sample space, remove the oldest data set
    d_ptr->m_dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.y());
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
        row->d_ptr->verifySize(d_ptr->m_sampleCount.x());
        d_ptr->m_dataSet->addRow(row);
        row++;
    }
    d_ptr->m_heightNormalizer = d_ptr->m_dataSet->d_ptr->highestValue();
    d_ptr->m_dataSet->setLabels(xAxis, zAxis, yAxis, labelsRow, labelsColumn);
    d_ptr->m_dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.y());
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
        row->d_ptr->verifySize(d_ptr->m_sampleCount.x());
        d_ptr->m_dataSet->addRow(row);
        row++;
    }
    d_ptr->m_heightNormalizer = d_ptr->m_dataSet->d_ptr->highestValue();
    d_ptr->m_dataSet->setLabels(xAxis, zAxis, yAxis, labelsRow, labelsColumn);
    d_ptr->m_dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.y());
}

void Q3DBars::addDataSet(QDataSet* dataSet)
{
    delete d_ptr->m_dataSet;
    // Check sizes
    dataSet->d_ptr->verifySize(d_ptr->m_sampleCount.y(), d_ptr->m_sampleCount.x());
    // Take ownership of given set
    d_ptr->m_dataSet = dataSet;
    // Find highest value
    d_ptr->m_heightNormalizer = d_ptr->m_dataSet->d_ptr->highestValue();
    // Give drawer to data set
    d_ptr->m_dataSet->d_ptr->setDrawer(d_ptr->m_drawer);
}

Q3DBarsPrivate::Q3DBarsPrivate(Q3DBars *q)
    : q_ptr(q),
      m_paintDevice(0),
      m_barShader(0),
      m_selectionShader(0),
      m_backgroundShader(0),
      m_labelShader(0),
      m_barObj(0),
      m_backgroundObj(0),
      m_labelObj(0),
      m_sampleCount(QPoint(0, 0)),
      m_objFile(QStringLiteral(":/defaultMeshes/bar")),
      m_mousePressed(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_zoomLevel(100),
      m_horizontalRotation(-45.0f),
      m_verticalRotation(15.0f),
      m_barThickness(QPointF(0.75f, 0.75f)),
      m_barSpacing(m_barThickness * 3.0f),
      m_heightNormalizer(0.0f),
      m_rowWidth(0),
      m_columnDepth(0),
      m_maxDimension(0),
      m_scaleX(0),
      m_scaleZ(0),
      m_scaleFactorX(0),
      m_scaleFactorZ(0),
      m_sceneScale(0),
      m_maxSceneSize(40.0),
      m_theme(new Theme()),
      m_isInitialized(false),
      m_selectionMode(Q3DBars::Bar),
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
      m_labelTransparency(Q3DBars::TransparencyNone),
      m_fontSize(10.0f),
      m_font(QFont(QStringLiteral("Arial"))),
      m_drawer(new Drawer(*m_theme, m_font, m_labelTransparency)),
      m_xFlipped(false),
      m_zFlipped(false),
      m_selectionFrameBuffer(0),
      m_selectionDepthBuffer(0),
      m_selectionTexture(0)
{
    m_dataSet->d_ptr->setDrawer(m_drawer);
}

Q3DBarsPrivate::~Q3DBarsPrivate()
{
    qDebug() << "Destroying Q3DBarsPrivate";
#ifndef USE_HAX0R_SELECTION
    m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);
    m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
    m_textureHelper->deleteTexture(&m_selectionTexture);
    //m_textureHelper->deleteTexture(&m_selectionDepthTexture);
#endif
    delete m_dataSet;
    if (m_zoomSelection) {
        m_zoomSelection->d_ptr->clear();
        delete m_zoomSelection;
    }
    delete m_barShader;
    delete m_selectionShader;
    delete m_backgroundShader;
    delete m_barObj;
    delete m_backgroundObj;
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

void Q3DBarsPrivate::initSelectionBuffer()
{
#ifndef USE_HAX0R_SELECTION
    m_selectionTexture = m_textureHelper->createSelectionTexture(q_ptr->size(),
                                                                 m_selectionFrameBuffer,
                                                                 m_selectionDepthBuffer);
#endif
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

void Q3DBarsPrivate::calculateSceneScalingFactors()
{
    // Calculate scene scaling and translation factors
    m_rowWidth = ((m_sampleCount.x() + 1) * m_barSpacing.x()) / 2.0f;
    m_columnDepth = ((m_sampleCount.y() + 1) * m_barSpacing.y()) / 2.0f;
    m_maxDimension = qMax(m_rowWidth, m_columnDepth);
    m_scaleX = m_barThickness.x() / m_sampleCount.x() * (m_maxSceneSize / m_maxDimension);
    m_scaleZ = m_barThickness.y() / m_sampleCount.x() * (m_maxSceneSize / m_maxDimension);
    m_sceneScale = qMin(m_scaleX, m_scaleZ);
    GLfloat minThickness = qMin(m_barThickness.x(), m_barThickness.y());
    m_sceneScale = m_sceneScale / minThickness;
    m_scaleFactorX = m_sampleCount.x() * (m_maxDimension / m_maxSceneSize);
    m_scaleFactorZ = m_sampleCount.x() * (m_maxDimension / m_maxSceneSize);
    //qDebug() << "m_scaleX" << m_scaleX << "m_scaleFactorX" << m_scaleFactorX;
    //qDebug() << "m_scaleZ" << m_scaleZ << "m_scaleFactorZ" << m_scaleFactorZ;
    //qDebug() << "m_rowWidth:" << m_rowWidth << "m_columnDepth:" << m_columnDepth << "m_maxDimension:" << m_maxDimension;
    //qDebug() << m_rowWidth * m_sceneScale << m_columnDepth * m_sceneScale;
}

Q3DBarsPrivate::SelectionType Q3DBarsPrivate::isSelected(GLint row, GLint bar,
                                                         const QVector3D &selection)
{
    SelectionType isSelectedType = None;
    if (selection == Utils::vectorFromColor(m_theme->m_windowColor))
        return isSelectedType; // skip window
    QVector3D current = QVector3D((GLubyte)(((GLfloat)(row + 2) / (GLfloat)(m_sampleCount.y() + 2))
                                            * 255.0 + 0.49f), // +0.49 to fix rounding (there are conversions from unsigned short to GLfloat and back)
                                  (GLubyte)(((GLfloat)(bar + 2) / (GLfloat)(m_sampleCount.x() + 2))
                                            * 255.0 + 0.49f), // +0.49 to fix rounding (there are conversions from unsigned short to GLfloat and back)
                                  0);
    if (current == selection)
        isSelectedType = Bar;
    else if (current.y() == selection.y() && (m_selectionMode == Q3DBars::BarAndColumn
                                              || m_selectionMode == Q3DBars::BarRowAndColumn
                                              || m_selectionMode == Q3DBars::ZoomColumn))
        isSelectedType = Column;
    else if (current.x() == selection.x() && (m_selectionMode == Q3DBars::BarAndRow
                                              || m_selectionMode == Q3DBars::BarRowAndColumn
                                              || m_selectionMode == Q3DBars::ZoomRow))
        isSelectedType = Row;
    return isSelectedType;
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE
