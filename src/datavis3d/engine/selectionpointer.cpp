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

#include "selectionpointer_p.h"
#include "surface3dcontroller_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"
#include "camerahelper_p.h"
#include "drawer_p.h"
#include "utils_p.h"

#include <QImage>
#include <QMatrix4x4>

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

SelectionPointer::SelectionPointer(Surface3dController *controller)
    : QObject(controller),
      m_controller(controller),
      m_labelShader(0),
      m_pointShader(0),
      m_labelObj(0),
      m_pointObj(0),
      m_textureHelper(0),
      m_isInitialized(false),
      m_font(QFont(QStringLiteral("Arial"))),
      m_labelTransparency(QDataVis::TransparencyFromTheme),
      m_drawer(new Drawer(m_cachedTheme, m_font, m_labelTransparency))
{
    initializeOpenGL();
}

SelectionPointer::~SelectionPointer()
{
    delete m_labelShader;
    delete m_pointShader;
    delete m_labelObj;
    delete m_pointObj;
    delete m_textureHelper;
    delete m_drawer;
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

    updateTheme(m_controller->theme());

    // Set initialized -flag
    m_isInitialized = true;
}

void SelectionPointer::render(CameraHelper *camera, const GLuint defaultFboHandle)
{
    Q_UNUSED(defaultFboHandle)

    QSize textureSize = m_labelItem.size();

    QMatrix4x4 itModelMatrix;

    // Calculate view matrix
    QMatrix4x4 viewMatrix = m_controller->calculateViewMatrix(
                100.0f, //TODO: m_zoomLevel * m_autoScaleAdjustment
                m_mainViewPort.width(),
                m_mainViewPort.height(),
                false/*m_hasNegativeValues*/);

    itModelMatrix.scale(m_scale);

    // Calculate scale factor to get uniform font size
    GLfloat scaledFontSize = 0.05f + m_font.pointSizeF() / 500.0f;
    GLfloat scaleFactor = scaledFontSize / (GLfloat)textureSize.height();

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, (GLfloat)m_mainViewPort.width()
                                 / (GLfloat)m_mainViewPort.height(), 0.1f, 100.0f);

    QMatrix4x4 modelMatrix;
    QMatrix4x4 MVPMatrix;

    // Position the pointer ball
    modelMatrix.translate(m_position * m_scale + QVector3D(0.0f, 0.0f, zComp));

    // Scale the point with fixed values (at this point)
    modelMatrix.scale(QVector3D(0.05f, 0.05f, 0.05f));

    MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    QVector3D lightPos = camera->calculateLightPosition(defaultLightPos);

    //
    // Draw the point
    //
    m_pointShader->bind();
    m_pointShader->setUniformValue(m_pointShader->lightP(), lightPos);
    m_pointShader->setUniformValue(m_pointShader->view(), viewMatrix);
    m_pointShader->setUniformValue(m_pointShader->model(), modelMatrix);
    m_pointShader->setUniformValue(m_pointShader->nModel(), itModelMatrix.inverted().transposed());
    m_pointShader->setUniformValue(m_pointShader->color(), QVector3D(1.0f, 0.0f, 0.0f));
    m_pointShader->setUniformValue(m_pointShader->MVP(), MVPMatrix);
    m_pointShader->setUniformValue(m_pointShader->ambientS(), m_cachedTheme.m_ambientStrength);
    m_pointShader->setUniformValue(m_pointShader->lightS(), m_cachedTheme.m_lightStrength * 2.0f);

    m_drawer->drawObject(m_pointShader, m_pointObj);

    m_pointShader->release();

    //
    // Draw the label
    //
    if (m_labelItem.textureId()) {
        QMatrix4x4 modelMatrixLabel;

        // Position label
        QVector3D labelAlign(0.0f, 1.0f * scaledFontSize + 0.05f, 0.0f);
        modelMatrixLabel.translate(m_position * m_scale + labelAlign + QVector3D(0.0f, 0.0f, zComp));

        // Position the label towards the camera
        QPointF camRotations = camera->getCameraRotations();
        modelMatrixLabel.rotate(-camRotations.x(), 0.0f, 1.0f, 0.0f);
        modelMatrixLabel.rotate(-camRotations.y(), 1.0f, 0.0f, 0.0f);

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
}

void SelectionPointer::setPosition(QVector3D position)
{
    m_position = position;
}

void SelectionPointer::setScaling(QVector3D scaling)
{
    m_scale = scaling;
}

void SelectionPointer::setLabel(QString label)
{
    m_labelItem.clear();

    // Print label into a QImage
    QImage image = Utils::printTextToImage(m_font,
                                           label,
                                           m_cachedTheme.m_textBackgroundColor,
                                           m_cachedTheme.m_textColor,
                                           m_labelTransparency);
    //label.save("C:\\Users\\misalmel\\Work\\gerrit\\qtdatavis3d_2\\notification.png");

    // Set label size
    m_labelItem.setSize(image.size());
    // Insert text texture into label (also deletes the old texture)
    m_labelItem.setTextureId(m_textureHelper->create2DTexture(image, true, true));
}

void SelectionPointer::updateTheme(Theme theme)
{
    m_cachedTheme.setFromTheme(theme);
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
    m_pointShader = new ShaderHelper(this, QStringLiteral(":/shaders/vertex"),
                                     QStringLiteral(":/shaders/fragment"));
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
    m_pointObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/sphere"));
    m_pointObj->load();
}

QT_DATAVIS3D_END_NAMESPACE
