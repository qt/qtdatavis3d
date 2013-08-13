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

#ifndef Q3DSURFACE_H
#define Q3DSURFACE_H

#include <QtDataVis3D/qdatavis3dnamespace.h>
#include <QtDataVis3D/q3dwindow.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

class Q3DSurfacePrivate;

class QT_DATAVIS3D_EXPORT Q3DSurface : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(bool smoothSurface READ smoothSurface WRITE setSmoothSurface)
    Q_PROPERTY(bool surfaceGrid READ surfaceGrid WRITE setSurfaceGrid)

public:
    explicit Q3DSurface();
    ~Q3DSurface();

    // Enable or disable the smoothes of the surface
    void setSmoothSurface(bool enable);
    bool smoothSurface() const;

    // Enable or disable the grid on the surface
    void setSurfaceGrid(bool enable);
    bool surfaceGrid() const;

    void setGradientColorAt(qreal pos, const QColor &color);

    // Set segment count and step. Note; segmentCount * step should be the maximum possible value of data
    // set. Minimum is the absolute minimum possible value a bar can have. This is especially
    // important to set if values can be negative.
    void setSegmentCount(int segmentCount, qreal step, qreal minimum = 0.0f);

//TODO part
    void appendSeries(QList<qreal> series, int width, int depth);
    void showData() const;
//END TODO

    // TODO: Do these need to be public? Where are they called from?
    // Size
    void setWidth(const int width);
    void setHeight(const int height);

protected:
    void render();

#if defined(Q_OS_ANDROID)
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
#endif
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QScopedPointer<Q3DSurfacePrivate> d_ptr;
    Q_DISABLE_COPY(Q3DSurface)
};

QT_DATAVIS3D_END_NAMESPACE

#endif // Q3DSURFACE_H
