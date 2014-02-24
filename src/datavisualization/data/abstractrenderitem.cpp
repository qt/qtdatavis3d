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

#include "abstractrenderitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

AbstractRenderItem::AbstractRenderItem()
{
}

AbstractRenderItem::AbstractRenderItem(const AbstractRenderItem &other)
{
    m_translation = other.m_translation;
    m_rotation = other.m_rotation;
}

AbstractRenderItem::~AbstractRenderItem()
{
}

QT_END_NAMESPACE_DATAVISUALIZATION
