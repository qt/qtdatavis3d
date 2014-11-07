/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef QBAR3DSERIES_H
#define QBAR3DSERIES_H

#include <QtDataVisualization/qabstract3dseries.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QtCore/QPoint>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QBar3DSeriesPrivate;

class QT_DATAVISUALIZATION_EXPORT QBar3DSeries : public QAbstract3DSeries
{
    Q_OBJECT
    Q_PROPERTY(QBarDataProxy *dataProxy READ dataProxy WRITE setDataProxy NOTIFY dataProxyChanged)
    Q_PROPERTY(QPoint selectedBar READ selectedBar WRITE setSelectedBar NOTIFY selectedBarChanged)
    Q_PROPERTY(float meshAngle READ meshAngle WRITE setMeshAngle NOTIFY meshAngleChanged)

public:
    explicit QBar3DSeries(QObject *parent = 0);
    explicit QBar3DSeries(QBarDataProxy *dataProxy, QObject *parent = 0);
    virtual ~QBar3DSeries();

    void setDataProxy(QBarDataProxy *proxy);
    QBarDataProxy *dataProxy() const;

    void setSelectedBar(const QPoint &position);
    QPoint selectedBar() const;
    static QPoint invalidSelectionPosition();

    void setMeshAngle(float angle);
    float meshAngle() const;

signals:
    void dataProxyChanged(QBarDataProxy *proxy);
    void selectedBarChanged(const QPoint &position);
    void meshAngleChanged(float angle);

protected:
    QBar3DSeriesPrivate *dptr();
    const QBar3DSeriesPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QBar3DSeries)

    friend class Bars3DController;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
