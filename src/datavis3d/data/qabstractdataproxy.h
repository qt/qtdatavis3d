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

#ifndef QABSTRACTDATAPROXY_H
#define QABSTRACTDATAPROXY_H

#include <QtDataVis3D/qdatavis3dnamespace.h>
#include <QObject>
#include <QScopedPointer>
#include <QMutex>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QAbstractDataProxyPrivate;

class QT_DATAVIS3D_EXPORT QAbstractDataProxy : public QObject
{
    Q_OBJECT
    Q_ENUMS(DataType)
    Q_PROPERTY(DataType type READ type)

public:
    enum DataType {
        DataTypeNone = 0,
        DataTypeBar = 1,
        DataTypeMap = 2,
        DataTypeScatter = 4,
        DataTypeSurface = 8
    };

protected:
    explicit QAbstractDataProxy(QAbstractDataProxyPrivate *d);
public:
    virtual ~QAbstractDataProxy();

    DataType type() const;

    // Items use this string to format single item labels, unless custom proxy initializes
    // item labels with something else.
    void setItemLabelFormat(const QString &format);
    QString itemLabelFormat() const;

    QMutex *mutex();

signals:
    void itemLabelFormatChanged();

protected:
    QScopedPointer<QAbstractDataProxyPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QAbstractDataProxy)
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QABSTRACTDATAPROXY_H
