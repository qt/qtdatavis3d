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

#include "q3dsurface.h"
#include "q3dsurface_p.h"
#include "qvalue3daxis.h"
#include "qsurfacedataproxy.h"
#include "q3dcamera.h"
#include "qsurface3dseries_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class Q3DSurface
 * \inmodule QtDataVisualization
 * \brief The Q3DSurface class provides methods for rendering 3D surface plots.
 * \since Qt Data Visualization 1.0
 *
 * This class enables developers to render 3D surface plots and to view them by rotating the scene
 * freely. The surface plotting includes also gridline that can be set on or off.
 * The visual appearance of the surface can be changed by controlling the smooth status.
 *
 * The Q3DSurface supports selection by showing a highlighted ball on the data point where the user has clicked
 * with left mouse button (when default input handler is in use) or selected via QSurface3DSeries.
 * The selection pointer is accompanied with a label which in default case shows the value of the
 * data point and the coordinates of the point.
 *
 * The value range and the label format shown on the axis can be controlled through QValue3DAxis.
 *
 * To rotate the graph, hold down the right mouse button and move the mouse. Zooming is done using mouse
 * wheel. Both assume the default input handler is in use.
 *
 * If no axes are set explicitly to Q3DSurface, temporary default axes with no labels are created.
 * These default axes can be modified via axis accessors, but as soon any axis is set explicitly
 * for the orientation, the default axis for that orientation is destroyed.
 *
 * Q3DSurface supports only single series at a time.
 *
 * \section1 How to construct a minimal Q3DSurface graph
 *
 * First, construct Q3DSurface. Since we are running the graph as top level window
 * in this example, we need to clear the \c Qt::FramelessWindowHint flag, which gets set by
 * default:
 *
 * \snippet doc_src_q3dsurface_construction.cpp 0
 *
 * Now Q3DSurface is ready to receive data to be rendered. Create data elements to receive values:
 *
 * \snippet doc_src_q3dsurface_construction.cpp 1
 *
 * First feed the data to the row elements and then add their pointers to the data element:
 *
 * \snippet doc_src_q3dsurface_construction.cpp 2
 *
 * Create a new series and set data to it:
 *
 * \snippet doc_src_q3dsurface_construction.cpp 3
 *
 * Finally you will need to set it visible:
 *
 * \snippet doc_src_q3dsurface_construction.cpp 4
 *
 * The complete code needed to create and display this graph is:
 *
 * \snippet doc_src_q3dsurface_construction.cpp 5
 *
 * And this is what those few lines of code produce:
 *
 * \image q3dsurface-minimal.png
 *
 * The scene can be rotated and zoomed into, but no other interaction is included in this minimal
 * code example.
 *
 *
 * \sa Q3DBars, Q3DScatter, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs a new 3D surface graph with optional \a parent window.
 */
Q3DSurface::Q3DSurface(QWindow *parent)
    : QAbstract3DGraph(new Q3DSurfacePrivate(this), parent)
{
    dptr()->m_shared = new Surface3DController(geometry());
    d_ptr->setVisualController(dptr()->m_shared);
    dptr()->m_shared->initializeOpenGL();
}

/*!
 *  Destroys the 3D surface graph.
 */
Q3DSurface::~Q3DSurface()
{
}

/*!
 * Adds the \a series to the graph.
 *
 * \note The surface graph currently supports only a single series at a time.
 */
void Q3DSurface::addSeries(QSurface3DSeries *series)
{
    dptr()->m_shared->addSeries(series);
}

/*!
 * Removes the \a series from the graph.
 */
void Q3DSurface::removeSeries(QSurface3DSeries *series)
{
    dptr()->m_shared->removeSeries(series);
}

/*!
 * \return list of series added to this graph.
 *
 * \note The surface graph currently supports only a single series at a time.
 */
QList<QSurface3DSeries *> Q3DSurface::seriesList()
{
    return dptr()->m_shared->surfaceSeriesList();
}

Q3DSurfacePrivate *Q3DSurface::dptr()
{
    return static_cast<Q3DSurfacePrivate *>(d_ptr.data());
}

const Q3DSurfacePrivate *Q3DSurface::dptrc() const
{
    return static_cast<const Q3DSurfacePrivate *>(d_ptr.data());
}

/*!
 * Sets a user-defined X-axis. Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is set explicitly to the same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DSurface::setAxisX(QValue3DAxis *axis)
{
    dptr()->m_shared->setAxisX(axis);
}

/*!
 * \return used X-axis.
 */
QValue3DAxis *Q3DSurface::axisX() const
{
    return static_cast<QValue3DAxis *>(dptrc()->m_shared->axisX());
}

/*!
 * Sets a user-defined Y-axis. Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is set explicitly to the same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DSurface::setAxisY(QValue3DAxis *axis)
{
    dptr()->m_shared->setAxisY(axis);
}

/*!
 * \return used Y-axis.
 */
QValue3DAxis *Q3DSurface::axisY() const
{
    return static_cast<QValue3DAxis *>(dptrc()->m_shared->axisY());
}

/*!
 * Sets a user-defined Z-axis. Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is set explicitly to the same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DSurface::setAxisZ(QValue3DAxis *axis)
{
    dptr()->m_shared->setAxisZ(axis);
}

/*!
 * \return used Z-axis.
 */
QValue3DAxis *Q3DSurface::axisZ() const
{
    return static_cast<QValue3DAxis *>(dptrc()->m_shared->axisZ());
}

/*!
 * Adds \a axis to the graph. The axes added via addAxis are not yet taken to use,
 * addAxis is simply used to give the ownership of the \a axis to the graph.
 * The \a axis must not be null or added to another graph.
 *
 * \sa releaseAxis(), setAxisX(), setAxisY(), setAxisZ()
 */
void Q3DSurface::addAxis(QValue3DAxis *axis)
{
    dptr()->m_shared->addAxis(axis);
}

/*!
 * Releases the ownership of the \a axis back to the caller, if it is added to this graph.
 * If the released \a axis is in use, a new default axis will be created and set active.
 *
 * If the default axis is released and added back later, it behaves as any other axis would.
 *
 * \sa addAxis(), setAxisX(), setAxisY(), setAxisZ()
 */
void Q3DSurface::releaseAxis(QValue3DAxis *axis)
{
    dptr()->m_shared->releaseAxis(axis);
}

/*!
 * \return list of all added axes.
 *
 * \sa addAxis()
 */
QList<QValue3DAxis *> Q3DSurface::axes() const
{
    QList<QAbstract3DAxis *> abstractAxes = dptrc()->m_shared->axes();
    QList<QValue3DAxis *> retList;
    foreach (QAbstract3DAxis *axis, abstractAxes)
        retList.append(static_cast<QValue3DAxis *>(axis));

    return retList;
}

/////////////////// PRIVATE ///////////////////////////////////

Q3DSurfacePrivate::Q3DSurfacePrivate(Q3DSurface *q)
    : QAbstract3DGraphPrivate(q)
{
}

Q3DSurfacePrivate::~Q3DSurfacePrivate()
{
}

Q3DSurface *Q3DSurfacePrivate::qptr()
{
    return static_cast<Q3DSurface *>(q_ptr);
}

QT_DATAVISUALIZATION_END_NAMESPACE
