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

    inline const QPointF &scatterPosition() const { return m_scatterPosition; }
    inline void setScatterPosition(const QPointF &pos) { m_scatterPosition = pos; }

    //    inline void setSize(qreal size);
    //    inline qreal size() const { return m_size; }

    // TODO should be in abstract, but currently there is no abstract renderer
    // TODO change when maps refactored
    inline void setRenderer(Scatter3DRenderer *renderer) { m_renderer = renderer; }

protected:
    virtual void formatLabel();

    Scatter3DRenderer *m_renderer;
    QPointF m_scatterPosition;
    //    qreal m_size; // TODO in case we need a fourth variable that adjusts scatter item size

    friend class QScatterDataItem;
};

typedef QVector<ScatterRenderItem> ScatterRenderItemArray;

QT_DATAVIS3D_END_NAMESPACE

#endif
