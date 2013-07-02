/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "q3dsurface.h"
#include "q3dsurface_p.h"

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
    d_ptr->m_shared->render();
}

#if defined(Q_OS_ANDROID)
void Q3DSurface::mouseDoubleClickEvent(QMouseEvent *event)
{
}
void Q3DSurface::touchEvent(QTouchEvent *event)
{
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


// TODO /////////////////////////////////////////
void Q3DSurface::appendSeries(QList<qreal> series)
{
    d_ptr->appendSeries(series);
    d_ptr->m_shared->setData(series, 9, 5);
}

void Q3DSurface::showData()
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
    qDebug() << "Q3DSurfacePrivate::~Q3DSurfacePrivate";
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
