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

#ifndef Q3DINPUTHANDLER_H
#define Q3DINPUTHANDLER_H

#include <QtDataVisualization/qabstract3dinputhandler.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DInputHandlerPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DInputHandler : public QAbstract3DInputHandler
{
    Q_OBJECT
    Q_PROPERTY(bool rotationEnabled READ isRotationEnabled WRITE setRotationEnabled NOTIFY rotationEnabledChanged)
    Q_PROPERTY(bool zoomEnabled READ isZoomEnabled WRITE setZoomEnabled NOTIFY zoomEnabledChanged)
    Q_PROPERTY(bool selectionEnabled READ isSelectionEnabled WRITE setSelectionEnabled NOTIFY selectionEnabledChanged)
    Q_PROPERTY(bool zoomAtTargetEnabled READ isZoomAtTargetEnabled WRITE setZoomAtTargetEnabled NOTIFY zoomAtTargetEnabledChanged)

public:
    explicit Q3DInputHandler(QObject *parent = 0);
    virtual ~Q3DInputHandler();

    void setRotationEnabled(bool enable);
    bool isRotationEnabled() const;
    void setZoomEnabled(bool enable);
    bool isZoomEnabled() const;
    void setSelectionEnabled(bool enable);
    bool isSelectionEnabled() const;
    void setZoomAtTargetEnabled(bool enable);
    bool isZoomAtTargetEnabled() const;

    // Input event listeners
    virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void wheelEvent(QWheelEvent *event);

signals:
    void rotationEnabledChanged(bool enable);
    void zoomEnabledChanged(bool enable);
    void selectionEnabledChanged(bool enable);
    void zoomAtTargetEnabledChanged(bool enable);

private:
    Q_DISABLE_COPY(Q3DInputHandler)

    QScopedPointer<Q3DInputHandlerPrivate> d_ptr;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif
