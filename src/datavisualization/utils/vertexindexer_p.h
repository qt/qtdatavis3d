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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef VERTEXINDEXER_P_H
#define VERTEXINDEXER_P_H

#include "datavisualizationglobal_p.h"

#include <QVector>
#include <QVector2D>
#include <QVector3D>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class VertexIndexer
{
public:
    struct PackedVertex {
        QVector3D position;
        QVector2D uv;
        QVector3D normal;
        bool operator<(const PackedVertex that) const {
            return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
        }
    };

    static void indexVBO(const QVector<QVector3D> &in_vertices,
                         const QVector<QVector2D> &in_uvs,
                         const QVector<QVector3D> &in_normals,
                         QVector<unsigned short> &out_indices,
                         QVector<QVector3D> &out_vertices,
                         QVector<QVector2D> &out_uvs,
                         QVector<QVector3D> &out_normals);

    static void indexVBO_TBN(const QVector<QVector3D> &in_vertices,
                             const QVector<QVector2D> &in_uvs,
                             const QVector<QVector3D> &in_normals,
                             const QVector<QVector3D> &in_tangents,
                             const QVector<QVector3D> &in_bitangents,
                             QVector<unsigned short> &out_indices,
                             QVector<QVector3D> &out_vertices,
                             QVector<QVector2D> &out_uvs,
                             QVector<QVector3D> &out_normals,
                             QVector<QVector3D> &out_tangents,
                             QVector<QVector3D> &out_bitangents);

private:
    static bool is_near(float v1, float v2);
    static bool getSimilarVertexIndex(const QVector3D &in_vertex,
                                      const QVector2D &in_uv,
                                      const QVector3D &in_normal,
                                      QVector<QVector3D> &out_vertices,
                                      QVector<QVector2D> &out_uvs,
                                      QVector<QVector3D> &out_normals,
                                      unsigned short &result);
    static bool getSimilarVertexIndex_fast(const PackedVertex &packed,
                                           QMap<PackedVertex, unsigned short> &VertexToOutIndex,
                                           unsigned short &result);
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
