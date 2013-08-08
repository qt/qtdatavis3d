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

#ifndef AXISRENDERCACHE_P_H
#define AXISRENDERCACHE_P_H

#include "datavis3dglobal_p.h"
#include "labelitem_p.h"
#include "qabstractaxis_p.h"
#include "drawer_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class AxisRenderCache : public QObject
{
    Q_OBJECT
public:
    AxisRenderCache();
    virtual ~AxisRenderCache();

    void setDrawer(Drawer *drawer);

    void setType(QAbstractAxis::AxisType type);
    inline QAbstractAxis::AxisType type() const { return m_type; }
    void setTitle(const QString &title);
    inline const QString &title() { return m_title; }
    void setLabels(const QStringList &labels);
    inline const QStringList &labels() { return m_labels; }
    void setMin(qreal min);
    inline qreal min() { return m_min; }
    void setMax(qreal max);
    inline qreal max() { return m_max; }
    void setTickCount(int count);
    inline int tickCount() const { return m_tickCount; }
    void setSubTickCount(int count);
    inline int subTickCount() const { return m_subTickCount; }

    inline LabelItem &titleItem() { return m_titleItem; }
    inline QList<LabelItem *> &labelItems() { return m_labelItems; }
    inline GLfloat tickStep() const { return m_tickStep; }
    inline GLfloat subTickStep() const { return m_subTickStep; }

public slots:
    void updateTextures();

private:
    void updateTickStep();
    void updateSubTickStep();

    // Cached axis values
    QAbstractAxis::AxisType m_type;
    QString m_title;
    QStringList m_labels;
    qreal m_min;
    qreal m_max;
    int m_tickCount;
    int m_subTickCount;

    // Renderer items
    Drawer *m_drawer; // Not owned
    LabelItem m_titleItem;
    QList<LabelItem *> m_labelItems;
    GLfloat m_tickStep;
    GLfloat m_subTickStep;

    Q_DISABLE_COPY(AxisRenderCache)
};

QT_DATAVIS3D_END_NAMESPACE

#endif
