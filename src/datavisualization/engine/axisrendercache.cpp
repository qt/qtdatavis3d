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

#include "axisrendercache_p.h"

#include <QtCore/qmath.h>
#include <QtGui/QFontMetrics>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

AxisRenderCache::AxisRenderCache()
    : m_type(QAbstract3DAxis::AxisTypeNone),
      m_min(0.0f),
      m_max(10.0f),
      m_segmentCount(5),
      m_subSegmentCount(1),
      m_font(QFont(QStringLiteral("Arial"))),
      m_formatter(0),
      m_ctrlFormatter(0),
      m_drawer(0),
      m_positionsDirty(true),
      m_translate(0.0f),
      m_scale(1.0f)
{
}

AxisRenderCache::~AxisRenderCache()
{
    foreach (LabelItem *label, m_labelItems)
        delete label;

    delete m_formatter;
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

void AxisRenderCache::updateAllPositions()
{
    // As long as grid and subgrid lines are drawn identically, we can further optimize
    // by caching all grid and subgrid positions into a single vector.
    // If subgrid lines are ever themed separately, this array will probably become obsolete.
    if (m_formatter) {
        int subGridCount = m_subSegmentCount - 1;
        int fullSize = m_segmentCount + 1 + (m_segmentCount * subGridCount);
        m_adjustedGridLinePositions.resize(fullSize);
        m_adjustedLabelPositions.resize(m_segmentCount + 1);
        int index = 0;
        int segment = 0;
        for (; segment < m_segmentCount; segment++) {
            m_adjustedLabelPositions[segment] =
                    m_formatter->labelPositions().at(segment) * m_scale + m_translate;
            m_adjustedGridLinePositions[index++] =
                    m_formatter->gridPositions().at(segment) * m_scale + m_translate;
            if (subGridCount > 0) {
                for (int subGrid = 0; subGrid < subGridCount; subGrid++) {
                    m_adjustedGridLinePositions[index++] =
                            m_formatter->subGridPositions().at(segment).at(subGrid) * m_scale + m_translate;
                }
            }
        }
        // Last gridline
        m_adjustedLabelPositions[segment] =
                m_formatter->labelPositions().at(segment) * m_scale + m_translate;
        m_adjustedGridLinePositions[index] =
                m_formatter->gridPositions().at(segment) * m_scale + m_translate;

        m_positionsDirty = false;
    }
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

QT_END_NAMESPACE_DATAVISUALIZATION
