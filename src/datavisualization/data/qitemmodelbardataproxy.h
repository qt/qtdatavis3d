/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
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

#ifndef QITEMMODELBARDATAPROXY_H
#define QITEMMODELBARDATAPROXY_H

#include <QtDataVisualization/qbardataproxy.h>
#include <QAbstractItemModel>
#include <QStringList>

namespace QtDataVisualization {

class QItemModelBarDataProxyPrivate;

class QT_DATAVISUALIZATION_EXPORT QItemModelBarDataProxy : public QBarDataProxy
{
    Q_OBJECT
    Q_PROPERTY(const QAbstractItemModel* itemModel READ itemModel WRITE setItemModel NOTIFY itemModelChanged)
    Q_PROPERTY(QString rowRole READ rowRole WRITE setRowRole NOTIFY rowRoleChanged)
    Q_PROPERTY(QString columnRole READ columnRole WRITE setColumnRole NOTIFY columnRoleChanged)
    Q_PROPERTY(QString valueRole READ valueRole WRITE setValueRole NOTIFY valueRoleChanged)
    Q_PROPERTY(QStringList rowCategories READ rowCategories WRITE setRowCategories NOTIFY rowCategoriesChanged)
    Q_PROPERTY(QStringList columnCategories READ columnCategories WRITE setColumnCategories NOTIFY columnCategoriesChanged)
    Q_PROPERTY(bool useModelCategories READ useModelCategories WRITE setUseModelCategories NOTIFY useModelCategoriesChanged)
    Q_PROPERTY(bool autoRowCategories READ autoRowCategories WRITE setAutoRowCategories NOTIFY autoRowCategoriesChanged)
    Q_PROPERTY(bool autoColumnCategories READ autoColumnCategories WRITE setAutoColumnCategories NOTIFY autoColumnCategoriesChanged)

public:
    explicit QItemModelBarDataProxy(QObject *parent = 0);
    QItemModelBarDataProxy(const QAbstractItemModel *itemModel, QObject *parent = 0);
    QItemModelBarDataProxy(const QAbstractItemModel *itemModel, const QString &valueRole,
                           QObject *parent = 0);
    QItemModelBarDataProxy(const QAbstractItemModel *itemModel, const QString &rowRole,
                           const QString &columnRole, const QString &valueRole,
                           QObject *parent = 0);
    QItemModelBarDataProxy(const QAbstractItemModel *itemModel, const QString &rowRole,
                           const QString &columnRole, const QString &valueRole,
                           const QStringList &rowCategories, const QStringList &columnCategories,
                           QObject *parent = 0);
    virtual ~QItemModelBarDataProxy();

    void setItemModel(const QAbstractItemModel *itemModel);
    const QAbstractItemModel *itemModel() const;

    void setRowRole(const QString &role);
    QString rowRole() const;
    void setColumnRole(const QString &role);
    QString columnRole() const;
    void setValueRole(const QString &role);
    QString valueRole() const;

    void setRowCategories(const QStringList &categories);
    QStringList rowCategories() const;
    void setColumnCategories(const QStringList &categories);
    QStringList columnCategories() const;

    void setUseModelCategories(bool enable);
    bool useModelCategories() const;
    void setAutoRowCategories(bool enable);
    bool autoRowCategories() const;
    void setAutoColumnCategories(bool enable);
    bool autoColumnCategories() const;

    void remap(const QString &rowRole, const QString &columnRole,
               const QString &valueRole, const QStringList &rowCategories,
               const QStringList &columnCategories);

    Q_INVOKABLE int rowCategoryIndex(const QString& category);
    Q_INVOKABLE int columnCategoryIndex(const QString& category);

signals:
    void itemModelChanged(const QAbstractItemModel* itemModel);
    void rowRoleChanged(QString role);
    void columnRoleChanged(QString role);
    void valueRoleChanged(QString role);
    void rowCategoriesChanged(QStringList categories);
    void columnCategoriesChanged(QStringList categories);
    void useModelCategoriesChanged(bool enable);
    void autoRowCategoriesChanged(bool enable);
    void autoColumnCategoriesChanged(bool enable);

protected:
    QItemModelBarDataProxyPrivate *dptr();
    const QItemModelBarDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QItemModelBarDataProxy)

    friend class BarItemModelHandler;
};

}

#endif
