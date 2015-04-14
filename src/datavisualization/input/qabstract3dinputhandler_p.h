/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
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

#ifndef QABSTRACT3DINPUTHANDLER_P_H
#define QABSTRACT3DINPUTHANDLER_P_H

#include "datavisualizationglobal_p.h"
#include "qabstract3dinputhandler.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QAbstract3DInputHandler;
class Q3DScene;

class QAbstract3DInputHandlerPrivate
{
public:
    QAbstract3DInputHandlerPrivate(QAbstract3DInputHandler *q);
    ~QAbstract3DInputHandlerPrivate();

public:
    enum InputState {
        InputStateNone = 0,
        InputStateSelecting,
        InputStateRotating,
        InputStatePinching
    };

    QAbstract3DInputHandler *q_ptr;
    int m_prevDistance;
    QPoint m_previousInputPos;

private:
    QAbstract3DInputHandler::InputView m_inputView;
    QPoint m_inputPosition;

    Q3DScene *m_scene;
    bool m_isDefaultHandler;

    friend class QAbstract3DInputHandler;
    friend class Abstract3DController;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
