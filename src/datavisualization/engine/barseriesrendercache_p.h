/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
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

#ifndef BARSERIESRENDERCACHE_P_H
#define BARSERIESRENDERCACHE_P_H

#include "datavisualizationglobal_p.h"
#include "seriesrendercache_p.h"
#include "qbar3dseries_p.h"
#include "barrenderitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class BarSeriesRenderCache : public SeriesRenderCache
{
public:
    BarSeriesRenderCache(QAbstract3DSeries *series, Abstract3DRenderer *renderer);
    virtual ~BarSeriesRenderCache();

    void cleanup(TextureHelper *texHelper);

    inline BarRenderItemArray &renderArray() { return m_renderArray; }
    inline QBar3DSeries *series() const { return static_cast<QBar3DSeries *>(m_series); }
    inline QVector<BarRenderSliceItem> &sliceArray() { return m_sliceArray; }
    inline void setVisualIndex(int index) { m_visualIndex = index; }
    inline int visualIndex() {return m_visualIndex; }

protected:
    BarRenderItemArray m_renderArray;
    QVector<BarRenderSliceItem> m_sliceArray;
    int m_visualIndex; // order of the series is relevant
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
