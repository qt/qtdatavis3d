/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
******************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q3DINPUTHANDLER_P_H
#define Q3DINPUTHANDLER_P_H

#include "qabstract3dinputhandler_p.h"
#include "q3dinputhandler.h"

QT_BEGIN_NAMESPACE

class Abstract3DController;

class Q3DInputHandlerPrivate : public QObject
{
    Q_OBJECT
public:
    Q3DInputHandlerPrivate(Q3DInputHandler *q);
    ~Q3DInputHandlerPrivate();

public Q_SLOTS:
    void handleSceneChange(Q3DScene *scene);
    void handleQueriedGraphPositionChange();

private:
    Q3DInputHandler *q_ptr;
protected:
    QAbstract3DInputHandlerPrivate::InputState m_inputState;

    bool m_rotationEnabled;
    bool m_zoomEnabled;
    bool m_selectionEnabled;
    bool m_zoomAtTargetEnabled;
    bool m_zoomAtTargetPending;

    Abstract3DController *m_controller; // Not owned

    float m_requestedZoomLevel;
    float m_driftMultiplier;

    friend class Q3DInputHandler;
};

QT_END_NAMESPACE

#endif
