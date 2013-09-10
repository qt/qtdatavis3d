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

#ifndef QITEMMODELSURFACEDATAPROXY_H
#define QITEMMODELSURFACEDATAPROXY_H

#include <QtDataVisualization/qsurfacedataproxy.h>
#include <QtDataVisualization/qitemmodelsurfacedatamapping.h>
#include <QAbstractItemModel>
#include <QStringList>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QItemModelSurfaceDataProxyPrivate;

class QT_DATAVISUALIZATION_EXPORT QItemModelSurfaceDataProxy : public QSurfaceDataProxy
{
    Q_OBJECT
    Q_PROPERTY(const QAbstractItemModel* itemModel READ itemModel WRITE setItemModel)
    Q_PROPERTY(QItemModelSurfaceDataMapping* activeMapping READ activeMapping WRITE setActiveMapping)

public:
    explicit QItemModelSurfaceDataProxy();
    explicit QItemModelSurfaceDataProxy(const QAbstractItemModel *itemModel,
                                        QItemModelSurfaceDataMapping *mapping);
    virtual ~QItemModelSurfaceDataProxy();

    void setItemModel(const QAbstractItemModel *itemModel);
    const QAbstractItemModel *itemModel() const;

    void setActiveMapping(QItemModelSurfaceDataMapping *mapping);
    QItemModelSurfaceDataMapping *activeMapping() const;
    void addMapping(QItemModelSurfaceDataMapping *mapping);
    void releaseMapping(QItemModelSurfaceDataMapping *mapping);
    QList<QItemModelSurfaceDataMapping *> mappings() const;

protected:
    QItemModelSurfaceDataProxyPrivate *dptr();
    const QItemModelSurfaceDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QItemModelSurfaceDataProxy)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif
