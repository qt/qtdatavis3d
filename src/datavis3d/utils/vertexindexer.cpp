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

#include "vertexindexer_p.h"

#include <string.h> // for memcmp
#include <qmath.h>

#include <QDebug>

QTENTERPRISE_DATAVIS3D_BEGIN_NAMESPACE

int unique_vertices = 0;

// Returns true if v1 can be considered equal to v2
bool VertexIndexer::is_near(float v1, float v2)
{
    return qFabs(v1 - v2) < 0.01f;
}

// Searches through all already-exported vertices
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

QTENTERPRISE_DATAVIS3D_END_NAMESPACE
