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

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QDataItem
 * \inmodule QtDataVis3D
 * \brief The QDataItem class provides a container for data to be added to graphs.
 * \since 1.0.0
 *
 * A QDataItem holds data for a single bar in a Q3DMaps or Q3DBars graph.
 *
 * \sa QDataRow, QDataSet, {Qt Data Visualization 3D C++ Classes}
 */

/*!
 * \a value A float value of the data item.
 *
 * \a label A QString label for the item.
 *
 * Constructs QDataItem.
 */
QDataItem::QDataItem(float value, const QString &label)
    : QObject(0),
      d_ptr(new QDataItemPrivate(this, value, label))
{
}

/*!
 * \overload QDataItem::QDataItem
 *
 * \a item A float value of the data item.
 *
 * Constructs QDataItem.
 */
QDataItem::QDataItem(QDataItem &item)
    : QObject(0),
      d_ptr(new QDataItemPrivate(&item))
{
    d_ptr->m_labelString = item.d_ptr->m_labelString;
    d_ptr->m_position = item.d_ptr->m_position;
    d_ptr->m_prependLabel = item.d_ptr->m_prependLabel;
    d_ptr->m_translation = item.d_ptr->m_translation;
    d_ptr->m_value = item.d_ptr->m_value;
}

/*!
 * Destroys QDataItem.
 */
QDataItem::~QDataItem()
{
}

/*!
 * \property QDataItem::label
 *
 * \a label A QString label for the data item. Unit, for example.
 *
 * \a prepend A flag to indicate if the label is to be prepended or appended to the value.
 * \c false by default.
 *
 * Sets label for the data item.
 */
void QDataItem::setLabel(const QString &label, bool prepend)
{
    d_ptr->m_labelString = label;
    d_ptr->m_prependLabel = prepend;
}

QString QDataItem::label()
{
    return d_ptr->m_labelString;
}

/*!
 * \property QDataItem::value
 *
 * \a value A float value for the data item.
 *
 * Sets value for the data item.
 */
void QDataItem::setValue(float value)
{
    d_ptr->m_value = value;
}

/*!
 * \overload QDataItem::value
 *
 * \a value An int value for the data item.
 *
 * Sets value for the data item.
 */
void QDataItem::setValue(int value)
{
    d_ptr->m_value = (float)value;
}

float QDataItem::value()
{
    return d_ptr->m_value;
}

/*!
 * \property QDataItem::position
 *
 * \a position A QPointF position for the data item.
 *
 * Sets position for the data item. Has no effect in Q3DBars.
 */
void QDataItem::setPosition(const QPointF &position)
{
    d_ptr->m_position = position;
}

/*!
 * \overload QDataItem::position
 *
 * \a position A QPoint position for the data item.
 *
 * Sets position for the data item. Has no effect in Q3DBars.
 */
void QDataItem::setPosition(const QPoint &position)
{
    d_ptr->m_position = (QPointF)position;
}

QPointF QDataItem::position()
{
    return d_ptr->m_position;
}

QDataItemPrivate::QDataItemPrivate(QDataItem *q, float value, const QString &label)
    : q_ptr(q),
      m_value(value),
      m_labelString(label),
      m_prependLabel(false),
      m_translation(QVector3D(0, 0, 0))

{
}

QDataItemPrivate::QDataItemPrivate(QDataItem *q)
    : q_ptr(q),
      m_value(q->value()),
      m_labelString(q->label()),
      m_prependLabel(false),
      m_translation(QVector3D(0, 0, 0))

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

QPointF QDataItemPrivate::position()
{
    return m_position;
}

QT_DATAVIS3D_END_NAMESPACE
