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

#include "customrenderitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

CustomRenderItem::CustomRenderItem()
    : AbstractRenderItem(),
      m_texture(0),
      m_object(0),
      m_visible(true),
      m_renderer(0)
{
}

CustomRenderItem::~CustomRenderItem()
{
    ObjectHelper::releaseObjectHelper(m_renderer, m_object);
}

void CustomRenderItem::setMesh(const QString &meshFile)
{
    ObjectHelper::resetObjectHelper(m_renderer, m_object, meshFile);
}

QT_END_NAMESPACE_DATAVISUALIZATION
