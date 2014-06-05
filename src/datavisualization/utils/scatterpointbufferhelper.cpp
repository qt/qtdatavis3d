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

#include "scatterpointbufferhelper_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

const GLfloat itemScaler = 3.0f;
const QVector3D hiddenPos(-1000.0f, -1000.0f, -1000.0f);

ScatterPointBufferHelper::ScatterPointBufferHelper()
    : m_pointbuffer(0),
      m_oldRemoveIndex(0),
      m_oldRemove(false)
{
    m_indicesType = GL_UNSIGNED_INT;
}

ScatterPointBufferHelper::~ScatterPointBufferHelper()
{
    if (QOpenGLContext::currentContext())
        glDeleteBuffers(1, &m_pointbuffer);
}

GLuint ScatterPointBufferHelper::pointBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_pointbuffer;
}

void ScatterPointBufferHelper::pushPoint(uint pointIndex)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_pointbuffer);

    if (m_oldRemove && m_oldRemoveIndex < pointIndex) {
        glBufferSubData(GL_ARRAY_BUFFER, m_oldRemoveIndex * sizeof(QVector3D),
                        sizeof(QVector3D), &m_bufferedPoints.at(m_oldRemoveIndex));
    }

    glBufferSubData(GL_ARRAY_BUFFER, pointIndex * sizeof(QVector3D),
                    sizeof(QVector3D),
                    &hiddenPos);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_oldRemoveIndex = pointIndex;
    m_oldRemove = true;
}

void ScatterPointBufferHelper::popPoint()
{
    if (m_oldRemove) {
        glBindBuffer(GL_ARRAY_BUFFER, m_pointbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, m_oldRemoveIndex * sizeof(QVector3D),
                        sizeof(QVector3D), &m_bufferedPoints.at(m_oldRemoveIndex));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    m_oldRemoveIndex = 0;
    m_oldRemove = false;
}

void ScatterPointBufferHelper::load(ScatterSeriesRenderCache *cache)
{
    initializeOpenGLFunctions();

    ScatterRenderItemArray &renderArray = cache->renderArray();
    const int renderArraySize = renderArray.size();

    if (m_meshDataLoaded) {
        // Delete old data
        glDeleteBuffers(1, &m_pointbuffer);
        m_bufferedPoints.clear();
    }

    m_bufferedPoints.resize(renderArraySize);
    for (int i = 0; i < renderArraySize; i++) {
        ScatterRenderItem &item = renderArray[i];
        if (!item.isVisible())
            m_bufferedPoints[i] = hiddenPos;
        else
            m_bufferedPoints[i] = item.translation();
    }

    m_indexCount = renderArraySize;

    glGenBuffers(1, &m_pointbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointbuffer);
    glBufferData(GL_ARRAY_BUFFER, m_bufferedPoints.size() * sizeof(QVector3D),
                 &m_bufferedPoints.at(0),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_meshDataLoaded = true;
}

QT_END_NAMESPACE_DATAVISUALIZATION
