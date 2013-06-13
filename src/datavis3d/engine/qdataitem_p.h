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
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QDATAITEM_P_H
#define QDATAITEM_P_H

#include "qdatavis3dglobal.h"
#include "qdataitem.h"
#include "labelitem_p.h"

#include <QOpenGLFunctions>
#include <QSize>
#include <QString>
#include <QVector3D>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QDataItemPrivate
{
    public:
    explicit QDataItemPrivate(QDataItem *q, float value = 0.0f,
                              const QString &label = QString());
    ~QDataItemPrivate();

    // Position in 3D scene
    void setTranslation(const QVector3D &translation);
    QVector3D translation();
    // Value of bar
    float value();
     // Value and label appended into a string. If label has prepend -flag set, append label and value
    QString valueStr();
    // Label item (containing valueStr as texture)
    void setLabel(const LabelItem &label);
    LabelItem label();
    // Selection label item (containing specialar selection texture, if mode is activated)
    void setSelectionLabel(const LabelItem &label);
    LabelItem selectionLabel();
    QPointF position();

    private:
    QDataItem *q_ptr;
    float m_value;
    QString m_labelString;
    bool m_prependLabel;
    QVector3D m_translation;
    LabelItem m_label;
    LabelItem m_selectionLabel;
    QPointF m_position;
    friend class QDataItem;
};

QT_DATAVIS3D_END_NAMESPACE

#endif
