/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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
****************************************************************************/

#include "abstractobjecthelper_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

AbstractObjectHelper::AbstractObjectHelper()
    : m_vertexbuffer(0),
      m_normalbuffer(0),
      m_uvbuffer(0),
      m_elementbuffer(0),
      m_indexCount(0),
      m_meshDataLoaded(false)
{
    initializeOpenGLFunctions();
}

AbstractObjectHelper::~AbstractObjectHelper()
{
    if (QOpenGLContext::currentContext()) {
        glDeleteBuffers(1, &m_vertexbuffer);
        glDeleteBuffers(1, &m_uvbuffer);
        glDeleteBuffers(1, &m_normalbuffer);
        glDeleteBuffers(1, &m_elementbuffer);
    }
}

GLuint AbstractObjectHelper::vertexBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_vertexbuffer;
}

GLuint AbstractObjectHelper::normalBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_normalbuffer;
}

GLuint AbstractObjectHelper::uvBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_uvbuffer;
}

GLuint AbstractObjectHelper::elementBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_elementbuffer;
}

GLuint AbstractObjectHelper::indexCount()
{
    return m_indexCount;
}

QT_END_NAMESPACE_DATAVISUALIZATION
