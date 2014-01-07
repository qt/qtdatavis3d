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

#ifndef Q3DSCATTER_H
#define Q3DSCATTER_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QtDataVisualization/qabstract3dgraph.h>
#include <QtDataVisualization/q3dscene.h>
#include <QFont>
#include <QLinearGradient>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DScatterPrivate;
class Q3DValueAxis;
class Q3DCategoryAxis;
class QScatter3DSeries;

class QT_DATAVISUALIZATION_EXPORT Q3DScatter : public QAbstract3DGraph
{
    Q_OBJECT
public:
    explicit Q3DScatter(QWindow *parent = 0);
    virtual ~Q3DScatter();

    void addSeries(QScatter3DSeries *series);
    void removeSeries(QScatter3DSeries *series);
    QList<QScatter3DSeries *> seriesList();

    void setAxisX(Q3DValueAxis *axis);
    Q3DValueAxis *axisX() const;
    void setAxisY(Q3DValueAxis *axis);
    Q3DValueAxis *axisY() const;
    void setAxisZ(Q3DValueAxis *axis);
    Q3DValueAxis *axisZ() const;
    void addAxis(Q3DValueAxis *axis);
    void releaseAxis(Q3DValueAxis *axis);
    QList<Q3DValueAxis *> axes() const;

private:
    Q3DScatterPrivate *dptr();
    const Q3DScatterPrivate *dptrc() const;
    Q_DISABLE_COPY(Q3DScatter)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif
