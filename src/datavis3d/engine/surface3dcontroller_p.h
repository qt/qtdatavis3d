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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef SURFACE3DCONTROLLER_P_H
#define SURFACE3DCONTROLLER_P_H

#include "abstract3dcontroller_p.h"
#include "datavis3dglobal_p.h"

#include <QLinearGradient>

QT_DATAVIS3D_BEGIN_NAMESPACE

class Surface3dRenderer;

class QT_DATAVIS3D_EXPORT Surface3dController : public Abstract3DController
{
    Q_OBJECT

private:
    Surface3dRenderer *m_renderer;
    bool m_isInitialized;
    QList<qreal> m_series; // TODO: TEMP
    int m_dataWidth;
    int m_dataDepth;
    bool m_smoothSurface;
    bool m_surfaceGrid;

    GLint m_segmentCount;
    GLfloat m_segmentStep;
    GLfloat m_segmentMinimum;

    // Interaction
    MouseState m_mouseState;
    QPoint m_mousePos;
    QDataVis::SelectionMode m_selectionMode;

public:
    explicit Surface3dController(QRect rect);
    ~Surface3dController();

    void initializeOpenGL();
    void render(const GLuint defaultFboHandle = 0);

    QPoint mousePosition();

    QMatrix4x4 calculateViewMatrix(int zoom, int viewPortWidth, int viewPortHeight, bool showUnder = false);

    void setWidth(const int width);
    void setHeight(const int height);

    // Enable or disable the smoothes of the surface
    void setSmoothSurface(bool enable);
    bool smoothSurface();

    // Enable or disable the grid on the surface
    void setSurfaceGrid(bool enable);
    bool surfaceGrid();

    void setGradientColorAt(qreal pos, const QColor &color);

    // Set segment count and step. Note; segmentCount * step should be the maximum possible value of data
    // set. Minimum is the absolute minimum possible value a bar can have. This is especially
    // important to set if values can be negative.
    void setSegmentCount(GLint segmentCount, GLfloat step, GLfloat minimum = 0.0f);

    //TODO: Temp solution
    void setData(QList<qreal> series, int width, int depth);

#if defined(Q_OS_ANDROID)
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
#endif
    void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    void wheelEvent(QWheelEvent *event);

    // TODO: abstract renderer should have accessor for Drawer instead
    virtual Drawer *drawer();

signals:
    void smoothStatusChanged(bool enable);
    void surfaceGridChanged(bool enable);
    void segmentCountChanged(GLint segmentCount, GLfloat step, GLfloat minimum);
    void leftMousePressed();

private:
    Q_DISABLE_COPY(Surface3dController)
};

QT_DATAVIS3D_END_NAMESPACE

#endif // SURFACE3DCONTROLLER_P_H
