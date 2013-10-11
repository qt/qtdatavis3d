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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef SURFACE3DCONTROLLER_P_H
#define SURFACE3DCONTROLLER_P_H

#include "abstract3dcontroller_p.h"
#include "datavisualizationglobal_p.h"

#include <QLinearGradient>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Surface3DRenderer;

struct Surface3DChangeBitField {
    bool gradientColorChanged     : 1;
    bool smoothStatusChanged      : 1;
    bool surfaceGridChanged       : 1;

    Surface3DChangeBitField() :
        gradientColorChanged(true),
        smoothStatusChanged(true),
        surfaceGridChanged(true)
    {
    }
};

class QT_DATAVISUALIZATION_EXPORT Surface3DController : public Abstract3DController
{
    Q_OBJECT

private:
    Surface3DChangeBitField m_changeTracker;

    // Rendering
    Surface3DRenderer *m_renderer;
    bool m_isSmoothSurfaceEnabled;
    bool m_isSurfaceGridEnabled;
    QLinearGradient m_userDefinedGradient;

public:
    explicit Surface3DController(QRect rect);
    ~Surface3DController();

    void initializeOpenGL();
    virtual void synchDataToRenderer();

    void setSmoothSurface(bool enable);
    bool smoothSurface();

    void setSurfaceGrid(bool enable);
    bool surfaceGrid();

    void setGradient(const QLinearGradient &gradient);
    QLinearGradient gradient() const;

    void setGradientColorAt(qreal pos, const QColor &color);

    void setSelectionMode(QDataVis::SelectionMode mode);

    virtual void setActiveDataProxy(QAbstractDataProxy *proxy);

    virtual void handleAxisAutoAdjustRangeChangedInOrientation(Q3DAbstractAxis::AxisOrientation orientation, bool autoAdjust);
    virtual void handleAxisRangeChangedBySender(QObject *sender);

public slots:
    void handleArrayReset();

signals:
    void smoothStatusChanged(bool enable);
    void surfaceGridChanged(bool enable);
    void segmentCountChanged(GLint segmentCount, GLfloat step, GLfloat minimum);

private:
    void adjustValueAxisRange();

    Q_DISABLE_COPY(Surface3DController)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // SURFACE3DCONTROLLER_P_H
