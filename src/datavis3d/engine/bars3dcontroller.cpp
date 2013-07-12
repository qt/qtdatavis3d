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

#include "bars3dcontroller_p.h"
#include "bars3drenderer_p.h"
#include "camerahelper_p.h"
#include "utils_p.h"
#include "qabstractaxis_p.h"
#include "qvalueaxis.h"
#include "qcategoryaxis.h"
#include "qbardataproxy_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QThread>
#include <QDebug>
#include <qmath.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

Bars3dController::Bars3dController(QRect boundRect)
    : Abstract3DController(boundRect),
      m_isInitialized(false),
      m_rowCount(0),
      m_columnCount(0),
      m_mouseState(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_selectionMode(ModeBar),
      m_isSlicingActivated(false),
      m_isBarSpecRelative(true),
      m_barThickness(QSizeF(0.75f, 0.75f)),
      m_barSpacing(m_barThickness * 3.0f),
      m_objFile(QStringLiteral(":/defaultMeshes/bar")),
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

    setDataProxy(new QBarDataProxy);
}

Bars3dController::~Bars3dController()
{
    delete m_data;
}

void Bars3dController::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (m_isInitialized)
        return;

    m_renderer = new Bars3dRenderer(this);
    if (m_axisX)
        m_axisX->d_ptr->setDrawer(m_renderer->drawer());
    if (m_axisY)
        m_axisY->d_ptr->setDrawer(m_renderer->drawer());
    if (m_axisZ)
        m_axisZ->d_ptr->setDrawer(m_renderer->drawer());
    m_isInitialized = true;
}

void Bars3dController::render(const GLuint defaultFboHandle)
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

QMatrix4x4 Bars3dController::calculateViewMatrix(int zoom, int viewPortWidth, int viewPortHeight, bool showUnder)
{
    return m_cameraHelper->calculateViewMatrix(m_mousePos,
                                               zoom,
                                               viewPortWidth,
                                               viewPortHeight,
                                               showUnder);
}

bool Bars3dController::isSlicingActive()
{
    return m_isSlicingActivated;
}

void Bars3dController::setSlicingActive(bool isSlicing)
{
    m_isSlicingActivated = isSlicing;
    emit slicingActiveChanged(m_isSlicingActivated);
}

Bars3dController::MouseState Bars3dController::mouseState()
{
    return m_mouseState;
}


#if defined(Q_OS_ANDROID)
void Bars3dController::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!m_isSlicingActivated) {
        m_mouseState = Bars3dController::MouseOnScene;
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = event->pos();
    }
}

void Bars3dController::touchEvent(QTouchEvent *event)
{
    static int prevDistance = 0;

    QList<QTouchEvent::TouchPoint> points;
    points = event->touchPoints();

    if (points.count() == 2) {
        m_mouseState = Bars3dController::MouseOnPinch;

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

void Bars3dController::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    QRect mainViewPort = m_renderer->mainViewPort();
    if (Qt::LeftButton == event->button()) {
        if (m_isSlicingActivated) {
            if (event->pos().x() <= mainViewPort.width()
                    && event->pos().y() <= mainViewPort.height()) {
                m_mouseState = Bars3dController::MouseOnOverview;
                //qDebug() << "Mouse pressed on overview";
            } else {
                m_mouseState = Bars3dController::MouseOnZoom;
                //qDebug() << "Mouse pressed on zoom";
            }
        } else {
#if !defined(Q_OS_ANDROID)
            m_mouseState = Bars3dController::MouseOnScene;
#else
            m_mouseState = Bars3dController::MouseRotating;
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
        m_mouseState = Bars3dController::MouseRotating;
#else
        m_mouseState = Bars3dController::MouseOnScene;
#endif
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos; //event->pos();
    }
    m_cameraHelper->updateMousePos(m_mousePos);
}

void Bars3dController::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    if (Bars3dController::MouseRotating == m_mouseState) {
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos; //event->pos();
        m_cameraHelper->updateMousePos(mousePos); //event->pos());
    }
    m_mouseState = Bars3dController::MouseNone;
}

void Bars3dController::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    if (Bars3dController::MouseRotating == m_mouseState)
        m_mousePos = mousePos; //event->pos();
}

void Bars3dController::wheelEvent(QWheelEvent *event)
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
Drawer *Bars3dController::drawer()
{
    if (m_renderer)
        return m_renderer->drawer();
    else
        return 0;
}

void Bars3dController::setDataProxy(QBarDataProxy *proxy)
{
    delete m_data;
    m_data = proxy;

    QObject::connect(m_data, &QBarDataProxy::arrayReset, this, &Bars3dController::handleArrayReset);
    QObject::connect(m_data, &QBarDataProxy::rowsAdded, this, &Bars3dController::handleRowsAdded);
    QObject::connect(m_data, &QBarDataProxy::rowsChanged, this, &Bars3dController::handleRowsChanged);
    QObject::connect(m_data, &QBarDataProxy::rowsRemoved, this, &Bars3dController::handleRowsRemoved);
    QObject::connect(m_data, &QBarDataProxy::rowsInserted, this, &Bars3dController::handleRowsInserted);

    // emit something? Renderer might be interested?
}

QBarDataProxy *Bars3dController::dataProxy()
{
    return m_data;
}

void Bars3dController::handleArrayReset()
{
    setSlicingActive(false);
    handleLimitChange();
    m_valuesDirty = true;
}

void Bars3dController::handleRowsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    handleLimitChange();
    m_valuesDirty = true;
}

void Bars3dController::handleRowsChanged(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    handleLimitChange();
    m_valuesDirty = true;
}

void Bars3dController::handleRowsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    handleLimitChange();
    m_valuesDirty = true;
}

void Bars3dController::handleRowsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    handleLimitChange();
    m_valuesDirty = true;
}

void Bars3dController::setBarSpecs(QSizeF thickness, QSizeF spacing, bool relative)
{
    m_barThickness      = thickness;
    m_barSpacing        = spacing;
    m_isBarSpecRelative = relative;
    emit barSpecsChanged(thickness, spacing, relative);
}

QSizeF Bars3dController::barThickness()
{
    return m_barThickness;
}

QSizeF Bars3dController::barSpacing()
{
    return m_barSpacing;
}

bool Bars3dController::isBarSpecRelative()
{
    return m_isBarSpecRelative;
}

QString Bars3dController::objFile()
{
    return m_objFile;
}

QPair<GLfloat, GLfloat> Bars3dController::limits()
{
    if (m_data)
        return m_data->dptr()->limitValues(0, m_rowCount, 0, m_columnCount);

    return qMakePair(0.f, 0.f);
}

void Bars3dController::setBarType(BarStyle style, bool smooth)
{
    if (style == Bars) {
        if (smooth)
            m_objFile = QStringLiteral(":/defaultMeshes/barSmooth");
        else
            m_objFile = QStringLiteral(":/defaultMeshes/bar");
    } else if (style == Pyramids) {
        if (smooth)
            m_objFile = QStringLiteral(":/defaultMeshes/pyramidSmooth");
        else
            m_objFile = QStringLiteral(":/defaultMeshes/pyramid");
    } else if (style == Cones) {
        if (smooth)
            m_objFile = QStringLiteral(":/defaultMeshes/coneSmooth");
        else
            m_objFile = QStringLiteral(":/defaultMeshes/cone");
    } else if (style == Cylinders) {
        if (smooth)
            m_objFile = QStringLiteral(":/defaultMeshes/cylinderSmooth");
        else
            m_objFile = QStringLiteral(":/defaultMeshes/cylinder");
    } else if (style == BevelBars) {
        if (smooth)
            m_objFile = QStringLiteral(":/defaultMeshes/bevelbarSmooth");
        else
            m_objFile = QStringLiteral(":/defaultMeshes/bevelbar");
    }

    emit objFileChanged(m_objFile);
}

void Bars3dController::setMeshFileName(const QString &objFileName)
{
    m_objFile = objFileName;

    emit objFileChanged(m_objFile);
}

// TODO: This sets data window. Needs more parameters, now assumes window always starts at 0,0.
void Bars3dController::setupSampleSpace(int rowCount, int columnCount)
{
    // Disable zoom mode if we're in it (causes crash if not, as zoom selection is deleted)
    setSlicingActive(false);

    m_rowCount = rowCount;
    m_columnCount = columnCount;

    emit sampleSpaceChanged(rowCount, columnCount);
}


void Bars3dController::setSelectionMode(SelectionMode mode)
{
    m_selectionMode = mode;
    // Disable zoom if selection mode changes
    setSlicingActive(false);
    emit selectionModeChanged(m_selectionMode);
}

QPoint Bars3dController::mousePosition()
{
    return m_mousePos;
}

SelectionMode Bars3dController::selectionMode()
{
    return m_selectionMode;
}

void Bars3dController::setFontSize(float fontsize)
{
    m_font.setPointSizeF(fontsize);
    emit fontChanged(m_font);
}

float Bars3dController::fontSize()
{
    return m_font.pointSizeF();
}

void Bars3dController::setFont(const QFont &font)
{
    m_font = font;
    emit fontChanged(m_font);
}

QFont Bars3dController::font()
{
    return m_font;
}

void Bars3dController::setGridEnabled(bool enable)
{
    m_isGridEnabled = enable;
    emit gridEnabledChanged(m_isGridEnabled);
}

bool Bars3dController::gridEnabled()
{
    return m_isGridEnabled;
}

void Bars3dController::setBackgroundEnabled(bool enable)
{
    m_isBackgroundEnabled = enable;
    emit backgroundEnabledChanged(m_isBackgroundEnabled);
}

bool Bars3dController::backgroundEnabled()
{
    return m_isBackgroundEnabled;
}

void Bars3dController::setTickCount(GLint tickCount, GLfloat step, GLfloat minimum)
{
    m_tickCount   = tickCount;
    m_tickStep    = step;
    m_tickMinimum = minimum;
    emit tickCountChanged(m_tickCount, m_tickStep, m_tickMinimum);
}

int Bars3dController::columnCount()
{
    return m_columnCount;
}

int Bars3dController::rowCount()
{
    return m_rowCount;
}

void Bars3dController::handleLimitChange()
{
    // Get the limits for data window
    QPair<GLfloat, GLfloat> limits = m_data->dptr()->limitValues(0, m_rowCount, 0, m_columnCount);

    emit limitsChanged(limits);
}

QT_DATAVIS3D_END_NAMESPACE
