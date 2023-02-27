// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "q3dscatter.h"
#include "qquickdatavisscatter_p.h"

QT_BEGIN_NAMESPACE

/*!
 * \class Q3DScatter
 * \inmodule QtGraphs
 * \brief The Q3DScatter class provides methods for rendering 3D scatter graphs.
 *
 * This class enables developers to render scatter graphs in 3D and to view them by rotating the scene
 * freely. Rotation is done by holding down the right mouse button and moving the mouse. Zooming
 * is done by mouse wheel. Selection, if enabled, is done by left mouse button. The scene can be
 * reset to default camera view by clicking mouse wheel. In touch devices rotation is done
 * by tap-and-move, selection by tap-and-hold and zoom by pinch.
 *
 * If no axes are set explicitly to Q3DScatter, temporary default axes with no labels are created.
 * These default axes can be modified via axis accessors, but as soon any axis is set explicitly
 * for the orientation, the default axis for that orientation is destroyed.
 *
 * Q3DScatter supports more than one series visible at the same time.
 *
 * \section1 How to construct a minimal Q3DScatter graph
 *
 * First, construct Q3DScatter. Since we are running the graph as top level window
 * in this example, we need to clear the \c Qt::FramelessWindowHint flag, which gets set by
 * default:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 0
 *
 * Now Q3DScatter is ready to receive data to be rendered. Add one series of 3 QVector3D items:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 1
 *
 * Finally you will need to set it visible:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 2
 *
 * The complete code needed to create and display this graph is:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 3
 *
 * And this is what those few lines of code produce:
 *
 * \image q3dscatter-minimal.png
 *
 * The scene can be rotated, zoomed into, and an item can be selected to view its position,
 * but no other interaction is included in this minimal code example.
 * You can learn more by familiarizing yourself with the examples provided, like
 * the \l{Scatter Example}.
 *
 * \sa Q3DBars, Q3DSurface, {Qt Graphs C++ Classes}
 */

/*!
 * Constructs a new 3D scatter graph with optional \a parent window
 * and surface \a format.
 */

/*!
 * Destroys the 3D scatter graph.
 */

/*!
 * Adds the \a series to the graph. A graph can contain multiple series, but has only one set of
 * axes. If the newly added series has specified a selected item, it will be highlighted and
 * any existing selection will be cleared. Only one added series can have an active selection.
 *
 * \sa QAbstract3DGraph::hasSeries()
 */

/*!
 * Removes the \a series from the graph.
 *
 * \sa QAbstract3DGraph::hasSeries()
 */

/*!
 * Returns the list of series added to this graph.
 *
 * \sa QAbstract3DGraph::hasSeries()
 */

/*!
 * \property Q3DScatter::axisX
 *
 * \brief The active x-axis.
 */

/*!
 * Sets \a axis as the active x-axis. Implicitly calls addAxis() to transfer the
 * ownership of the axis to this graph.
 *
 * If \a axis is null, a temporary default axis with no labels and an automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another axis is set explicitly to the
 * same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */

/*!
 * \property Q3DScatter::axisY
 *
 * \brief The active y-axis.
 */

/*!
 * Sets \a axis as the active y-axis. Implicitly calls addAxis() to transfer the
 * ownership of the axis to this graph.
 *
 * If \a axis is null, a temporary default axis with no labels and an automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another axis is set explicitly to the
 * same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */

/*!
 * \property Q3DScatter::axisZ
 *
 * \brief The active z-axis.
 */

/*!
 * Sets \a axis as the active z-axis. Implicitly calls addAxis() to transfer the
 * ownership of the axis to this graph.
 *
 * If \a axis is null, a temporary default axis with no labels and an automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another axis is set explicitly to the
 * same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */

/*!
 * Returns the used z-axis.
 */

/*!
 * \property Q3DScatter::selectedSeries
 *
 * \brief The selected series or null.
 */

/*!
 * Adds \a axis to the graph. The axes added via addAxis are not yet taken to use,
 * addAxis is simply used to give the ownership of the \a axis to the graph.
 * The \a axis must not be null or added to another graph.
 *
 * \sa releaseAxis(), setAxisX(), setAxisY(), setAxisZ()
 */

/*!
 * Releases the ownership of the \a axis back to the caller, if it is added to this graph.
 * If the released \a axis is in use, a new default axis will be created and set active.
 *
 * If the default axis is released and added back later, it behaves as any other axis would.
 *
 * \sa addAxis(), setAxisX(), setAxisY(), setAxisZ()
 */

/*!
 * Returns the list of all added axes.
 *
 * \sa addAxis()
 */

Q3DScatter::Q3DScatter() : QAbstract3DGraph()
{
    QQmlComponent *component = new QQmlComponent(engine(), this);
    component->setData("import QtQuick; import QtGraphs; Scatter3D { anchors.fill: parent; }", QUrl());
    d_ptr.reset(qobject_cast<QQuickDataVisScatter *>(component->create()));
    setContent(component->url(), component, d_ptr.data());
}

Q3DScatter::~Q3DScatter()
{
}

void Q3DScatter::addSeries(QScatter3DSeries *series)
{
    dptr()->addSeries(series);
}

void Q3DScatter::removeSeries(QScatter3DSeries *series)
{
    dptr()->removeSeries(series);
}

QList<QScatter3DSeries *> Q3DScatter::seriesList() const
{
    return dptrc()->m_scatterController->scatterSeriesList();
}

QQuickDataVisScatter *Q3DScatter::dptr()
{
    return static_cast<QQuickDataVisScatter *>(d_ptr.data());
}

const QQuickDataVisScatter *Q3DScatter::dptrc() const
{
    return static_cast<const QQuickDataVisScatter *>(d_ptr.data());
}

void Q3DScatter::setAxisX(QValue3DAxis *axis)
{
    dptr()->setAxisX(axis);
}

QValue3DAxis *Q3DScatter::axisX() const
{
    return static_cast<QValue3DAxis *>(dptrc()->axisX());
}

void Q3DScatter::setAxisY(QValue3DAxis *axis)
{
    dptr()->setAxisY(axis);
}

QValue3DAxis *Q3DScatter::axisY() const
{
    return static_cast<QValue3DAxis *>(dptrc()->axisY());
}

void Q3DScatter::setAxisZ(QValue3DAxis *axis)
{
    dptr()->setAxisZ(axis);
}

QValue3DAxis *Q3DScatter::axisZ() const
{
    return static_cast<QValue3DAxis *>(dptrc()->axisZ());
}

QScatter3DSeries *Q3DScatter::selectedSeries() const
{
    return dptrc()->selectedSeries();
}

void Q3DScatter::addAxis(QValue3DAxis *axis)
{
    dptr()->m_scatterController->addAxis(axis);
}

void Q3DScatter::releaseAxis(QValue3DAxis *axis)
{
    dptr()->m_scatterController->releaseAxis(axis);
}

QList<QValue3DAxis *> Q3DScatter::axes() const
{
    QList<QAbstract3DAxis *> abstractAxes = dptrc()->m_scatterController->axes();
    QList<QValue3DAxis *> retList;
    for (QAbstract3DAxis *axis : abstractAxes)
        retList.append(static_cast<QValue3DAxis *>(axis));

    return retList;
}

QT_END_NAMESPACE

