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

#ifndef GLSTATESTORE_P_H
#define GLSTATESTORE_P_H

//#define VERBOSE_STATE_STORE

#include <QObject>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLContext>
#ifdef VERBOSE_STATE_STORE
#include "enumtostringmap_p.h"
#endif

class GLStateStore : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLStateStore(QOpenGLContext *context, QObject *parent = 0);
    ~GLStateStore();

    void storeGLState();
    void restoreGLState();
    void initGLDefaultState();

#ifdef VERBOSE_STATE_STORE
    void printCurrentState(bool in);
    EnumToStringMap *m_map;
#endif

    GLint m_scissorBox[4];
    GLboolean m_isScissorTestEnabled;

#if !defined(QT_OPENGL_ES_2)
    GLint m_drawFramebuffer;
    GLint m_readFramebuffer;
#endif
    GLint m_renderbuffer;
    GLfloat m_clearColor[4];
    GLfloat m_clearDepth;
    GLboolean m_isBlendingEnabled;
    GLboolean m_isDepthTestEnabled;
    GLint m_depthFunc;
    GLboolean m_isDepthWriteEnabled;
    GLint m_currentProgram;
    GLint m_maxVertexAttribs;
    GLint *m_vertexAttribArrayEnabledStates;
    GLint *m_vertexAttribArrayBoundBuffers;
    GLint *m_vertexAttribArraySizes;
    GLint *m_vertexAttribArrayTypes;
    GLint *m_vertexAttribArrayNormalized;
    GLint *m_vertexAttribArrayStrides;
    GLint *m_vertexAttribArrayOffsets;

    GLint m_activeTexture;
    GLint m_texBinding2D;
    GLint m_frontFace;
    GLboolean m_isCullFaceEnabled;
    GLint m_cullFaceMode;
    GLint m_blendEquationRGB;
    GLint m_blendEquationAlpha;
    GLint m_blendDestAlpha;
    GLint m_blendDestRGB;
    GLint m_blendSrcAlpha;
    GLint m_blendSrcRGB;
    GLint m_boundArrayBuffer;
    GLint m_boundElementArrayBuffer;
    GLboolean m_polygonOffsetFillEnabled;
    GLfloat m_polygonOffsetFactor;
    GLfloat m_polygonOffsetUnits;
};

#endif
