/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
#include "qvalueaxis.h"

#include <QMouseEvent>

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

Q3DSurface::Q3DSurface()
    : d_ptr(new Q3DSurfacePrivate(this, geometry()))
{
    d_ptr->m_shared->initializeOpenGL();
}

Q3DSurface::~Q3DSurface()
{
}

void Q3DSurface::render()
{
    d_ptr->m_shared->synchDataToRenderer();
    d_ptr->m_shared->render();
}

#if defined(Q_OS_ANDROID)
void Q3DSurface::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}
void Q3DSurface::touchEvent(QTouchEvent *event)
{
    Q_UNUSED(event)
}
#endif

void Q3DSurface::mousePressEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mousePressEvent(event, event->pos());
}

void Q3DSurface::mouseReleaseEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseReleaseEvent(event, event->pos());
}

void Q3DSurface::mouseMoveEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseMoveEvent(event, event->pos());
}

void Q3DSurface::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
}

void Q3DSurface::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    d_ptr->m_shared->setWidth(width());
    d_ptr->m_shared->setHeight(height());
}

/*!
 * \property Q3DSurface::gridVisible
 *
 * \a visible Flag to enable or disable grid. \c true by default.
 *
 * Sets grid drawing on or off.
 */
void Q3DSurface::setGridVisible(bool visible)
{
    d_ptr->m_shared->setGridEnabled(visible);
}

bool Q3DSurface::isGridVisible() const
{
    return d_ptr->m_shared->gridEnabled();
}

/*!
 * \property Q3DSurface::backgroundVisible
 *
 * \a visible Flag to enable or disable background. \c true by default.
 *
 * Sets backround rendering on or off.
 */
void Q3DSurface::setBackgroundVisible(bool visible)
{
    d_ptr->m_shared->setBackgroundEnabled(visible);
}

bool Q3DSurface::isBackgroundVisible() const
{
    return d_ptr->m_shared->backgroundEnabled();
}

void Q3DSurface::setSmoothSurface(bool enable)
{
    d_ptr->m_shared->setSmoothSurface(enable);
}

bool Q3DSurface::smoothSurface() const
{
    return d_ptr->m_shared->smoothSurface();
}

void Q3DSurface::setSurfaceGrid(bool enable)
{
    d_ptr->m_shared->setSurfaceGrid(enable);
}

bool Q3DSurface::surfaceGrid() const
{
    return d_ptr->m_shared->surfaceGrid();
}

void Q3DSurface::setWidth(const int width)
{
    d_ptr->m_shared->setWidth(width);
    QWindow::setWidth(width);
}

void Q3DSurface::setHeight(const int height)
{
    d_ptr->m_shared->setHeight(height);
    QWindow::setHeight(height);
}

/*!
  TODO: REMOVE
 */
void Q3DSurface::setSegmentCount(int segmentCount, qreal step, qreal minimum)
{
    d_ptr->m_shared->setSegmentCount(GLint(segmentCount), GLfloat(step), GLfloat(minimum));
}

/*!
 * Sets a user-defined X-axis. Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is explicitly set to same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DSurface::setAxisX(QValueAxis *axis)
{
    d_ptr->m_shared->setAxisX(axis);
}

/*!
 * \return used X-axis.
 */
QValueAxis *Q3DSurface::axisX() const
{
    return static_cast<QValueAxis *>(d_ptr->m_shared->axisX());
}

/*!
 * Sets a user-defined Y-axis. Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is explicitly set to same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DSurface::setAxisY(QValueAxis *axis)
{
    d_ptr->m_shared->setAxisY(axis);
}

/*!
 * \return used Y-axis.
 */
QValueAxis *Q3DSurface::axisY() const
{
    return static_cast<QValueAxis *>(d_ptr->m_shared->axisY());
}

/*!
 * Sets a user-defined Z-axis. Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is explicitly set to same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DSurface::setAxisZ(QValueAxis *axis)
{
    d_ptr->m_shared->setAxisZ(axis);
}

/*!
 * \return used Z-axis.
 */
QValueAxis *Q3DSurface::axisZ() const
{
    return static_cast<QValueAxis *>(d_ptr->m_shared->axisZ());
}

/*!
 * Adds \a axis to the graph. The axes added via addAxis are not yet taken to use,
 * addAxis is simply used to give the ownership of the \a axis to the graph.
 * The \a axis must not be null or added to another graph.
 *
 * \sa releaseAxis(), setAxisX(), setAxisY(), setAxisZ()
 */
void Q3DSurface::addAxis(QValueAxis *axis)
{
    d_ptr->m_shared->addAxis(axis);
}

/*!
 * Releases the ownership of the \a axis back to the caller, if it is added to this graph.
 * If the released \a axis is in use, a new default axis will be created and set active.
 *
 * If the default axis is released and added back later, it behaves as any other axis would.
 *
 * \sa addAxis(), setAxisX(), setAxisY(), setAxisZ()
 */
void Q3DSurface::releaseAxis(QValueAxis *axis)
{
    d_ptr->m_shared->releaseAxis(axis);
}

/*!
 * \return list of all added axes.
 *
 * \sa addAxis()
 */
QList<QValueAxis *> Q3DSurface::axes() const
{
    QList<QAbstractAxis *> abstractAxes = d_ptr->m_shared->axes();
    QList<QValueAxis *> retList;
    foreach (QAbstractAxis *axis, abstractAxes)
        retList.append(static_cast<QValueAxis *>(axis));

    return retList;
}

void Q3DSurface::setGradientColorAt(qreal pos, const QColor &color)
{
    d_ptr->m_shared->setGradientColorAt(pos, color);
}

// TODO /////////////////////////////////////////
void Q3DSurface::appendSeries(QList<qreal> series, int width, int depth )
{
    d_ptr->appendSeries(series);
    d_ptr->m_shared->setData(series, width, depth);
}

void Q3DSurface::showData() const
{
    for (int i = 0; i < d_ptr->numOfSeries(); i++) {
        QList<qreal> s = d_ptr->seriesAt(i);
        qDebug() << "Series = ";
        foreach (qreal val, s) {
            qDebug() << val;
        }
    }
}

// TODO END //////////////////////////////////////////

/////////////////// PRIVATE ///////////////////////////////////

Q3DSurfacePrivate::Q3DSurfacePrivate(Q3DSurface *q, QRect rect)
    : q_ptr(q),
      m_shared(new Surface3DController(rect))
{
}

Q3DSurfacePrivate::~Q3DSurfacePrivate()
{
    delete m_shared;
}

void Q3DSurfacePrivate::appendSeries(QList<qreal> series)
{
    m_seriesList.append(series);
}

QList<qreal> Q3DSurfacePrivate::seriesAt(int i)
{
    return m_seriesList.at(i);
}

int Q3DSurfacePrivate::numOfSeries()
{
    return m_seriesList.count();
}

QT_DATAVIS3D_END_NAMESPACE
