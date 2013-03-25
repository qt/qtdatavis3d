/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "shaderhelper_p.h"

#include <QOpenGLShader>

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

ShaderHelper::ShaderHelper(QObject *parent
                           , const QString &vertexShader
                           , const QString &fragmentShader
                           , const QString &texture
                           , const QString &depthTexture)
    : m_caller(parent)
    , m_program(0)
    , m_vertexShaderFile(vertexShader)
    , m_fragmentShaderFile(fragmentShader)
    , m_textureFile(texture)
    , m_depthTextureFile(depthTexture)
{
}

ShaderHelper::~ShaderHelper()
{
    delete m_program;
}

void ShaderHelper::setShaders(const QString &vertexShader
                              , const QString &fragmentShader)
{
    m_vertexShaderFile = vertexShader;
    m_fragmentShaderFile = fragmentShader;
}

void ShaderHelper::setTextures(const QString &texture
                               , const QString &depthTexture)
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
    m_mvpMatrixUniform = m_program->uniformLocation("MVP");
    m_viewMatrixUniform = m_program->uniformLocation("V");
    m_modelMatrixUniform = m_program->uniformLocation("M");
    m_invTransModelMatrixUniform = m_program->uniformLocation("itM");
    m_lightPositionUniform = m_program->uniformLocation("lightPosition_wrld");
    m_lightStrengthUniform = m_program->uniformLocation("lightStrength");
    m_colorUniform = m_program->uniformLocation("color_mdl");

    m_positionAttr = m_program->attributeLocation("vertexPosition_mdl");
    m_uvAttr = m_program->attributeLocation("vertexUV");
    m_normalAttr = m_program->attributeLocation("vertexNormal_mdl");

    m_initialized = true;
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

GLuint ShaderHelper::color()
{
    if (!m_initialized)
        qFatal("Shader not initialized");
    return m_colorUniform;
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

// TODO: Add texture stuff

QTCOMMERCIALDATAVIS3D_END_NAMESPACE
