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

#ifndef QSURFACEDATAPROXY_H
#define QSURFACEDATAPROXY_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtDataVisualization/qabstractdataproxy.h>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

typedef qreal QSurfaceDataItem; // Let's stick to this for now, expand if needed
typedef QVector<QSurfaceDataItem> QSurfaceDataRow;
typedef QList<QSurfaceDataRow *> QSurfaceDataArray;

class QSurfaceDataProxyPrivate;

class QT_DATAVISUALIZATION_EXPORT QSurfaceDataProxy : public QAbstractDataProxy
{
    Q_OBJECT

public:
    explicit QSurfaceDataProxy(QObject *parent = 0);
    virtual ~QSurfaceDataProxy();

    int itemCount() const;
    const QSurfaceDataArray *array() const;
    const QSurfaceDataItem *itemAt(int index) const;

    void resetArray(QSurfaceDataArray *newArray);

signals:
    void arrayReset();

protected:
    explicit QSurfaceDataProxy(QSurfaceDataProxyPrivate *d, QObject *parent = 0);
    QSurfaceDataProxyPrivate *dptr();
    const QSurfaceDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QSurfaceDataProxy)

    friend class Surface3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QSURFACEDATAPROXY_H
