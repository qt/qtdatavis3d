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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef BARRENDERITEM_P_H
#define BARRENDERITEM_P_H

#include "abstractrenderitem_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class Bars3dRenderer;

class BarRenderItem : public AbstractRenderItem
{
public:
    BarRenderItem();
    virtual ~BarRenderItem();

    // Position relative to data window (for bar label generation)
    inline const QPoint &position() { return m_position; }
    inline void setPosition(const QPoint &pos) { m_position = pos; }

    // Actual cached data value of the bar (needed to trigger label reformats)
    inline void setValue(qreal value);
    inline qreal value() { return m_value; }

    // Normalized bar height
    inline void setHeight(GLfloat height) { m_height = height; }
    inline GLfloat height() { return m_height; }

    // TODO should be in abstract, but currently there is no abstract renderer
    inline void setRenderer(Bars3dRenderer *renderer) { m_renderer = renderer; }

protected:
    virtual void formatLabel();

    Bars3dRenderer *m_renderer;
    qreal m_value;
    QPoint m_position; // x = row, y = column
    GLfloat m_height;

    friend class QBarDataItem;
};

void BarRenderItem::setValue(qreal value)
{
    if (m_value != value) {
        m_value = value;
        if (!m_label.isNull())
            setLabel(QString()); // Forces reformatting on next access
    }
}

typedef QVector<BarRenderItem> BarRenderItemRow;
typedef QVector<BarRenderItemRow> BarRenderItemArray;

QT_DATAVIS3D_END_NAMESPACE

#endif
