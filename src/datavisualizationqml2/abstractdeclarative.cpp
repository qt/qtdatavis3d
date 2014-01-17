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
#include "qvalue3daxis.h"

#include <QThread>
#include <QGuiApplication>
#include <QSGSimpleRectNode>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

static QList<const QQuickWindow *> clearList;

AbstractDeclarative::AbstractDeclarative(QQuickItem *parent) :
    QQuickItem(parent),
    m_controller(0),
    m_clearWindowBeforeRendering(true)
{
    connect(this, &QQuickItem::windowChanged, this, &AbstractDeclarative::handleWindowChanged);
    setAntialiasing(true);
}

AbstractDeclarative::~AbstractDeclarative()
{
}

Declarative3DScene* AbstractDeclarative::scene() const
{
    return static_cast<Declarative3DScene *>(m_controller->scene());
}

void AbstractDeclarative::setTheme(Q3DTheme *theme)
{
    m_controller->setActiveTheme(theme);
}

Q3DTheme *AbstractDeclarative::theme() const
{
    return m_controller->activeTheme();
}

void AbstractDeclarative::clearSelection()
{
    m_controller->clearSelection();
}

void AbstractDeclarative::setClearWindowBeforeRendering(bool enable)
{
    if (m_clearWindowBeforeRendering != enable) {
        m_clearWindowBeforeRendering = enable;
        emit clearWindowBeforeRenderingChanged(enable);
    }
}

bool AbstractDeclarative::clearWindowBeforeRendering() const
{
    return m_clearWindowBeforeRendering;
}

void AbstractDeclarative::setSelectionMode(SelectionFlags mode)
{
    int intmode = int(mode);
    m_controller->setSelectionMode(QAbstract3DGraph::SelectionFlags(intmode));
}

AbstractDeclarative::SelectionFlags AbstractDeclarative::selectionMode() const
{
    int intmode = int(m_controller->selectionMode());
    return SelectionFlags(intmode);
}

void AbstractDeclarative::setShadowQuality(ShadowQuality quality)
{
    m_controller->setShadowQuality(QAbstract3DGraph::ShadowQuality(quality));
}

AbstractDeclarative::ShadowQuality AbstractDeclarative::shadowQuality() const
{
    return ShadowQuality(m_controller->shadowQuality());
}

void AbstractDeclarative::setSharedController(Abstract3DController *controller)
{
    Q_ASSERT(controller);
    m_controller = controller;
    QObject::connect(m_controller, &Abstract3DController::shadowQualityChanged, this,
                     &AbstractDeclarative::handleShadowQualityChange);
    QObject::connect(m_controller, &Abstract3DController::activeInputHandlerChanged, this,
                     &AbstractDeclarative::inputHandlerChanged);
    QObject::connect(m_controller, &Abstract3DController::activeThemeChanged, this,
                     &AbstractDeclarative::themeChanged);
    QObject::connect(m_controller, &Abstract3DController::selectionModeChanged, this,
                     &AbstractDeclarative::handleSelectionModeChange);

    QObject::connect(m_controller, &Abstract3DController::axisXChanged, this,
                     &AbstractDeclarative::handleAxisXChanged);
    QObject::connect(m_controller, &Abstract3DController::axisYChanged, this,
                     &AbstractDeclarative::handleAxisYChanged);
    QObject::connect(m_controller, &Abstract3DController::axisZChanged, this,
                     &AbstractDeclarative::handleAxisZChanged);
}

void AbstractDeclarative::synchDataToRenderer()
{
    if (m_clearWindowBeforeRendering && clearList.size())
        clearList.clear();
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
        if (win->devicePixelRatio() != scene->devicePixelRatio()) {
            scene->setDevicePixelRatio(win->devicePixelRatio());
            win->update();
        }

        if (win->size() != scene->d_ptr->windowSize()) {
            scene->d_ptr->setWindowSize(QSize(win->width(), win->height()));
            win->update();
        }

        QPointF point = QQuickItem::mapToScene(QPointF(0.0f, 0.0f));
        if (m_controller) {
            scene->d_ptr->setViewport(QRect(point.x(), point.y(), m_cachedGeometry.width(),
                                            m_cachedGeometry.height()));
        }
    }
}

void AbstractDeclarative::handleSelectionModeChange(QAbstract3DGraph::SelectionFlags mode)
{
    int intmode = int(mode);
    emit selectionModeChanged(SelectionFlags(intmode));
}

void AbstractDeclarative::handleShadowQualityChange(QAbstract3DGraph::ShadowQuality quality)
{
    emit shadowQualityChanged(ShadowQuality(quality));
}

void AbstractDeclarative::render()
{
    updateWindowParameters();

    // Clear the background once per window as that is not done by default
    const QQuickWindow *win = window();
    if (m_clearWindowBeforeRendering && !clearList.contains(win)) {
        clearList.append(win);
        QColor clearColor = win->color();
        glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

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
    window()->update();
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

QT_END_NAMESPACE_DATAVISUALIZATION
