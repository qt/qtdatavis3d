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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef DECLARATIVESERIES_P_H
#define DECLARATIVESERIES_P_H

#include "datavisualizationglobal_p.h"
#include "qbar3dseries.h"
#include "qscatter3dseries.h"
#include "qsurface3dseries.h"
#include <QQmlListProperty>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class DeclarativeBar3DSeries : public QBar3DSeries
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> seriesChildren READ seriesChildren)
    // selectedBar property is overloaded to use QPointF instead of QPoint to work around qml bug
    // where Qt.point(0, 0) can't be assigned due to error "Cannot assign QPointF to QPoint".
    Q_PROPERTY(QPointF selectedBar READ selectedBar WRITE setSelectedBar NOTIFY selectedBarChanged)
    Q_CLASSINFO("DefaultProperty", "seriesChildren")
public:
    DeclarativeBar3DSeries(QObject *parent = 0);
    virtual ~DeclarativeBar3DSeries();

    QQmlListProperty<QObject> seriesChildren();
    static void appendSeriesChildren(QQmlListProperty<QObject> *list, QObject *element);

    void setSelectedBar(const QPointF &position);
    QPointF selectedBar() const;
    Q_INVOKABLE QPointF invalidSelectionPosition() const;

signals:
    void selectedBarChanged(QPointF position);
};

class DeclarativeScatter3DSeries : public QScatter3DSeries
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> seriesChildren READ seriesChildren)
    Q_CLASSINFO("DefaultProperty", "seriesChildren")
public:
    DeclarativeScatter3DSeries(QObject *parent = 0);
    virtual ~DeclarativeScatter3DSeries();

    QQmlListProperty<QObject> seriesChildren();
    static void appendSeriesChildren(QQmlListProperty<QObject> *list, QObject *element);
};

class DeclarativeSurface3DSeries : public QSurface3DSeries
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> seriesChildren READ seriesChildren)
    Q_CLASSINFO("DefaultProperty", "seriesChildren")
public:
    DeclarativeSurface3DSeries(QObject *parent = 0);
    virtual ~DeclarativeSurface3DSeries();

    QQmlListProperty<QObject> seriesChildren();
    static void appendSeriesChildren(QQmlListProperty<QObject> *list, QObject *element);
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif
