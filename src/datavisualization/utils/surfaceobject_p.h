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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef SURFACEOBJECT_P_H
#define SURFACEOBJECT_P_H

#include "datavisualizationglobal_p.h"
#include "abstractobjecthelper_p.h"
#include "qsurfacedataproxy.h"

#include <QtCore/QRect>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Surface3DRenderer;
class AxisRenderCache;

class SurfaceObject : public AbstractObjectHelper
{
public:
    enum SurfaceType {
        SurfaceSmooth,
        SurfaceFlat,
        Undefined
    };

    enum DataDimensions {
        BothAscending = 0,
        XDescending = 1,
        ZDescending = 2,
        BothDescending = XDescending | ZDescending
    };

public:
    SurfaceObject(Surface3DRenderer *renderer);
    virtual ~SurfaceObject();

    void setUpData(const QSurfaceDataArray &dataArray, const QRect &space,
                   bool changeGeometry, bool polar, bool flipXZ = false);
    void setUpSmoothData(const QSurfaceDataArray &dataArray, const QRect &space,
                         bool changeGeometry, bool polar, bool flipXZ = false);
    void smoothUVs(const QSurfaceDataArray &dataArray, const QSurfaceDataArray &modelArray);
    void coarseUVs(const QSurfaceDataArray &dataArray, const QSurfaceDataArray &modelArray);
    void updateCoarseRow(const QSurfaceDataArray &dataArray, int rowIndex, bool polar);
    void updateSmoothRow(const QSurfaceDataArray &dataArray, int startRow, bool polar);
    void updateSmoothItem(const QSurfaceDataArray &dataArray, int row, int column, bool polar);
    void updateCoarseItem(const QSurfaceDataArray &dataArray, int row, int column, bool polar);
    void createSmoothIndices(int x, int y, int endX, int endY);
    void createCoarseSubSection(int x, int y, int columns, int rows);
    void createSmoothGridlineIndices(int x, int y, int endX, int endY);
    void createCoarseGridlineIndices(int x, int y, int endX, int endY);
    void uploadBuffers();
    GLuint gridElementBuf();
    GLuint uvBuf();
    GLuint gridIndexCount();
    QVector3D vertexAt(int column, int row);
    void clear();
    float minYValue() const { return m_minY; }
    float maxYValue() const { return m_maxY; }
    inline void activateSurfaceTexture(bool value) { m_returnTextureBuffer = value; }

private:
    void createCoarseIndices(GLint *indices, int &p, int row, int upperRow, int j);
    void createNormals(int &p, int row, int upperRow, int j);
    void createSmoothNormalBodyLine(int &totalIndex, int column);
    void createSmoothNormalUpperLine(int &totalIndex);
    QVector3D createSmoothNormalBodyLineItem(int x, int y);
    QVector3D createSmoothNormalUpperLineItem(int x, int y);
    QVector3D normal(const QVector3D &a, const QVector3D &b, const QVector3D &c);
    void createBuffers(const QVector<QVector3D> &vertices, const QVector<QVector2D> &uvs,
                       const QVector<QVector3D> &normals, const GLint *indices);
    void checkDirections(const QSurfaceDataArray &array);
    inline void getNormalizedVertex(const QSurfaceDataItem &data, QVector3D &vertex, bool polar,
                                    bool flipXZ);

private:
    SurfaceType m_surfaceType;
    int m_columns;
    int m_rows;
    GLuint m_gridElementbuffer;
    GLuint m_gridIndexCount;
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_normals;
    // Caches are not owned
    AxisRenderCache &m_axisCacheX;
    AxisRenderCache &m_axisCacheY;
    AxisRenderCache &m_axisCacheZ;
    Surface3DRenderer *m_renderer;
    float m_minY;
    float m_maxY;
    GLuint m_uvTextureBuffer;
    bool m_returnTextureBuffer;
    int m_dataDimension;
    int m_oldDataDimension;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
