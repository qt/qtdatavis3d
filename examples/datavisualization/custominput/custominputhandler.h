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

#ifndef CUSTOMINPUTHANDLER_H
#define CUSTOMINPUTHANDLER_H

#include <QtDataVisualization/QAbstract3DInputHandler>

using namespace QtDataVisualization;

class CustomInputHandler : public QAbstract3DInputHandler
{
    Q_OBJECT
public:
    explicit CustomInputHandler(QObject *parent = 0);

    virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void wheelEvent(QWheelEvent *event);
};

#endif
