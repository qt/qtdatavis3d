/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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

#include "vertexindexer_p.h"

#include <string.h> // for memcmp
#include <qmath.h>

#include <QDebug>

namespace QtDataVisualization {

/*!
 * \class QtDataVisualization::VertexIndexer
 * \internal
 */

int unique_vertices = 0;

// Returns true if v1 can be considered equal to v2
bool VertexIndexer::is_near(float v1, float v2)
{
    return qAbs(v1 - v2) < 0.01f;
}

// Searches through all already exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool VertexIndexer::getSimilarVertexIndex(const QVector3D &in_vertex,
                                          const QVector2D &in_uv,
                                          const QVector3D &in_normal,
                                          QVector<QVector3D> &out_vertices,
                                          QVector<QVector2D> &out_uvs,
                                          QVector<QVector3D> &out_normals,
                                          unsigned short &result)
{
    // Lame linear search
    for (int i = 0; i < out_vertices.size(); i++) {
        if (is_near(in_vertex.x() , out_vertices[i].x())
                && is_near(in_vertex.y() , out_vertices[i].y())
                && is_near(in_vertex.z() , out_vertices[i].z())
                && is_near(in_uv.x()     , out_uvs     [i].x())
                && is_near(in_uv.y()     , out_uvs     [i].y())
                && is_near(in_normal.x() , out_normals [i].x())
                && is_near(in_normal.y() , out_normals [i].y())
                && is_near(in_normal.z() , out_normals [i].z())) {
            result = i;
            return true;
        }
    }
    // No other vertex could be used instead.
    // Looks like we'll have to add it to the VBO.
    return false;
}

bool VertexIndexer::getSimilarVertexIndex_fast(const PackedVertex &packed,
                                               QMap<PackedVertex, unsigned short> &VertexToOutIndex,
                                               unsigned short &result)
{
    QMap<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
    if (it == VertexToOutIndex.end()) {
        return false;
    } else {
        result = it.value();
        return true;
    }
}

void VertexIndexer::indexVBO(const QVector<QVector3D> &in_vertices,
                             const QVector<QVector2D> &in_uvs,
                             const QVector<QVector3D> &in_normals,
                             QVector<unsigned short> &out_indices,
                             QVector<QVector3D> &out_vertices,
                             QVector<QVector2D> &out_uvs,
                             QVector<QVector3D> &out_normals)
{
    unique_vertices = 0;
    QMap<PackedVertex, unsigned short> VertexToOutIndex;

    // For each input vertex
    for (int i = 0; i < in_vertices.size(); i++) {
        PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};

        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex_fast(packed, VertexToOutIndex, index);

        if (found) {
            out_indices.append(index);
        } else {
            unique_vertices++;
            out_vertices.append(in_vertices[i]);
            out_uvs.append(in_uvs[i]);
            out_normals.append(in_normals[i]);
            unsigned short newindex = (unsigned short)out_vertices.size() - 1;
            out_indices.append(newindex);
            VertexToOutIndex[packed] = newindex;
        }
    }
    //qDebug() << "unique vertices" << unique_vertices;
}

void VertexIndexer::indexVBO_TBN(const QVector<QVector3D> &in_vertices,
                                 const QVector<QVector2D> &in_uvs,
                                 const QVector<QVector3D> &in_normals,
                                 const QVector<QVector3D> &in_tangents,
                                 const QVector<QVector3D> &in_bitangents,
                                 QVector<unsigned short> &out_indices,
                                 QVector<QVector3D> &out_vertices,
                                 QVector<QVector2D> &out_uvs,
                                 QVector<QVector3D> &out_normals,
                                 QVector<QVector3D> &out_tangents,
                                 QVector<QVector3D> &out_bitangents)
{
    unique_vertices = 0;
    // For each input vertex
    for (int i = 0; i < in_vertices.size(); i++) {

        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i],
                                           out_vertices, out_uvs, out_normals, index);

        if (found) {
            out_indices.append(index);

            // Average the tangents and the bitangents
            out_tangents[index] += in_tangents[i];
            out_bitangents[index] += in_bitangents[i];
        } else {
            unique_vertices++;
            out_vertices.append(in_vertices[i]);
            out_uvs.append(in_uvs[i]);
            out_normals.append(in_normals[i]);
            out_tangents.append(in_tangents[i]);
            out_bitangents.append(in_bitangents[i]);
            out_indices.append((unsigned short)out_vertices.size() - 1);
        }
    }
    //qDebug() << "unique vertices" << unique_vertices;
}

}
