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

#ifndef QDEFAULT3DINPUTHANDLER_H
#define QDEFAULT3DINPUTHANDLER_H

const int minZoomLevel         = 10;
const int halfSizeZoomLevel    = 50;
const int oneToOneZoomLevel    = 100;
const int maxZoomLevel         = 500;

const int nearZoomRangeDivider = 12;
const int midZoomRangeDivider  = 60;
const int farZoomRangeDivider  = 120;

#include "qabstract3dinputhandler.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class Q3DInputHandler : public QAbstract3DInputHandler
{
public:
    explicit Q3DInputHandler();

    // Input event listeners
    virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void wheelEvent(QWheelEvent *event);
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QDEFAULT3DINPUTHANDLER_H
