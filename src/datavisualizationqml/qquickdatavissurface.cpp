// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickdatavissurface_p.h"
#include <QtCore/QMutexLocker>
#include <private/declarativescene_p.h>

#include <private/declarativescene_p.h>
#include <private/surface3dcontroller_p.h>
#include <private/qsurface3dseries_p.h>

QT_BEGIN_NAMESPACE

QQuickDataVisSurface::QQuickDataVisSurface(QQuickItem *parent)
    : QQuickDataVisItem(parent),
      m_surfaceController(0)
{
    setAcceptedMouseButtons(Qt::AllButtons);

    // Create the shared component on the main GUI thread.
    m_surfaceController = new Surface3DController(boundingRect().toRect(), new Declarative3DScene);
    setSharedController(m_surfaceController);

    QObject::connect(m_surfaceController, &Surface3DController::selectedSeriesChanged,
                     this, &QQuickDataVisSurface::selectedSeriesChanged);
    QObject::connect(m_surfaceController, &Surface3DController::flipHorizontalGridChanged,
                     this, &QQuickDataVisSurface::flipHorizontalGridChanged);
}

QQuickDataVisSurface::~QQuickDataVisSurface()
{
    QMutexLocker locker(m_nodeMutex.data());
    const QMutexLocker locker2(mutex());
    delete m_surfaceController;
}

QValue3DAxis *QQuickDataVisSurface::axisX() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisX());
}

void QQuickDataVisSurface::setAxisX(QValue3DAxis *axis)
{
    m_surfaceController->setAxisX(axis);
}

QValue3DAxis *QQuickDataVisSurface::axisY() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisY());
}

void QQuickDataVisSurface::setAxisY(QValue3DAxis *axis)
{
    m_surfaceController->setAxisY(axis);
}

QValue3DAxis *QQuickDataVisSurface::axisZ() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisZ());
}

void QQuickDataVisSurface::setAxisZ(QValue3DAxis *axis)
{
    m_surfaceController->setAxisZ(axis);
}

QSurface3DSeries *QQuickDataVisSurface::selectedSeries() const
{
    return m_surfaceController->selectedSeries();
}

void QQuickDataVisSurface::setFlipHorizontalGrid(bool flip)
{
   m_surfaceController->setFlipHorizontalGrid(flip);
}

bool QQuickDataVisSurface::flipHorizontalGrid() const
{
    return m_surfaceController->flipHorizontalGrid();
}

QQmlListProperty<QSurface3DSeries> QQuickDataVisSurface::seriesList()
{
    return QQmlListProperty<QSurface3DSeries>(this, this,
                                              &QQuickDataVisSurface::appendSeriesFunc,
                                              &QQuickDataVisSurface::countSeriesFunc,
                                              &QQuickDataVisSurface::atSeriesFunc,
                                              &QQuickDataVisSurface::clearSeriesFunc);
}

void QQuickDataVisSurface::appendSeriesFunc(QQmlListProperty<QSurface3DSeries> *list,
                                          QSurface3DSeries *series)
{
    reinterpret_cast<QQuickDataVisSurface *>(list->data)->addSeries(series);
}

qsizetype QQuickDataVisSurface::countSeriesFunc(QQmlListProperty<QSurface3DSeries> *list)
{
    return reinterpret_cast<QQuickDataVisSurface *>(list->data)->m_surfaceController->surfaceSeriesList().size();
}

QSurface3DSeries *QQuickDataVisSurface::atSeriesFunc(QQmlListProperty<QSurface3DSeries> *list,
                                                   qsizetype index)
{
    return reinterpret_cast<QQuickDataVisSurface *>(list->data)->m_surfaceController->surfaceSeriesList().at(index);
}

void QQuickDataVisSurface::clearSeriesFunc(QQmlListProperty<QSurface3DSeries> *list)
{
    QQuickDataVisSurface *declSurface = reinterpret_cast<QQuickDataVisSurface *>(list->data);
    QList<QSurface3DSeries *> realList = declSurface->m_surfaceController->surfaceSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declSurface->removeSeries(realList.at(i));
}

void QQuickDataVisSurface::addSeries(QSurface3DSeries *series)
{
    m_surfaceController->addSeries(series);
}

void QQuickDataVisSurface::removeSeries(QSurface3DSeries *series)
{
    m_surfaceController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

void QQuickDataVisSurface::handleAxisXChanged(QAbstract3DAxis *axis)
{
    emit axisXChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisSurface::handleAxisYChanged(QAbstract3DAxis *axis)
{
    emit axisYChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisSurface::handleAxisZChanged(QAbstract3DAxis *axis)
{
    emit axisZChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisSurface::componentComplete()
{
    QQuickDataVisItem::componentComplete();

    m_model = new QQuick3DModel();
    m_model->setParent(QQuick3DViewport::scene());
    m_model->setParentItem(QQuick3DViewport::scene());

    m_gridModel = new QQuick3DModel();
    m_gridModel->setParent(QQuick3DViewport::scene());
    m_gridModel->setParentItem(QQuick3DViewport::scene());
}

QT_END_NAMESPACE
