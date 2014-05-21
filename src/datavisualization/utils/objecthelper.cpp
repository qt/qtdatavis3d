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

#include "meshloader_p.h"
#include "vertexindexer_p.h"
#include "objecthelper_p.h"
#include "abstractobjecthelper_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

ObjectHelper::ObjectHelper(const QString &objectFile)
    : m_objectFile(objectFile)
{
    m_indicesType = GL_UNSIGNED_SHORT;
    load();
}

struct ObjectHelperRef {
    int refCount;
    ObjectHelper *obj;
};

// The "Abstract3DRenderer *" key identifies the renderer
static QHash<const Abstract3DRenderer *, QHash<QString, ObjectHelperRef *> *> cacheTable;

ObjectHelper::~ObjectHelper()
{
}

void ObjectHelper::resetObjectHelper(const Abstract3DRenderer *cacheId, ObjectHelper *&obj,
                                     const QString &meshFile)
{
    Q_ASSERT(cacheId);

    if (obj) {
        const QString &oldFile = obj->objectFile();
        if (meshFile == oldFile)
            return; // same file, do nothing
        releaseObjectHelper(cacheId, obj);
    }
    obj = getObjectHelper(cacheId, meshFile);
}

void ObjectHelper::releaseObjectHelper(const Abstract3DRenderer *cacheId, ObjectHelper *&obj)
{
    Q_ASSERT(cacheId);

    if (obj) {
        QHash<QString, ObjectHelperRef *> *objectTable = cacheTable.value(cacheId, 0);
        if (objectTable) {
            // Delete object if last reference is released
            ObjectHelperRef *objRef = objectTable->value(obj->m_objectFile, 0);
            if (objRef) {
                objRef->refCount--;
                if (objRef->refCount <= 0) {
                    objectTable->remove(obj->m_objectFile);
                    delete objRef->obj;
                    delete objRef;
                }
            }
            if (objectTable->isEmpty()) {
                // Remove the entire cache if last object was removed
                cacheTable.remove(cacheId);
                delete objectTable;
            }
        } else {
            // Just delete the object if unknown cache
            delete obj;
        }
        obj = 0;
    }
}

ObjectHelper *ObjectHelper::getObjectHelper(const Abstract3DRenderer *cacheId,
                                            const QString &objectFile)
{
    if (objectFile.isEmpty())
        return 0;

    QHash<QString, ObjectHelperRef *> *objectTable = cacheTable.value(cacheId, 0);
    if (!objectTable) {
        objectTable = new QHash<QString, ObjectHelperRef *>;
        cacheTable.insert(cacheId, objectTable);
    }

    // Check if object helper for this mesh already exists
    ObjectHelperRef *objRef = objectTable->value(objectFile, 0);
    if (!objRef) {
        objRef = new ObjectHelperRef;
        objRef->refCount = 0;
        objRef->obj = new ObjectHelper(objectFile);
        objectTable->insert(objectFile, objRef);
    }
    objRef->refCount++;
    return objRef->obj;
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

    // Index vertices
    QVector<unsigned short> indices;
    QVector<QVector3D> indexed_vertices;
    QVector<QVector2D> indexed_uvs;
    QVector<QVector3D> indexed_normals;
    VertexIndexer::indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs,
                            indexed_normals);

    m_indexCount = indices.size();

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

QT_END_NAMESPACE_DATAVISUALIZATION
