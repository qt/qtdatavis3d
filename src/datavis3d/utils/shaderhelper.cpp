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

#include "shaderhelper_p.h"

#include <QOpenGLShader>

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

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
    m_mvpMatrixUniform = m_program->uniformLocation("MVP");
    m_viewMatrixUniform = m_program->uniformLocation("V");
    m_modelMatrixUniform = m_program->uniformLocation("M");
    m_invTransModelMatrixUniform = m_program->uniformLocation("itM");
    m_depthMatrixUniform = m_program->uniformLocation("depthMVP");
    m_lightPositionUniform = m_program->uniformLocation("lightPosition_wrld");
    m_lightStrengthUniform = m_program->uniformLocation("lightStrength");
    m_ambientStrengthUniform = m_program->uniformLocation("ambientStrength");
    m_colorUniform = m_program->uniformLocation("color_mdl");
    m_textureUniform = m_program->uniformLocation("textureSampler");
    m_shadowUniform = m_program->uniformLocation("shadowMap");

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

QTCOMMERCIALDATAVIS3D_END_NAMESPACE
