/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
**
** Licensees holding valid Qt Enterprise licenses may use this file in
** accordance with the Qt Enterprise License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
**
****************************************************************************/

#ifndef QITEMMODELBARDATAMAPPING_H
#define QITEMMODELBARDATAMAPPING_H

#include <QtDataVis3D/qdatavis3denums.h>
#include <QStringList>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QItemModelBarDataMappingPrivate;

class QT_DATAVIS3D_EXPORT QItemModelBarDataMapping : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString rowRole READ rowRole WRITE setRowRole NOTIFY mappingChanged)
    Q_PROPERTY(QString columnRole READ columnRole WRITE setColumnRole NOTIFY mappingChanged)
    Q_PROPERTY(QString valueRole READ valueRole WRITE setValueRole NOTIFY mappingChanged)
    Q_PROPERTY(QStringList rowCategories READ rowCategories WRITE setRowCategories NOTIFY mappingChanged)
    Q_PROPERTY(QStringList columnCategories READ columnCategories WRITE setColumnCategories NOTIFY mappingChanged)
public:
    explicit QItemModelBarDataMapping();
    QItemModelBarDataMapping(const QItemModelBarDataMapping &other);
    QItemModelBarDataMapping(const QString &valueRole);
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
