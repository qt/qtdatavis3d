/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
******************************************************************************/

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

#include <QtGui/QOpenGLFunctions>
#include <QtCore/QScopedArrayPointer>
#include "enumtostringmap_p.h"

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

#if !QT_CONFIG(opengles2)
    GLint m_drawFramebuffer;
    GLint m_readFramebuffer;
    GLint m_renderbuffer;
#endif
    GLfloat m_clearColor[4];
    GLfloat m_clearDepth;
    GLboolean m_isBlendingEnabled;
    GLboolean m_isDepthTestEnabled;
    GLint m_depthFunc;
    GLboolean m_isDepthWriteEnabled;
    GLint m_currentProgram;
    GLint m_maxVertexAttribs;
    QScopedArrayPointer<GLint> m_vertexAttribArrayEnabledStates;
    QScopedArrayPointer<GLint> m_vertexAttribArrayBoundBuffers;
    QScopedArrayPointer<GLint> m_vertexAttribArraySizes;
    QScopedArrayPointer<GLint> m_vertexAttribArrayTypes;
    QScopedArrayPointer<GLint> m_vertexAttribArrayNormalized;
    QScopedArrayPointer<GLint> m_vertexAttribArrayStrides;
    QScopedArrayPointer<void *> m_vertexAttribArrayOffsets;

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
