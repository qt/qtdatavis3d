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
    : m_labelItem(0),
      m_selectionLabel(0)
{
}

AbstractRenderItem::~AbstractRenderItem()
{
    delete m_labelItem;
    delete m_selectionLabel;
}

LabelItem &AbstractRenderItem::labelItem()
{
    if (!m_labelItem)
        m_labelItem = new LabelItem;
    return *m_labelItem;
}

LabelItem &AbstractRenderItem::selectionLabel()
{
    if (!m_selectionLabel)
        m_selectionLabel = new LabelItem;
    return *m_selectionLabel;
}

QString &AbstractRenderItem::label()
{
    if (m_label.isNull())
        formatLabel();
    return m_label;
}

void AbstractRenderItem::setLabel(const QString &label)
{
    if (m_labelItem)
        m_labelItem->clear();
    if (m_selectionLabel)
        m_selectionLabel->clear();
    m_label = label;
}

QT_DATAVIS3D_END_NAMESPACE
