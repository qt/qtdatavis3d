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

#ifndef QABSTRACT3DINPUTHANDLER_P_H
#define QABSTRACT3DINPUTHANDLER_P_H

#include <QMouseEvent>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QAbstract3DInputHandler;
class CameraHelper;

class QAbstract3DInputHandlerPrivate
{
public:
    QAbstract3DInputHandlerPrivate(QAbstract3DInputHandler *q);
    ~QAbstract3DInputHandlerPrivate();

public:
    QAbstract3DInputHandler *q_ptr;
    int m_prevDistance;

private:
    QDataVis::InputState m_inputState;
    bool m_isSlicingActivated;
    QPoint m_inputPosition;
    int m_zoomLevel;
    QRect m_mainViewPort;

    // TODO: Replace with proper camera once it's available
    CameraHelper *m_camera;

    friend class QAbstract3DInputHandler;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QABSTRACT3DINPUTHANDLER_P_H
