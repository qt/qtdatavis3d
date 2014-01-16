/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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

#ifndef SCATTERRENDERITEM_P_H
#define SCATTERRENDERITEM_P_H

#include "abstractrenderitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Scatter3DRenderer;

class ScatterRenderItem : public AbstractRenderItem
{
public:
    ScatterRenderItem();
    ScatterRenderItem(const ScatterRenderItem &other);
    virtual ~ScatterRenderItem();

    inline const QVector3D &position() const { return m_position; }
    inline void setPosition(const QVector3D &pos)
    {
        if (m_position != pos) {
            m_position = pos;
            // Force reformatting on next access by setting label string to null string
            if (!m_selectionLabel.isNull())
                setSelectionLabel(QString());
        }
    }

    inline QQuaternion rotation() const { return m_rotation; }
    inline void setRotation(const QQuaternion &rotation)
    {
        if (m_rotation != rotation)
            m_rotation = rotation;
    }


    inline bool isVisible() const { return m_visible; }
    inline void setVisible(bool visible) { m_visible = visible; }

    // TODO: Will we ever support item specific size? If not, remove.
    //inline void setSize(float size);
    //inline float size() const { return m_size; }

protected:
    QVector3D m_position;
    QQuaternion m_rotation;
    bool m_visible;
    //float m_size; // TODO in case we need a fourth variable that adjusts scatter item size

    friend class QScatterDataItem;
};
typedef QVector<ScatterRenderItem> ScatterRenderItemArray;

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
