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
#include "utils_p.h"
#include "qabstractaxis_p.h"
#include "qvalueaxis_p.h"
#include "qscatterdataproxy_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QThread>
#include <QDebug>
#include <qmath.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

Scatter3DController::Scatter3DController(QRect boundRect)
    : Abstract3DController(boundRect),
      m_isInitialized(false),
      m_mouseState(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_selectionMode(ModeItem),
      m_isSlicingActivated(false),
      m_objFile(QStringLiteral(":/defaultMeshes/sphere")),
      m_font(QFont(QStringLiteral("Arial"))),
      m_isGridEnabled(true),
      m_isBackgroundEnabled(true),
      m_renderer(0),
      m_data(0),
      m_valuesDirty(false)
{
    // Default axes
    setAxisX(new QValueAxis());
    setAxisY(new QValueAxis());
    setAxisZ(new QValueAxis());

    setDataProxy(new QScatterDataProxy);
}

Scatter3DController::~Scatter3DController()
{
    delete m_data;
}

void Scatter3DController::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (m_isInitialized)
        return;

    m_renderer = new Scatter3DRenderer(this);
    m_isInitialized = true;
}

void Scatter3DController::render(const GLuint defaultFboHandle)
{
    if (!m_isInitialized)
        return;

    // TODO do not give the entire data array, just the data window
    // TODO Would it be enough to just mutex cache update in renderer?
    // TODO --> Only if there is no need to store m_dataProxy for later, e.g. for string formatting
    // TODO Also, m_valuesDirty flag setting needs to be under same mutex
    QMutexLocker(m_data->mutex());
    m_renderer->render(m_data, m_valuesDirty, m_cameraHelper, defaultFboHandle);

    m_valuesDirty = false;

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
            if (event->pos().x() <= mainViewPort.width()
                    && event->pos().y() <= mainViewPort.height()) {
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
            m_mousePos = event->pos();
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
        m_mousePos = mousePos; //event->pos();
    }
    m_cameraHelper->updateMousePos(m_mousePos);
}

void Scatter3DController::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    if (Scatter3DController::MouseRotating == m_mouseState) {
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos; //event->pos();
        m_cameraHelper->updateMousePos(mousePos); //event->pos());
    }
    m_mouseState = Scatter3DController::MouseNone;
}

void Scatter3DController::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    if (Scatter3DController::MouseRotating == m_mouseState)
        m_mousePos = mousePos; //event->pos();
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
    m_valuesDirty = true;
}

QScatterDataProxy *Scatter3DController::dataProxy()
{
    return m_data;
}

void Scatter3DController::handleArrayReset()
{
    setSlicingActive(false);
    adjustValueAxisRange();
    m_valuesDirty = true;
}

void Scatter3DController::handleItemsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_valuesDirty = true;
}

void Scatter3DController::handleItemsChanged(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_valuesDirty = true;
}

void Scatter3DController::handleItemsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_valuesDirty = true;
}

void Scatter3DController::handleItemsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    adjustValueAxisRange();
    m_valuesDirty = true;
}

void Scatter3DController::handleAxisAutoAdjustRangeChanged(bool autoAdjust)
{
    Q_UNUSED(autoAdjust)
    adjustValueAxisRange();
}

QString Scatter3DController::objFile()
{
    return m_objFile;
}

void Scatter3DController::setObjectType(MeshStyle style, bool smooth)
{
    if (style == Spheres) {
        if (smooth)
            m_objFile = QStringLiteral(":/defaultMeshes/sphereSmooth");
        else
            m_objFile = QStringLiteral(":/defaultMeshes/sphere");
    } else {
        if (smooth)
            m_objFile = QStringLiteral(":/defaultMeshes/dotSmooth");
        else
            m_objFile = QStringLiteral(":/defaultMeshes/dot");
    }

    emit objFileChanged(m_objFile);
}

void Scatter3DController::setMeshFileName(const QString &objFileName)
{
    m_objFile = objFileName;

    emit objFileChanged(m_objFile);
}

void Scatter3DController::setSelectionMode(SelectionMode mode)
{
    if (mode > ModeItem) {
        qWarning("Unsupported selection mode.");
        return;
    }
    m_selectionMode = mode;
    // Disable zoom if selection mode changes
    setSlicingActive(false);
    emit selectionModeChanged(m_selectionMode);
}

QPoint Scatter3DController::mousePosition()
{
    return m_mousePos;
}

SelectionMode Scatter3DController::selectionMode()
{
    return m_selectionMode;
}

void Scatter3DController::setFontSize(float fontsize)
{
    m_font.setPointSizeF(fontsize);
    emit fontChanged(m_font);
}

float Scatter3DController::fontSize()
{
    return m_font.pointSizeF();
}

void Scatter3DController::setFont(const QFont &font)
{
    m_font = font;
    emit fontChanged(m_font);
}

QFont Scatter3DController::font()
{
    return m_font;
}

void Scatter3DController::setGridEnabled(bool enable)
{
    m_isGridEnabled = enable;
    emit gridEnabledChanged(m_isGridEnabled);
}

bool Scatter3DController::gridEnabled()
{
    return m_isGridEnabled;
}

void Scatter3DController::setBackgroundEnabled(bool enable)
{
    m_isBackgroundEnabled = enable;
    emit backgroundEnabledChanged(m_isBackgroundEnabled);
}

bool Scatter3DController::backgroundEnabled()
{
    return m_isBackgroundEnabled;
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
