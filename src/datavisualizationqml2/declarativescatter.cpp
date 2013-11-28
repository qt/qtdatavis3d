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
#include "declarativescatterrenderer_p.h"
#include "qitemmodelscatterdataproxy.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

DeclarativeScatter::DeclarativeScatter(QQuickItem *parent)
    : AbstractDeclarative(parent),
      m_shared(0),
      m_initialisedSize(0, 0)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);

    // Create the shared component on the main GUI thread.
    m_shared = new Scatter3DController(boundingRect().toRect());
    setSharedController(m_shared);
}

DeclarativeScatter::~DeclarativeScatter()
{
    delete m_shared;
}

QSGNode *DeclarativeScatter::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    // If old node exists and has right size, reuse it.
    if (oldNode && m_initialisedSize == boundingRect().size().toSize()) {
        // Update bounding rectangle (that has same size as before).
        static_cast<DeclarativeScatterRenderer *>( oldNode )->setRect(boundingRect());
        return oldNode;
    }

    // Create a new render node when size changes or if there is no node yet
    m_initialisedSize = boundingRect().size().toSize();

    // Delete old node
    if (oldNode)
        delete oldNode;

    // Create a new one and set it's bounding rectangle
    DeclarativeScatterRenderer *node = new DeclarativeScatterRenderer(window(), m_shared);
    node->setRect(boundingRect());
    m_shared->setBoundingRect(boundingRect().toRect());
    return node;
}

Q3DValueAxis *DeclarativeScatter::axisX() const
{
    return static_cast<Q3DValueAxis *>(m_shared->axisX());
}

void DeclarativeScatter::setAxisX(Q3DValueAxis *axis)
{
    m_shared->setAxisX(axis);
}

Q3DValueAxis *DeclarativeScatter::axisY() const
{
    return static_cast<Q3DValueAxis *>(m_shared->axisY());
}

void DeclarativeScatter::setAxisY(Q3DValueAxis *axis)
{
    m_shared->setAxisY(axis);
}

Q3DValueAxis *DeclarativeScatter::axisZ() const
{
    return static_cast<Q3DValueAxis *>(m_shared->axisZ());
}

void DeclarativeScatter::setAxisZ(Q3DValueAxis *axis)
{
    m_shared->setAxisZ(axis);
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
    return reinterpret_cast<DeclarativeScatter *>(list->data)->m_shared->scatterSeriesList().size();
}

QScatter3DSeries *DeclarativeScatter::atSeriesFunc(QQmlListProperty<QScatter3DSeries> *list, int index)
{
    return reinterpret_cast<DeclarativeScatter *>(list->data)->m_shared->scatterSeriesList().at(index);
}

void DeclarativeScatter::clearSeriesFunc(QQmlListProperty<QScatter3DSeries> *list)
{
    DeclarativeScatter *declScatter = reinterpret_cast<DeclarativeScatter *>(list->data);
    QList<QScatter3DSeries *> realList = declScatter->m_shared->scatterSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declScatter->removeSeries(realList.at(i));
}

void DeclarativeScatter::addSeries(QScatter3DSeries *series)
{
    m_shared->addSeries(series);
}

void DeclarativeScatter::removeSeries(QScatter3DSeries *series)
{
    m_shared->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

QT_DATAVISUALIZATION_END_NAMESPACE
