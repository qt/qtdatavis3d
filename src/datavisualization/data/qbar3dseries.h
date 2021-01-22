/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
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
    explicit QBar3DSeries(QObject *parent = nullptr);
    explicit QBar3DSeries(QBarDataProxy *dataProxy, QObject *parent = nullptr);
    virtual ~QBar3DSeries();

    void setDataProxy(QBarDataProxy *proxy);
    QBarDataProxy *dataProxy() const;

    void setSelectedBar(const QPoint &position);
    QPoint selectedBar() const;
    static QPoint invalidSelectionPosition();

    void setMeshAngle(float angle);
    float meshAngle() const;

Q_SIGNALS:
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
