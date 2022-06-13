// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef AXESINPUTHANDLER_H
#define AXESINPUTHANDLER_H

#include <QtDataVisualization/Q3DInputHandler>
#include <QtDataVisualization/QAbstract3DGraph>
#include <QtDataVisualization/QValue3DAxis>

//! [0]
class AxesInputHandler : public Q3DInputHandler
//! [0]
{
    Q_OBJECT

    enum InputState {
        StateNormal = 0,
        StateDraggingX,
        StateDraggingZ,
        StateDraggingY
    };

public:
    explicit AxesInputHandler(QAbstract3DGraph *graph, QObject *parent = 0);

    inline void setAxes(QValue3DAxis *axisX, QValue3DAxis *axisZ, QValue3DAxis *axisY) {
        m_axisX = axisX;
        m_axisZ = axisZ;
        m_axisY = axisY;
    }

    //! [1]
    inline void setDragSpeedModifier(float modifier) { m_speedModifier = modifier; }
    //! [1]

    virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);

private:
    void handleElementSelected(QAbstract3DGraph::ElementType type);
    void handleAxisDragging();

private:
    bool m_mousePressed;
    InputState m_state;
    QValue3DAxis *m_axisX;
    QValue3DAxis *m_axisZ;
    QValue3DAxis *m_axisY;
    float m_speedModifier;
};

#endif
