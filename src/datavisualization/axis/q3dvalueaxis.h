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

#ifndef QVALUEAXIS_H
#define QVALUEAXIS_H

#include <QtDataVisualization/q3dabstractaxis.h>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DValueAxisPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DValueAxis : public Q3DAbstractAxis
{
    Q_OBJECT
    Q_PROPERTY(qreal min READ min WRITE setMin NOTIFY rangeChanged)
    Q_PROPERTY(qreal max READ max WRITE setMax NOTIFY rangeChanged)
    Q_PROPERTY(int segmentCount READ segmentCount WRITE setSegmentCount NOTIFY segmentCountChanged)
    Q_PROPERTY(int subSegmentCount READ subSegmentCount WRITE setSubSegmentCount NOTIFY subSegmentCountChanged)
    Q_PROPERTY(bool autoAdjustRange READ isAutoAdjustRange WRITE setAutoAdjustRange NOTIFY autoAdjustRangeChanged)
    Q_PROPERTY(QString labelFormat READ labelFormat WRITE setLabelFormat NOTIFY labelFormatChanged)

public:
    explicit Q3DValueAxis(QObject *parent = 0);
    virtual ~Q3DValueAxis();

    qreal min() const;
    qreal max() const;
    int segmentCount() const;
    int subSegmentCount() const;
    bool isAutoAdjustRange() const;
    QString labelFormat() const;

public slots:
    void setRange(qreal min, qreal max);
    void setMin(qreal min);
    void setMax(qreal max);
    void setSegmentCount(int count);
    void setSubSegmentCount(int count);
    void setAutoAdjustRange(bool autoAdjust);
    void setLabelFormat(const QString &format);

signals:
    void rangeChanged(qreal min, qreal max);
    void segmentCountChanged(int count);
    void subSegmentCountChanged(int count);
    void autoAdjustRangeChanged(bool autoAdjust);
    void labelFormatChanged(QString format);

protected:
    Q3DValueAxisPrivate *dptr();
    const Q3DValueAxisPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(Q3DValueAxis)
    friend class Bars3DController;
    friend class Scatter3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QVALUEAXIS_H
