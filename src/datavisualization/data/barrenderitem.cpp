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

#include "barrenderitem_p.h"
#include "bars3drenderer_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

BarRenderItem::BarRenderItem()
    : AbstractRenderItem(),
      m_value(0),
      m_height(0.0f)
{
}

BarRenderItem::BarRenderItem(const BarRenderItem &other)
    : AbstractRenderItem(other)
{
    m_value = other.m_value;
    m_position = other.m_position;
    m_height = other.m_height;
}

BarRenderItem::~BarRenderItem()
{
}

BarRenderSliceItem::BarRenderSliceItem()
    : BarRenderItem(),
      m_sliceLabelItem(0)
{
}

BarRenderSliceItem::BarRenderSliceItem(const BarRenderSliceItem &other)
    : BarRenderItem(other)
{
    m_sliceLabel = other.m_sliceLabel;
    m_sliceLabelItem = 0;
}

BarRenderSliceItem::~BarRenderSliceItem()
{
    delete m_sliceLabelItem;
}

void BarRenderSliceItem::setItem(const BarRenderItem &renderItem)
{
    m_translation = renderItem.translation();
    m_rotation = renderItem.rotation();
    m_value = renderItem.value();
    m_position = renderItem.position();
    m_height = renderItem.height();
    m_sliceLabel = QString();
    m_sliceLabelItem = 0;
}

LabelItem &BarRenderSliceItem::sliceLabelItem()
{
    if (!m_sliceLabelItem)
        m_sliceLabelItem = new LabelItem;
    return *m_sliceLabelItem;
}

void BarRenderSliceItem::setSliceLabel(const QString &label)
{
    if (m_sliceLabelItem)
        m_sliceLabelItem->clear();
    m_sliceLabel = label;
}

const QString &BarRenderSliceItem::sliceLabel() const
{
    return m_sliceLabel;
}

QT_END_NAMESPACE_DATAVISUALIZATION
