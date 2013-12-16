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

#include "q3dscatter.h"
#include "q3dscatter_p.h"
#include "scatter3dcontroller_p.h"
#include "q3dvalueaxis.h"
#include "q3dcamera.h"
#include "qscatter3dseries_p.h"

#include <QMouseEvent>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class Q3DScatter
 * \inmodule QtDataVisualization
 * \brief The Q3DScatter class provides methods for rendering 3D scatter graphs.
 * \since Qt Data Visualization 1.0
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
 * Methods are provided for changing themes, item selection modes and so on. See the
 * methods for more detailed descriptions.
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
 * The scene can be rotated and zoomed into, but no other interaction is included in this minimal
 * code example. You can learn more by familiarizing yourself with the examples provided, like
 * the \l{Scatter Example}.
 *
 * \sa Q3DBars, Q3DSurface, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs a new 3D scatter graph with optional \a parent window.
 */
Q3DScatter::Q3DScatter(QWindow *parent)
    : Q3DWindow(new Q3DScatterPrivate(this), parent)
{
    dptr()->m_shared = new Scatter3DController(geometry());
    d_ptr->setVisualController(dptr()->m_shared);
    dptr()->m_shared->initializeOpenGL();
    QObject::connect(dptr()->m_shared, &Abstract3DController::selectionModeChanged, this,
                     &Q3DScatter::selectionModeChanged);
    QObject::connect(dptr()->m_shared, &Abstract3DController::shadowQualityChanged, this,
                     &Q3DScatter::shadowQualityChanged);
    QObject::connect(dptr()->m_shared, &Abstract3DController::themeChanged, this,
                     &Q3DScatter::themeChanged);
    QObject::connect(dptr()->m_shared, &Abstract3DController::needRender, d_ptr.data(),
                     &Q3DWindowPrivate::renderLater);
    QObject::connect(dptr()->m_shared, &Abstract3DController::shadowQualityChanged, dptr(),
                     &Q3DScatterPrivate::handleShadowQualityUpdate);
}

/*!
 * Destroys the 3D scatter graph.
 */
Q3DScatter::~Q3DScatter()
{
}

/*!
 * Adds the \a series to the graph. A graph can contain multiple series, but has only one set of
 * axes. If the newly added series has specified a selected item, it will be highlighted and
 * any existing selection will be cleared. Only one added series can have an active selection.
 */
void Q3DScatter::addSeries(QScatter3DSeries *series)
{
    dptr()->m_shared->addSeries(series);
}

/*!
 * Removes the \a series from the graph.
 */
void Q3DScatter::removeSeries(QScatter3DSeries *series)
{
    dptr()->m_shared->removeSeries(series);
}

/*!
 * \return list of series added to this graph.
 */
QList<QScatter3DSeries *> Q3DScatter::seriesList()
{
    return dptr()->m_shared->scatterSeriesList();
}

/*!
 * \internal
 */
void Q3DScatter::mouseDoubleClickEvent(QMouseEvent *event)
{
    dptr()->m_shared->mouseDoubleClickEvent(event);
}

/*!
 * \internal
 */
void Q3DScatter::touchEvent(QTouchEvent *event)
{
    dptr()->m_shared->touchEvent(event);
}

/*!
 * \internal
 */
void Q3DScatter::mousePressEvent(QMouseEvent *event)
{
    dptr()->m_shared->mousePressEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DScatter::mouseReleaseEvent(QMouseEvent *event)
{
    dptr()->m_shared->mouseReleaseEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DScatter::mouseMoveEvent(QMouseEvent *event)
{
    dptr()->m_shared->mouseMoveEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DScatter::wheelEvent(QWheelEvent *event)
{
    dptr()->m_shared->wheelEvent(event);
}

Q3DScatterPrivate *Q3DScatter::dptr()
{
    return static_cast<Q3DScatterPrivate *>(d_ptr.data());
}

const Q3DScatterPrivate *Q3DScatter::dptrc() const
{
    return static_cast<const Q3DScatterPrivate *>(d_ptr.data());
}

/*!
 * \property Q3DScatter::theme
 *
 * A \a theme to be used for the graph. Ownership of the \a theme is transferred. Previous theme
 * is deleted when a new one is set. Properties of the \a theme can be modified even after setting
 * it, and the modifications take effect immediately.
 */
void Q3DScatter::setTheme(Q3DTheme *theme)
{
    dptr()->m_shared->setTheme(theme);
}

Q3DTheme *Q3DScatter::theme() const
{
    return dptrc()->m_shared->theme();
}

/*!
 * \property Q3DScatter::selectionMode
 *
 * Sets item selection \a mode to a combination of \c QDataVis::SelectionFlags. It is preset to
 * \c QDataVis::SelectionItem by default.
 *
 * \note Only \c QDataVis::SelectionItem and \c QDataVis::SelectionNone are supported.
 */
void Q3DScatter::setSelectionMode(QDataVis::SelectionFlags mode)
{
    dptr()->m_shared->setSelectionMode(mode);
}

QDataVis::SelectionFlags Q3DScatter::selectionMode() const
{
    return dptrc()->m_shared->selectionMode();
}

/*!
 * \property Q3DScatter::scene
 *
 * This property contains the read only Q3DScene that can be used to access e.g. camera object.
 */
Q3DScene *Q3DScatter::scene() const
{
    return dptrc()->m_shared->scene();
}

/*!
 * \property Q3DScatter::shadowQuality
 *
 * Sets shadow \a quality to one of \c QDataVis::ShadowQuality. It is preset to
 * \c QDataVis::ShadowQualityMedium by default.
 *
 * \note If setting QDataVis::ShadowQuality of a certain level fails, a level is lowered
 * until it is successful and shadowQualityChanged signal is emitted for each time the change is
 * done.
 */
void Q3DScatter::setShadowQuality(QDataVis::ShadowQuality quality)
{
    return dptr()->m_shared->setShadowQuality(quality);
}

QDataVis::ShadowQuality Q3DScatter::shadowQuality() const
{
    return dptrc()->m_shared->shadowQuality();
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
void Q3DScatter::setAxisX(Q3DValueAxis *axis)
{
    dptr()->m_shared->setAxisX(axis);
}

/*!
 * \return used X-axis.
 */
Q3DValueAxis *Q3DScatter::axisX() const
{
    return static_cast<Q3DValueAxis *>(dptrc()->m_shared->axisX());
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
void Q3DScatter::setAxisY(Q3DValueAxis *axis)
{
    dptr()->m_shared->setAxisY(axis);
}

/*!
 * \return used Y-axis.
 */
Q3DValueAxis *Q3DScatter::axisY() const
{
    return static_cast<Q3DValueAxis *>(dptrc()->m_shared->axisY());
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
void Q3DScatter::setAxisZ(Q3DValueAxis *axis)
{
    dptr()->m_shared->setAxisZ(axis);
}

/*!
 * \return used Z-axis.
 */
Q3DValueAxis *Q3DScatter::axisZ() const
{
    return static_cast<Q3DValueAxis *>(dptrc()->m_shared->axisZ());
}

/*!
 * Adds \a axis to the graph. The axes added via addAxis are not yet taken to use,
 * addAxis is simply used to give the ownership of the \a axis to the graph.
 * The \a axis must not be null or added to another graph.
 *
 * \sa releaseAxis(), setAxisX(), setAxisY(), setAxisZ()
 */
void Q3DScatter::addAxis(Q3DValueAxis *axis)
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
void Q3DScatter::releaseAxis(Q3DValueAxis *axis)
{
    dptr()->m_shared->releaseAxis(axis);
}

/*!
 * \return list of all added axes.
 *
 * \sa addAxis()
 */
QList<Q3DValueAxis *> Q3DScatter::axes() const
{
    QList<Q3DAbstractAxis *> abstractAxes = dptrc()->m_shared->axes();
    QList<Q3DValueAxis *> retList;
    foreach (Q3DAbstractAxis *axis, abstractAxes)
        retList.append(static_cast<Q3DValueAxis *>(axis));

    return retList;
}

/*!
 * \fn void Q3DScatter::shadowQualityChanged(QDataVis::ShadowQuality quality)
 *
 * This signal is emitted when shadow \a quality changes.
 */

Q3DScatterPrivate::Q3DScatterPrivate(Q3DScatter *q)
    : Q3DWindowPrivate(q)
{
}

Q3DScatterPrivate::~Q3DScatterPrivate()
{
    delete m_shared;
}

void Q3DScatterPrivate::handleShadowQualityUpdate(QDataVis::ShadowQuality quality)
{
    emit qptr()->shadowQualityChanged(quality);
}

Q3DScatter *Q3DScatterPrivate::qptr()
{
    return static_cast<Q3DScatter *>(q_ptr);
}

QT_DATAVISUALIZATION_END_NAMESPACE

