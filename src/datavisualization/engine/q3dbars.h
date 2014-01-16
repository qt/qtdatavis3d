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

#ifndef Q3DBARS_H
#define Q3DBARS_H

#include <QtDataVisualization/qabstract3dgraph.h>
#include <QFont>
#include <QLinearGradient>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DBarsPrivate;
class QAbstract3DAxis;
class QCategory3DAxis;
class QValue3DAxis;
class QBar3DSeries;

class QT_DATAVISUALIZATION_EXPORT Q3DBars : public QAbstract3DGraph
{
    Q_OBJECT
    Q_PROPERTY(float barThickness READ barThickness WRITE setBarThickness NOTIFY barThicknessChanged)
    Q_PROPERTY(QSizeF barSpacing READ barSpacing WRITE setBarSpacing NOTIFY barSpacingChanged)
    Q_PROPERTY(bool barSpacingRelative READ isBarSpacingRelative WRITE setBarSpacingRelative NOTIFY barSpacingRelativeChanged)
    Q_PROPERTY(QCategory3DAxis *rowAxis READ rowAxis WRITE setRowAxis NOTIFY rowAxisChanged)
    Q_PROPERTY(QCategory3DAxis *columnAxis READ columnAxis WRITE setColumnAxis NOTIFY columnAxisChanged)
    Q_PROPERTY(QValue3DAxis *valueAxis READ valueAxis WRITE setValueAxis NOTIFY valueAxisChanged)
    Q_PROPERTY(QBar3DSeries *primarySeries READ primarySeries WRITE setPrimarySeries NOTIFY primarySeriesChanged)

public:
    explicit Q3DBars(const QSurfaceFormat *format = 0, QWindow *parent = 0);
    virtual ~Q3DBars();

    void setPrimarySeries(QBar3DSeries *series);
    QBar3DSeries *primarySeries() const;
    void addSeries(QBar3DSeries *series);
    void removeSeries(QBar3DSeries *series);
    void insertSeries(int index, QBar3DSeries *series);
    QList<QBar3DSeries *> seriesList();

    void setBarThickness(float thicknessRatio);
    float barThickness();

    void setBarSpacing(QSizeF spacing);
    QSizeF barSpacing();

    void setBarSpacingRelative(bool relative);
    bool isBarSpacingRelative();

    void setRowAxis(QCategory3DAxis *axis);
    QCategory3DAxis *rowAxis() const;
    void setColumnAxis(QCategory3DAxis *axis);
    QCategory3DAxis *columnAxis() const;
    void setValueAxis(QValue3DAxis *axis);
    QValue3DAxis *valueAxis() const;
    void addAxis(QAbstract3DAxis *axis);
    void releaseAxis(QAbstract3DAxis *axis);
    QList<QAbstract3DAxis *> axes() const;

signals:
    void barThicknessChanged(float thicknessRatio);
    void barSpacingChanged(QSizeF spacing);
    void barSpacingRelativeChanged(bool relative);
    void rowAxisChanged(QCategory3DAxis *axis);
    void columnAxisChanged(QCategory3DAxis *axis);
    void valueAxisChanged(QValue3DAxis *axis);
    void primarySeriesChanged(QBar3DSeries *series);

private:
    Q3DBarsPrivate *dptr();
    const Q3DBarsPrivate *dptrc() const;
    Q_DISABLE_COPY(Q3DBars)
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
