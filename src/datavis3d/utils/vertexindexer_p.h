#ifndef VERTEXINDEXER_P_H
#define VERTEXINDEXER_P_H

#include "qdatavis3dglobal.h"

#include <QVector>
#include <QVector2D>
#include <QVector3D>

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

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

    static void indexVBO(const QVector<QVector3D> &in_vertices
                         , const QVector<QVector2D> &in_uvs
                         , const QVector<QVector3D> &in_normals
                         , QVector<unsigned short> &out_indices
                         , QVector<QVector3D> &out_vertices
                         , QVector<QVector2D> &out_uvs
                         , QVector<QVector3D> &out_normals);

    static void indexVBO_TBN(const QVector<QVector3D> &in_vertices
                             , const QVector<QVector2D> &in_uvs
                             , const QVector<QVector3D> &in_normals
                             , const QVector<QVector3D> &in_tangents
                             , const QVector<QVector3D> &in_bitangents
                             , QVector<unsigned short> &out_indices
                             , QVector<QVector3D> &out_vertices
                             , QVector<QVector2D> &out_uvs
                             , QVector<QVector3D> &out_normals
                             , QVector<QVector3D> &out_tangents
                             , QVector<QVector3D> &out_bitangents);

    private:
    static bool is_near(float v1, float v2);
    static bool getSimilarVertexIndex(const QVector3D &in_vertex
                                      , const QVector2D &in_uv
                                      , const QVector3D &in_normal
                                      , QVector<QVector3D> &out_vertices
                                      , QVector<QVector2D> &out_uvs
                                      , QVector<QVector3D> &out_normals
                                      , unsigned short &result);
    static bool getSimilarVertexIndex_fast(const PackedVertex &packed
                                           , QMap<PackedVertex, unsigned short> &VertexToOutIndex
                                           , unsigned short &result);
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif
