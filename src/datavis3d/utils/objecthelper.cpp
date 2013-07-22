/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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
#include "abstractobjecthelper_p.h"

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

ObjectHelper::ObjectHelper(const QString &objectFile)
    : m_objectFile(objectFile)
{
    m_indicesType = GL_UNSIGNED_SHORT;
}

ObjectHelper::~ObjectHelper()
{
    qDebug() << "ObjectHelper::~ObjectHelper()";
}

void ObjectHelper::setObjectFile(const QString &objectFile)
{
    m_objectFile = objectFile;
}

void ObjectHelper::load()
{
    initializeOpenGLFunctions();
    if (m_meshDataLoaded) {
        // Delete old data
        glDeleteBuffers(1, &m_vertexbuffer);
        glDeleteBuffers(1, &m_uvbuffer);
        glDeleteBuffers(1, &m_normalbuffer);
        glDeleteBuffers(1, &m_elementbuffer);
    }
    QVector<QVector3D> vertices;
    QVector<QVector2D> uvs;
    QVector<QVector3D> normals;
    bool loadOk = MeshLoader::loadOBJ(m_objectFile, vertices, uvs, normals);
    if (!loadOk)
        qFatal("loading failed");

    //qDebug() << "vertex count" << vertices.size();;

    // Index vertices
    QVector<unsigned short> indices;
    QVector<QVector3D> indexed_vertices;
    QVector<QVector2D> indexed_uvs;
    QVector<QVector3D> indexed_normals;
    VertexIndexer::indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs,
                            indexed_normals);

    m_indexCount = indices.size();
    //qDebug() << "index count" << m_indexCount;

    glGenBuffers(1, &m_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(QVector3D),
                 &indexed_vertices.at(0),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &m_normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(QVector3D),
                 &indexed_normals.at(0),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &m_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(QVector2D),
                 &indexed_uvs.at(0), GL_STATIC_DRAW);

    glGenBuffers(1, &m_elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short),
                 &indices.at(0), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_meshDataLoaded = true;
}

QT_DATAVIS3D_END_NAMESPACE
