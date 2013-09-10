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

#ifndef Q3DABSTRACTAXIS_H
#define Q3DABSTRACTAXIS_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QObject>
#include <QScopedPointer>
#include <QVector>
#include <QStringList>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DAbstractAxisPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DAbstractAxis : public QObject
{
    Q_OBJECT
    Q_ENUMS(AxisOrientation)
    Q_ENUMS(AxisType)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QStringList labels READ labels NOTIFY labelsChanged)
    Q_PROPERTY(AxisOrientation orientation READ orientation)
    Q_PROPERTY(AxisType type READ type)

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
        //AxisTypeLogValue = 6 // inherits valueaxis (4 + 2) // TODO
    };

protected:
    explicit Q3DAbstractAxis(Q3DAbstractAxisPrivate *d, QObject *parent = 0);

public:
    virtual ~Q3DAbstractAxis();

    QString title() const;
    QStringList labels() const;

    AxisOrientation orientation() const;
    AxisType type() const;

public slots:
    void setTitle(QString title);

signals:
    void titleChanged(QString newTitle);
    void labelsChanged();

protected:
    QScopedPointer<Q3DAbstractAxisPrivate> d_ptr;

private:
    Q_DISABLE_COPY(Q3DAbstractAxis)

    friend class Abstract3DController;
    friend class Bars3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QABSTRACTAXIS_H
