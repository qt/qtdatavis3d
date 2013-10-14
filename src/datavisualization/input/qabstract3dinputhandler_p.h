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

#ifndef QABSTRACT3DINPUTHANDLER_P_H
#define QABSTRACT3DINPUTHANDLER_P_H

#include "datavisualizationglobal_p.h"
#include "qabstract3dinputhandler.h"
#include <QRect>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QAbstract3DInputHandler;
class Q3DScene;

class QAbstract3DInputHandlerPrivate
{
public:
    QAbstract3DInputHandlerPrivate(QAbstract3DInputHandler *q);
    ~QAbstract3DInputHandlerPrivate();

public:
    QAbstract3DInputHandler *q_ptr;
    int m_prevDistance;
    QPoint m_previousInputPos;

    GLfloat m_defaultXRotation;
    GLfloat m_defaultYRotation;

private:
    QDataVis::InputState m_inputState;
    QPoint m_inputPosition;
    QRect m_mainViewPort;

    // TODO: Check if this could be avoided with signals/slots or some other way.
    Q3DScene *m_scene;
    bool m_isDefaultHandler;

    friend class QAbstract3DInputHandler;
    friend class Abstract3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QABSTRACT3DINPUTHANDLER_P_H
