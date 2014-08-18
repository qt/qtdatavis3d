/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef SHADERHELPER_P_H
#define SHADERHELPER_P_H

#include "datavisualizationglobal_p.h"

class QOpenGLShaderProgram;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class ShaderHelper
{
    public:
    ShaderHelper(QObject *parent,
                 const QString &vertexShader = QString(),
                 const QString &fragmentShader = QString(),
                 const QString &texture = QString(),
                 const QString &depthTexture = QString());
    ~ShaderHelper();

    void setShaders(const QString &vertexShader, const QString &fragmentShader);
    void setTextures(const QString &texture, const QString &depthTexture);

    void initialize();
    bool testCompile();
    void bind();
    void release();
    void setUniformValue(GLuint uniform, const QVector3D &value);
    void setUniformValue(GLuint uniform, const QVector4D &value);
    void setUniformValue(GLuint uniform, const QMatrix4x4 &value);
    void setUniformValue(GLuint uniform, GLfloat value);
    void setUniformValue(GLuint uniform, GLint value);
    void setUniformValueArray(GLuint uniform,  const QVector4D *values, int count);

    GLuint MVP();
    GLuint view();
    GLuint model();
    GLuint nModel();
    GLuint depth();
    GLuint lightP();
    GLuint lightS();
    GLuint ambientS();
    GLuint shadowQ();
    GLuint color();
    GLuint texture();
    GLuint shadow();
    GLuint gradientMin();
    GLuint gradientHeight();
    GLuint lightColor();
    GLuint volumeSliceIndices();
    GLuint colorIndex();
    GLuint cameraPositionRelativeToModel();
    GLuint color8Bit();
    GLuint textureDimensions();
    GLuint sampleCount();

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
    GLuint m_depthMatrixUniform;
    GLuint m_mvpMatrixUniform;
    GLuint m_lightPositionUniform;
    GLuint m_lightStrengthUniform;
    GLuint m_ambientStrengthUniform;
    GLuint m_shadowQualityUniform;
    GLuint m_textureUniform;
    GLuint m_shadowUniform;
    GLuint m_gradientMinUniform;
    GLuint m_gradientHeightUniform;
    GLuint m_lightColorUniform;
    GLuint m_volumeSliceIndicesUniform;
    GLuint m_colorIndexUniform;
    GLuint m_cameraPositionRelativeToModelUniform;
    GLuint m_color8BitUniform;
    GLuint m_textureDimensionsUniform;
    GLuint m_sampleCountUniform;

    GLboolean m_initialized;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
