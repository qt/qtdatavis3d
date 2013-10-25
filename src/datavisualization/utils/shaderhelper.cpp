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

#include "shaderhelper_p.h"

#include <QOpenGLShader>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

void discardDebugMsgs(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type)
    Q_UNUSED(context)
    Q_UNUSED(msg)
    // Used to discard warnings generated during shader test compilation
}

ShaderHelper::ShaderHelper(QObject *parent,
                           const QString &vertexShader,
                           const QString &fragmentShader,
                           const QString &texture,
                           const QString &depthTexture)
    : m_caller(parent),
      m_program(0),
      m_vertexShaderFile(vertexShader),
      m_fragmentShaderFile(fragmentShader),
      m_textureFile(texture),
      m_depthTextureFile(depthTexture)
{
}

ShaderHelper::~ShaderHelper()
{
    delete m_program;
}

void ShaderHelper::setShaders(const QString &vertexShader,
                              const QString &fragmentShader)
{
    m_vertexShaderFile = vertexShader;
    m_fragmentShaderFile = fragmentShader;
}

void ShaderHelper::setTextures(const QString &texture,
                               const QString &depthTexture)
{
    m_textureFile = texture;
    m_depthTextureFile = depthTexture;
}

void ShaderHelper::initialize()
{
    if (m_program)
        delete m_program;
    m_program = new QOpenGLShaderProgram(m_caller);
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, m_vertexShaderFile))
        qFatal("Compiling Vertex shader failed");
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, m_fragmentShaderFile))
        qFatal("Compiling Fragment shader failed");
    m_program->link();

    m_positionAttr = m_program->attributeLocation("vertexPosition_mdl");
    m_normalAttr = m_program->attributeLocation("vertexNormal_mdl");
    m_uvAttr = m_program->attributeLocation("vertexUV");

    m_mvpMatrixUniform = m_program->uniformLocation("MVP");
    m_viewMatrixUniform = m_program->uniformLocation("V");
    m_modelMatrixUniform = m_program->uniformLocation("M");
    m_invTransModelMatrixUniform = m_program->uniformLocation("itM");
    m_depthMatrixUniform = m_program->uniformLocation("depthMVP");
    m_lightPositionUniform = m_program->uniformLocation("lightPosition_wrld");
    m_lightStrengthUniform = m_program->uniformLocation("lightStrength");
    m_ambientStrengthUniform = m_program->uniformLocation("ambientStrength");
    m_shadowQualityUniform = m_program->uniformLocation("shadowQuality");
    m_colorUniform = m_program->uniformLocation("color_mdl");
    m_textureUniform = m_program->uniformLocation("textureSampler");
    m_shadowUniform = m_program->uniformLocation("shadowMap");

    m_initialized = true;
}

bool ShaderHelper::testCompile()
{
    bool result = true;
    // Discard warnings, we only need the result
    QtMessageHandler handler = qInstallMessageHandler(discardDebugMsgs);
    if (m_program)
        delete m_program;
    m_program = new QOpenGLShaderProgram(m_caller);
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, m_vertexShaderFile))
        result = false;
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, m_fragmentShaderFile))
        result = false;
    // Restore actual message handler
    qInstallMessageHandler(handler);
    return result;
}

void ShaderHelper::bind()
{
    m_program->bind();
}

void ShaderHelper::release()
{
    m_program->release();
}

void ShaderHelper::setUniformValue(GLuint uniform, const QVector3D &value)
{
    m_program->setUniformValue(uniform, value);
}

void ShaderHelper::setUniformValue(GLuint uniform, const QVector4D &value)
{
    m_program->setUniformValue(uniform, value);
}

void ShaderHelper::setUniformValue(GLuint uniform, const QMatrix4x4 &value)
{
    m_program->setUniformValue(uniform, value);
}

void ShaderHelper::setUniformValue(GLuint uniform, GLfloat value)
{
    m_program->setUniformValue(uniform, value);
}

void ShaderHelper::setUniformValue(GLuint uniform, GLint value)
{
    m_program->setUniformValue(uniform, value);
}

GLuint ShaderHelper::MVP()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_mvpMatrixUniform;
}

GLuint ShaderHelper::view()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_viewMatrixUniform;
}

GLuint ShaderHelper::model()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_modelMatrixUniform;
}

GLuint ShaderHelper::nModel()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_invTransModelMatrixUniform;
}

GLuint ShaderHelper::depth()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_depthMatrixUniform;
}

GLuint ShaderHelper::lightP()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_lightPositionUniform;
}

GLuint ShaderHelper::lightS()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_lightStrengthUniform;
}

GLuint ShaderHelper::ambientS()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_ambientStrengthUniform;
}

GLuint ShaderHelper::shadowQ()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_shadowQualityUniform;
}

GLuint ShaderHelper::color()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_colorUniform;
}

GLuint ShaderHelper::texture()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_textureUniform;
}

GLuint ShaderHelper::shadow()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_shadowUniform;
}

GLuint ShaderHelper::posAtt()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_positionAttr;
}

GLuint ShaderHelper::uvAtt()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_uvAttr;
}

GLuint ShaderHelper::normalAtt()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_normalAttr;
}

QT_DATAVISUALIZATION_END_NAMESPACE
