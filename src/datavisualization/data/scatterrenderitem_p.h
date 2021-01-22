/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
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

#ifndef SCATTERRENDERITEM_P_H
#define SCATTERRENDERITEM_P_H

#include "abstractrenderitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class ScatterRenderItem : public AbstractRenderItem
{
public:
    ScatterRenderItem();
    ScatterRenderItem(const ScatterRenderItem &other);
    virtual ~ScatterRenderItem();

    inline const QVector3D &position() const { return m_position; }
    inline void setPosition(const QVector3D &pos)
    {
        if (m_position != pos)
            m_position = pos;
    }

    inline bool isVisible() const { return m_visible; }
    inline void setVisible(bool visible) { m_visible = visible; }

protected:
    QVector3D m_position;
    bool m_visible;
};
typedef QVector<ScatterRenderItem> ScatterRenderItemArray;

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
