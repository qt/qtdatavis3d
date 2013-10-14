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

#include "declarativesurface_p.h"
#include "declarativesurfacerenderer_p.h"
#include "q3dvalueaxis.h"
#include "qitemmodelsurfacedataproxy.h"
#include "theme_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

DeclarativeSurface::DeclarativeSurface(QQuickItem *parent)
    : AbstractDeclarative(parent),
      m_shared(0),
      m_initialisedSize(0, 0),
      m_gradient(0)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);

    // Create the shared component on the main GUI thread.
    m_shared = new Surface3DController(boundingRect().toRect());
    setSharedController(m_shared);

    QItemModelSurfaceDataProxy *proxy = new QItemModelSurfaceDataProxy;
    m_shared->setActiveDataProxy(proxy);
}

DeclarativeSurface::~DeclarativeSurface()
{
    delete m_shared;
}

void DeclarativeSurface::handleGradientUpdate()
{
    if (m_gradient)
        setControllerGradient(*m_gradient);
}

QSGNode *DeclarativeSurface::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    // If old node exists and has right size, reuse it.
    if (oldNode && m_initialisedSize == boundingRect().size().toSize()) {
        // Update bounding rectangle (that has same size as before).
        static_cast<DeclarativeSurfaceRenderer *>( oldNode )->setRect(boundingRect());
        return oldNode;
    }

    // Create a new render node when size changes or if there is no node yet
    m_initialisedSize = boundingRect().size().toSize();

    // Delete old node
    if (oldNode)
        delete oldNode;

    // Create a new one and set it's bounding rectangle
    DeclarativeSurfaceRenderer *node = new DeclarativeSurfaceRenderer(window(), m_shared);
    node->setRect(boundingRect());
    m_shared->setBoundingRect(boundingRect().toRect());
    return node;
}

void DeclarativeSurface::setDataProxy(QSurfaceDataProxy *dataProxy)
{
    m_shared->setActiveDataProxy(dataProxy);
}

QSurfaceDataProxy *DeclarativeSurface::dataProxy() const
{
    return static_cast<QSurfaceDataProxy *>(m_shared->activeDataProxy());
}

Q3DValueAxis *DeclarativeSurface::axisX() const
{
    return static_cast<Q3DValueAxis *>(m_shared->axisX());
}

void DeclarativeSurface::setAxisX(Q3DValueAxis *axis)
{
    m_shared->setAxisX(axis);
}

Q3DValueAxis *DeclarativeSurface::axisY() const
{
    return static_cast<Q3DValueAxis *>(m_shared->axisY());
}

void DeclarativeSurface::setAxisY(Q3DValueAxis *axis)
{
    m_shared->setAxisY(axis);
}

Q3DValueAxis *DeclarativeSurface::axisZ() const
{
    return static_cast<Q3DValueAxis *>(m_shared->axisZ());
}

void DeclarativeSurface::setAxisZ(Q3DValueAxis *axis)
{
    m_shared->setAxisZ(axis);
}

void DeclarativeSurface::setSmoothSurfaceEnabled(bool enabled)
{
    m_shared->setSmoothSurface(enabled);
}

bool DeclarativeSurface::isSmoothSurfaceEnabled() const
{
    return m_shared->smoothSurface();
}

void DeclarativeSurface::setSurfaceGridEnabled(bool enabled)
{
    m_shared->setSurfaceGrid(enabled);
}

bool DeclarativeSurface::isSurfaceGridEnabled() const
{
    return m_shared->surfaceGrid();
}

void DeclarativeSurface::setGradient(ColorGradient *gradient)
{
    // connect new / disconnect old
    if (gradient != m_gradient) {
        if (m_gradient)
            QObject::disconnect(m_gradient, 0, this, 0);

        m_gradient = gradient;

        if (m_gradient) {
            QObject::connect(m_gradient, &ColorGradient::updated, this,
                             &DeclarativeSurface::handleGradientUpdate);
        }
    }

    if (m_gradient)
        setControllerGradient(*m_gradient);
}

ColorGradient *DeclarativeSurface::gradient() const
{

    return m_gradient;
}

void DeclarativeSurface::setControllerGradient(const ColorGradient &gradient)
{
    QLinearGradient newGradient;
    QGradientStops stops;
    QList<ColorGradientStop *> qmlstops = gradient.m_stops;

    // Get sorted gradient stops
    for (int i = 0; i < qmlstops.size(); i++) {
        int j = 0;
        while (j < stops.size() && stops.at(j).first < qmlstops[i]->position())
            j++;
        stops.insert(j, QGradientStop(qmlstops.at(i)->position(), qmlstops.at(i)->color()));
    }

    newGradient.setStops(stops);
    m_shared->setGradient(newGradient);
}

QT_DATAVISUALIZATION_END_NAMESPACE
