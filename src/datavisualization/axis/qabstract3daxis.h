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

#ifndef QABSTRACT3DAXIS_H
#define QABSTRACT3DAXIS_H

#include <QtDataVisualization/qdatavisualizationglobal.h>

#include <QObject>
#include <QScopedPointer>
#include <QVector>
#include <QStringList>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QAbstract3DAxisPrivate;

class QT_DATAVISUALIZATION_EXPORT QAbstract3DAxis : public QObject
{
    Q_OBJECT
    Q_ENUMS(AxisOrientation)
    Q_ENUMS(AxisType)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QStringList labels READ labels WRITE setLabels NOTIFY labelsChanged)
    Q_PROPERTY(AxisOrientation orientation READ orientation NOTIFY orientationChanged)
    Q_PROPERTY(AxisType type READ type CONSTANT)
    Q_PROPERTY(float min READ min WRITE setMin NOTIFY minChanged)
    Q_PROPERTY(float max READ max WRITE setMax NOTIFY maxChanged)
    Q_PROPERTY(bool autoAdjustRange READ isAutoAdjustRange WRITE setAutoAdjustRange NOTIFY autoAdjustRangeChanged)

public:
    enum AxisOrientation {
        AxisOrientationNone = 0,
        AxisOrientationX = 1,
        AxisOrientationY = 2,
        AxisOrientationZ = 4
    };

    enum AxisType {
        AxisTypeNone = 0,
        AxisTypeCategory = 1,
        AxisTypeValue = 2
    };

protected:
    explicit QAbstract3DAxis(QAbstract3DAxisPrivate *d, QObject *parent = 0);

public:
    virtual ~QAbstract3DAxis();

    void setTitle(QString title);
    QString title() const;

    void setLabels(const QStringList &labels);
    QStringList labels() const;

    AxisOrientation orientation() const;
    AxisType type() const;

    void setMin(float min);
    float min() const;

    void setMax(float max);
    float max() const;

    void setAutoAdjustRange(bool autoAdjust);
    bool isAutoAdjustRange() const;

    void setRange(float min, float max);

signals:
    void titleChanged(QString newTitle);
    void labelsChanged();
    void orientationChanged(AxisOrientation orientation);
    void minChanged(float value);
    void maxChanged(float value);
    void rangeChanged(float min, float max);
    void autoAdjustRangeChanged(bool autoAdjust);

protected:
    QScopedPointer<QAbstract3DAxisPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QAbstract3DAxis)

    friend class Abstract3DController;
    friend class Bars3DController;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
