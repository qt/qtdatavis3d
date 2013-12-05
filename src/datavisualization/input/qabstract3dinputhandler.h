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

#ifndef QABSTRACT3DINPUTHANDLER_H
#define QABSTRACT3DINPUTHANDLER_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QtDataVisualization/q3dscene.h>
#include <QMouseEvent>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QAbstract3DInputHandlerPrivate;

class QT_DATAVISUALIZATION_EXPORT QAbstract3DInputHandler : public QObject
{
    Q_OBJECT
    Q_ENUMS(InputState)
    Q_PROPERTY(InputState inputState READ inputState WRITE setInputState NOTIFY inputStateChanged)
    Q_PROPERTY(QPoint inputPosition READ inputPosition WRITE setInputPosition NOTIFY positionChanged)
    Q_PROPERTY(Q3DScene *scene READ scene WRITE setScene NOTIFY sceneChanged)

public:
    enum InputState {
        InputStateNone = 0,
        InputStateOnScene,
        InputStateOnPrimaryView,
        InputStateOnSecondaryView,
        InputStateRotating,
        InputStatePinching
    };

public:
    explicit QAbstract3DInputHandler(QObject *parent = 0);
    virtual ~QAbstract3DInputHandler();

    // Input event listeners
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void touchEvent(QTouchEvent *event);
    virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void wheelEvent(QWheelEvent *event);

public:
    // TODO: Check if the inputState needs to be visible outside of subclasses in the final architecture
    InputState inputState();
    void setInputState(InputState inputState);

    QPoint inputPosition() const;
    void setInputPosition(const QPoint &position);

    Q3DScene *scene() const;
    void setScene(Q3DScene *scene);

signals:
    void positionChanged(QPoint position);
    void inputStateChanged(InputState state);
    void sceneChanged(const Q3DScene *scene);

protected:
    void setPrevDistance(int distance);
    int prevDistance() const;
    void setPreviousInputPos(const QPoint &position);
    QPoint previousInputPos() const;

private:
    Q_DISABLE_COPY(QAbstract3DInputHandler)

    QScopedPointer<QAbstract3DInputHandlerPrivate> d_ptr;

    friend class Abstract3DController;
    friend class QTouch3DInputHandlerPrivate;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QABSTRACT3DINPUTHANDLER_H
