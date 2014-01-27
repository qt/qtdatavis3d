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

#ifndef QSCATTER3DSERIES_H
#define QSCATTER3DSERIES_H

#include <QtDataVisualization/qabstract3dseries.h>
#include <QtDataVisualization/qscatterdataproxy.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QScatter3DSeriesPrivate;

class QT_DATAVISUALIZATION_EXPORT QScatter3DSeries : public QAbstract3DSeries
{
    Q_OBJECT
    Q_PROPERTY(QScatterDataProxy *dataProxy READ dataProxy WRITE setDataProxy NOTIFY dataProxyChanged)
    Q_PROPERTY(int selectedItem READ selectedItem WRITE setSelectedItem NOTIFY selectedItemChanged)
    Q_PROPERTY(float itemSize READ itemSize WRITE setItemSize NOTIFY itemSizeChanged)

public:
    explicit QScatter3DSeries(QObject *parent = 0);
    explicit QScatter3DSeries(QScatterDataProxy *dataProxy, QObject *parent = 0);
    virtual ~QScatter3DSeries();

    void setDataProxy(QScatterDataProxy *proxy);
    QScatterDataProxy *dataProxy() const;

    void setSelectedItem(int index);
    int selectedItem() const;
    static int invalidSelectionIndex();

    void setItemSize(float size);
    float itemSize() const;

signals:
    void dataProxyChanged(QScatterDataProxy *proxy);
    void selectedItemChanged(int index);
    void itemSizeChanged(float size);

protected:
    explicit QScatter3DSeries(QScatter3DSeriesPrivate *d, QObject *parent = 0);
    QScatter3DSeriesPrivate *dptr();
    const QScatter3DSeriesPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QScatter3DSeries)

    friend class Scatter3DController;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
