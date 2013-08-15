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

#include "bars3dcontroller_p.h"
#include "bars3drenderer_p.h"
#include "camerahelper_p.h"
#include "qabstractaxis_p.h"
#include "qvalueaxis_p.h"
#include "qcategoryaxis.h"
#include "qbardataproxy_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <qmath.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

Bars3dController::Bars3dController(QRect boundRect)
    : Abstract3DController(boundRect),
      m_rowCount(0),
      m_columnCount(0),
      m_mouseState(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_isSlicingActivated(false),
      m_isBarSpecRelative(true),
      m_barThickness(QSizeF(0.75f, 0.75f)),
      m_barSpacing(m_barThickness * 3.0f),
      m_renderer(0),
      m_data(0)
{
    // Default axes
    setAxisX(new QCategoryAxis());
    setAxisY(new QValueAxis());
    setAxisZ(new QCategoryAxis());

    setBarType(QDataVis::Bars, false); // default object type

    setDataProxy(new QBarDataProxy);
}

Bars3dController::~Bars3dController()
{
    delete m_data;
}

void Bars3dController::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (isInitialized())
        return;

    m_renderer = new Bars3dRenderer(this);
    setRenderer(m_renderer);
}

void Bars3dController::synchDataToRenderer()
{
    Abstract3DController::synchDataToRenderer();

    if (!isInitialized())
        return;

    // Notify changes to renderer
    if (m_changeTracker.slicingActiveChanged) {
        m_renderer->updateSlicingActive(m_isSlicingActivated);
        m_changeTracker.slicingActiveChanged = false;
    }

    if (m_changeTracker.sampleSpaceChanged) {
        m_renderer->updateSampleSpace(m_rowCount, m_columnCount);
        m_changeTracker.sampleSpaceChanged = false;
    }

    if (m_changeTracker.barSpecsChanged) {
        m_renderer->updateBarSpecs(m_barThickness, m_barSpacing, m_isBarSpecRelative);
        m_changeTracker.barSpecsChanged = false;
    }

    if (m_changeTracker.zoomLevelChanged) {
        m_renderer->updateZoomLevel(m_zoomLevel);
        m_changeTracker.zoomLevelChanged = false;
    }

    if (m_isDataDirty) {
        m_renderer->updateDataModel(m_data);
        m_isDataDirty = false;
    }
}

QMatrix4x4 Bars3dController::calculateViewMatrix(int zoom, int viewPortWidth,
                                                 int viewPortHeight, bool showUnder)
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

    m_changeTracker.slicingActiveChanged = true;
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

    if (!m_isSlicingActivated && points.count() == 2) {
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
            if (mousePos.x() <= mainViewPort.width()
                    && mousePos.y() <= mainViewPort.height()) {
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
            m_mousePos = mousePos;
            //qDebug() << "Mouse pressed on scene";
        }
    } else if (Qt::MiddleButton == event->button()) {
        // reset rotations
        m_mousePos = QPoint(0, 0);
    } else if (!m_isSlicingActivated && Qt::RightButton == event->button()) {
        // disable rotating when in slice view
#if !defined(Q_OS_ANDROID)
        m_mouseState = Bars3dController::MouseRotating;
#else
        m_mouseState = Bars3dController::MouseOnScene;
#endif
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos;
    }
    m_cameraHelper->updateMousePos(m_mousePos);
}

void Bars3dController::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    if (Bars3dController::MouseRotating == m_mouseState) {
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos;
        m_cameraHelper->updateMousePos(mousePos);
    }
    m_mouseState = Bars3dController::MouseNone;
}

void Bars3dController::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    if (Bars3dController::MouseRotating == m_mouseState)
        m_mousePos = mousePos;
}

void Bars3dController::wheelEvent(QWheelEvent *event)
{
    // disable zooming if in slice view
    if (m_isSlicingActivated)
        return;

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

void Bars3dController::setDataProxy(QBarDataProxy *proxy)
{
    delete m_data;
    m_data = proxy;

    QObject::connect(m_data, &QBarDataProxy::arrayReset, this,
                     &Bars3dController::handleArrayReset);
    QObject::connect(m_data, &QBarDataProxy::rowsAdded, this,
                     &Bars3dController::handleRowsAdded);
    QObject::connect(m_data, &QBarDataProxy::rowsChanged, this,
                     &Bars3dController::handleRowsChanged);
    QObject::connect(m_data, &QBarDataProxy::rowsRemoved, this,
                     &Bars3dController::handleRowsRemoved);
    QObject::connect(m_data, &QBarDataProxy::rowsInserted, this,
                     &Bars3dController::handleRowsInserted);
    QObject::connect(m_data, &QBarDataProxy::itemChanged, this,
                     &Bars3dController::handleItemChanged);

    adjustValueAxisRange();
    m_isDataDirty = true;
}

QBarDataProxy *Bars3dController::dataProxy()
{
    return m_data;
}

void Bars3dController::handleArrayReset()
{
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Bars3dController::handleRowsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Bars3dController::handleRowsChanged(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Bars3dController::handleRowsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Bars3dController::handleRowsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Bars3dController::handleItemChanged(int rowIndex, int columnIndex)
{
    Q_UNUSED(rowIndex)
    Q_UNUSED(columnIndex)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Bars3dController::handleAxisAutoAdjustRangeChangedInOrientation(QAbstractAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)
    adjustValueAxisRange();
}

void Bars3dController::setBarSpecs(QSizeF thickness, QSizeF spacing, bool relative)
{
    m_barThickness      = thickness;
    m_barSpacing        = spacing;
    m_isBarSpecRelative = relative;

    m_changeTracker.barSpecsChanged = true;
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

void Bars3dController::setBarType(QDataVis::MeshStyle style, bool smooth)
{
    QString objFile;
    if (style == QDataVis::Bars) {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/barSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/bar");
    } else if (style == QDataVis::Pyramids) {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/pyramidSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/pyramid");
    } else if (style == QDataVis::Cones) {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/coneSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/cone");
    } else if (style == QDataVis::Cylinders) {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/cylinderSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/cylinder");
    } else if (style == QDataVis::BevelBars) {
        if (smooth)
            objFile = QStringLiteral(":/defaultMeshes/bevelbarSmooth");
        else
            objFile = QStringLiteral(":/defaultMeshes/bevelbar");
    }
    Abstract3DController::setMeshFileName(objFile);
}

// TODO: This sets data window. Needs more parameters, now assumes window always starts at 0,0.
void Bars3dController::setupSampleSpace(int rowCount, int columnCount)
{
    // Disable zoom mode if we're in it (causes crash if not, as zoom selection is deleted)
    setSlicingActive(false);

    m_rowCount = rowCount;
    m_columnCount = columnCount;

    adjustValueAxisRange();

    m_changeTracker.sampleSpaceChanged = true;
    emit sampleSpaceChanged(rowCount, columnCount);
}

void Bars3dController::setSelectionMode(QDataVis::SelectionMode mode)
{
    // Disable zoom if selection mode changes
    setSlicingActive(false);
    Abstract3DController::setSelectionMode(mode);
}

QPoint Bars3dController::mousePosition()
{
    return m_mousePos;
}

int Bars3dController::columnCount()
{
    return m_columnCount;
}

int Bars3dController::rowCount()
{
    return m_rowCount;
}

void Bars3dController::adjustValueAxisRange()
{
    QValueAxis *valueAxis = static_cast<QValueAxis *>(m_axisY);
    if (valueAxis && valueAxis->isAutoAdjustRange() && m_data) {
        QPair<GLfloat, GLfloat> limits = m_data->dptr()->limitValues(0, m_rowCount,
                                                                     0, m_columnCount);
        if (limits.first < 0) {
            // TODO: Currently we only support symmetric y-axis for bar chart if there are negative values
            qreal maxAbs = qMax(qFabs(limits.first), qFabs(limits.second));
            // Call private implementation to avoid unsetting auto adjust flag
            valueAxis->dptr()->setRange(-maxAbs, maxAbs);
        } else if (limits.second == 0.0) {
            valueAxis->dptr()->setRange(0.0, 1.0); // Only zero value values in data set, set range to something.
        } else {
            valueAxis->dptr()->setRange(0.0, limits.second);
        }
    }
}

QT_DATAVIS3D_END_NAMESPACE
