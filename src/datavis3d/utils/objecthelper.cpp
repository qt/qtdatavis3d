/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "meshloader_p.h"
#include "vertexindexer_p.h"
#include "objecthelper_p.h"

#include <QDebug>

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

ObjectHelper::ObjectHelper(QOpenGLFunctions *parent, const QString &objectFile)
    : m_caller(parent)
    , m_objectFile(objectFile)
    , m_vertexbuffer(0)
    , m_normalbuffer(0)
    , m_uvbuffer(0)
    , m_elementbuffer(0)
    , m_indexCount(0)
    , m_meshDataLoaded(false)
{
}

ObjectHelper::~ObjectHelper()
{
    m_caller->glDeleteBuffers(1, &m_vertexbuffer);
    m_caller->glDeleteBuffers(1, &m_uvbuffer);
    m_caller->glDeleteBuffers(1, &m_normalbuffer);
    m_caller->glDeleteBuffers(1, &m_elementbuffer);
}

void ObjectHelper::setObjectFile(const QString &objectFile)
{
    m_objectFile = objectFile;
}

void ObjectHelper::load()
{
    if (m_meshDataLoaded) {
        // Delete old data
        m_caller->glDeleteBuffers(1, &m_vertexbuffer);
        m_caller->glDeleteBuffers(1, &m_uvbuffer);
        m_caller->glDeleteBuffers(1, &m_normalbuffer);
        m_caller->glDeleteBuffers(1, &m_elementbuffer);
    }
    QVector<QVector3D> vertices;
    QVector<QVector2D> uvs;
    QVector<QVector3D> normals;
    bool loadOk = MeshLoader::loadOBJ(m_objectFile, vertices, uvs, normals);
    if (!loadOk)
        qFatal("loading failed");

    qDebug() << "vertex count" << vertices.size();;

    // Index vertices
    QVector<unsigned short> indices;
    QVector<QVector3D> indexed_vertices;
    QVector<QVector2D> indexed_uvs;
    QVector<QVector3D> indexed_normals;
    VertexIndexer::indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs
                            , indexed_normals);

    m_indexCount = indices.size();
    //qDebug() << "index count" << m_indexCount;

    m_caller->glGenBuffers(1, &m_vertexbuffer);
    m_caller->glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    m_caller->glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(QVector3D)
                        , &indexed_vertices.at(0)
                        , GL_STATIC_DRAW);

    m_caller->glGenBuffers(1, &m_normalbuffer);
    m_caller->glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
    m_caller->glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(QVector3D)
                        , &indexed_normals.at(0)
                        , GL_STATIC_DRAW);

    //m_caller->glGenBuffers(1, &m_uvbuffer);
    //m_caller->glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
    //m_caller->glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(QVector2D), &indexed_uvs.at(0)
    //        , GL_STATIC_DRAW);

    m_caller->glGenBuffers(1, &m_elementbuffer);
    m_caller->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
    m_caller->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices.at(0)
                        , GL_STATIC_DRAW);

    m_caller->glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_caller->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_meshDataLoaded = true;
}

GLuint ObjectHelper::vertexBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_vertexbuffer;
}

GLuint ObjectHelper::normalBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_normalbuffer;
}

GLuint ObjectHelper::uvBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_uvbuffer;
}

GLuint ObjectHelper::elementBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_elementbuffer;
}

GLuint ObjectHelper::indexCount()
{
    return m_indexCount;
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE
