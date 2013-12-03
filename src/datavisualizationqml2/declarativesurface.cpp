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
#include "declarativerenderer_p.h"
#include "q3dvalueaxis.h"
#include "qitemmodelsurfacedataproxy.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

DeclarativeSurface::DeclarativeSurface(QQuickItem *parent)
    : AbstractDeclarative(parent),
      m_surfaceController(0),
      m_gradient(0)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);

    // Create the shared component on the main GUI thread.
    m_surfaceController = new Surface3DController(boundingRect().toRect());
    setSharedController(m_surfaceController);
}

DeclarativeSurface::~DeclarativeSurface()
{
    delete m_surfaceController;
}

void DeclarativeSurface::handleGradientUpdate()
{
    if (m_gradient)
        setControllerGradient(*m_gradient);
}

Q3DValueAxis *DeclarativeSurface::axisX() const
{
    return static_cast<Q3DValueAxis *>(m_surfaceController->axisX());
}

void DeclarativeSurface::setAxisX(Q3DValueAxis *axis)
{
    m_surfaceController->setAxisX(axis);
}

Q3DValueAxis *DeclarativeSurface::axisY() const
{
    return static_cast<Q3DValueAxis *>(m_surfaceController->axisY());
}

void DeclarativeSurface::setAxisY(Q3DValueAxis *axis)
{
    m_surfaceController->setAxisY(axis);
}

Q3DValueAxis *DeclarativeSurface::axisZ() const
{
    return static_cast<Q3DValueAxis *>(m_surfaceController->axisZ());
}

void DeclarativeSurface::setAxisZ(Q3DValueAxis *axis)
{
    m_surfaceController->setAxisZ(axis);
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
    m_surfaceController->setGradient(newGradient);
}

QQmlListProperty<QSurface3DSeries> DeclarativeSurface::seriesList()
{
    return QQmlListProperty<QSurface3DSeries>(this, this,
                                          &DeclarativeSurface::appendSeriesFunc,
                                          &DeclarativeSurface::countSeriesFunc,
                                          &DeclarativeSurface::atSeriesFunc,
                                          &DeclarativeSurface::clearSeriesFunc);
}

void DeclarativeSurface::appendSeriesFunc(QQmlListProperty<QSurface3DSeries> *list, QSurface3DSeries *series)
{
    reinterpret_cast<DeclarativeSurface *>(list->data)->addSeries(series);
}

int DeclarativeSurface::countSeriesFunc(QQmlListProperty<QSurface3DSeries> *list)
{
    return reinterpret_cast<DeclarativeSurface *>(list->data)->m_surfaceController->surfaceSeriesList().size();
}

QSurface3DSeries *DeclarativeSurface::atSeriesFunc(QQmlListProperty<QSurface3DSeries> *list, int index)
{
    return reinterpret_cast<DeclarativeSurface *>(list->data)->m_surfaceController->surfaceSeriesList().at(index);
}

void DeclarativeSurface::clearSeriesFunc(QQmlListProperty<QSurface3DSeries> *list)
{
    DeclarativeSurface *declSurface = reinterpret_cast<DeclarativeSurface *>(list->data);
    QList<QSurface3DSeries *> realList = declSurface->m_surfaceController->surfaceSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declSurface->removeSeries(realList.at(i));
}

void DeclarativeSurface::addSeries(QSurface3DSeries *series)
{
    m_surfaceController->addSeries(series);
}

void DeclarativeSurface::removeSeries(QSurface3DSeries *series)
{
    m_surfaceController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

QT_DATAVISUALIZATION_END_NAMESPACE
