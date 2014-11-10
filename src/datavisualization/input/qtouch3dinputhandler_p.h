/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef QTOUCH3DINPUTHANDLER_P_H
#define QTOUCH3DINPUTHANDLER_P_H

#include "q3dinputhandler_p.h"
#include "qtouch3dinputhandler.h"

class QTimer;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QAbstract3DInputHandler;

class QTouch3DInputHandlerPrivate : public Q3DInputHandlerPrivate
{
    Q_OBJECT

public:
    QTouch3DInputHandlerPrivate(QTouch3DInputHandler *q);
    ~QTouch3DInputHandlerPrivate();

    void handlePinchZoom(float distance, const QPoint &pos);
    void handleTapAndHold();
    void handleSelection(const QPointF &position);
    void handleRotation(const QPointF &position);

private:
    QTouch3DInputHandler *q_ptr;
public:
    QTimer *m_holdTimer;
    QAbstract3DInputHandlerPrivate::InputState m_inputState;
    QPointF m_startHoldPos;
    QPointF m_touchHoldPos;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
