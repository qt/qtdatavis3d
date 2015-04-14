/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
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

#ifndef SCATTERPOINTBUFFERHELPER_P_H
#define SCATTERPOINTBUFFERHELPER_P_H

#include "datavisualizationglobal_p.h"
#include "abstractobjecthelper_p.h"
#include "scatterseriesrendercache_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class ScatterPointBufferHelper : public AbstractObjectHelper
{
public:
    ScatterPointBufferHelper();
    virtual ~ScatterPointBufferHelper();

    GLuint pointBuf();

    void pushPoint(uint pointIndex);
    void popPoint();
    void load(ScatterSeriesRenderCache *cache);
    void update(ScatterSeriesRenderCache *cache);
    void setScaleY(float scale) { m_scaleY = scale; }
    void updateUVs(ScatterSeriesRenderCache *cache);

public:
    GLuint m_pointbuffer;

private:
    void createRangeGradientUVs(ScatterSeriesRenderCache *cache,
                                QVector<QVector2D> &buffered_uvs);

private:
    QVector<QVector3D> m_bufferedPoints;
    int m_oldRemoveIndex;
    float m_scaleY;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
