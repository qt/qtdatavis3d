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

#ifndef QSURFACE3DSERIES_H
#define QSURFACE3DSERIES_H

#include <QtDataVisualization/qabstract3dseries.h>
#include <QtDataVisualization/qsurfacedataproxy.h>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QSurface3DSeriesPrivate;

class QT_DATAVISUALIZATION_EXPORT QSurface3DSeries : public QAbstract3DSeries
{
    Q_OBJECT
    Q_PROPERTY(QSurfaceDataProxy *dataProxy READ dataProxy WRITE setDataProxy NOTIFY dataProxyChanged)
    Q_PROPERTY(QPoint selectedPoint READ selectedPoint WRITE setSelectedPoint NOTIFY selectedPointChanged)

public:
    explicit QSurface3DSeries(QObject *parent = 0);
    explicit QSurface3DSeries(QSurfaceDataProxy *dataProxy, QObject *parent = 0);
    virtual ~QSurface3DSeries();

    void setDataProxy(QSurfaceDataProxy *proxy);
    QSurfaceDataProxy *dataProxy() const;

    void setSelectedPoint(const QPoint &position);
    QPoint selectedPoint() const;
    QPoint invalidSelectionPosition() const;

signals:
    void dataProxyChanged(QSurfaceDataProxy *proxy);
    void selectedPointChanged(QPoint position);

protected:
    explicit QSurface3DSeries(QSurface3DSeriesPrivate *d, QObject *parent = 0);
    QSurface3DSeriesPrivate *dptr();
    const QSurface3DSeriesPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QSurface3DSeries)

    friend class Surface3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif
