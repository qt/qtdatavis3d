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

#include "abstractrenderitem_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

AbstractRenderItem::AbstractRenderItem()
    : m_selectionLabelItem(0)
{
}

AbstractRenderItem::AbstractRenderItem(const AbstractRenderItem &other)
{
    m_selectionLabel = other.m_selectionLabel;
    m_translation = other.m_translation;
    m_selectionLabelItem = 0;
}

AbstractRenderItem::~AbstractRenderItem()
{
    delete m_selectionLabelItem;
}

LabelItem &AbstractRenderItem::selectionLabelItem()
{
    if (!m_selectionLabelItem)
        m_selectionLabelItem = new LabelItem;
    return *m_selectionLabelItem;
}

void AbstractRenderItem::setSelectionLabel(const QString &label)
{
    if (m_selectionLabelItem)
        m_selectionLabelItem->clear();
    m_selectionLabel = label;
}

QString &AbstractRenderItem::selectionLabel()
{
    return m_selectionLabel;
}


QT_DATAVIS3D_END_NAMESPACE
