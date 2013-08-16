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

#ifndef QITEMMODELMAPDATAPROXY_H
#define QITEMMODELMAPDATAPROXY_H

#include <QtDataVis3D/qmapdataproxy.h>
#include <QtDataVis3D/qitemmodelmapdatamapping.h>
#include <QAbstractItemModel>
#include <QStringList>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QItemModelMapDataProxyPrivate;

class QT_DATAVIS3D_EXPORT QItemModelMapDataProxy : public QMapDataProxy
{
    Q_OBJECT

public:
    explicit QItemModelMapDataProxy();
    explicit QItemModelMapDataProxy(QAbstractItemModel *itemModel, QItemModelMapDataMapping *mapping);
    virtual ~QItemModelMapDataProxy();

    // Doesn't gain ownership of the model, but does connect to it to listen for data changes.
    void setItemModel(QAbstractItemModel *itemModel);
    QAbstractItemModel *itemModel();

    // Map maps role (label, xPos, yPos, value) to role in model
    // Doesn't gain ownership of mapping, but does connect to it to listen for mapping changes.
    // Modifying mapping that is set to proxy will trigger dataset re-resolving.
    void setMapping(QItemModelMapDataMapping *mapping);
    QItemModelMapDataMapping *mapping();

protected:
    QItemModelMapDataProxyPrivate *dptr();

private:
    Q_DISABLE_COPY(QItemModelMapDataProxy)
};

QT_DATAVIS3D_END_NAMESPACE

#endif
