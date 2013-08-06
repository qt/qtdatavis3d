/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "axisrendercache_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

AxisRenderCache::AxisRenderCache()
    : m_type(QAbstractAxis::AxisTypeNone),
      m_min(0.0),
      m_max(0.0),
      m_drawer(0)
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
    if (m_drawer) {
        QObject::connect(m_drawer, &Drawer::drawerChanged, this, &AxisRenderCache::updateTextures);
        updateTextures();
    }
}

void AxisRenderCache::setType(QAbstractAxis::AxisType type)
{
    m_type = type;

    // If type is set, it means completely new axis instance, so clear all generated label items.
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

        if (m_drawer) {
            for (int i = 0; i < newSize; i++) {
                if (i >= oldSize)
                    m_labelItems.append(new LabelItem);
                if (labels.at(i).isEmpty())
                    m_labelItems[i]->clear();
                else if (i >= oldSize || labels.at(i) != m_labels.at(i))
                    m_drawer->generateLabelItem(*m_labelItems[i], labels.at(i));
            }
        }
        m_labels = labels;
    }
}

void AxisRenderCache::updateTextures()
{
    if (m_title.isEmpty())
        m_titleItem.clear();
    else
        m_drawer->generateLabelItem(m_titleItem, m_title);

    for (int i = 0; i < m_labels.size(); i++) {
        if (m_labels.at(i).isEmpty())
            m_labelItems[i]->clear();
        else
            m_drawer->generateLabelItem(*m_labelItems[i], m_labels.at(i));
    }
}

QT_DATAVIS3D_END_NAMESPACE
