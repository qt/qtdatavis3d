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

#include "qdataitem.h"
#include "qdataitem_p.h"

#include <QPoint>
#include <QString>

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

QDataItem::QDataItem(float value, const QString &label)
    : d_ptr(new QDataItemPrivate(this, value, label))
{
    //qDebug("QDataItem");
}

QDataItem::~QDataItem()
{
    //qDebug("~QDataItem");
}

void QDataItem::setLabel(const QString &label, bool prepend)
{
    d_ptr->m_labelString = label;
    d_ptr->m_prependLabel = prepend;
}

void QDataItem::setValue(float value)
{
    d_ptr->m_value = value;
}

void QDataItem::setPosition(const QPointF &position)
{
    d_ptr->m_position = position;
}

QDataItemPrivate::QDataItemPrivate(QDataItem *q, float value, const QString &label)
    : q_ptr(q),
      m_value(value),
      m_labelString(label),
      m_prependLabel(false),
      m_translation(QVector3D(0, 0, 0)),
      m_label(LabelItem()),
      m_selectionLabel(LabelItem())
{
}

QDataItemPrivate::~QDataItemPrivate()
{
}

void QDataItemPrivate::setTranslation(const QVector3D &translation)
{
    m_translation = translation;
}

QVector3D QDataItemPrivate::translation()
{
    return m_translation;
}

float QDataItemPrivate::value()
{
    return m_value;
}

QString QDataItemPrivate::valueStr()
{
    QString strVal;
    QString numStr;
    numStr.setNum(m_value);
    if (m_prependLabel) {
        strVal.append(m_labelString);
        strVal.append(QStringLiteral(" "));
        strVal.append(numStr);
    } else {
        strVal.append(numStr);
        strVal.append(m_labelString);
    }
    return strVal;
}

void QDataItemPrivate::setLabel(const LabelItem &label)
{
    m_label = label;
}

LabelItem QDataItemPrivate::label()
{
    return m_label;
}

void QDataItemPrivate::setSelectionLabel(const LabelItem &label)
{
    m_selectionLabel = label;
}

LabelItem QDataItemPrivate::selectionLabel()
{
    return m_selectionLabel;
}

QPointF QDataItemPrivate::position()
{
    return m_position;
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE
