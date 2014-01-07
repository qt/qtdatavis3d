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

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QtDataVisualization/qabstract3dgraph.h>
#include <QFont>
#include <QLinearGradient>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DBarsPrivate;
class Q3DAbstractAxis;
class Q3DCategoryAxis;
class Q3DValueAxis;
class QBar3DSeries;

class QT_DATAVISUALIZATION_EXPORT Q3DBars : public QAbstract3DGraph
{
    Q_OBJECT
    Q_PROPERTY(float barThickness READ barThickness WRITE setBarThickness NOTIFY barThicknessChanged)
    Q_PROPERTY(QSizeF barSpacing READ barSpacing WRITE setBarSpacing NOTIFY barSpacingChanged)
    Q_PROPERTY(bool barSpacingRelative READ isBarSpacingRelative WRITE setBarSpacingRelative NOTIFY barSpacingRelativeChanged)

public:
    explicit Q3DBars(QWindow *parent = 0);
    virtual ~Q3DBars();

    void addSeries(QBar3DSeries *series);
    void removeSeries(QBar3DSeries *series);
    QList<QBar3DSeries *> seriesList();

    void setBarThickness(float thicknessRatio);
    float barThickness();

    void setBarSpacing(QSizeF spacing);
    QSizeF barSpacing();

    void setBarSpacingRelative(bool relative);
    bool isBarSpacingRelative();

    void setRowAxis(Q3DCategoryAxis *axis);
    Q3DCategoryAxis *rowAxis() const;
    void setColumnAxis(Q3DCategoryAxis *axis);
    Q3DCategoryAxis *columnAxis() const;
    void setValueAxis(Q3DValueAxis *axis);
    Q3DValueAxis *valueAxis() const;
    void addAxis(Q3DAbstractAxis *axis);
    void releaseAxis(Q3DAbstractAxis *axis);
    QList<Q3DAbstractAxis *> axes() const;

signals:
    void barThicknessChanged(float thicknessRatio);
    void barSpacingChanged(QSizeF spacing);
    void barSpacingRelativeChanged(bool relative);

private:
    Q3DBarsPrivate *dptr();
    const Q3DBarsPrivate *dptrc() const;
    Q_DISABLE_COPY(Q3DBars)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif
