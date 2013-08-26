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

#ifndef QITEMMODELSCATTERDATAPROXY_H
#define QITEMMODELSCATTERDATAPROXY_H

#include <QtDataVis3D/qscatterdataproxy.h>
#include <QtDataVis3D/qitemmodelscatterdatamapping.h>
#include <QAbstractItemModel>
#include <QStringList>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QItemModelScatterDataProxyPrivate;

class QT_DATAVIS3D_EXPORT QItemModelScatterDataProxy : public QScatterDataProxy
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* itemModel READ itemModel WRITE setItemModel)
    Q_PROPERTY(QItemModelScatterDataMapping* mapping READ mapping WRITE setMapping)

public:
    explicit QItemModelScatterDataProxy();
    explicit QItemModelScatterDataProxy(QAbstractItemModel *itemModel,
                                        QItemModelScatterDataMapping *mapping);
    virtual ~QItemModelScatterDataProxy();

    void setItemModel(QAbstractItemModel *itemModel);
    QAbstractItemModel *itemModel();

    void setMapping(QItemModelScatterDataMapping *mapping);
    QItemModelScatterDataMapping *mapping();

protected:
    QItemModelScatterDataProxyPrivate *dptr();

private:
    Q_DISABLE_COPY(QItemModelScatterDataProxy)
};

QT_DATAVIS3D_END_NAMESPACE

#endif
