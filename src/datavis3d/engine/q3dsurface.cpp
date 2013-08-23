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

void Q3DSurface::setValueAxisX(QValueAxis *axis)
{
    Q_ASSERT(axis);

    return d_ptr->m_shared->setAxisX(axis);
}

QValueAxis *Q3DSurface::valueAxisX()
{
    return static_cast<QValueAxis *>(d_ptr->m_shared->axisX());
}

void Q3DSurface::setValueAxisY(QValueAxis *axis)
{
    Q_ASSERT(axis);

    return d_ptr->m_shared->setAxisY(axis);
}

QValueAxis *Q3DSurface::valueAxisY()
{
    return static_cast<QValueAxis *>(d_ptr->m_shared->axisY());
}

void Q3DSurface::setValueAxisZ(QValueAxis *axis)
{
    Q_ASSERT(axis);

    return d_ptr->m_shared->setAxisZ(axis);
}

QValueAxis *Q3DSurface::valueAxisZ()
{
    return static_cast<QValueAxis *>(d_ptr->m_shared->axisZ());
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
      m_shared(new Surface3dController(rect))
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
