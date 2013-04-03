/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#ifndef SHADERHELPER_P_H
#define SHADERHELPER_P_H

#include "qdatavis3dglobal.h"
#include <QOpenGLFunctions>

class QOpenGLShaderProgram;

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

class ShaderHelper
{
    public:
    ShaderHelper(QObject *parent
                 , const QString &vertexShader = QString()
                 , const QString &fragmentShader = QString()
                 , const QString &texture = QString()
                 , const QString &depthTexture = QString());
    ~ShaderHelper();

    void setShaders(const QString &vertexShader, const QString &fragmentShader);
    void setTextures(const QString &texture, const QString &depthTexture);

    void initialize();
    void bind();
    void release();
    void setUniformValue(GLuint uniform, const QVector3D &value);
    void setUniformValue(GLuint uniform, const QVector4D &value);
    void setUniformValue(GLuint uniform, const QMatrix4x4 &value);
    void setUniformValue(GLuint uniform, GLfloat value);

    GLuint MVP();
    GLuint view();
    GLuint model();
    GLuint nModel();
    GLuint lightP();
    GLuint lightS();
    GLuint ambientS();
    GLuint color();

    GLuint posAtt();
    GLuint uvAtt();
    GLuint normalAtt();

    private:
    QObject *m_caller;
    QOpenGLShaderProgram *m_program;

    QString m_vertexShaderFile;
    QString m_fragmentShaderFile;

    QString m_textureFile;
    QString m_depthTextureFile;

    GLuint m_positionAttr;
    GLuint m_uvAttr;
    GLuint m_normalAttr;

    GLuint m_colorUniform;
    GLuint m_viewMatrixUniform;
    GLuint m_modelMatrixUniform;
    GLuint m_invTransModelMatrixUniform;
    GLuint m_mvpMatrixUniform;
    GLuint m_lightPositionUniform;
    GLuint m_lightStrengthUniform;
    GLuint m_ambientStrengthUniform;

    //GLuint m_texture; // currently unused
    //GLuint m_depthTexture; // currently unused

    GLboolean m_initialized;
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif
