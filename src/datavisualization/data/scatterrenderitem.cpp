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

#include "scatterrenderitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

ScatterRenderItem::ScatterRenderItem()
    : AbstractRenderItem(),
      m_visible(false)
{
}

ScatterRenderItem::ScatterRenderItem(const ScatterRenderItem &other)
    : AbstractRenderItem(other)
{
    m_position = other.m_position;
    m_visible = other.m_visible;
}

ScatterRenderItem::~ScatterRenderItem()
{
}

QT_END_NAMESPACE_DATAVISUALIZATION
