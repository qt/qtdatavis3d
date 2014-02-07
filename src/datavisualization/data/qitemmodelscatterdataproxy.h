/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#ifndef QITEMMODELSCATTERDATAPROXY_H
#define QITEMMODELSCATTERDATAPROXY_H

#include <QtDataVisualization/qscatterdataproxy.h>
#include <QAbstractItemModel>
#include <QStringList>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QItemModelScatterDataProxyPrivate;

class QT_DATAVISUALIZATION_EXPORT QItemModelScatterDataProxy : public QScatterDataProxy
{
    Q_OBJECT
    Q_PROPERTY(const QAbstractItemModel* itemModel READ itemModel WRITE setItemModel NOTIFY itemModelChanged)
    Q_PROPERTY(QString xPosRole READ xPosRole WRITE setXPosRole NOTIFY xPosRoleChanged)
    Q_PROPERTY(QString yPosRole READ yPosRole WRITE setYPosRole NOTIFY yPosRoleChanged)
    Q_PROPERTY(QString zPosRole READ zPosRole WRITE setZPosRole NOTIFY zPosRoleChanged)
    Q_PROPERTY(QString rotationRole READ rotationRole WRITE setRotationRole NOTIFY rotationRoleChanged)

public:
    explicit QItemModelScatterDataProxy(QObject *parent = 0);
    QItemModelScatterDataProxy(const QAbstractItemModel *itemModel, QObject *parent = 0);
    QItemModelScatterDataProxy(const QAbstractItemModel *itemModel,
                               const QString &xPosRole, const QString &yPosRole,
                               const QString &zPosRole, QObject *parent = 0);
    QItemModelScatterDataProxy(const QAbstractItemModel *itemModel,
                               const QString &xPosRole, const QString &yPosRole,
                               const QString &zPosRole, const QString &rotationRole,
                               QObject *parent = 0);
    virtual ~QItemModelScatterDataProxy();

    void setItemModel(const QAbstractItemModel *itemModel);
    const QAbstractItemModel *itemModel() const;

    void setXPosRole(const QString &role);
    QString xPosRole() const;
    void setYPosRole(const QString &role);
    QString yPosRole() const;
    void setZPosRole(const QString &role);
    QString zPosRole() const;
    void setRotationRole(const QString &role);
    QString rotationRole() const;

    void remap(const QString &xPosRole, const QString &yPosRole, const QString &zPosRole,
               const QString &rotationRole);

signals:
    void itemModelChanged(const QAbstractItemModel* itemModel);
    void xPosRoleChanged(QString role);
    void yPosRoleChanged(QString role);
    void zPosRoleChanged(QString role);
    void rotationRoleChanged(QString role);

protected:
    QItemModelScatterDataProxyPrivate *dptr();
    const QItemModelScatterDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QItemModelScatterDataProxy)

    friend class ScatterItemModelHandler;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
