/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "scatter3dcontroller_p.h"
#include "scatter3drenderer_p.h"
#include "camerahelper_p.h"
#include "utils_p.h"
#include "qabstractaxis_p.h"
#include "qvalueaxis.h"
#include "qcategoryaxis.h"
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
      m_selectionMode(ModeBar),
      m_isSlicingActivated(false),
      m_objFile(QStringLiteral(":/defaultMeshes/dot")),
      m_font(QFont(QStringLiteral("Arial"))),
      m_isGridEnabled(true),
      m_isBackgroundEnabled(true),
      m_tickCount(0),
      m_tickStep(0),
      m_tickMinimum(0.0f),
      m_renderer(0),
      m_data(0),
      m_valuesDirty(false)
{
    // Default axes. Only Y axis can actually be changed by user.
    setAxisX(new QCategoryAxis());
    setAxisY(new QValueAxis());
    setAxisZ(new QCategoryAxis());

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
    if (m_axisX)
        m_axisX->d_ptr->setDrawer(m_renderer->drawer());
    if (m_axisY)
        m_axisY->d_ptr->setDrawer(m_renderer->drawer());
    if (m_axisZ)
        m_axisZ->d_ptr->setDrawer(m_renderer->drawer());
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
    m_renderer->render(m_data, m_valuesDirty, m_cameraHelper, m_axisX->d_ptr->titleItem(),
                       m_axisY->d_ptr->titleItem(), m_axisZ->d_ptr->titleItem(), defaultFboHandle);

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

// TODO: abstract renderer should have accessor for Drawer instead
Drawer *Scatter3DController::drawer()
{
    if (m_renderer)
        return m_renderer->drawer();
    else
        return 0;
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

    // emit something? Renderer might be interested?
}

QScatterDataProxy *Scatter3DController::dataProxy()
{
    return m_data;
}

void Scatter3DController::handleArrayReset()
{
    setSlicingActive(false);
    handleLimitChange();
    m_valuesDirty = true;
}

void Scatter3DController::handleItemsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    handleLimitChange();
    m_valuesDirty = true;
}

void Scatter3DController::handleItemsChanged(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    handleLimitChange();
    m_valuesDirty = true;
}

void Scatter3DController::handleItemsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    handleLimitChange();
    m_valuesDirty = true;
}

void Scatter3DController::handleItemsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO should dirty only affected values?
    handleLimitChange();
    m_valuesDirty = true;
}

QString Scatter3DController::objFile()
{
    return m_objFile;
}

void Scatter3DController::setBarType(BarStyle style, bool smooth)
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

void Scatter3DController::setTickCount(GLint tickCount, GLfloat step, GLfloat minimum)
{
    if (tickCount <= 0) {
        qCritical("Invalid tick count. It must be positive.");
        return;
    }
    m_tickCount = tickCount;
    m_tickStep = step;
    m_tickMinimum = minimum;
    emit tickCountChanged(m_tickCount, m_tickStep, m_tickMinimum);
}

void Scatter3DController::handleLimitChange()
{
    QPair<GLfloat, GLfloat> limits = m_data->dptr()->limitValues();
    // TODO do something with changed limits?
    //m_heightNormalizer = (GLfloat)qMax(qFabs(limits.second), qFabs(limits.first));
    //calculateHeightAdjustment(limits);

    emit limitsChanged(limits);
}

QT_DATAVIS3D_END_NAMESPACE
