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

#ifndef MAPRENDERITEM_P_H
#define MAPRENDERITEM_P_H

#include "barrenderitem_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class Maps3DRenderer;
class Maps3DController; // TODO remove when maps refactored

class MapRenderItem : public BarRenderItem
{
public:
    MapRenderItem();
    virtual ~MapRenderItem();

    inline const QPointF &mapPosition() const { return m_mapPosition; }
    inline void setMapPosition(const QPointF &pos) { m_mapPosition = pos; }

    inline const QString &itemLabel() const { return m_itemLabel; }
    inline void setItemLabel(const QString &label) { m_itemLabel = label; }

    // TODO should be in abstract, but currently there is no abstract renderer
    // TODO change when maps refactored
    inline void setRenderer(Maps3DController *renderer) { m_renderer = renderer; }

protected:
    Maps3DController *m_renderer;
    QPointF m_mapPosition;
    QString m_itemLabel; // from QMapDataItem::label() - unformatted item label

    friend class QMapDataItem;
};

typedef QVector<MapRenderItem> MapRenderItemArray;

QT_DATAVIS3D_END_NAMESPACE

#endif
