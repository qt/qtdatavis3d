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

Bars3DController::Bars3DController(QRect boundRect)
    : Abstract3DController(boundRect),
      m_rowCount(10),
      m_columnCount(10),
      m_mouseState(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_isSlicingActivated(false),
      m_selectedBarPos(noSelectionPoint()),
      m_isBarSpecRelative(true),
      m_barThicknessRatio(1.0f),
      m_barSpacing(QSizeF(1.0, 1.0)),
      m_renderer(0)
{
    // Default bar type; specific to bars
    setBarType(QDataVis::Bars, false);

    setActiveDataProxy(0);

    // Setting a null axis creates a new default axis according to orientation and chart type.
    // Note: These cannot be set in Abstract3DController constructor, as they will call virtual
    //       functions implemented by subclasses.
    setAxisX(0);
    setAxisY(0);
    setAxisZ(0);
}

Bars3DController::~Bars3DController()
{
}

void Bars3DController::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (isInitialized())
        return;

    m_renderer = new Bars3DRenderer(this);
    setRenderer(m_renderer);
    synchDataToRenderer();

    QObject::connect(m_renderer, &Bars3DRenderer::selectedBarPosChanged, this,
                     &Bars3DController::handleSelectedBarPosChanged, Qt::QueuedConnection);
}

void Bars3DController::synchDataToRenderer()
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
        m_renderer->updateBarSpecs(m_barThicknessRatio, m_barSpacing, m_isBarSpecRelative);
        m_changeTracker.barSpecsChanged = false;
    }

    if (m_changeTracker.selectedBarPosChanged) {
        m_renderer->updateSelectedBarPos(m_selectedBarPos);
        m_changeTracker.selectedBarPosChanged = false;
    }

    if (m_isDataDirty) {
        m_renderer->updateDataModel(static_cast<QBarDataProxy *>(m_data));
        m_isDataDirty = false;
    }
}

QMatrix4x4 Bars3DController::calculateViewMatrix(int zoom, int viewPortWidth,
                                                 int viewPortHeight, bool showUnder)
{
    return m_cameraHelper->calculateViewMatrix(m_mousePos,
                                               zoom,
                                               viewPortWidth,
                                               viewPortHeight,
                                               showUnder);
}

bool Bars3DController::isSlicingActive()
{
    return m_isSlicingActivated;
}

void Bars3DController::setSlicingActive(bool isSlicing)
{
    m_isSlicingActivated = isSlicing;

    m_changeTracker.slicingActiveChanged = true;
    emit slicingActiveChanged(m_isSlicingActivated);
}

Bars3DController::MouseState Bars3DController::mouseState()
{
    return m_mouseState;
}


#if defined(Q_OS_ANDROID)
void Bars3DController::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!m_isSlicingActivated) {
        m_mouseState = Bars3DController::MouseOnScene;
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = event->pos();
    }
}

void Bars3DController::touchEvent(QTouchEvent *event)
{
    static int prevDistance = 0;

    QList<QTouchEvent::TouchPoint> points;
    points = event->touchPoints();

    if (!m_isSlicingActivated && points.count() == 2) {
        m_mouseState = Bars3DController::MouseOnPinch;

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

void Bars3DController::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    QRect mainViewPort = m_renderer->mainViewPort();
    if (Qt::LeftButton == event->button()) {
        if (m_isSlicingActivated) {
            if (mousePos.x() <= mainViewPort.width()
                    && mousePos.y() <= mainViewPort.height()) {
                m_mouseState = Bars3DController::MouseOnOverview;
                //qDebug() << "Mouse pressed on overview";
            } else {
                m_mouseState = Bars3DController::MouseOnZoom;
                //qDebug() << "Mouse pressed on zoom";
            }
        } else {
#if !defined(Q_OS_ANDROID)
            m_mouseState = Bars3DController::MouseOnScene;
#else
            m_mouseState = Bars3DController::MouseRotating;
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
        m_mouseState = Bars3DController::MouseRotating;
#else
        m_mouseState = Bars3DController::MouseOnScene;
#endif
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos;
    }
    m_cameraHelper->updateMousePos(m_mousePos);
}

void Bars3DController::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    if (Bars3DController::MouseRotating == m_mouseState) {
        // update mouse positions to prevent jumping when releasing or repressing a button
        m_mousePos = mousePos;
        m_cameraHelper->updateMousePos(mousePos);
    }
    m_mouseState = Bars3DController::MouseNone;
}

void Bars3DController::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event);
    if (Bars3DController::MouseRotating == m_mouseState)
        m_mousePos = mousePos;
}

void Bars3DController::wheelEvent(QWheelEvent *event)
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

void Bars3DController::setActiveDataProxy(QAbstractDataProxy *proxy)
{
    // Setting null proxy indicates default proxy
    if (!proxy) {
        proxy = new QBarDataProxy;
        proxy->d_ptr->setDefaultProxy(true);
    }

    Q_ASSERT(proxy->type() == QAbstractDataProxy::DataTypeBar);

    Abstract3DController::setActiveDataProxy(proxy);

    QBarDataProxy *barDataProxy = static_cast<QBarDataProxy *>(m_data);

    QObject::connect(barDataProxy, &QBarDataProxy::arrayReset, this,
                     &Bars3DController::handleArrayReset);
    QObject::connect(barDataProxy, &QBarDataProxy::rowsAdded, this,
                     &Bars3DController::handleRowsAdded);
    QObject::connect(barDataProxy, &QBarDataProxy::rowsChanged, this,
                     &Bars3DController::handleRowsChanged);
    QObject::connect(barDataProxy, &QBarDataProxy::rowsRemoved, this,
                     &Bars3DController::handleRowsRemoved);
    QObject::connect(barDataProxy, &QBarDataProxy::rowsInserted, this,
                     &Bars3DController::handleRowsInserted);
    QObject::connect(barDataProxy, &QBarDataProxy::itemChanged, this,
                     &Bars3DController::handleItemChanged);

    adjustValueAxisRange();
    m_isDataDirty = true;
    setSelectedBarPos(noSelectionPoint());
}

void Bars3DController::handleArrayReset()
{
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
    setSelectedBarPos(noSelectionPoint());
}

void Bars3DController::handleRowsAdded(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Bars3DController::handleRowsChanged(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Bars3DController::handleRowsRemoved(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
    // TODO this will break once data window offset is implemented
    if (startIndex >= static_cast<QBarDataProxy *>(m_data)->rowCount())
        setSelectedBarPos(noSelectionPoint());
}

void Bars3DController::handleRowsInserted(int startIndex, int count)
{
    Q_UNUSED(startIndex)
    Q_UNUSED(count)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Bars3DController::handleItemChanged(int rowIndex, int columnIndex)
{
    Q_UNUSED(rowIndex)
    Q_UNUSED(columnIndex)
    // TODO check if affects data window
    // TODO should update slice instead of deactivating?
    setSlicingActive(false);
    adjustValueAxisRange();
    m_isDataDirty = true;
}

void Bars3DController::handleSelectedBarPosChanged(const QPoint &position)
{
    QPoint pos = position;
    if (pos == QPoint(255, 255))
        pos = noSelectionPoint();
    if (pos != m_selectedBarPos) {
        m_selectedBarPos = pos;
        emit selectedBarPosChanged(pos);
    }
}

void Bars3DController::handleAxisAutoAdjustRangeChangedInOrientation(
        QAbstractAxis::AxisOrientation orientation, bool autoAdjust)
{
    Q_UNUSED(orientation)
    Q_UNUSED(autoAdjust)
    adjustValueAxisRange();
}

QPoint Bars3DController::noSelectionPoint()
{
    static QPoint noSelectionPos(-1, -1);
    return noSelectionPos;
}

void Bars3DController::setBarSpecs(GLfloat thicknessRatio, const QSizeF &spacing, bool relative)
{
    m_barThicknessRatio = thicknessRatio;
    m_barSpacing        = spacing;
    m_isBarSpecRelative = relative;

    m_changeTracker.barSpecsChanged = true;
    emit barSpecsChanged(thicknessRatio, spacing, relative);
}

GLfloat Bars3DController::barThickness()
{
    return m_barThicknessRatio;
}

QSizeF Bars3DController::barSpacing()
{
    return m_barSpacing;
}

bool Bars3DController::isBarSpecRelative()
{
    return m_isBarSpecRelative;
}

void Bars3DController::setBarType(QDataVis::MeshStyle style, bool smooth)
{
    QString objFile;
    if (style == QDataVis::Bars)
        objFile = QStringLiteral(":/defaultMeshes/bar");
    else if (style == QDataVis::Pyramids)
        objFile = QStringLiteral(":/defaultMeshes/pyramid");
    else if (style == QDataVis::Cones)
        objFile = QStringLiteral(":/defaultMeshes/cone");
    else if (style == QDataVis::Cylinders)
        objFile = QStringLiteral(":/defaultMeshes/cylinder");
    else if (style == QDataVis::BevelBars)
        objFile = QStringLiteral(":/defaultMeshes/bevelbar");

    if (smooth)
        objFile += QStringLiteral("Smooth");

    Abstract3DController::setMeshFileName(objFile);
}

// TODO: This sets data window. Needs more parameters, now assumes window always starts at 0,0.
void Bars3DController::setDataWindow(int rowCount, int columnCount)
{
    // Disable zoom mode if we're in it (causes crash if not, as zoom selection is deleted)
    setSlicingActive(false);

    m_rowCount = rowCount;
    m_columnCount = columnCount;

    adjustValueAxisRange();

    if (m_selectedBarPos.x() >= rowCount || m_selectedBarPos.y() >= columnCount)
        setSelectedBarPos(noSelectionPoint());

    m_changeTracker.sampleSpaceChanged = true;
    m_isDataDirty = true; // Render item array is recreated in renderer
    emit sampleSpaceChanged(rowCount, columnCount);
}

void Bars3DController::setSelectionMode(QDataVis::SelectionMode mode)
{
    // Disable zoom if selection mode changes
    setSlicingActive(false);
    Abstract3DController::setSelectionMode(mode);
}

void Bars3DController::setSelectedBarPos(const QPoint &position)
{
    // TODO this will break once data window offset is implemented
    QPoint pos = position;
    if (pos.x() < 0 || pos.y() < 0
            || pos.x() >= static_cast<QBarDataProxy *>(m_data)->rowCount()
            || pos.y() >= static_cast<QBarDataProxy *>(m_data)->rowAt(pos.x())->size()) {
        pos = noSelectionPoint();
    }

    if (pos != m_selectedBarPos) {
        m_selectedBarPos = pos;
        m_changeTracker.selectedBarPosChanged = true;
        emit selectedBarPosChanged(pos);
    }
}

QPoint Bars3DController::selectedBarPos() const
{
    return m_selectedBarPos;
}

QPoint Bars3DController::mousePosition()
{
    return m_mousePos;
}

int Bars3DController::columnCount()
{
    return m_columnCount;
}

int Bars3DController::rowCount()
{
    return m_rowCount;
}

void Bars3DController::adjustValueAxisRange()
{
    QValueAxis *valueAxis = static_cast<QValueAxis *>(m_axisY);
    if (valueAxis && valueAxis->isAutoAdjustRange() && m_data) {
        QPair<GLfloat, GLfloat> limits =
                static_cast<QBarDataProxy *>(m_data)->dptr()->limitValues(0, m_rowCount,
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

QAbstractAxis *Bars3DController::createDefaultAxis(QAbstractAxis::AxisOrientation orientation)
{
    QAbstractAxis *defaultAxis = 0;

    if (orientation == QAbstractAxis::AxisOrientationY)
        defaultAxis = createDefaultValueAxis();
    else
        defaultAxis = createDefaultCategoryAxis();

    return defaultAxis;
}

QT_DATAVIS3D_END_NAMESPACE
