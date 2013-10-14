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

#include "barrenderitem_p.h"
#include "bars3drenderer_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

BarRenderItem::BarRenderItem()
    : AbstractRenderItem(),
      m_value(0),
      m_height(0.0f),
      m_sliceLabelItem(0)
{
}

BarRenderItem::BarRenderItem(const BarRenderItem &other)
    : AbstractRenderItem(other)
{
    m_value = other.m_value;
    m_position = other.m_position;
    m_height = other.m_height;
    m_sliceLabel = other.m_sliceLabel;
    m_sliceLabelItem = 0;
}

BarRenderItem::~BarRenderItem()
{
    delete m_sliceLabelItem;
}

LabelItem &BarRenderItem::sliceLabelItem()
{
    if (!m_sliceLabelItem)
        m_sliceLabelItem = new LabelItem;
    return *m_sliceLabelItem;
}

void BarRenderItem::setSliceLabel(const QString &label)
{
    if (m_sliceLabelItem)
        m_sliceLabelItem->clear();
    m_sliceLabel = label;
}

QString &BarRenderItem::sliceLabel()
{
    return m_sliceLabel;
}


QT_DATAVISUALIZATION_END_NAMESPACE
