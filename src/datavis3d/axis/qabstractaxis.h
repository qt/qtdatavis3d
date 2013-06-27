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

#ifndef QABSTRACTAXIS_H
#define QABSTRACTAXIS_H

#include "qdatavis3dnamespace.h"
#include <QObject>
#include <QScopedPointer>
#include <QVector>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QAbstractAxisPrivate;

class QT_DATAVIS3D_EXPORT QAbstractAxis : public QObject
{
    Q_OBJECT
    Q_ENUMS(AxisOrientation)
    Q_ENUMS(AxisType)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(AxisOrientation orientation READ orientation)
    Q_PROPERTY(AxisType type READ type)

public:
    enum AxisOrientation {
        AxisOrientationNone = 0,
        AxisOrientationX = 1,
        AxisOrientationY = 2,
        AxisOrientationZ = 4
    };

    enum AxisType {
        AxisTypeNone = 0,
        AxisTypeCategory = 1,
        AxisTypeValue = 2
        //AxisTypeLogValue = 4 // TODO
    };

protected:
    explicit QAbstractAxis(QAbstractAxisPrivate *d, QObject *parent = 0);
public:
    virtual ~QAbstractAxis();

    QString title() const;
    QVector<QString> labels() const;

    AxisOrientation orientation() const;
    AxisType type() const;

public slots:
    void setTitle(QString title);

signals:
    void titleChanged(QString newTitle);

protected:
    QScopedPointer<QAbstractAxisPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QAbstractAxis)

    friend class QAbstractDataSetPrivate;
    friend class QDataSetPrivate;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QABSTRACTAXIS_H
