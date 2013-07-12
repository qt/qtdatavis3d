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
    virtual void formatLabel();

    Maps3DController *m_renderer;
    QPointF m_mapPosition;
    QString m_itemLabel; // from QMapDataItem::label() - unformatted item label

    friend class QMapDataItem;
};

typedef QVector<MapRenderItem> MapRenderItemArray;

QT_DATAVIS3D_END_NAMESPACE

#endif
