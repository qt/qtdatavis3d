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

#include "declarativeseries_p.h"
#include "qbardataproxy.h"
#include "qscatterdataproxy.h"
#include "qsurfacedataproxy.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

DeclarativeBar3DSeries::DeclarativeBar3DSeries(QObject *parent)
    : QBar3DSeries(parent)
{
    QObject::connect(this, &QBar3DSeries::selectedBarChanged, this,
                     &DeclarativeBar3DSeries::selectedBarChanged);
}

DeclarativeBar3DSeries::~DeclarativeBar3DSeries()
{
}

QQmlListProperty<QObject> DeclarativeBar3DSeries::seriesChildren()
{
    return QQmlListProperty<QObject>(this, this, &DeclarativeBar3DSeries::appendSeriesChildren
                                     , 0, 0, 0);
}

void DeclarativeBar3DSeries::appendSeriesChildren(QQmlListProperty<QObject> *list, QObject *element)
{
    QBarDataProxy *proxy = qobject_cast<QBarDataProxy *>(element);
    if (proxy)
        reinterpret_cast<DeclarativeBar3DSeries *>(list->data)->setDataProxy(proxy);
}

void DeclarativeBar3DSeries::setSelectedBar(const QPointF &position)
{
    QBar3DSeries::setSelectedBar(position.toPoint());
}

QPointF DeclarativeBar3DSeries::selectedBar() const
{
    return QBar3DSeries::selectedBar();
}

QPointF DeclarativeBar3DSeries::invalidSelectionPosition() const
{
    return QPointF(QBar3DSeries::invalidSelectionPosition());
}

DeclarativeScatter3DSeries::DeclarativeScatter3DSeries(QObject *parent)
    : QScatter3DSeries(parent)
{
}

DeclarativeScatter3DSeries::~DeclarativeScatter3DSeries()
{
}

QQmlListProperty<QObject> DeclarativeScatter3DSeries::seriesChildren()
{
    return QQmlListProperty<QObject>(this, this, &DeclarativeScatter3DSeries::appendSeriesChildren
                                     , 0, 0, 0);
}

void DeclarativeScatter3DSeries::appendSeriesChildren(QQmlListProperty<QObject> *list, QObject *element)
{
    QScatterDataProxy *proxy = qobject_cast<QScatterDataProxy *>(element);
    if (proxy)
        reinterpret_cast<DeclarativeScatter3DSeries *>(list->data)->setDataProxy(proxy);
}

DeclarativeSurface3DSeries::DeclarativeSurface3DSeries(QObject *parent)
    : QSurface3DSeries(parent)
{
}

DeclarativeSurface3DSeries::~DeclarativeSurface3DSeries()
{
}

QQmlListProperty<QObject> DeclarativeSurface3DSeries::seriesChildren()
{
    return QQmlListProperty<QObject>(this, this, &DeclarativeSurface3DSeries::appendSeriesChildren
                                     , 0, 0, 0);
}

void DeclarativeSurface3DSeries::appendSeriesChildren(QQmlListProperty<QObject> *list, QObject *element)
{
    QSurfaceDataProxy *proxy = qobject_cast<QSurfaceDataProxy *>(element);
    if (proxy)
        reinterpret_cast<DeclarativeSurface3DSeries *>(list->data)->setDataProxy(proxy);
}

QT_DATAVISUALIZATION_END_NAMESPACE
