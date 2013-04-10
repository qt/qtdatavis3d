/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
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

    m_caller->glGenBuffers(1, &m_uvbuffer);
    m_caller->glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
    m_caller->glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(QVector2D)
                           , &indexed_uvs.at(0), GL_STATIC_DRAW);

    m_caller->glGenBuffers(1, &m_elementbuffer);
    m_caller->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
    m_caller->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short)
                           , &indices.at(0), GL_STATIC_DRAW);

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
