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

#include "scatterobjectbufferhelper_p.h"
#include "objecthelper_p.h"
#include <QtGui/QVector2D>
#include <QtGui/QMatrix4x4>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

const GLfloat itemScaler = 3.0f;

ScatterObjectBufferHelper::ScatterObjectBufferHelper()
    : m_scaleY(0.0f)
{
    m_indicesType = GL_UNSIGNED_INT;
}

ScatterObjectBufferHelper::~ScatterObjectBufferHelper()
{
}

void ScatterObjectBufferHelper::fullLoad(ScatterSeriesRenderCache *cache, qreal dotScale)
{
    initializeOpenGLFunctions();

    m_meshDataLoaded = false;
    m_indexCount = 0;

    ObjectHelper *dotObj = cache->object();
    const ScatterRenderItemArray &renderArray = cache->renderArray();
    const uint renderArraySize = renderArray.size();

    if (renderArraySize == 0)
        return;  // No use to go forward

    uint itemCount = renderArraySize;
    QQuaternion seriesRotation(cache->meshRotation());

    if (m_meshDataLoaded) {
        // Delete old data
        glDeleteBuffers(1, &m_vertexbuffer);
        glDeleteBuffers(1, &m_uvbuffer);
        glDeleteBuffers(1, &m_normalbuffer);
        glDeleteBuffers(1, &m_elementbuffer);
    }

    // Index vertices
    const QVector<unsigned short> indices = dotObj->indices();
    const QVector<QVector3D> indexed_vertices = dotObj->indexedvertices();
    const QVector<QVector2D> indexed_uvs = dotObj->indexedUVs();
    const QVector<QVector3D> indexed_normals = dotObj->indexedNormals();
    const int indicesCount = indices.count();
    const int verticeCount = indexed_vertices.count();
    const int uvsCount = indexed_uvs.count();
    const int normalsCount = indexed_normals.count();

    float itemSize = cache->itemSize() / itemScaler;
    if (itemSize == 0.0f)
        itemSize = dotScale;
    QVector3D modelScaler(itemSize, itemSize, itemSize);
    QMatrix4x4 modelMatrix;
    if (!seriesRotation.isIdentity()) {
        QMatrix4x4 matrix;
        matrix.rotate(seriesRotation);
        modelMatrix = matrix.transposed();
    }
    modelMatrix.scale(modelScaler);

    QVector<QVector3D> scaled_vertices;
    scaled_vertices.resize(verticeCount);
    for (int i = 0; i < verticeCount; i++)
        scaled_vertices[i] = indexed_vertices[i] * modelMatrix;

    QVector<GLuint> buffered_indices;
    QVector<QVector3D> buffered_vertices;
    QVector<QVector2D> buffered_uvs;
    QVector<QVector3D> buffered_normals;

    buffered_indices.resize(indicesCount * renderArraySize);
    buffered_vertices.resize(verticeCount * renderArraySize);
    buffered_normals.resize(normalsCount * renderArraySize);
    buffered_uvs.resize(uvsCount * renderArraySize);
    uint pos = 0;

    if (cache->colorStyle() == Q3DTheme::ColorStyleRangeGradient)
        createRangeGradientUVs(cache, buffered_uvs);
    else if (cache->colorStyle() == Q3DTheme::ColorStyleObjectGradient)
        createObjectGradientUVs(cache, buffered_uvs, indexed_vertices);

    QVector2D dummyUV(0.0f, 0.0f);

    for (uint i = 0; i < renderArraySize; i++) {
        const ScatterRenderItem &item = renderArray.at(i);
        if (!item.isVisible()) {
            itemCount--;
            continue;
        }

        int offset = pos * verticeCount;
        if (item.rotation().isIdentity()) {
            for (int j = 0; j < verticeCount; j++) {
                buffered_vertices[j + offset] = scaled_vertices[j] + item.translation();
                buffered_normals[j + offset] = indexed_normals[j];
            }
        } else {
            QMatrix4x4 matrix;
            QQuaternion totalRotation = seriesRotation * item.rotation();
            matrix.rotate(totalRotation);
            matrix.scale(modelScaler);
            QMatrix4x4 itModelMatrix = matrix.inverted();
            modelMatrix = matrix.transposed(); // Because of row-column major difference

            for (int j = 0; j < verticeCount; j++) {
                buffered_vertices[j + offset] = indexed_vertices[j] * modelMatrix
                        + item.translation();
                buffered_normals[j + offset] = indexed_normals[j] * itModelMatrix;
            }
        }

        if (cache->colorStyle() == Q3DTheme::ColorStyleUniform) {
            offset = pos * uvsCount;
            for (int j = 0; j < uvsCount; j++)
                buffered_uvs[j + offset] = dummyUV;
        }

        int offsetVertice = i * verticeCount;
        offset = pos * indicesCount;
        for (int j = 0; j < indicesCount; j++) {
            buffered_indices[j + offset] = GLuint(indices[j] + offsetVertice);
        }

        pos++;
    }

    m_indexCount = indicesCount * itemCount;

    if (itemCount > 0) {
        glGenBuffers(1, &m_vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, verticeCount * itemCount * sizeof(QVector3D),
                     &buffered_vertices.at(0),
                     GL_STATIC_DRAW);

        glGenBuffers(1, &m_normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, normalsCount * itemCount * sizeof(QVector3D),
                     &buffered_normals.at(0),
                     GL_STATIC_DRAW);

        glGenBuffers(1, &m_uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, uvsCount * itemCount * sizeof(QVector2D),
                     &buffered_uvs.at(0), GL_STATIC_DRAW);

        glGenBuffers(1, &m_elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * itemCount * sizeof(GLint),
                     &buffered_indices.at(0), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        m_meshDataLoaded = true;
    }
}

void ScatterObjectBufferHelper::updateUVs(ScatterSeriesRenderCache *cache)
{
    ObjectHelper *dotObj = cache->object();
    const int uvsCount = dotObj->indexedUVs().count();
    const ScatterRenderItemArray &renderArray = cache->renderArray();
    const uint renderArraySize = renderArray.size();
    QVector<QVector2D> buffered_uvs;
    buffered_uvs.resize(uvsCount * renderArraySize);

    uint itemCount = 0;
    if (cache->colorStyle() == Q3DTheme::ColorStyleRangeGradient) {
        itemCount = createRangeGradientUVs(cache, buffered_uvs);
    } else if (cache->colorStyle() == Q3DTheme::ColorStyleObjectGradient) {
        const QVector<QVector3D> indexed_vertices = dotObj->indexedvertices();
        itemCount = createObjectGradientUVs(cache, buffered_uvs, indexed_vertices);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvsCount * itemCount * sizeof(QVector2D),
                 &buffered_uvs.at(0), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

uint ScatterObjectBufferHelper::createRangeGradientUVs(ScatterSeriesRenderCache *cache,
                                                       QVector<QVector2D> &buffered_uvs)
{
    ObjectHelper *dotObj = cache->object();
    const int uvsCount = dotObj->indexedUVs().count();
    const ScatterRenderItemArray &renderArray = cache->renderArray();
    const uint renderArraySize = renderArray.size();

    QVector2D uv;
    uv.setX(0.0f);
    uint pos = 0;
    for (uint i = 0; i < renderArraySize; i++) {
        const ScatterRenderItem &item = renderArray.at(i);
        if (!item.isVisible())
            continue;

        float y = ((item.translation().y() + m_scaleY) * 0.5f ) / m_scaleY;
        uv.setY(y);
        int offset = pos * uvsCount;
        for (int j = 0; j < uvsCount; j++)
            buffered_uvs[j + offset] = uv;

        pos++;
    }

    return pos;
}

uint ScatterObjectBufferHelper::createObjectGradientUVs(ScatterSeriesRenderCache *cache,
                                                        QVector<QVector2D> &buffered_uvs,
                                                        const QVector<QVector3D> &indexed_vertices)
{
    ObjectHelper *dotObj = cache->object();
    const int uvsCount = dotObj->indexedUVs().count();
    const ScatterRenderItemArray &renderArray = cache->renderArray();
    const uint renderArraySize = renderArray.size();

    QVector2D uv;
    uv.setX(0.0f);
    uint pos = 0;
    for (uint i = 0; i < renderArraySize; i++) {
        const ScatterRenderItem &item = renderArray.at(i);
        if (!item.isVisible())
            continue;

        int offset = pos * uvsCount;
        for (int j = 0; j < uvsCount; j++) {
            uv.setY((indexed_vertices.at(j).y() + 1.0f) / 2.0f);
            buffered_uvs[j + offset] = uv;
        }

        pos++;
    }

    return pos;
}

void ScatterObjectBufferHelper::update(ScatterSeriesRenderCache *cache, qreal dotScale)
{
    initializeOpenGLFunctions();

    ObjectHelper *dotObj = cache->object();
    const ScatterRenderItemArray &renderArray = cache->renderArray();
    const int renderArraySize = renderArray.size();
    QQuaternion seriesRotation(cache->meshRotation());

    // Index vertices
    const QVector<QVector3D> indexed_vertices = dotObj->indexedvertices();
    int verticeCount = indexed_vertices.count();

    float itemSize = cache->itemSize() / itemScaler;
    if (itemSize == 0.0f)
        itemSize = dotScale;
    QVector3D modelScaler(itemSize, itemSize, itemSize);
    QMatrix4x4 modelMatrix;
    if (!seriesRotation.isIdentity()) {
        QMatrix4x4 matrix;
        matrix.rotate(seriesRotation);
        modelMatrix = matrix.transposed();
    }
    modelMatrix.scale(modelScaler);

    QVector<QVector3D> scaled_vertices;
    scaled_vertices.resize(verticeCount);
    for (int i = 0; i < verticeCount; i++)
        scaled_vertices[i] = indexed_vertices[i] * modelMatrix;

    QVector<QVector3D> buffered_vertices;

    buffered_vertices.resize(verticeCount * renderArraySize);
    for (int i = 0; i < renderArraySize; i++) {
        const ScatterRenderItem &item = renderArray.at(i);
        if (!item.isVisible())
            continue;

        const int offset = i * verticeCount;
        if (item.rotation().isIdentity()) {
            for (int j = 0; j < verticeCount; j++)
                buffered_vertices[j + offset] = scaled_vertices[j] + item.translation();
        } else {
            QMatrix4x4 matrix;
            matrix.rotate(seriesRotation * item.rotation());
            modelMatrix = matrix.transposed();
            modelMatrix.scale(modelScaler);

            for (int j = 0; j < verticeCount; j++)
                buffered_vertices[j + offset] = indexed_vertices[j] * modelMatrix
                        + item.translation();
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, buffered_vertices.size() * sizeof(QVector3D),
                 &buffered_vertices.at(0),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_meshDataLoaded = true;
}

QT_END_NAMESPACE_DATAVISUALIZATION
