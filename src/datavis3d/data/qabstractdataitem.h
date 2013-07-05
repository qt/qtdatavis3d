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

#ifndef QABSTRACTDATAITEM_H
#define QABSTRACTDATAITEM_H

#include "qdatavis3dnamespace.h"
#include <QScopedPointer>
#include <QString>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QAbstractDataItemPrivate;

class QT_DATAVIS3D_EXPORT QAbstractDataItem
{
protected:
    explicit QAbstractDataItem(QAbstractDataItemPrivate *d);
public:
    virtual ~QAbstractDataItem();

    // Set custom label format for this item - otherwise uses label format specified by the proxy
    //TODO void setLabelFormat(const QString &labelFormat);

    // formatted label for value
    QString &label() const;

protected:
    QScopedPointer<QAbstractDataItemPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QAbstractDataItem)

    friend class Bars3dRenderer;
    friend class Bars3dController;
    friend class Maps3DController;
    friend class QDataRowPrivate;
    friend class Drawer;
};

QT_DATAVIS3D_END_NAMESPACE

#endif
