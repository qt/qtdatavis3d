/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

#include <QtDataVis3D/qabstractaxis.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QValueAxisPrivate;

class QT_DATAVIS3D_EXPORT QValueAxis : public QAbstractAxis
{
    Q_OBJECT
    Q_PROPERTY(qreal min READ min WRITE setMin NOTIFY rangeChanged)
    Q_PROPERTY(qreal max READ max WRITE setMax NOTIFY rangeChanged)
    Q_PROPERTY(int segmentCount READ segmentCount WRITE setSegmentCount NOTIFY segmentCountChanged)
    Q_PROPERTY(int subSegmentCount READ subSegmentCount WRITE setSubSegmentCount NOTIFY subSegmentCountChanged)
    Q_PROPERTY(bool autoAdjustRange READ isAutoAdjustRange WRITE setAutoAdjustRange NOTIFY autoAdjustRangeChanged)
    Q_PROPERTY(QString labelFormat READ labelFormat WRITE setLabelFormat NOTIFY labelFormatChanged)

public:
    explicit QValueAxis(QObject *parent = 0);
    ~QValueAxis();

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
    QValueAxisPrivate *dptr();
    const QValueAxisPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QValueAxis)
    friend class Bars3dController;
    friend class Scatter3DController;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QVALUEAXIS_H
