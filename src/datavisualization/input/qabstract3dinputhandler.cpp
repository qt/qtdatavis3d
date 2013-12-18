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
#include "qabstract3dinputhandler_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QAbstract3DInputHandler
 * \inmodule QtDataVisualization
 * \brief The base class for implementations of input handlers.
 * \since Qt Data Visualization 1.0
 *
 * QAbstract3DInputHandler is the base class that is subclassed by different input handling implementations
 * that take input events and translate those to camera and light movements. Input handlers also translate
 * raw input events to slicing and selection events in the scene.
 */

/*!
 * \enum QAbstract3DInputHandler::InputState
 *
 * Predefined input states for mouse and touch based input handlers. All states are not valid
 * with all input handlers.
 *
 * \value InputStateNone
 *        Default "no input received" state.
 * \value InputStateOnScene
 *        Mouse or touch input received on the 3D scene.
 * \value InputStateOnPrimaryView
 *        Mouse or touch input received on the primary view area.
 * \value InputStateOnSecondaryView
 *        Mouse or touch input received on the secondary view area.
 * \value InputStateRotating
 *        Rotation of the 3D geometry ongoing.
 * \value InputStatePinching
 *        Pinch/punch multitouch input ongoing.
 */

/*!
 * \qmltype AbstractInputHandler3D
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QAbstract3DInputHandler
 * \brief Base type for all QtDataVisualization input handlers.
 *
 * This type is uncreatable.
 */

/*!
 * Constructs the base class. An optional \a parent parameter can be given
 * and is then passed to QObject constructor.
 */
QAbstract3DInputHandler::QAbstract3DInputHandler(QObject *parent) :
    QObject(parent),
    d_ptr(new QAbstract3DInputHandlerPrivate(this))
{
}

/*!
 *  Destroys the base class.
 */
QAbstract3DInputHandler::~QAbstract3DInputHandler()
{
}

// Input event listeners
/*!
 * Override this to handle mouse double click events.
 * Mouse double click event is given in the \a event.
 */
void QAbstract3DInputHandler::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

/*!
 * Override this to handle touch input events.
 * Touch event is given in the \a event.
 */
void QAbstract3DInputHandler::touchEvent(QTouchEvent *event)
{
    Q_UNUSED(event);
}

/*!
 * Override this to handle mouse press events.
 * Mouse press event is given in the \a event and the mouse position in \a mousePos.
 */
void QAbstract3DInputHandler::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    Q_UNUSED(mousePos);
}

/*!
 * Override this to handle mouse release events.
 * Mouse release event is given in the \a event and the mouse position in \a mousePos.
 */
void QAbstract3DInputHandler::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    Q_UNUSED(mousePos);
}

/*!
 * Override this to handle mouse move events.
 * Mouse move event is given in the \a event and the mouse position in \a mousePos.
 */
void QAbstract3DInputHandler::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    Q_UNUSED(mousePos);
}

/*!
 * Override this to handle wheel events.
 * Wheel event is given in the \a event.
 */
void QAbstract3DInputHandler::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
}

// Property get/set
/*!
 * \property QAbstract3DInputHandler::inputState
 *
 * Current enumerated input state based on the processed input events.
 * When the state changes inputStateChanged() is emitted.
 */
QAbstract3DInputHandler::InputState QAbstract3DInputHandler::inputState()
{
    return d_ptr->m_inputState;
}

void QAbstract3DInputHandler::setInputState(InputState inputState)
{
    if (inputState != d_ptr->m_inputState) {
        d_ptr->m_inputState = inputState;
        emit inputStateChanged(inputState);
    }
}

/*!
 * \property QAbstract3DInputHandler::inputPosition
 *
 * Last input position based on the processed input events.
 */
QPoint QAbstract3DInputHandler::inputPosition() const
{
    return d_ptr->m_inputPosition;
}

void QAbstract3DInputHandler::setInputPosition(const QPoint &position)
{
    if (position != d_ptr->m_inputPosition) {
        d_ptr->m_inputPosition = position;
        emit positionChanged(position);
    }
}

/*!
 * \return the manhattan length between last two input positions.
 */
int QAbstract3DInputHandler::prevDistance() const
{
    return d_ptr->m_prevDistance;
}

/*!
 * Sets the \a distance (manhattan length) between last two input positions.
 */
void QAbstract3DInputHandler::setPrevDistance(int distance)
{
    d_ptr->m_prevDistance = distance;
}

/*!
 * \property QAbstract3DInputHandler::scene
 *
 * The 3D scene this abstract input handler is controlling. Only one scene can
 * be controlled by one input handler. Setting a \a scene to an input handler doesn't
 * transfer the ownership of the \a scene.
 */
Q3DScene *QAbstract3DInputHandler::scene() const
{
    return d_ptr->m_scene;
}

void QAbstract3DInputHandler::setScene(Q3DScene *scene)
{
    if (scene != d_ptr->m_scene) {
        d_ptr->m_scene = scene;
        emit sceneChanged(scene);
    }
}

/*!
 * Sets the previous input position to the point given by \a position.
 */
void QAbstract3DInputHandler::setPreviousInputPos(const QPoint &position)
{
    d_ptr->m_previousInputPos = position;
}

/*!
 * Returns the previous input position.
 * \return Previous input position.
 */
QPoint QAbstract3DInputHandler::previousInputPos() const
{
    return d_ptr->m_previousInputPos;
}


QAbstract3DInputHandlerPrivate::QAbstract3DInputHandlerPrivate(QAbstract3DInputHandler *q) :
    q_ptr(q),
    m_prevDistance(0),
    m_previousInputPos(QPoint(0,0)),
    m_inputState(QAbstract3DInputHandler::InputStateNone),
    m_inputPosition(QPoint(0,0)),
    m_scene(0)
{
}

QAbstract3DInputHandlerPrivate::~QAbstract3DInputHandlerPrivate()
{

}

QT_DATAVISUALIZATION_END_NAMESPACE
