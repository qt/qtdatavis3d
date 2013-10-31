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

#include "abstract3dcontroller_p.h"
#include "camerahelper_p.h"
#include "q3dabstractaxis_p.h"
#include "q3dvalueaxis.h"
#include "q3dcategoryaxis.h"
#include "abstract3drenderer_p.h"
#include "q3dcamera.h"
#include "q3dlight.h"
#include "qabstractdataproxy_p.h"
#include "qabstract3dinputhandler_p.h"
#include "qtouch3dinputhandler.h"

#include <QThread>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Abstract3DController::Abstract3DController(QRect boundRect, QObject *parent) :
    QObject(parent),
    m_boundingRect(boundRect.x(), boundRect.y(), boundRect.width(), boundRect.height()),
    m_theme(),
    m_font(QFont(QStringLiteral("Arial"))),
    m_selectionMode(QDataVis::SelectionItem),
    m_shadowQuality(QDataVis::ShadowQualityMedium),
    m_labelStyle(QDataVis::LabelStyleTransparent),
    m_isBackgroundEnabled(true),
    m_isGridEnabled(true),
    m_scene(new Q3DScene()),
    m_activeInputHandler(0),
    m_axisX(0),
    m_axisY(0),
    m_axisZ(0),
    m_renderer(0),
    m_isDataDirty(true),
    m_data(0),
    m_renderPending(false)
{
    m_theme.useTheme(QDataVis::ThemeQt);

    // Populate the scene
    m_scene->activeLight()->setPosition(defaultLightPos);

    // Create initial default input handler
    QAbstract3DInputHandler *inputHandler;
    inputHandler = new QTouch3DInputHandler();
    inputHandler->d_ptr->m_isDefaultHandler = true;
    setActiveInputHandler(inputHandler);
    connect(inputHandler, &QAbstract3DInputHandler::inputStateChanged, this,
            &Abstract3DController::handleInputStateChanged);
    connect(inputHandler, &QAbstract3DInputHandler::positionChanged, this,
            &Abstract3DController::handleInputPositionChanged);
    connect(m_scene, &Q3DScene::needRender, this,
            &Abstract3DController::emitNeedRender);
}

Abstract3DController::~Abstract3DController()
{
    // Renderer can be in another thread, don't delete it directly in that case
    if (m_renderer && m_renderer->thread() != QThread::currentThread())
        m_renderer->deleteLater();
    else
        delete m_renderer;
    delete m_scene;
}

void Abstract3DController::setRenderer(Abstract3DRenderer *renderer)
{
    m_renderer = renderer;
}

void Abstract3DController::synchDataToRenderer()
{
    // If we don't have a renderer, don't do anything
    if (!m_renderer)
        return;

    if (m_changeTracker.boundingRectChanged || m_changeTracker.sizeChanged) {
        m_renderer->updateBoundingRect(m_boundingRect);
        m_changeTracker.boundingRectChanged = false;
        m_changeTracker.sizeChanged = false;
    }

    if (m_changeTracker.positionChanged) {
        m_renderer->updatePosition(m_boundingRect);
        m_changeTracker.positionChanged = false;
    }

    m_renderer->updateScene(m_scene);

    if (m_changeTracker.inputPositionChanged) {
        m_renderer->updateInputPosition(inputPosition());
        m_changeTracker.inputPositionChanged = false;
    }

    if (m_changeTracker.inputStateChanged) {
        m_renderer->updateInputState(inputState());
        m_changeTracker.inputStateChanged = false;
    }

    if (m_changeTracker.themeChanged) {
        m_renderer->updateTheme(m_theme);
        m_changeTracker.themeChanged = false;
    }

    if (m_changeTracker.fontChanged) {
        m_renderer->updateFont(m_font);
        m_changeTracker.fontChanged = false;
    }

    if (m_changeTracker.labelStyleChanged) {
        m_renderer->updateLabelStyle(m_labelStyle);
        m_changeTracker.labelStyleChanged = false;
    }

    if (m_changeTracker.shadowQualityChanged) {
        m_renderer->updateShadowQuality(m_shadowQuality);
        m_changeTracker.shadowQualityChanged = false;
    }

    if (m_changeTracker.selectionModeChanged) {
        m_renderer->updateSelectionMode(m_selectionMode);
        m_changeTracker.selectionModeChanged = false;
    }

    if (m_changeTracker.objFileChanged) {
        m_renderer->updateMeshFileName(m_objFile);
        m_changeTracker.objFileChanged = false;
    }

    if (m_changeTracker.gridEnabledChanged) {
        m_renderer->updateGridEnabled(m_isGridEnabled);
        m_changeTracker.gridEnabledChanged = false;
    }

    if (m_changeTracker.backgroundEnabledChanged) {
        m_renderer->updateBackgroundEnabled(m_isBackgroundEnabled);
        m_changeTracker.backgroundEnabledChanged = false;
    }

    if (m_changeTracker.axisXTypeChanged) {
        m_renderer->updateAxisType(Q3DAbstractAxis::AxisOrientationX, m_axisX->type());
        m_changeTracker.axisXTypeChanged = false;
    }

    if (m_changeTracker.axisYTypeChanged) {
        m_renderer->updateAxisType(Q3DAbstractAxis::AxisOrientationY, m_axisY->type());
        m_changeTracker.axisYTypeChanged = false;
    }

    if (m_changeTracker.axisZTypeChanged) {
        m_renderer->updateAxisType(Q3DAbstractAxis::AxisOrientationZ, m_axisZ->type());
        m_changeTracker.axisZTypeChanged = false;
    }

    if (m_changeTracker.axisXTitleChanged) {
        m_renderer->updateAxisTitle(Q3DAbstractAxis::AxisOrientationX, m_axisX->title());
        m_changeTracker.axisXTitleChanged = false;
    }

    if (m_changeTracker.axisYTitleChanged) {
        m_renderer->updateAxisTitle(Q3DAbstractAxis::AxisOrientationY, m_axisY->title());
        m_changeTracker.axisYTitleChanged = false;
    }

    if (m_changeTracker.axisZTitleChanged) {
        m_renderer->updateAxisTitle(Q3DAbstractAxis::AxisOrientationZ, m_axisZ->title());
        m_changeTracker.axisZTitleChanged = false;
    }

    if (m_changeTracker.axisXLabelsChanged) {
        m_renderer->updateAxisLabels(Q3DAbstractAxis::AxisOrientationX, m_axisX->labels());
        m_changeTracker.axisXLabelsChanged = false;
    }

    if (m_changeTracker.axisYLabelsChanged) {
        m_renderer->updateAxisLabels(Q3DAbstractAxis::AxisOrientationY, m_axisY->labels());
        m_changeTracker.axisYLabelsChanged = false;
    }
    if (m_changeTracker.axisZLabelsChanged) {
        m_renderer->updateAxisLabels(Q3DAbstractAxis::AxisOrientationZ, m_axisZ->labels());
        m_changeTracker.axisZLabelsChanged = false;
    }

    if (m_changeTracker.axisXRangeChanged) {
        m_renderer->updateAxisRange(Q3DAbstractAxis::AxisOrientationX, m_axisX->min(),
                                    m_axisX->max());
        m_changeTracker.axisXRangeChanged = false;
    }

    if (m_changeTracker.axisYRangeChanged) {
        m_renderer->updateAxisRange(Q3DAbstractAxis::AxisOrientationY, m_axisY->min(),
                                    m_axisY->max());
        m_changeTracker.axisYRangeChanged = false;
    }

    if (m_changeTracker.axisZRangeChanged) {
        m_renderer->updateAxisRange(Q3DAbstractAxis::AxisOrientationZ, m_axisZ->min(),
                                    m_axisZ->max());
        m_changeTracker.axisZRangeChanged = false;
    }

    if (m_changeTracker.axisXSegmentCountChanged) {
        m_changeTracker.axisXSegmentCountChanged = false;
        if (m_axisX->type() & Q3DAbstractAxis::AxisTypeValue) {
            Q3DValueAxis *valueAxisX = static_cast<Q3DValueAxis *>(m_axisX);
            m_renderer->updateAxisSegmentCount(Q3DAbstractAxis::AxisOrientationX,
                                               valueAxisX->segmentCount());
        }
    }

    if (m_changeTracker.axisYSegmentCountChanged) {
        m_changeTracker.axisYSegmentCountChanged = false;
        if (m_axisY->type() & Q3DAbstractAxis::AxisTypeValue) {
            Q3DValueAxis *valueAxisY = static_cast<Q3DValueAxis *>(m_axisY);
            m_renderer->updateAxisSegmentCount(Q3DAbstractAxis::AxisOrientationY,
                                               valueAxisY->segmentCount());
        }
    }

    if (m_changeTracker.axisZSegmentCountChanged) {
        m_changeTracker.axisZSegmentCountChanged = false;
        if (m_axisZ->type() & Q3DAbstractAxis::AxisTypeValue) {
            Q3DValueAxis *valueAxisZ = static_cast<Q3DValueAxis *>(m_axisZ);
            m_renderer->updateAxisSegmentCount(Q3DAbstractAxis::AxisOrientationZ,
                                               valueAxisZ->segmentCount());
        }
    }

    if (m_changeTracker.axisXSubSegmentCountChanged) {
        m_changeTracker.axisXSubSegmentCountChanged = false;
        if (m_axisX->type() & Q3DAbstractAxis::AxisTypeValue) {
            Q3DValueAxis *valueAxisX = static_cast<Q3DValueAxis *>(m_axisX);
            m_renderer->updateAxisSubSegmentCount(Q3DAbstractAxis::AxisOrientationX,
                                                  valueAxisX->subSegmentCount());
        }
    }

    if (m_changeTracker.axisYSubSegmentCountChanged) {
        m_changeTracker.axisYSubSegmentCountChanged = false;
        if (m_axisY->type() & Q3DAbstractAxis::AxisTypeValue) {
            Q3DValueAxis *valueAxisY = static_cast<Q3DValueAxis *>(m_axisY);
            m_renderer->updateAxisSubSegmentCount(Q3DAbstractAxis::AxisOrientationY,
                                                  valueAxisY->subSegmentCount());
        }
    }

    if (m_changeTracker.axisZSubSegmentCountChanged) {
        m_changeTracker.axisZSubSegmentCountChanged = false;
        if (m_axisZ->type() & Q3DAbstractAxis::AxisTypeValue) {
            Q3DValueAxis *valueAxisZ = static_cast<Q3DValueAxis *>(m_axisZ);
            m_renderer->updateAxisSubSegmentCount(Q3DAbstractAxis::AxisOrientationZ,
                                                  valueAxisZ->subSegmentCount());
        }
    }

    if (m_changeTracker.axisXLabelFormatChanged) {
        m_changeTracker.axisXLabelFormatChanged = false;
        if (m_axisX->type() & Q3DAbstractAxis::AxisTypeValue) {
            Q3DValueAxis *valueAxisX = static_cast<Q3DValueAxis *>(m_axisX);
            m_renderer->updateAxisLabelFormat(Q3DAbstractAxis::AxisOrientationX,
                                              valueAxisX->labelFormat());
        }
    }

    if (m_changeTracker.axisYLabelFormatChanged) {
        m_changeTracker.axisYLabelFormatChanged = false;
        if (m_axisY->type() & Q3DAbstractAxis::AxisTypeValue) {
            Q3DValueAxis *valueAxisY = static_cast<Q3DValueAxis *>(m_axisY);
            m_renderer->updateAxisLabelFormat(Q3DAbstractAxis::AxisOrientationY,
                                              valueAxisY->labelFormat());
        }
    }

    if (m_changeTracker.axisZLabelFormatChanged) {
        m_changeTracker.axisZLabelFormatChanged = false;
        if (m_axisZ->type() & Q3DAbstractAxis::AxisTypeValue) {
            Q3DValueAxis *valueAxisZ = static_cast<Q3DValueAxis *>(m_axisZ);
            m_renderer->updateAxisLabelFormat(Q3DAbstractAxis::AxisOrientationZ,
                                              valueAxisZ->labelFormat());
        }
    }
}

void Abstract3DController::render(const GLuint defaultFboHandle)
{
    m_renderPending = false;

    // If not initialized, do nothing.
    if (!m_renderer)
        return;

    m_renderer->render(defaultFboHandle);

#ifdef DISPLAY_RENDER_SPEED
    // To get meaningful framerate, don't just do render on demand.
    emitNeedRender();
#endif
}

void Abstract3DController::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_activeInputHandler)
        m_activeInputHandler->mouseDoubleClickEvent(event);

    emitNeedRender();
}

void Abstract3DController::touchEvent(QTouchEvent *event)
{
    if (m_activeInputHandler)
        m_activeInputHandler->touchEvent(event);

    emitNeedRender();
}

void Abstract3DController::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    if (m_activeInputHandler)
        m_activeInputHandler->mousePressEvent(event, mousePos);

    emitNeedRender();
}

void Abstract3DController::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    if (m_activeInputHandler)
        m_activeInputHandler->mouseReleaseEvent(event, mousePos);

    emitNeedRender();
}

void Abstract3DController::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    if (m_activeInputHandler)
        m_activeInputHandler->mouseMoveEvent(event, mousePos);

    emitNeedRender();
}

void Abstract3DController::wheelEvent(QWheelEvent *event)
{
    if (m_activeInputHandler)
        m_activeInputHandler->wheelEvent(event);

    emitNeedRender();
}

void Abstract3DController::setSize(const int width, const int height)
{
    m_boundingRect.setWidth(width);
    m_boundingRect.setHeight(height);
    m_scene->setViewportSize(width, height);

    m_changeTracker.boundingRectChanged = true;
    emitNeedRender();
}

const QSize Abstract3DController::size()
{
    return m_boundingRect.size();
}

const QRect Abstract3DController::boundingRect()
{
    return m_boundingRect;
}

void Abstract3DController::setBoundingRect(const QRect boundingRect)
{
    m_boundingRect = boundingRect;
    m_scene->setViewport(boundingRect);

    m_changeTracker.boundingRectChanged = true;
    emitNeedRender();
}

void Abstract3DController::setWidth(const int width)
{
    m_boundingRect.setWidth(width);
    m_scene->setViewportSize(width, m_scene->viewport().height());

    m_changeTracker.sizeChanged = true;
    emitNeedRender();
}

int Abstract3DController::width()
{
    return m_boundingRect.width();
}

void Abstract3DController::setHeight(const int height)
{
    m_boundingRect.setHeight(height);
    m_scene->setViewportSize(m_scene->viewport().width(), height);

    m_changeTracker.sizeChanged = true;
    emitNeedRender();
}

int Abstract3DController::height()
{
    return m_boundingRect.height();
}

void Abstract3DController::setX(const int x)
{
    m_boundingRect.setX(x);

    m_changeTracker.positionChanged = true;
    emitNeedRender();
}

int Abstract3DController::x()
{
    return m_boundingRect.x();
}

void Abstract3DController::setY(const int y)
{
    m_boundingRect.setY(y);

    m_changeTracker.positionChanged = true;
    emitNeedRender();
}

int Abstract3DController::y()
{
    return m_boundingRect.y();
}

QRect Abstract3DController::primarySubViewport() const
{
    return m_scene->primarySubViewport();
}

void Abstract3DController::setPrimarySubViewport(const QRect &primarySubViewport)
{
    m_scene->setPrimarySubViewport(primarySubViewport);
}

QRect Abstract3DController::secondarySubViewport() const
{
    return m_scene->secondarySubViewport();
}

void Abstract3DController::setSecondarySubViewport(const QRect &secondarySubViewport)
{
    m_scene->setSecondarySubViewport(secondarySubViewport);
}

void Abstract3DController::updateDevicePixelRatio(qreal ratio)
{
    m_scene->setDevicePixelRatio(ratio);
}

void Abstract3DController::setAxisX(Q3DAbstractAxis *axis)
{
    setAxisHelper(Q3DAbstractAxis::AxisOrientationX, axis, &m_axisX);
}

Q3DAbstractAxis *Abstract3DController::axisX()
{
    return m_axisX;
}

void Abstract3DController::setAxisY(Q3DAbstractAxis *axis)
{
    setAxisHelper(Q3DAbstractAxis::AxisOrientationY, axis, &m_axisY);
}

Q3DAbstractAxis *Abstract3DController::axisY()
{
    return m_axisY;
}

void Abstract3DController::setAxisZ(Q3DAbstractAxis *axis)
{
    setAxisHelper(Q3DAbstractAxis::AxisOrientationZ, axis, &m_axisZ);
}

Q3DAbstractAxis *Abstract3DController::axisZ()
{
    return m_axisZ;
}

void Abstract3DController::addAxis(Q3DAbstractAxis *axis)
{
    Q_ASSERT(axis);
    Abstract3DController *owner = qobject_cast<Abstract3DController *>(axis->parent());
    if (owner != this) {
        Q_ASSERT_X(!owner, "addAxis", "Axis already attached to a graph.");
        axis->setParent(this);
    }
    if (!m_axes.contains(axis))
        m_axes.append(axis);
}

void Abstract3DController::releaseAxis(Q3DAbstractAxis *axis)
{
    if (axis && m_axes.contains(axis)) {
        // Clear the default status from released default axes
        if (axis->d_ptr->isDefaultAxis())
            axis->d_ptr->setDefaultAxis(false);

        // If the axis is in use, replace it with a temporary one
        switch (axis->orientation()) {
        case Q3DAbstractAxis::AxisOrientationX:
            setAxisX(0);
            break;
        case Q3DAbstractAxis::AxisOrientationY:
            setAxisY(0);
            break;
        case Q3DAbstractAxis::AxisOrientationZ:
            setAxisZ(0);
            break;
        default:
            break;
        }

        m_axes.removeAll(axis);
        axis->setParent(0);
    }
}

QList<Q3DAbstractAxis *> Abstract3DController::axes() const
{
    return m_axes;
}

QAbstractDataProxy *Abstract3DController::activeDataProxy() const
{
    return m_data;
}

void Abstract3DController::addDataProxy(QAbstractDataProxy *proxy)
{
    Q_ASSERT(proxy);
    Abstract3DController *owner = qobject_cast<Abstract3DController *>(proxy->parent());
    if (owner != this) {
        Q_ASSERT_X(!owner, "addDataProxy", "Proxy already attached to a graph.");
        proxy->setParent(this);
    }
    if (!m_dataProxies.contains(proxy))
        m_dataProxies.append(proxy);
}

void Abstract3DController::releaseDataProxy(QAbstractDataProxy *proxy)
{
    if (proxy && m_dataProxies.contains(proxy)) {
        // Clear the default status from released default proxies
        if (proxy->d_ptr->isDefaultProxy())
            proxy->d_ptr->setDefaultProxy(false);

        // If the proxy is in use, replace it with a temporary one
        if (m_data == proxy)
            setActiveDataProxy(0);

        m_dataProxies.removeAll(proxy);
        proxy->setParent(0);
    }
}

QList<QAbstractDataProxy *> Abstract3DController::dataProxies() const
{
    return m_dataProxies;
}

void Abstract3DController::setActiveDataProxy(QAbstractDataProxy *proxy)
{
    // If existing proxy is the default proxy, delete it
    if (m_data) {
        if (m_data->d_ptr->isDefaultProxy()) {
            m_dataProxies.removeAll(m_data);
            delete m_data;
        } else {
            // Disconnect the old proxy from use
            QObject::disconnect(m_data, 0, this, 0);
        }
    }

    // Assume ownership and activate
    addDataProxy(proxy);
    m_data = proxy;
    m_isDataDirty = true;
    emitNeedRender();
}

void Abstract3DController::addInputHandler(QAbstract3DInputHandler *inputHandler)
{
    Q_ASSERT(inputHandler);
    Abstract3DController *owner = qobject_cast<Abstract3DController *>(inputHandler->parent());
    if (owner != this) {
        Q_ASSERT_X(!owner, "addInputHandler", "Input handler already attached to another component.");
        inputHandler->setParent(this);
    }

    if (!m_inputHandlers.contains(inputHandler))
        m_inputHandlers.append(inputHandler);
}

void Abstract3DController::releaseInputHandler(QAbstract3DInputHandler *inputHandler)
{
    if (inputHandler && m_inputHandlers.contains(inputHandler)) {
        // Clear the default status from released default input handler
        if (inputHandler->d_ptr->m_isDefaultHandler)
            inputHandler->d_ptr->m_isDefaultHandler = false;

        // If the input handler is in use, remove it
        if (m_activeInputHandler == inputHandler)
            setActiveInputHandler(0);

        m_inputHandlers.removeAll(inputHandler);
        inputHandler->setParent(0);
    }
}

void Abstract3DController::setActiveInputHandler(QAbstract3DInputHandler *inputHandler)
{
    if (inputHandler == m_activeInputHandler)
        return;

    // If existing input handler is the default input handler, delete it
    if (m_activeInputHandler) {
        if (m_activeInputHandler->d_ptr->m_isDefaultHandler) {
            m_inputHandlers.removeAll(m_activeInputHandler);
            delete m_activeInputHandler;
        } else {
            // Disconnect the old input handler from the scene
            m_activeInputHandler->setScene(0);
        }
    }

    // Assume ownership and connect to this controller's scene
    if (inputHandler)
        addInputHandler(inputHandler);

    m_activeInputHandler = inputHandler;
    if (m_activeInputHandler)
        m_activeInputHandler->setScene(m_scene);

    // Notify change of input handler
    emit activeInputHandlerChanged(m_activeInputHandler);
}

QAbstract3DInputHandler* Abstract3DController::activeInputHandler()
{
    return m_activeInputHandler;
}

int Abstract3DController::zoomLevel()
{
    return m_scene->activeCamera()->zoomLevel();
}

void Abstract3DController::setZoomLevel(int zoomLevel)
{
    m_scene->activeCamera()->setZoomLevel(zoomLevel);

    m_changeTracker.zoomLevelChanged = true;
    emitNeedRender();
}

void Abstract3DController::setObjectColor(const QColor &baseColor, bool uniform)
{
    m_theme.m_baseColor = baseColor;
    m_theme.m_uniformColor = uniform;

    m_changeTracker.themeChanged = true;
    emitNeedRender();
}

QColor Abstract3DController::objectColor() const
{
    return m_theme.m_baseColor;
}

void Abstract3DController::setTheme(QDataVis::Theme theme)
{
    m_theme.useTheme(theme);

    m_changeTracker.themeChanged = true;
    emitNeedRender();
}

Theme Abstract3DController::theme()
{
    return m_theme;
}

void Abstract3DController::setFont(const QFont &font)
{
    m_font = font;

    m_changeTracker.fontChanged = true;
    emitNeedRender();
}

QFont Abstract3DController::font()
{
    return m_font;
}

void Abstract3DController::setSelectionMode(QDataVis::SelectionFlags mode)
{
    if (mode != m_selectionMode) {
        m_selectionMode = mode;
        m_changeTracker.selectionModeChanged = true;
        emitNeedRender();
    }
}

QDataVis::SelectionFlags Abstract3DController::selectionMode()
{
    return m_selectionMode;
}

void Abstract3DController::setShadowQuality(QDataVis::ShadowQuality quality)
{
    m_shadowQuality = quality;

    m_changeTracker.shadowQualityChanged = true;
    emit shadowQualityChanged(m_shadowQuality);
    emitNeedRender();
}

QDataVis::ShadowQuality Abstract3DController::shadowQuality()
{
    return m_shadowQuality;
}

void Abstract3DController::setLabelStyle(QDataVis::LabelStyle style)
{
    m_labelStyle = style;

    m_changeTracker.labelStyleChanged = true;
    emitNeedRender();
}

QDataVis::LabelStyle Abstract3DController::labelStyle()
{
    return m_labelStyle;
}

void Abstract3DController::setBackgroundEnabled(bool enable)
{
    m_isBackgroundEnabled = enable;
    m_changeTracker.backgroundEnabledChanged = true;
    emitNeedRender();
}

bool Abstract3DController::backgroundEnabled()
{
    return m_isBackgroundEnabled;
}

void Abstract3DController::setGridEnabled(bool enable)
{
    m_isGridEnabled = enable;
    m_changeTracker.gridEnabledChanged = true;
    emitNeedRender();
}

bool Abstract3DController::gridEnabled()
{
    return m_isGridEnabled;
}

bool Abstract3DController::isSlicingActive()
{
    return m_scene->isSlicingActive();
}

void Abstract3DController::setSlicingActive(bool isSlicing)
{
    m_scene->setSlicingActive(isSlicing);
    emitNeedRender();
}

QDataVis::InputState Abstract3DController::inputState()
{
    if (m_activeInputHandler)
        return m_activeInputHandler->inputState();
    else
        return QDataVis::InputStateNone;
}

QPoint Abstract3DController::inputPosition()
{
    if (m_activeInputHandler)
        return m_activeInputHandler->inputPosition();
    else
        return QPoint(0, 0);
}

void Abstract3DController::setMeshFileName(const QString &fileName)
{
    m_objFile = fileName;
    m_changeTracker.objFileChanged = true;
    emitNeedRender();
}

QString Abstract3DController::meshFileName()
{
    return m_objFile;
}

Q3DScene *Abstract3DController::scene()
{
    return m_scene;
}

void Abstract3DController::handleAxisTitleChanged(const QString &title)
{
    Q_UNUSED(title)
    handleAxisTitleChangedBySender(sender());
}

void Abstract3DController::handleAxisTitleChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXTitleChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYTitleChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZTitleChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    emitNeedRender();
}

void Abstract3DController::handleAxisLabelsChanged()
{
    handleAxisLabelsChangedBySender(sender());
}

void Abstract3DController::handleAxisLabelsChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXLabelsChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYLabelsChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZLabelsChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    emitNeedRender();
}

void Abstract3DController::handleAxisRangeChanged(qreal min, qreal max)
{
    Q_UNUSED(min)
    Q_UNUSED(max)
    handleAxisRangeChangedBySender(sender());
}

void Abstract3DController::handleAxisRangeChangedBySender(QObject *sender)
{
    if (sender == m_axisX) {
        m_isDataDirty = true;
        m_changeTracker.axisXRangeChanged = true;
    } else if (sender == m_axisY) {
        m_isDataDirty = true;
        m_changeTracker.axisYRangeChanged = true;
    } else if (sender == m_axisZ) {
        m_isDataDirty = true;
        m_changeTracker.axisZRangeChanged = true;
    } else {
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    }
    emitNeedRender();
}

void Abstract3DController::handleAxisSegmentCountChanged(int count)
{
    Q_UNUSED(count)
    handleAxisSegmentCountChangedBySender(sender());
}

void Abstract3DController::handleAxisSegmentCountChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXSegmentCountChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYSegmentCountChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZSegmentCountChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    emitNeedRender();
}

void Abstract3DController::handleAxisSubSegmentCountChanged(int count)
{
    Q_UNUSED(count)
    handleAxisSubSegmentCountChangedBySender(sender());
}

void Abstract3DController::handleAxisSubSegmentCountChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXSubSegmentCountChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYSubSegmentCountChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZSubSegmentCountChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    emitNeedRender();
}

void Abstract3DController::handleAxisAutoAdjustRangeChanged(bool autoAdjust)
{
    QObject *sender = QObject::sender();
    if (sender != m_axisX && sender != m_axisY && sender != m_axisZ)
        return;

    Q3DAbstractAxis *axis = static_cast<Q3DAbstractAxis*>(sender);
    handleAxisAutoAdjustRangeChangedInOrientation(axis->orientation(), autoAdjust);
}

void Abstract3DController::handleAxisLabelFormatChanged(const QString &format)
{
    Q_UNUSED(format)
    handleAxisLabelFormatChangedBySender(sender());
}

void Abstract3DController::handleInputStateChanged(QDataVis::InputState state)
{
    // When in automatic slicing mode, input state change to overview disables slice mode
    if (m_selectionMode.testFlag(QDataVis::SelectionSlice)
            && state == QDataVis::InputStateOnOverview) {
        setSlicingActive(false);
    }

    m_changeTracker.inputStateChanged = true;
    emitNeedRender();
}

void Abstract3DController::handleInputPositionChanged(const QPoint &position)
{
    Q_UNUSED(position)

    m_changeTracker.inputPositionChanged = true;
    emitNeedRender();
}

void Abstract3DController::handleRequestShadowQuality(QDataVis::ShadowQuality quality)
{
    setShadowQuality(quality);
}

void Abstract3DController::handleAxisLabelFormatChangedBySender(QObject *sender)
{
    // Label format changing needs to dirty the data so that labels are reset.
    if (sender == m_axisX) {
        m_isDataDirty = true;
        m_changeTracker.axisXLabelFormatChanged = true;
    } else if (sender == m_axisY) {
        m_isDataDirty = true;
        m_changeTracker.axisYLabelFormatChanged = true;
    } else if (sender == m_axisZ) {
        m_isDataDirty = true;
        m_changeTracker.axisZLabelFormatChanged = true;
    } else {
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    }
    emitNeedRender();
}

void Abstract3DController::setAxisHelper(Q3DAbstractAxis::AxisOrientation orientation,
                                         Q3DAbstractAxis *axis, Q3DAbstractAxis **axisPtr)
{
    // Setting null axis indicates using default axis
    if (!axis)
        axis = createDefaultAxis(orientation);

    // If old axis is default axis, delete it
    Q3DAbstractAxis *oldAxis = *axisPtr;
    if (oldAxis) {
        if (oldAxis->d_ptr->isDefaultAxis()) {
            m_axes.removeAll(oldAxis);
            delete oldAxis;
            oldAxis = 0;
        } else {
            // Disconnect the old axis from use
            QObject::disconnect(oldAxis, 0, this, 0);
            oldAxis->d_ptr->setOrientation(Q3DAbstractAxis::AxisOrientationNone);
        }
    }

    // Assume ownership
    addAxis(axis);

    // Connect the new axis
    *axisPtr = axis;

    axis->d_ptr->setOrientation(orientation);

    QObject::connect(axis, &Q3DAbstractAxis::titleChanged,
                     this, &Abstract3DController::handleAxisTitleChanged);
    QObject::connect(axis, &Q3DAbstractAxis::labelsChanged,
                     this, &Abstract3DController::handleAxisLabelsChanged);
    QObject::connect(axis, &Q3DAbstractAxis::rangeChanged,
                     this, &Abstract3DController::handleAxisRangeChanged);
    QObject::connect(axis, &Q3DAbstractAxis::autoAdjustRangeChanged,
                     this, &Abstract3DController::handleAxisAutoAdjustRangeChanged);

    if (orientation == Q3DAbstractAxis::AxisOrientationX)
        m_changeTracker.axisXTypeChanged = true;
    else if (orientation == Q3DAbstractAxis::AxisOrientationY)
        m_changeTracker.axisYTypeChanged = true;
    else if (orientation == Q3DAbstractAxis::AxisOrientationZ)
        m_changeTracker.axisZTypeChanged = true;

    handleAxisTitleChangedBySender(axis);
    handleAxisLabelsChangedBySender(axis);
    handleAxisRangeChangedBySender(axis);
    handleAxisAutoAdjustRangeChangedInOrientation(axis->orientation(),
                                                  axis->isAutoAdjustRange());

    if (axis->type() & Q3DAbstractAxis::AxisTypeValue) {
        Q3DValueAxis *valueAxis = static_cast<Q3DValueAxis *>(axis);
        QObject::connect(valueAxis, &Q3DValueAxis::segmentCountChanged,
                         this, &Abstract3DController::handleAxisSegmentCountChanged);
        QObject::connect(valueAxis, &Q3DValueAxis::subSegmentCountChanged,
                         this, &Abstract3DController::handleAxisSubSegmentCountChanged);
        QObject::connect(valueAxis, &Q3DValueAxis::labelFormatChanged,
                         this, &Abstract3DController::handleAxisLabelFormatChanged);

        handleAxisSegmentCountChangedBySender(valueAxis);
        handleAxisSubSegmentCountChangedBySender(valueAxis);
        handleAxisLabelFormatChangedBySender(valueAxis);
    }
}

Q3DAbstractAxis *Abstract3DController::createDefaultAxis(Q3DAbstractAxis::AxisOrientation orientation)
{
    Q_UNUSED(orientation)

    // The default default axis is a value axis. If the graph type has a different default axis
    // for some orientation, this function needs to be overridden.
    Q3DAbstractAxis *defaultAxis = createDefaultValueAxis();
    return defaultAxis;
}

Q3DValueAxis *Abstract3DController::createDefaultValueAxis()
{
    // Default value axis has single segment, empty label format, and auto scaling
    // TODO: Grid should be also hidden, but that is not currently controlled by axis
    Q3DValueAxis *defaultAxis = new Q3DValueAxis;
    defaultAxis->setSegmentCount(1);
    defaultAxis->setSubSegmentCount(1);
    defaultAxis->setAutoAdjustRange(true);
    defaultAxis->setLabelFormat(QString());
    defaultAxis->d_ptr->setDefaultAxis(true);

    return defaultAxis;
}

Q3DCategoryAxis *Abstract3DController::createDefaultCategoryAxis()
{
    // Default category axis has no labels
    // TODO: Grid should be also hidden, but that is not currently controlled by axis.
    Q3DCategoryAxis *defaultAxis = new Q3DCategoryAxis;
    defaultAxis->setAutoAdjustRange(true);
    defaultAxis->d_ptr->setDefaultAxis(true);
    return defaultAxis;
}

void Abstract3DController::emitNeedRender()
{
    if (!m_renderPending) {
        emit needRender();
        m_renderPending = true;
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE
