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

#ifndef QDATAITEM_H
#define QDATAITEM_H

#include "QtDataVis3D/qdatavis3dglobal.h"
#include <QScopedPointer>
#include <QString>
#include <QObject>
#include <QPointF>

class QPoint;

QT_DATAVIS3D_BEGIN_NAMESPACE

class QDataItemPrivate;

class QT_DATAVIS3D_EXPORT QDataItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label WRITE setLabel)
    Q_PROPERTY(float value READ value WRITE setValue)
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(QPointF position READ position WRITE setPosition)

public:
    QDataItem(float value = 0.0f, const QString &label = QString());
    QDataItem(QDataItem &item);
    ~QDataItem();

    // TODO: Provide a Q_INVOKABLE version of this, or move prepend to it's own property.
    void setLabel(const QString &label, bool prepend = false); // label for value, unit for example
    QString label();
    void setValue(float value);
    void setValue(int value);
    float value();
    // Has no effect in Q3DBars
    void setPosition(const QPointF &position);
    void setPosition(const QPoint &position);
    QPointF position();

private:
    QScopedPointer<QDataItemPrivate> d_ptr;
    friend class Bars3dRenderer;
    friend class Q3DMaps;
    friend class Q3DMapsPrivate;
    friend class QDataRowPrivate;
    friend class Drawer;
};

QT_DATAVIS3D_END_NAMESPACE

#endif
