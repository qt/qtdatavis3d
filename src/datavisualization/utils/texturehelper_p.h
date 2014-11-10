/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
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

#ifndef TEXTUREHELPER_P_H
#define TEXTUREHELPER_P_H

#include "datavisualizationglobal_p.h"
#include <QtGui/QRgb>
#include <QtGui/QLinearGradient>
#if !defined(QT_OPENGL_ES_2)
// 3D Textures are not supported by ES set
#  include <QtGui/QOpenGLFunctions_2_1>
#endif

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class TextureHelper : protected QOpenGLFunctions
{
    public:
    TextureHelper();
    ~TextureHelper();

    // Ownership of created texture is transferred to caller
    GLuint create2DTexture(const QImage &image, bool useTrilinearFiltering = false,
                           bool convert = true, bool smoothScale = true, bool clampY = false);
    GLuint create3DTexture(const QVector<uchar> *data, int width, int height, int depth,
                           QImage::Format dataFormat);
    GLuint createCubeMapTexture(const QImage &image, bool useTrilinearFiltering = false);
    // Returns selection texture and inserts generated framebuffers to framebuffer parameters
    GLuint createSelectionTexture(const QSize &size, GLuint &frameBuffer, GLuint &depthBuffer);
    GLuint createCursorPositionTexture(const QSize &size, GLuint &frameBuffer);
    GLuint createUniformTexture(const QColor &color);
    GLuint createGradientTexture(const QLinearGradient &gradient);
    GLuint createDepthTexture(const QSize &size, GLuint textureSize);
    // Returns depth texture and inserts generated framebuffer to parameter
    GLuint createDepthTextureFrameBuffer(const QSize &size, GLuint &frameBuffer, GLuint textureSize);
    void deleteTexture(GLuint *texture);

    private:
    QImage convertToGLFormat(const QImage &srcImage);
    void convertToGLFormatHelper(QImage &dstImage, const QImage &srcImage, GLenum texture_format);
    QRgb qt_gl_convertToGLFormatHelper(QRgb src_pixel, GLenum texture_format);

#if !defined(QT_OPENGL_ES_2)
    QOpenGLFunctions_2_1 *m_openGlFunctions_2_1; // Not owned
#endif
    friend class Bars3DRenderer;
    friend class Surface3DRenderer;
    friend class Scatter3DRenderer;
    friend class Abstract3DRenderer;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
