/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#ifndef QTOUCH3DINPUTHANDLER_P_H
#define QTOUCH3DINPUTHANDLER_P_H

#include "qabstract3dinputhandler_p.h"
#include "qtouch3dinputhandler.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QAbstract3DInputHandler;

class QTouch3DInputHandlerPrivate : public QObject
{
    Q_OBJECT

public:
    QTouch3DInputHandlerPrivate(QTouch3DInputHandler *q);
    ~QTouch3DInputHandlerPrivate();

    void handlePinchZoom(float distance);
    void handleTapAndHold();
    void handleSelection(const QPointF &position);
    void handleRotation(const QPointF &position);

public:
    QTouch3DInputHandler *q_ptr;
    QTimer *m_holdTimer;
    QAbstract3DInputHandlerPrivate::InputState m_inputState;
    QPointF m_startHoldPos;
    QPointF m_touchHoldPos;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
