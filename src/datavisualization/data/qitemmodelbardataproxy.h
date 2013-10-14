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
#include <QtDataVisualization/qitemmodelbardatamapping.h>
#include <QAbstractItemModel>
#include <QStringList>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QItemModelBarDataProxyPrivate;

class QT_DATAVISUALIZATION_EXPORT QItemModelBarDataProxy : public QBarDataProxy
{
    Q_OBJECT
    Q_PROPERTY(const QAbstractItemModel* itemModel READ itemModel WRITE setItemModel)
    Q_PROPERTY(QItemModelBarDataMapping* activeMapping READ activeMapping WRITE setActiveMapping)

public:
    explicit QItemModelBarDataProxy();
    explicit QItemModelBarDataProxy(const QAbstractItemModel *itemModel,
                                    QItemModelBarDataMapping *mapping);
    virtual ~QItemModelBarDataProxy();

    void setItemModel(const QAbstractItemModel *itemModel);
    const QAbstractItemModel *itemModel() const;

    void setActiveMapping(QItemModelBarDataMapping *mapping);
    QItemModelBarDataMapping *activeMapping() const;
    void addMapping(QItemModelBarDataMapping *mapping);
    void releaseMapping(QItemModelBarDataMapping *mapping);
    QList<QItemModelBarDataMapping *> mappings() const;

protected:
    QItemModelBarDataProxyPrivate *dptr();
    const QItemModelBarDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QItemModelBarDataProxy)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif
