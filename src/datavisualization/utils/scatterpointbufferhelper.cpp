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
#include <QtGui/QVector2D>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

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
    m_indexCount = 0;

    if (m_meshDataLoaded) {
        // Delete old data
        glDeleteBuffers(1, &m_pointbuffer);
        glDeleteBuffers(1, &m_uvbuffer);
        m_bufferedPoints.clear();
    }

    QVector<QVector2D> buffered_uvs;
    if (cache->colorStyle() == Q3DTheme::ColorStyleRangeGradient)
        createRangeGradientUVs(cache, buffered_uvs);

    bool itemsVisible = false;
    m_bufferedPoints.resize(renderArraySize);
    for (int i = 0; i < renderArraySize; i++) {
        ScatterRenderItem &item = renderArray[i];
        if (!item.isVisible()) {
            m_bufferedPoints[i] = hiddenPos;
        } else {
            itemsVisible = true;
            m_bufferedPoints[i] = item.translation();
        }
    }

    if (itemsVisible)
        m_indexCount = renderArraySize;

    if (m_indexCount > 0) {
        glGenBuffers(1, &m_pointbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_pointbuffer);
        glBufferData(GL_ARRAY_BUFFER, m_bufferedPoints.size() * sizeof(QVector3D),
                     &m_bufferedPoints.at(0),
                     GL_DYNAMIC_DRAW);

        if (buffered_uvs.size()) {
            glGenBuffers(1, &m_uvbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, buffered_uvs.size() * sizeof(QVector2D),
                         &buffered_uvs.at(0), GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_meshDataLoaded = true;
    }
}

void ScatterPointBufferHelper::updateUVs(ScatterSeriesRenderCache *cache)
{
    QVector<QVector2D> buffered_uvs;

    createRangeGradientUVs(cache, buffered_uvs);

    if (buffered_uvs.size()) {
        if (!m_uvbuffer)
            glGenBuffers(1, &m_uvbuffer);

        glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, buffered_uvs.size() * sizeof(QVector2D),
                     &buffered_uvs.at(0), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void ScatterPointBufferHelper::createRangeGradientUVs(ScatterSeriesRenderCache *cache,
                                                      QVector<QVector2D> &buffered_uvs)
{
    const ScatterRenderItemArray &renderArray = cache->renderArray();
    const uint renderArraySize = renderArray.size();
    buffered_uvs.resize(renderArraySize);

    QVector2D uv;
    uv.setX(0.0f);
    for (uint i = 0; i < renderArraySize; i++) {
        const ScatterRenderItem &item = renderArray.at(i);

        float y = ((item.translation().y() + m_scaleY) * 0.5f) / m_scaleY;
        uv.setY(y);
        buffered_uvs[i] = uv;
    }
}

QT_END_NAMESPACE_DATAVISUALIZATION
