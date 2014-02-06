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

#ifndef SURFACESERIESRENDERCACHE_P_H
#define SURFACESERIESRENDERCACHE_P_H

#include "datavisualizationglobal_p.h"
#include "seriesrendercache_p.h"
#include "qabstract3dseries_p.h"
#include "qsurface3dseries_p.h"
#include "surfaceobject_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Abstract3DRenderer;
class ObjectHelper;
class TextureHelper;

class SurfaceSeriesRenderCache : public SeriesRenderCache
{
public:
    SurfaceSeriesRenderCache();
    virtual ~SurfaceSeriesRenderCache();

    void populate(QSurface3DSeries *series, Abstract3DRenderer *renderer);
    void cleanup(TextureHelper *texHelper);

    inline bool surfaceVisible() const { return m_surfaceVisible; }
    inline bool surfaceGridVisible() const { return m_surfaceGridVisible; }
    inline bool isFlatShadingEnabled() const { return m_surfaceFlatShading; }
    inline void setFlatShadingEnabled(bool enabled) { m_surfaceFlatShading = enabled; }
    inline void setFlatChangeAllowed(bool allowed) { m_flatChangeAllowed = allowed; }
    inline void setValid(bool valid) { m_valid = valid; }
    inline bool isValid() const { return m_valid; }
    inline SurfaceObject *surfaceObject() { return m_surfaceObj; }
    inline SurfaceObject *sliceSurfaceObject() { return m_sliceSurfaceObj; }
    inline const QRect sampleSpace() const { return m_sampleSpace; }
    inline void setSampleSpace(const QRect &sampleSpace) { m_sampleSpace = sampleSpace; }
    inline QSurface3DSeries *series() const { return static_cast<QSurface3DSeries *>(m_series); }
    inline QSurfaceDataArray &dataArray() { return m_dataArray; }
    inline QSurfaceDataArray &sliceDataArray() { return m_sliceDataArray; }
    inline bool isSeriesVisible() const { return m_series->isVisible(); }
    inline void setObjectDirty(bool state) { m_objectDirty = state; }
    inline bool objectDirty() const { return m_objectDirty; }
    inline void setSelectionTexture(GLuint texture) { m_selectionTexture = texture; }
    inline GLuint selectionTexture() const { return m_selectionTexture; }
    inline void setSelectionIdRange(uint start, uint end) { m_selectionIdStart = start;
                                                            m_selectionIdEnd = end; }
    inline uint selectionIdStart() const { return m_selectionIdStart; }
    inline bool isWithinIdRange(uint selection) const { return selection >= m_selectionIdStart &&
                                                        selection <= m_selectionIdEnd; }
    inline bool isFlatStatusDirty() const { return m_flatStatusDirty; }
    inline void setFlatStatusDirty(bool status) { m_flatStatusDirty = status; }

protected:
    bool m_surfaceVisible;
    bool m_surfaceGridVisible;
    bool m_surfaceFlatShading;
    SurfaceObject *m_surfaceObj;
    SurfaceObject *m_sliceSurfaceObj;
    QRect m_sampleSpace;
    QSurfaceDataArray m_dataArray;
    QSurfaceDataArray m_sliceDataArray;
    GLuint m_selectionTexture;
    uint m_selectionIdStart;
    uint m_selectionIdEnd;
    bool m_flatChangeAllowed;
    bool m_flatStatusDirty;

    bool m_valid;
    bool m_objectDirty;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
