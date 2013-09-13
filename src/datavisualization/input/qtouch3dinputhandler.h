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

#ifndef QTOUCH3DINPUTHANDLER_H
#define QTOUCH3DINPUTHANDLER_H

#include <QtDataVisualization/q3dinputhandler.h>
#include "q3dcamera.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QTouch3DInputHandlerPrivate;

class QT_DATAVISUALIZATION_EXPORT QTouch3DInputHandler : public Q3DInputHandler
{
    Q_OBJECT

public:
    explicit QTouch3DInputHandler(QObject *parent = 0);
    virtual ~QTouch3DInputHandler();

    // Input event listeners
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void touchEvent(QTouchEvent *event);
    virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);

private:
    Q_DISABLE_COPY(QTouch3DInputHandler)

    QScopedPointer<QTouch3DInputHandlerPrivate> d_ptr;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QTOUCH3DINPUTHANDLER_H
