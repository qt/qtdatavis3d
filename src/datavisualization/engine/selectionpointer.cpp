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

#include "selectionpointer_p.h"
#include "surface3dcontroller_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"
#include "q3dcamera.h"
#include "q3dcamera_p.h"
#include "drawer_p.h"
#include "utils_p.h"
#include "q3dlight.h"

#include <QImage>
#include <QMatrix4x4>

#include <QDebug>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

const GLfloat sliceUnits = 2.5;

SelectionPointer::SelectionPointer(Drawer *drawer)
    : QObject(0),
      m_labelShader(0),
      m_pointShader(0),
      m_labelObj(0),
      m_pointObj(0),
      m_textureHelper(0),
      m_isInitialized(false),
      m_cachedTheme(drawer->theme()),
      m_labelStyle(QDataVis::LabelStyleFromTheme),
      m_drawer(drawer),
      m_cachedScene(0)
{
    initializeOpenGL();

    QObject::connect(m_drawer, &Drawer::drawerChanged,
                     this, &SelectionPointer::handleDrawerChange);
}

SelectionPointer::~SelectionPointer()
{
    delete m_labelShader;
    delete m_pointShader;
    delete m_labelObj;
    delete m_pointObj;
    delete m_textureHelper;
}

void SelectionPointer::initializeOpenGL()
{
    if (m_isInitialized)
        return;

    initializeOpenGLFunctions();

    m_textureHelper = new TextureHelper();
    m_drawer->initializeOpenGL();

    initShaders();

    loadLabelMesh();
    loadPointMesh();

    // Set initialized -flag
    m_isInitialized = true;
}

void SelectionPointer::updateScene(Q3DScene *scene)
{
    m_cachedScene = scene;
}

void SelectionPointer::render(GLuint defaultFboHandle)
{
    Q_UNUSED(defaultFboHandle)

    glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
               m_mainViewPort.width(), m_mainViewPort.height());

    Q3DCamera *camera = m_cachedScene->activeCamera();
    QSize textureSize = m_labelItem.size();

    QMatrix4x4 itModelMatrix;

    // Get view matrix
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projectionMatrix;
    if (m_cachedIsSlicingActivated) {
        GLfloat aspect = (GLfloat)m_mainViewPort.width() / (GLfloat)m_mainViewPort.height();
        viewMatrix.lookAt(QVector3D(0.0f, 0.0f, 1.0f), zeroVector, upVector);
        projectionMatrix.ortho(-sliceUnits * aspect, sliceUnits * aspect,
                               -sliceUnits, sliceUnits, -1.0f, 4.0f);
    } else {
        viewMatrix = camera->viewMatrix();
        projectionMatrix.perspective(45.0f, (GLfloat)m_mainViewPort.width()
                                     / (GLfloat)m_mainViewPort.height(), 0.1f, 100.0f);
    }

    // Calculate scale factor to get uniform font size
    GLfloat scaledFontSize = 0.05f + m_drawer->font().pointSizeF() / 500.0f;
    GLfloat scaleFactor = scaledFontSize / (GLfloat)textureSize.height();

    QMatrix4x4 modelMatrix;
    QMatrix4x4 MVPMatrix;

    // Position the pointer ball
    modelMatrix.translate(m_position);

    // Scale the point with fixed values (at this point)
    modelMatrix.scale(QVector3D(0.05f, 0.05f, 0.05f));

    MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    QVector3D lightPos =  m_cachedScene->activeLight()->position();

    //
    // Draw the point
    //
    m_pointShader->bind();
    m_pointShader->setUniformValue(m_pointShader->lightP(), lightPos);
    m_pointShader->setUniformValue(m_pointShader->view(), viewMatrix);
    m_pointShader->setUniformValue(m_pointShader->model(), modelMatrix);
    m_pointShader->setUniformValue(m_pointShader->nModel(), itModelMatrix.inverted().transposed());
    m_pointShader->setUniformValue(m_pointShader->color(),
                                   Utils::vectorFromColor(m_cachedTheme.m_highlightBarColor));
    m_pointShader->setUniformValue(m_pointShader->MVP(), MVPMatrix);
    m_pointShader->setUniformValue(m_pointShader->ambientS(), m_cachedTheme.m_ambientStrength);
    m_pointShader->setUniformValue(m_pointShader->lightS(), m_cachedTheme.m_lightStrength * 2.0f);

    m_drawer->drawObject(m_pointShader, m_pointObj);

    m_pointShader->release();

    //
    // Draw the label
    //
    QMatrix4x4 modelMatrixLabel;

    // Position label
    QVector3D labelAlign(0.0f, 1.0f * scaledFontSize + 0.05f, 0.0f);
    modelMatrixLabel.translate(m_position + labelAlign);

    // Position the label towards the camera
    qreal camRotationsX = camera->xRotation();
    qreal camRotationsY = camera->yRotation();
    if (!m_cachedIsSlicingActivated) {
        modelMatrixLabel.rotate(-camRotationsX, 0.0f, 1.0f, 0.0f);
        modelMatrixLabel.rotate(-camRotationsY, 1.0f, 0.0f, 0.0f);
    }

    // Scale label based on text size
    modelMatrixLabel.scale(QVector3D((GLfloat)textureSize.width() * scaleFactor,
                                     scaledFontSize,
                                     0.0f));

    // Make label to be always on top
    glDisable(GL_DEPTH_TEST);

    // Make label transparent
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_labelShader->bind();

    // Set shader bindings
    MVPMatrix = projectionMatrix * viewMatrix * modelMatrixLabel;
    m_labelShader->setUniformValue(m_labelShader->MVP(), MVPMatrix);

    // Draw the object
    m_drawer->drawObject(m_labelShader, m_labelObj, m_labelItem.textureId());

    m_labelShader->release();

    // Disable textures
    glDisable(GL_TEXTURE_2D);

    // Disable transparency
    glDisable(GL_BLEND);

    // Depth test back to normal
    glEnable(GL_DEPTH_TEST);
}

void SelectionPointer::setPosition(QVector3D position)
{
    m_position = position;
}

void SelectionPointer::updateSliceData(bool sliceActivated, GLfloat autoScaleAdjustment)
{
    m_cachedIsSlicingActivated = sliceActivated;
    m_autoScaleAdjustment = autoScaleAdjustment;
}

void SelectionPointer::setLabel(QString label)
{
    m_label = label;

    m_drawer->generateLabelItem(m_labelItem, m_label);
}

void SelectionPointer::handleDrawerChange()
{
    m_cachedTheme = m_drawer->theme();
    setLabel(m_label);
}

void SelectionPointer::updateBoundingRect(QRect rect)
{
    m_mainViewPort = rect;
}

void SelectionPointer::initShaders()
{
    // The shader for printing the text label
    if (m_labelShader)
        delete m_labelShader;
    m_labelShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexLabel"),
                                     QStringLiteral(":/shaders/fragmentLabel"));
    m_labelShader->initialize();

    // The shader for the small point ball
    if (m_pointShader)
        delete m_pointShader;
#if !defined(QT_OPENGL_ES_2)
    m_pointShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertex"),
                                     QStringLiteral(":/shaders/fragment"));
#else
    m_pointShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertexES2"),
                                     QStringLiteral(":/shaders/fragmentES2"));
#endif
    m_pointShader->initialize();

}

void SelectionPointer::loadLabelMesh()
{
    if (m_labelObj)
        delete m_labelObj;
    m_labelObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/label"));
    m_labelObj->load();
}

void SelectionPointer::loadPointMesh()
{
    if (m_pointObj)
        delete m_pointObj;
    m_pointObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/sphereSmooth"));
    m_pointObj->load();
}

QT_DATAVISUALIZATION_END_NAMESPACE
