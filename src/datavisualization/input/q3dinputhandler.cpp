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

#include "datavisualizationglobal_p.h"
#include "q3dinputhandler.h"
#include "q3dcamera_p.h"
#include "q3dlight.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

const int minZoomLevel         = 10;
const int halfSizeZoomLevel    = 50;
const int oneToOneZoomLevel    = 100;
const int maxZoomLevel         = 500;

const int nearZoomRangeDivider = 12;
const int midZoomRangeDivider  = 60;
const int farZoomRangeDivider  = 120;

const float rotationSpeed      = 100.0f;

/*!
 * \class Q3DInputHandler
 * \inmodule QtDataVisualization
 * \brief Basic wheel mouse based input handler.
 * \since 1.0.0
 *
 * Q3DInputHandler is the basic input handler for wheel mouse type of input devices.
 *
 * Default input handler has the following functionalty:
 * \table
 *   \header
 *     \li Mouse action                 \li Action
 *   \row
 *     \li Right button pressed         \li Rotate graph within limits set for Q3DCamera
 *   \row
 *     \li Left click                   \li Select item under cursor or remove selection if none
 *   \row
 *     \li Mouse wheel                  \li Zoom in/out within default range (10...500%)
 *   \row
 *     \li Left click on secodanry view \li Return to primary view when in slice mode
 *                                      \note Slice mode is available in Q3DBars and Q3DSurface only
 * \endtable
 */

/*!
 * Constructs the basic mouse input handler. An optional \a parent parameter can be given
 * and is then passed to QObject constructor.
 */
Q3DInputHandler::Q3DInputHandler(QObject *parent) :
    QAbstract3DInputHandler(parent)
{
}

/*!
 *  Destroys the input handler.
 */
Q3DInputHandler::~Q3DInputHandler()
{
}

// Input event listeners
/*!
 * Override this to change handling of mouse press events.
 * Mouse press event is given in the \a event and the mouse position in \a mousePos.
 */
void Q3DInputHandler::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
#if defined(Q_OS_ANDROID)
    Q_UNUSED(event);
    Q_UNUSED(mousePos);
#else
    if (Qt::LeftButton == event->button()) {
        if (scene()->isSlicingActive()) {
            if (scene()->isPointInPrimarySubView(mousePos)) {
                setInputState(QDataVis::InputStateOnOverview);
            } else if (scene()->isPointInSecondarySubView(mousePos)) {
                setInputState(QDataVis::InputStateOnSlice);
            } else {
                setInputState(QDataVis::InputStateNone);
            }
        } else {
            setInputState(QDataVis::InputStateOnScene);
            // update mouse positions to prevent jumping when releasing or repressing a button
            setInputPosition(mousePos);
        }
    } else if (Qt::MiddleButton == event->button()) {
        // reset rotations
        setInputPosition(QPoint(0, 0));
    } else if (Qt::RightButton == event->button()) {
        // disable rotating when in slice view
        if (!scene()->isSlicingActive())
            setInputState(QDataVis::InputStateRotating);
        // update mouse positions to prevent jumping when releasing or repressing a button
        setInputPosition(mousePos);
    }
#endif
}

/*!
 * Override this to change handling of mouse release events.
 * Mouse release event is given in the \a event and the mouse position in \a mousePos.
 */
void Q3DInputHandler::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
#if defined (Q_OS_ANDROID)
    Q_UNUSED(mousePos);
#else
    if (QDataVis::InputStateRotating == inputState()) {
        // update mouse positions to prevent jumping when releasing or repressing a button
        setInputPosition(mousePos);
    }
    setInputState(QDataVis::InputStateNone);
#endif
}

/*!
 * Override this to change handling of mouse move events.
 * Mouse move event is given in the \a event and the mouse position in \a mousePos.
 */
void Q3DInputHandler::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
#if defined (Q_OS_ANDROID)
    Q_UNUSED(mousePos);
#else
    if (QDataVis::InputStateRotating == inputState()) {
        // Calculate mouse movement since last frame
        QPointF rotations = scene()->activeCamera()->rotations();
        float xRotation = rotations.x();
        float yRotation = rotations.y();
        float mouseMoveX = float(inputPosition().x() - mousePos.x())
                / (scene()->viewport().width() / rotationSpeed);
        float mouseMoveY = float(inputPosition().y() - mousePos.y())
                / (scene()->viewport().height() / rotationSpeed);
        // Apply to rotations
        xRotation -= mouseMoveX;
        yRotation -= mouseMoveY;
        scene()->activeCamera()->setRotations(QPointF(xRotation, yRotation));
        scene()->activeCamera()->d_ptr->updateViewMatrix(1.0f);

        setPreviousInputPos(inputPosition());
        setInputPosition(mousePos);
    }
#endif
}

/*!
 * Override this to change handling of wheel events.
 * The wheel event is given in the \a event.
 */
void Q3DInputHandler::wheelEvent(QWheelEvent *event)
{
    // disable zooming if in slice view
    if (scene()->isSlicingActive())
        return;

    // Adjust zoom level based on what zoom range we're in.
    int zoomLevel = scene()->activeCamera()->zoomLevel();
    if (zoomLevel > oneToOneZoomLevel)
        zoomLevel += event->angleDelta().y() / nearZoomRangeDivider;
    else if (zoomLevel > halfSizeZoomLevel)
        zoomLevel += event->angleDelta().y() / midZoomRangeDivider;
    else
        zoomLevel += event->angleDelta().y() / farZoomRangeDivider;
    if (zoomLevel > maxZoomLevel)
        zoomLevel = maxZoomLevel;
    else if (zoomLevel < minZoomLevel)
        zoomLevel = minZoomLevel;

    scene()->activeCamera()->setZoomLevel(zoomLevel);
}

QT_DATAVISUALIZATION_END_NAMESPACE
