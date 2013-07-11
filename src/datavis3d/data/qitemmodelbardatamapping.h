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

#ifndef QITEMMODELBARDATAMAPPING_H
#define QITEMMODELBARDATAMAPPING_H

#include "qdatavis3dnamespace.h"
#include <QStringList>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QItemModelBarDataMappingPrivate;

class QT_DATAVIS3D_EXPORT QItemModelBarDataMapping : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString rowRole READ rowRole WRITE setRowRole)
    Q_PROPERTY(QString columnRole READ columnRole WRITE setColumnRole)
    Q_PROPERTY(QString valueRole READ valueRole WRITE setValueRole)
    Q_PROPERTY(QStringList rowCategories READ rowCategories WRITE setRowCategories)
    Q_PROPERTY(QStringList columnCategories READ columnCategories WRITE setColumnCategories)
public:
    explicit QItemModelBarDataMapping();
    QItemModelBarDataMapping(const QItemModelBarDataMapping &other);
    QItemModelBarDataMapping(const QString &rowRole, const QString &columnRole,
                             const QString &valueRole, const QStringList &rowCategories,
                             const QStringList &columnCategories);
    virtual ~QItemModelBarDataMapping();

    QItemModelBarDataMapping &operator=(const QItemModelBarDataMapping &other);

    // If row categories or column categories is an empty list, use item models's rows and columns for rows and columns.
    // If the categories are both defined, ignore item model's rows and columns and figure out the rows and columns from
    // the values of the set roles for each item.

    void setRowRole(const QString &role);
    QString rowRole() const;
    void setColumnRole(const QString &role);
    QString columnRole() const;
    void setValueRole(const QString &role);
    QString valueRole() const;

    void setRowCategories(const QStringList &categories);
    const QStringList &rowCategories() const;
    void setColumnCategories(const QStringList &categories);
    const QStringList &columnCategories() const;

    void remap(const QString &rowRole, const QString &columnRole,
               const QString &valueRole, const QStringList &rowCategories,
               const QStringList &columnCategories);
signals:
    void mappingChanged();

private:
    QScopedPointer<QItemModelBarDataMappingPrivate> d_ptr;
};


QT_DATAVIS3D_END_NAMESPACE

#endif
