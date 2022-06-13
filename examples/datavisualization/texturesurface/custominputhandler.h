// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef CUSTOMINPUTHANDLER_H
#define CUSTOMINPUTHANDLER_H

#include <QtDataVisualization/Q3DInputHandler>
#include <QtDataVisualization/QAbstract3DGraph>
#include <QtDataVisualization/QValue3DAxis>
#include "highlightseries.h"

class CustomInputHandler : public Q3DInputHandler
{
    Q_OBJECT

    enum InputState {
        StateNormal = 0,
        StateDraggingX,
        StateDraggingZ,
        StateDraggingY
    };

public:
    explicit CustomInputHandler(QAbstract3DGraph *graph, QObject *parent = 0);

    inline void setLimits(float min, float max, float minRange) {
        m_areaMinValue = min;
        m_areaMaxValue = max;
        m_axisXMinValue = m_areaMinValue;
        m_axisXMaxValue = m_areaMaxValue;
        m_axisZMinValue = m_areaMinValue;
        m_axisZMaxValue = m_areaMaxValue;
        m_axisXMinRange = minRange;
        m_axisZMinRange = minRange;
    }
    inline void setAxes(QValue3DAxis *axisX, QValue3DAxis *axisY, QValue3DAxis *axisZ) {
        m_axisX = axisX;
        m_axisY = axisY;
        m_axisZ = axisZ;
    }
    inline void setAspectRatio(float ratio) { m_aspectRatio = ratio; }
    inline void setHighlightSeries(HighlightSeries *series) { m_highlight = series; }
    inline void setDragSpeedModifier(float modifier) { m_speedModifier = modifier; }

    virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void wheelEvent(QWheelEvent *event);

private:
    void handleElementSelected(QAbstract3DGraph::ElementType type);
    void handleAxisDragging();
    void checkConstraints();

private:
    HighlightSeries *m_highlight;
    bool m_mousePressed;
    InputState m_state;
    QValue3DAxis *m_axisX;
    QValue3DAxis *m_axisY;
    QValue3DAxis *m_axisZ;
    float m_speedModifier;
    float m_aspectRatio;
    float m_axisXMinValue;
    float m_axisXMaxValue;
    float m_axisXMinRange;
    float m_axisZMinValue;
    float m_axisZMaxValue;
    float m_axisZMinRange;
    float m_areaMinValue;
    float m_areaMaxValue;
};

#endif
