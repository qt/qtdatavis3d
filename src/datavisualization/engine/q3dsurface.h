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

#ifndef Q3DSURFACE_H
#define Q3DSURFACE_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QtDataVisualization/qabstract3dgraph.h>
#include <QtDataVisualization/q3dscene.h>
#include <QFont>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DSurfacePrivate;
class QValue3DAxis;
class QSurface3DSeries;

class QT_DATAVISUALIZATION_EXPORT Q3DSurface : public QAbstract3DGraph
{
    Q_OBJECT
    Q_PROPERTY(QValue3DAxis *axisX READ axisX WRITE setAxisX NOTIFY axisXChanged)
    Q_PROPERTY(QValue3DAxis *axisY READ axisY WRITE setAxisY NOTIFY axisYChanged)
    Q_PROPERTY(QValue3DAxis *axisZ READ axisZ WRITE setAxisZ NOTIFY axisZChanged)

public:
    explicit Q3DSurface(QWindow *parent = 0);
    virtual ~Q3DSurface();

    void addSeries(QSurface3DSeries *series);
    void removeSeries(QSurface3DSeries *series);
    QList<QSurface3DSeries *> seriesList();

    // Axes
    void setAxisX(QValue3DAxis *axis);
    QValue3DAxis *axisX() const;
    void setAxisY(QValue3DAxis *axis);
    QValue3DAxis *axisY() const;
    void setAxisZ(QValue3DAxis *axis);
    QValue3DAxis *axisZ() const;
    void addAxis(QValue3DAxis *axis);
    void releaseAxis(QValue3DAxis *axis);
    QList<QValue3DAxis *> axes() const;

signals:
    void axisXChanged(QValue3DAxis *axis);
    void axisYChanged(QValue3DAxis *axis);
    void axisZChanged(QValue3DAxis *axis);

private:
    Q3DSurfacePrivate *dptr();
    const Q3DSurfacePrivate *dptrc() const;
    Q_DISABLE_COPY(Q3DSurface)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // Q3DSURFACE_H
