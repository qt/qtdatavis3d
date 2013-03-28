#ifndef MESHLOADER_P_H
#define MESHLOADER_P_H

#include "qdatavis3dglobal.h"

class QVector2D;
class QVector3D;

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

class MeshLoader
{
public:
    static bool loadOBJ(const QString &path,
                        QVector<QVector3D> &out_vertices,
                        QVector<QVector2D> &out_uvs,
                        QVector<QVector3D> &out_normals);
    // TODO: add loaders for other formats?
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif
