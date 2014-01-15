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

#include "axisrendercache_p.h"
#include "qmath.h"
#include <QFontMetrics>

namespace QtDataVisualization {

AxisRenderCache::AxisRenderCache()
    : m_type(QAbstract3DAxis::AxisTypeNone),
      m_min(0.0f),
      m_max(10.0f),
      m_segmentCount(5),
      m_subSegmentCount(1),
      m_font(QFont(QStringLiteral("Arial"))),
      m_drawer(0),
      m_segmentStep(10.0f),
      m_subSegmentStep(10.0f)
{
}

AxisRenderCache::~AxisRenderCache()
{
    foreach (LabelItem *label, m_labelItems)
        delete label;
}

void AxisRenderCache::setDrawer(Drawer *drawer)
{
    m_drawer = drawer;
    m_font = m_drawer->font();
    if (m_drawer) {
        QObject::connect(m_drawer, &Drawer::drawerChanged, this, &AxisRenderCache::updateTextures);
        updateTextures();
    }
}

void AxisRenderCache::setType(QAbstract3DAxis::AxisType type)
{
    m_type = type;

    // If type is set, it means completely new axis instance, so clear all old data
    m_labels.clear();
    m_title.clear();
    m_min = 0.0f;
    m_max = 10.0f;
    m_segmentCount = 5;
    m_subSegmentCount = 1;
    m_labelFormat.clear();

    m_titleItem.clear();
    foreach (LabelItem *label, m_labelItems)
        delete label;
    m_labelItems.clear();
    m_segmentStep = 10.0f;
    m_subSegmentStep = 10.0f;
}

void AxisRenderCache::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        // Generate axis label texture
        if (m_drawer)
            m_drawer->generateLabelItem(m_titleItem, title);
    }
}

void AxisRenderCache::setLabels(const QStringList &labels)
{
    if (m_labels != labels) {
        int newSize(labels.size());
        int oldSize(m_labels.size());

        for (int i = newSize; i < oldSize; i++)
            delete m_labelItems.takeLast();

        m_labelItems.reserve(newSize);

        int widest = maxLabelWidth(labels);

        for (int i = 0; i < newSize; i++) {
            if (i >= oldSize)
                m_labelItems.append(new LabelItem);
            if (m_drawer) {
                if (labels.at(i).isEmpty())
                    m_labelItems[i]->clear();
                else if (i >= oldSize || labels.at(i) != m_labels.at(i))
                    m_drawer->generateLabelItem(*m_labelItems[i], labels.at(i), widest);
            }
        }
        m_labels = labels;
    }
}

void AxisRenderCache::setMin(float min)
{
    m_min = min;
    updateSegmentStep();
}

void AxisRenderCache::setMax(float max)
{
    m_max = max;
    updateSegmentStep();
}

void AxisRenderCache::setSegmentCount(int count)
{
    m_segmentCount = count;
    updateSegmentStep();
}

void AxisRenderCache::setSubSegmentCount(int count)
{
    m_subSegmentCount = count;
    updateSubSegmentStep();
}

void AxisRenderCache::updateTextures()
{
    m_font = m_drawer->font();

    if (m_title.isEmpty())
        m_titleItem.clear();
    else
        m_drawer->generateLabelItem(m_titleItem, m_title);

    int widest = maxLabelWidth(m_labels);

    for (int i = 0; i < m_labels.size(); i++) {
        if (m_labels.at(i).isEmpty())
            m_labelItems[i]->clear();
        else
            m_drawer->generateLabelItem(*m_labelItems[i], m_labels.at(i), widest);
    }
}

void AxisRenderCache::updateSegmentStep()
{
    if (m_segmentCount > 0)
        m_segmentStep = qFabs((m_max - m_min) / m_segmentCount);
    else
        m_segmentStep = 0.0f; // Irrelevant
    updateSubSegmentStep();
}

void AxisRenderCache::updateSubSegmentStep()
{
    if (m_subSegmentCount > 1)
        m_subSegmentStep = m_segmentStep / m_subSegmentCount;
    else
        m_subSegmentStep = m_segmentStep;
}

int AxisRenderCache::maxLabelWidth(const QStringList &labels) const
{
    int labelWidth = 0;
    QFont labelFont = m_font;
    labelFont.setPointSize(textureFontSize);
    QFontMetrics labelFM(labelFont);
    for (int i = 0; i < labels.size(); i++) {
        int newWidth = labelFM.width(labels.at(i));
        if (labelWidth < newWidth)
            labelWidth = newWidth;
    }
    return labelWidth;
}

}
