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

#ifndef QABSTRACT3DINPUTHANDLER_H
#define QABSTRACT3DINPUTHANDLER_H

#include <QObject>

#include "qdatavis3denums.h"
#include "qabstract3dinputhandler_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class QAbstract3DInputHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QtDataVis3D::QDataVis::InputState inputState READ inputState WRITE setInputState NOTIFY inputStateChanged)
    Q_PROPERTY(QPoint inputPosition READ inputPosition WRITE setInputPosition NOTIFY positionChanged)
    Q_PROPERTY(bool slicingActivated READ slicingActivated WRITE setSlicingActivated NOTIFY slicingActiveChanged)
    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QRect mainViewPortRect READ mainViewPortRect WRITE setMainViewPortRect)

    Q_PROPERTY(CameraHelper *camera READ camera WRITE setCamera)

private:
    QScopedPointer<QAbstract3DInputHandlerPrivate> d_ptr;

public:
    explicit QAbstract3DInputHandler();

    // Input event listeners
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void touchEvent(QTouchEvent *event);
    virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void wheelEvent(QWheelEvent *event);

public:
    // Property get/set functions

    // TODO: Check if the inputState needs to be visible outside of subclasses in the final architecture
    QDataVis::InputState inputState();
    void setInputState(const QDataVis::InputState inputState);
    void setInputPosition(const QPoint position);
    QPoint inputPosition();
    void setSlicingActivated(const bool isSlicing);
    bool slicingActivated();
    void setZoomLevel(const int zoomLevel);
    int zoomLevel();
    QRect mainViewPortRect();
    void setMainViewPortRect(const QRect viewPort);

    // TODO: Modify for proper camera once that is available
    CameraHelper *camera();
    void setCamera(CameraHelper *camera);

protected:
    void setPrevDistance(int distance);
    int prevDistance();

signals:
    void positionChanged(QPoint position);
    void inputStateChanged(QDataVis::InputState state);
    void slicingActiveChanged(bool isSlicing);
    void zoomLevelChanged(int zoomLevel);
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QABSTRACT3DINPUTHANDLER_H
