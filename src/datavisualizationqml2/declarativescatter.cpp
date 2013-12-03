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

#include "declarativescatter_p.h"
#include "declarativerenderer_p.h"
#include "qitemmodelscatterdataproxy.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

DeclarativeScatter::DeclarativeScatter(QQuickItem *parent)
    : AbstractDeclarative(parent),
      m_scatterController(0)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);

    // Create the shared component on the main GUI thread.
    m_scatterController = new Scatter3DController(boundingRect().toRect());
    setSharedController(m_scatterController);
}

DeclarativeScatter::~DeclarativeScatter()
{
    delete m_scatterController;
}

Q3DValueAxis *DeclarativeScatter::axisX() const
{
    return static_cast<Q3DValueAxis *>(m_scatterController->axisX());
}

void DeclarativeScatter::setAxisX(Q3DValueAxis *axis)
{
    m_scatterController->setAxisX(axis);
}

Q3DValueAxis *DeclarativeScatter::axisY() const
{
    return static_cast<Q3DValueAxis *>(m_scatterController->axisY());
}

void DeclarativeScatter::setAxisY(Q3DValueAxis *axis)
{
    m_scatterController->setAxisY(axis);
}

Q3DValueAxis *DeclarativeScatter::axisZ() const
{
    return static_cast<Q3DValueAxis *>(m_scatterController->axisZ());
}

void DeclarativeScatter::setAxisZ(Q3DValueAxis *axis)
{
    m_scatterController->setAxisZ(axis);
}

QQmlListProperty<QScatter3DSeries> DeclarativeScatter::seriesList()
{
    return QQmlListProperty<QScatter3DSeries>(this, this,
                                          &DeclarativeScatter::appendSeriesFunc,
                                          &DeclarativeScatter::countSeriesFunc,
                                          &DeclarativeScatter::atSeriesFunc,
                                          &DeclarativeScatter::clearSeriesFunc);
}

void DeclarativeScatter::appendSeriesFunc(QQmlListProperty<QScatter3DSeries> *list, QScatter3DSeries *series)
{
    reinterpret_cast<DeclarativeScatter *>(list->data)->addSeries(series);
}

int DeclarativeScatter::countSeriesFunc(QQmlListProperty<QScatter3DSeries> *list)
{
    return reinterpret_cast<DeclarativeScatter *>(list->data)->m_scatterController->scatterSeriesList().size();
}

QScatter3DSeries *DeclarativeScatter::atSeriesFunc(QQmlListProperty<QScatter3DSeries> *list, int index)
{
    return reinterpret_cast<DeclarativeScatter *>(list->data)->m_scatterController->scatterSeriesList().at(index);
}

void DeclarativeScatter::clearSeriesFunc(QQmlListProperty<QScatter3DSeries> *list)
{
    DeclarativeScatter *declScatter = reinterpret_cast<DeclarativeScatter *>(list->data);
    QList<QScatter3DSeries *> realList = declScatter->m_scatterController->scatterSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declScatter->removeSeries(realList.at(i));
}

void DeclarativeScatter::addSeries(QScatter3DSeries *series)
{
    m_scatterController->addSeries(series);
}

void DeclarativeScatter::removeSeries(QScatter3DSeries *series)
{
    m_scatterController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

QT_DATAVISUALIZATION_END_NAMESPACE
