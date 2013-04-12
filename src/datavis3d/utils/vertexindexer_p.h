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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

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
