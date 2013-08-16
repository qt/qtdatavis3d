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

#include "scatter3dcontroller_p.h"
#include "scatter3drenderer_p.h"
#include "camerahelper_p.h"
#include "qabstractaxis_p.h"
#include "qvalueaxis_p.h"
#include "qscatterdataproxy_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <qmath.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

Scatter3DController::Scatter3DController(QRect boundRect)
    : Abstract3DController(boundRect),
      m_mouseState(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_isSlicingActivated(false),
      m_renderer(0),
      m_data(0)
{
    // Default axes
    setAxisX(new QValueAxis());
    setAxisY(new QValueAxis());
    setAxisZ(new QValueAxis());

    // Default object type; specific to scatter
    setObjectType(QDataVis::Spheres, false);

    setDataProxy(new QScatterDataProxy);
}

Scatter3DController::~Scatter3DController()
{
    delete m_data;
}

void Scatter3DController::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (isInitialized())
        return;

    m_renderer = new Scatter3DRenderer(this);
    setRenderer(m_renderer);
}

void Scatter3DController::synchDataToRenderer()
{
    Abstract3DController::synchDataToRenderer();

    if (!isInitialized())
        return;

    // Notify changes to renderer
    if (m_changeTracker.slicingActiveChanged) {
        // TODO: Add notification.
        m_changeTracker.slicingActiveChanged = false;
    }

    if (m_isDataDirty) {
        m_renderer->updateDataModel(m_data);
        m_isDataDirty = false;
    }
}

QMatrix4x4 Scatter3DController::calculateViewMatrix(int zoom, int viewPortWidth,
                                                    int viewPortHeight, bool showUnder)
{
    return m_cameraHelper->calculateViewMatrix(m_mousePos,
                                               zoom,
                                               viewPortWidth,
                                               viewPortHeight,
                                               showUnder);
}

bool Scatter3DController::isSlicingActive()
{
    return m_isSlicingActivated;
}

void Scatter3DController::setSlicingActive(bool isSlicing)
{
    m_isSlicingActivated = isSlicing;

    m_changeTracker.slicingActiveChanged = true;
    emit slicingActiveChanged(m_isSlicingActivated);
}

Scatter3DController::MouseState Scatter3DController::mouseState()
{
    return m_mouseState;
}

#if defined(Q_OS_ANDROID)
void Scatter3DController::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!m_isSlicingActivated) {
        m_mouseState = Scatter3DController::MouseOnScene;
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = event->pos();
    }
}

void Scatter3DController::touchEvent(QTouchEvent *event)
{
    static int prevDistance = 0;

    QList<QTouchEvent::TouchPoint> points;
    points = event->touchPoints();

    if (points.count() == 2) {
        m_mouseState = Scatter3DController::MouseOnPinch;

        QPointF distance = points.at(0).pos() - points.at(1).pos();
        int newDistance = distance.manhattanLength();
        int zoomRate = 1;
        int zoomLevel = m_zoomLevel;
        if (zoomLevel > 100)
            zoomRate = 5;
        if (newDistance > prevDistance)
            zoomLevel += zoomRate;
        else
            zoomLevel -= zoomRate;
        if (zoomLevel > 500)
            zoomLevel = 500;
        else if (zoomLevel < 10)
            zoomLevel = 10;
        setZoomLevel(zoomLevel);
        prevDistance = newDistance;
        //qDebug() << "distance" << distance.manhattanLength();
    }
}
#endif

void Scatter3DController::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    QRect mainViewPort = m_renderer->mainViewPort();
    if (Qt::LeftButton == event->button()) {
        if (m_isSlicingActivated) {
            if (mousePos.x() <= mainViewPort.width()
                    && mousePos.y() <= mainViewPort.height()) {
                m_mouseState = Scatter3DController::MouseOnOverview;
                //qDebug() << "Mouse pressed on overview";
            } else {
                m_mouseState = Scatter3DController::MouseOnZoom;
                //qDebug() << "Mouse pressed on zoom";
            }
        } else {
#if !defined(Q_OS_ANDROID)
            m_mouseState = Scatter3DController::MouseOnScene;
#else
            m_mouseState = Scatter3DController::MouseRotating;
#endif
            // update mouse positions to prevent jumping when releasing or repressing a button
            m_mousePos = mousePos;
            //qDebug() << "Mouse pressed on scene";
        }
    } else if (Qt::MiddleButton == event->button()) {
        // reset rotations
        m_mousePos = QPoint(0, 0);
    } else if (Qt::RightButton == event->button()) {
#if !defined(Q_OS_ANDROID)
        m_mouseState = Scatter3DController::MouseRotating;
#else
        m_mouseState = Scatter3DController::MouseOnScene;
#endif
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos;
    }
    m_cameraHelper->updateMousePos(m_mousePos);
}

void Scatter3DController::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    if (Scatter3DController::MouseRotating == m_mouseState) {
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos;
        m_cameraHelper->updateMousePos(mousePos);
    }
    m_mouseState = Scatter3DController::MouseNone;
}

void Scatter3DController::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    if (Scatter3DController::MouseRotating == m_mouseState)
        m_mousePos = mousePos;
}

void Scatter3DController::wheelEvent(QWheelEvent *event)
{
    int zoomLevel = m_zoomLevel;
    if (zoomLevel > 100)
        zoomLevel += event->angleDelta().y() / 12;
    else if (zoomLevel > 50)
        zoomLevel += event->angleDelta().y() / 60;
    else
        zoomLevel += event->angleDelta().y() / 120;
    if (zoomLevel > 500)
        zoomLevel = 500;
    else if (zoomLevel < 10)
        zoomLevel = 10;

    setZoomLevel(zoomLevel);
}

void Scatter3DController::setDataProxy(QScatterDataProxy *proxy)
{
    delete m_data;
    m_data = proxy;

    QObject::connect(m_data, &QScatterDataProxy::arrayReset,
                     this, &Scatter3DController::handleArrayReset);
    QObject::connect(m_data, &QScatterDataProxy::itemsAdded,
                     this, &Scatter3DController::handleItemsAdded);
    QObject::connect(m_data, &QScatterDataProxy::itemsChanged,
                     this, &Scatter3DController::handleItemsChanged);
    QObject::connect(m_data, &QScatterDataProxy::itemsRemoved,
                     this, &Scatter3DController::handleItemsRemoved);
    QObject::connect(m_data, &QScatterDataProxy::itemsInserted,
                     this, &Scatter3DController::handleItemsInserted);

    adjustValueAxisRange();
    m_isDataDirty = true;
}

QScatterDataProxy *Scatter3DController::dataProxy()
{
    return m_data;
}

void Scatter3DController::handleArrayReset()
{
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Scatter3DController::handleItemsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Scatter3DController::handleItemsChanged(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Scatter3DController::handleItemsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Scatter3DController::handleItemsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Scatter3DController::handleAxisAutoAdjustRangeChangedInOrientation(
        QAbstractAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)
    adjustValueAxisRange();
}

void Scatter3DController::setObjectType(QDataVis::MeshStyle style, bool smooth)
{
    QString objFile;
    if (style == QDataVis::Spheres) {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/sphereSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/sphere");
    } else {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/dotSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/dot");
    }
    Abstract3DController::setMeshFileName(objFile);
}

void Scatter3DController::setSelectionMode(QDataVis::SelectionMode mode)
{
    if (mode > QDataVis::ModeItem) {
        qWarning("Unsupported selection mode.");
        return;
    }
    // Disable zoom if selection mode changes
    setSlicingActive(false);
    Abstract3DController::setSelectionMode(mode);
}

QPoint Scatter3DController::mousePosition()
{
    return m_mousePos;
}

void Scatter3DController::adjustValueAxisRange()
{
    if (m_data) {
        QVector3D limits = m_data->dptr()->limitValues();
        QValueAxis *valueAxis = static_cast<QValueAxis *>(m_axisX);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (limits.x() > 0)
                valueAxis->dptr()->setRange(-limits.x(), limits.x());
            else
                valueAxis->dptr()->setRange(-1.0, 1.0); // Only zero value values in data set, set range to default.
        }

        valueAxis = static_cast<QValueAxis *>(m_axisY);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (limits.y() > 0)
                valueAxis->dptr()->setRange(-limits.y(), limits.y());
            else
                valueAxis->dptr()->setRange(-1.0, 1.0); // Only zero value values in data set, set range to default.
        }

        valueAxis = static_cast<QValueAxis *>(m_axisZ);
        if (valueAxis && valueAxis->isAutoAdjustRange()) {
            if (limits.z() > 0)
                valueAxis->dptr()->setRange(-limits.z(), limits.z());
            else
                valueAxis->dptr()->setRange(-1.0, 1.0); // Only zero value values in data set, set range to default.
        }
    }
}

QT_DATAVIS3D_END_NAMESPACE
