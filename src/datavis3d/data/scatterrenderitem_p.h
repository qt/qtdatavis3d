/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef SCATTERRENDERITEM_P_H
#define SCATTERRENDERITEM_P_H

#include "barrenderitem_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class Scatter3DRenderer;

class ScatterRenderItem : public BarRenderItem
{
public:
    ScatterRenderItem();
    virtual ~ScatterRenderItem();

    inline const QVector3D &position() const { return m_position; }
    inline void setPosition(const QVector3D &pos) { m_position = pos; }

    //inline void setSize(qreal size);
    //inline qreal size() const { return m_size; }

    // TODO should be in abstract, but currently there is no abstract renderer
    // TODO change when maps refactored
    inline void setRenderer(Scatter3DRenderer *renderer) { m_renderer = renderer; }

protected:
    Scatter3DRenderer *m_renderer;
    QVector3D m_position;
    //qreal m_size; // TODO in case we need a fourth variable that adjusts scatter item size

    friend class QScatterDataItem;
};

typedef QVector<ScatterRenderItem> ScatterRenderItemArray;

QT_DATAVIS3D_END_NAMESPACE

#endif
