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

#include "abstractdeclarative_p.h"
#include "q3dvalueaxis.h"
#include <QThread>
#include <QGuiApplication>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

AbstractDeclarative::AbstractDeclarative(QQuickItem *parent) :
    QQuickItem(parent),
    m_controller(0),
    m_isFirstRender(true)
{
    connect(this, &QQuickItem::windowChanged, this, &AbstractDeclarative::handleWindowChanged);
    setAntialiasing(true);
}

AbstractDeclarative::~AbstractDeclarative()
{
}

Q3DScene* AbstractDeclarative::scene() const
{
    return m_controller->scene();
}

void AbstractDeclarative::setTheme(Q3DTheme *theme)
{
    m_controller->setTheme(theme);
}

Q3DTheme *AbstractDeclarative::theme() const
{
    return m_controller->theme();
}

void AbstractDeclarative::setSelectionMode(QDataVis::SelectionFlags mode)
{
    m_controller->setSelectionMode(mode);
}

QDataVis::SelectionFlags AbstractDeclarative::selectionMode() const
{
    return m_controller->selectionMode();
}

void AbstractDeclarative::setShadowQuality(QDataVis::ShadowQuality quality)
{
    m_controller->setShadowQuality(quality);
}

QDataVis::ShadowQuality AbstractDeclarative::shadowQuality() const
{
    return m_controller->shadowQuality();
}

void AbstractDeclarative::setSharedController(Abstract3DController *controller)
{
    Q_ASSERT(controller);
    m_controller = controller;
    QObject::connect(m_controller, &Abstract3DController::shadowQualityChanged, this,
                     &AbstractDeclarative::shadowQualityChanged);
    QObject::connect(m_controller, &Abstract3DController::activeInputHandlerChanged, this,
                     &AbstractDeclarative::inputHandlerChanged);
    QObject::connect(m_controller, &Abstract3DController::themeChanged, this,
                     &AbstractDeclarative::themeChanged);
    QObject::connect(m_controller, &Abstract3DController::selectionModeChanged, this,
                     &AbstractDeclarative::selectionModeChanged);
}

void AbstractDeclarative::synchDataToRenderer()
{
    m_controller->initializeOpenGL();
    m_controller->synchDataToRenderer();
}

void AbstractDeclarative::handleWindowChanged(QQuickWindow *window)
{
    if (!window)
        return;

    // Disable clearing of the window as we render underneath
    window->setClearBeforeRendering(false);

    connect(window, &QQuickWindow::beforeSynchronizing, this,
            &AbstractDeclarative::synchDataToRenderer, Qt::DirectConnection);
    connect(window, &QQuickWindow::beforeRendering, this,
            &AbstractDeclarative::render, Qt::DirectConnection);
    connect(m_controller, &Abstract3DController::needRender, window,
            &QQuickWindow::update);

    updateWindowParameters();
}

void AbstractDeclarative::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

    m_cachedGeometry = newGeometry;

    updateWindowParameters();
}

void AbstractDeclarative::itemChange(ItemChange change, const ItemChangeData & value)
{
    QQuickItem::itemChange(change, value);

    updateWindowParameters();
}

void AbstractDeclarative::updateWindowParameters()
{
    // Update the device pixel ratio, window size and bounding box
    QQuickWindow *win = window();
    Q3DScene *scene = m_controller->scene();
    if (win) {
        if (win->devicePixelRatio() != scene->devicePixelRatio())
            scene->setDevicePixelRatio(win->devicePixelRatio());

        if (win->size() != scene->windowSize())
            scene->setWindowSize(QSize(win->width(), win->height()));

        QPointF point = QQuickItem::mapToScene(QPointF(m_cachedGeometry.x(), m_cachedGeometry.y()));
        if (m_controller) {
            scene->setViewport(QRect(point.x(), point.y(), m_cachedGeometry.width(), m_cachedGeometry.height()));
        }

    }
}

void AbstractDeclarative::render()
{
    // Needed to catch the window size change upon first render call
    if (m_isFirstRender) {
        m_isFirstRender = false;
        updateWindowParameters();
    }

    // Clear the background as that is not done by default
    glViewport(0, 0, window()->width(), window()->height());
    QColor clearColor = window()->color();
    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: Store the state of these and restore before returning
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);

    m_controller->render();

    glEnable(GL_BLEND);
}

QAbstract3DInputHandler* AbstractDeclarative::inputHandler() const
{
    return m_controller->activeInputHandler();
}

void AbstractDeclarative::setInputHandler(QAbstract3DInputHandler *inputHandler)
{
    m_controller->setActiveInputHandler(inputHandler);
}

void AbstractDeclarative::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_controller->mouseDoubleClickEvent(event);
}

void AbstractDeclarative::touchEvent(QTouchEvent *event)
{
    m_controller->touchEvent(event);
    update();
}

void AbstractDeclarative::mousePressEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    m_controller->mousePressEvent(event, mousePos);
}

void AbstractDeclarative::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    m_controller->mouseReleaseEvent(event, mousePos);
}

void AbstractDeclarative::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    m_controller->mouseMoveEvent(event, mousePos);
}

void AbstractDeclarative::wheelEvent(QWheelEvent *event)
{
    m_controller->wheelEvent(event);
}

QT_DATAVISUALIZATION_END_NAMESPACE
