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

#ifndef QDEFAULT3DINPUTHANDLER_H
#define QDEFAULT3DINPUTHANDLER_H

#include <QtDataVisualization/qabstract3dinputhandler.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DInputHandlerPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DInputHandler : public QAbstract3DInputHandler
{
    Q_OBJECT

public:
    explicit Q3DInputHandler(QObject *parent = 0);
    virtual ~Q3DInputHandler();

    // Input event listeners
    virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void wheelEvent(QWheelEvent *event);

private:
    Q_DISABLE_COPY(Q3DInputHandler)

    QScopedPointer<Q3DInputHandlerPrivate> *d_ptr;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif // QDEFAULT3DINPUTHANDLER_H
