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

#ifndef Q3DSCATTER_H
#define Q3DSCATTER_H

#include <QtDataVisualization/qabstract3dgraph.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/qscatter3dseries.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DScatterPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DScatter : public QAbstract3DGraph
{
    Q_OBJECT
    Q_PROPERTY(QValue3DAxis *axisX READ axisX WRITE setAxisX NOTIFY axisXChanged)
    Q_PROPERTY(QValue3DAxis *axisY READ axisY WRITE setAxisY NOTIFY axisYChanged)
    Q_PROPERTY(QValue3DAxis *axisZ READ axisZ WRITE setAxisZ NOTIFY axisZChanged)
    Q_PROPERTY(QScatter3DSeries *selectedSeries READ selectedSeries NOTIFY selectedSeriesChanged)

public:
    explicit Q3DScatter(const QSurfaceFormat *format = 0, QWindow *parent = 0);
    virtual ~Q3DScatter();

    void addSeries(QScatter3DSeries *series);
    void removeSeries(QScatter3DSeries *series);
    QList<QScatter3DSeries *> seriesList() const;

    void setAxisX(QValue3DAxis *axis);
    QValue3DAxis *axisX() const;
    void setAxisY(QValue3DAxis *axis);
    QValue3DAxis *axisY() const;
    void setAxisZ(QValue3DAxis *axis);
    QValue3DAxis *axisZ() const;
    void addAxis(QValue3DAxis *axis);
    void releaseAxis(QValue3DAxis *axis);
    QList<QValue3DAxis *> axes() const;

    QScatter3DSeries *selectedSeries() const;

signals:
    void axisXChanged(QValue3DAxis *axis);
    void axisYChanged(QValue3DAxis *axis);
    void axisZChanged(QValue3DAxis *axis);
    void selectedSeriesChanged(QScatter3DSeries *series);

private:
    Q3DScatterPrivate *dptr();
    const Q3DScatterPrivate *dptrc() const;
    Q_DISABLE_COPY(Q3DScatter)
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
