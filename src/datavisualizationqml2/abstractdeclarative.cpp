/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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
#include "declarativetheme_p.h"
#include "declarativerendernode_p.h"

#include <QtGui/QGuiApplication>
#if defined(Q_OS_IOS)
#include <QtCore/QTimer>
#endif

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

static QList<const QQuickWindow *> clearList;
static QHash<AbstractDeclarative *, QQuickWindow *> graphWindowList;
static QHash<QQuickWindow *, bool> windowClearList;

AbstractDeclarative::AbstractDeclarative(QQuickItem *parent) :
    QQuickItem(parent),
    m_controller(0),
    m_contextWindow(0),
    m_renderMode(RenderIndirect),
    #if defined(QT_OPENGL_ES_2)
    m_samples(0),
    #else
    m_samples(4),
    #endif
    m_windowSamples(0),
    m_initialisedSize(0, 0),
    #ifdef USE_SHARED_CONTEXT
    m_context(0),
    #else
    m_stateStore(0),
    #endif
    m_qtContext(0),
    m_mainThread(QThread::currentThread()),
    m_contextThread(0)
{
    connect(this, &QQuickItem::windowChanged, this, &AbstractDeclarative::handleWindowChanged);
    setAntialiasing(m_samples > 0);

    // Set contents to false in case we are in qml designer to make component look nice
    m_runningInDesigner = QGuiApplication::applicationDisplayName() == "Qml2Puppet";
    setFlag(ItemHasContents, !m_runningInDesigner);
}

AbstractDeclarative::~AbstractDeclarative()
{
#ifdef USE_SHARED_CONTEXT
    // Context can be in another thread, don't delete it directly in that case
    if (m_contextThread && m_contextThread != m_mainThread) {
        if (m_context)
            m_context->deleteLater();
        m_context = 0;
    } else {
        delete m_context;
    }
#else
    if (m_contextThread && m_contextThread != m_mainThread) {
        if (m_stateStore)
            m_stateStore->deleteLater();
        m_stateStore = 0;
    } else {
        delete m_stateStore;
    }
#endif

    disconnect(this, 0, this, 0);
    checkWindowList(0);
}

void AbstractDeclarative::setRenderingMode(AbstractDeclarative::RenderingMode mode)
{
    if (mode == m_renderMode)
        return;

    RenderingMode previousMode = m_renderMode;

    m_renderMode = mode;

    QQuickWindow *win = window();

    switch (mode) {
    case RenderDirectToBackground:
        // Intentional flowthrough
    case RenderDirectToBackground_NoClear:
        m_initialisedSize = QSize(0, 0);
        if (previousMode == RenderIndirect) {
            update();
            setFlag(ItemHasContents, false);
            if (win) {
                QObject::connect(win, &QQuickWindow::beforeRendering, this,
                                 &AbstractDeclarative::render, Qt::DirectConnection);
                checkWindowList(win);
                setAntialiasing(m_windowSamples > 0);
                if (m_windowSamples != m_samples)
                    emit msaaSamplesChanged(m_windowSamples);
            }
        }
        break;
    case RenderIndirect:
        m_initialisedSize = QSize(0, 0);
        setFlag(ItemHasContents, !m_runningInDesigner);
        update();
        if (win) {
            QObject::disconnect(win, &QQuickWindow::beforeRendering, this,
                                &AbstractDeclarative::render);
            checkWindowList(win);
        }
        setAntialiasing(m_samples > 0);
        if (m_windowSamples != m_samples)
            emit msaaSamplesChanged(m_samples);
        break;
    }

    updateWindowParameters();

    emit renderingModeChanged(mode);
}

AbstractDeclarative::RenderingMode AbstractDeclarative::renderingMode() const
{
    return m_renderMode;
}

QSGNode *AbstractDeclarative::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QSize boundingSize = boundingRect().size().toSize() * m_controller->scene()->devicePixelRatio();
    if (m_runningInDesigner || boundingSize.width() <= 0 || boundingSize.height() <= 0
            || m_controller.isNull() || !window()) {
        delete oldNode;
        return 0;
    }
    DeclarativeRenderNode *node = static_cast<DeclarativeRenderNode *>(oldNode);

    if (!node) {
        node = new DeclarativeRenderNode(this);
        node->setController(m_controller.data());
        node->setQuickWindow(window());
    }

    node->setSize(boundingSize);
    node->setSamples(m_samples);
    node->update();
    node->markDirty(QSGNode::DirtyMaterial);

    return node;
}

Declarative3DScene* AbstractDeclarative::scene() const
{
    return static_cast<Declarative3DScene *>(m_controller->scene());
}

void AbstractDeclarative::setTheme(Q3DTheme *theme)
{
    m_controller->setActiveTheme(theme, isComponentComplete());
}

Q3DTheme *AbstractDeclarative::theme() const
{
    return m_controller->activeTheme();
}

void AbstractDeclarative::clearSelection()
{
    m_controller->clearSelection();
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

bool AbstractDeclarative::shadowsSupported() const
{
    return m_controller->shadowsSupported();
}

int AbstractDeclarative::addCustomItem(const QString &meshFile, const QVector3D &position,
                                       const QVector3D &scaling, const QQuaternion &rotation,
                                       const QString &textureFile)
{
    QImage textureImage;
    if (!textureFile.isNull())
        textureImage = QImage(textureFile);
    return m_controller->addCustomItem(meshFile, position, scaling, rotation, textureImage);
}

void AbstractDeclarative::removeCustomItemAt(int index)
{
    m_controller->deleteCustomItem(index);
}

void AbstractDeclarative::removeCustomItemAt(const QVector3D &position)
{
    m_controller->deleteCustomItem(position);
}

void AbstractDeclarative::setSharedController(Abstract3DController *controller)
{
    Q_ASSERT(controller);
    m_controller = controller;

    // Reset default theme, as the default C++ theme is Q3DTheme, not DeclarativeTheme3D.
    DeclarativeTheme3D *defaultTheme = new DeclarativeTheme3D;
    defaultTheme->d_ptr->setDefaultTheme(true);
    defaultTheme->setType(Q3DTheme::ThemeQt);
    m_controller->setActiveTheme(defaultTheme);

    QObject::connect(m_controller.data(), &Abstract3DController::shadowQualityChanged, this,
                     &AbstractDeclarative::handleShadowQualityChange);
    QObject::connect(m_controller.data(), &Abstract3DController::activeInputHandlerChanged, this,
                     &AbstractDeclarative::inputHandlerChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::activeThemeChanged, this,
                     &AbstractDeclarative::themeChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::selectionModeChanged, this,
                     &AbstractDeclarative::handleSelectionModeChange);

    QObject::connect(m_controller.data(), &Abstract3DController::axisXChanged, this,
                     &AbstractDeclarative::handleAxisXChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::axisYChanged, this,
                     &AbstractDeclarative::handleAxisYChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::axisZChanged, this,
                     &AbstractDeclarative::handleAxisZChanged);

    QObject::connect(m_controller.data(), &Abstract3DController::measureFpsChanged, this,
                     &AbstractDeclarative::measureFpsChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::currentFpsChanged, this,
                     &AbstractDeclarative::currentFpsChanged);
}

void AbstractDeclarative::activateOpenGLContext(QQuickWindow *window)
{
#ifdef USE_SHARED_CONTEXT
    // We can assume we are not in middle of AbstractDeclarative destructor when we are here,
    // since m_context creation is always done when this function is called from
    // synchDataToRenderer(), which blocks main thread -> no need to mutex.
    if (!m_context || !m_qtContext || m_contextWindow != window) {
        QOpenGLContext *currentContext = QOpenGLContext::currentContext();

        // Note: Changing graph to different window when using multithreaded renderer will break!

        delete m_context;

        m_contextThread = QThread::currentThread();
        m_contextWindow = window;
        m_qtContext = currentContext;

        m_context = new QOpenGLContext();
        m_context->setFormat(m_qtContext->format());
        m_context->setShareContext(m_qtContext);
        m_context->create();

        m_context->makeCurrent(window);
        m_controller->initializeOpenGL();
    } else {
        m_context->makeCurrent(window);
    }
#else
    // Shared contexts don't work properly in some platforms, so just store the
    // context state on those
    if (!m_stateStore || !m_qtContext || m_contextWindow != window) {
        QOpenGLContext *currentContext = QOpenGLContext::currentContext();

        // Note: Changing graph to different window when using multithreaded renderer will break!

        delete m_stateStore;

        m_contextThread = QThread::currentThread();
        m_contextWindow = window;
        m_qtContext = currentContext;

        m_stateStore = new GLStateStore(QOpenGLContext::currentContext());

        m_stateStore->storeGLState();
        m_controller->initializeOpenGL();
    } else {
        m_stateStore->storeGLState();
    }
#endif
}

void AbstractDeclarative::doneOpenGLContext(QQuickWindow *window)
{
#ifdef USE_SHARED_CONTEXT
    m_qtContext->makeCurrent(window);
#else
    Q_UNUSED(window)
    m_stateStore->restoreGLState();
#endif
}

void AbstractDeclarative::synchDataToRenderer()
{
    if (m_renderMode == RenderDirectToBackground && clearList.size())
        clearList.clear();

    QQuickWindow *win = window();
    activateOpenGLContext(win);
    m_controller->synchDataToRenderer();
    doneOpenGLContext(win);
}

int AbstractDeclarative::msaaSamples() const
{
    if (m_renderMode == RenderIndirect)
        return m_samples;
    else
        return m_windowSamples;
}

void AbstractDeclarative::setMsaaSamples(int samples)
{
    if (m_renderMode != RenderIndirect) {
        qWarning("Multisampling cannot be adjusted in this render mode");
    } else {
#if defined(QT_OPENGL_ES_2)
        if (samples > 0)
            qWarning("Multisampling is not supported in OpenGL ES2");
#else
        if (m_samples != samples) {
            m_samples = samples;
            setAntialiasing(m_samples > 0);
            emit msaaSamplesChanged(samples);
            update();
        }
#endif
    }
}

void AbstractDeclarative::handleWindowChanged(QQuickWindow *window)
{
    checkWindowList(window);

    if (!window)
        return;

    connect(window, &QObject::destroyed, this, &AbstractDeclarative::windowDestroyed);

    int oldWindowSamples = m_windowSamples;
    m_windowSamples = window->format().samples();
    if (m_windowSamples < 0)
        m_windowSamples = 0;

    connect(window, &QQuickWindow::beforeSynchronizing,
            this, &AbstractDeclarative::synchDataToRenderer,
            Qt::DirectConnection);

    if (m_renderMode == RenderDirectToBackground_NoClear
            || m_renderMode == RenderDirectToBackground) {
        connect(window, &QQuickWindow::beforeRendering, this, &AbstractDeclarative::render,
                Qt::DirectConnection);
        setAntialiasing(m_windowSamples > 0);
        if (m_windowSamples != oldWindowSamples)
            emit msaaSamplesChanged(m_windowSamples);
    }

    connect(m_controller.data(), &Abstract3DController::needRender, window, &QQuickWindow::update);

    updateWindowParameters();

#if defined(Q_OS_IOS)
    // Scenegraph render cycle in iOS sometimes misses update after beforeSynchronizing signal.
    // This ensures we don't end up displaying the graph without any data, in case update is
    // skipped after synchDataToRenderer.
    QTimer::singleShot(0, window, SLOT(update()));
#endif
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
    if (win && !m_controller.isNull()) {
        Q3DScene *scene = m_controller->scene();
        if (win->devicePixelRatio() != scene->devicePixelRatio()) {
            scene->setDevicePixelRatio(win->devicePixelRatio());
            win->update();
        }

        bool directRender = m_renderMode == RenderDirectToBackground
                || m_renderMode == RenderDirectToBackground_NoClear;
        QSize windowSize;

        if (directRender)
            windowSize = win->size();
        else
            windowSize = m_cachedGeometry.size().toSize();

        if (windowSize != scene->d_ptr->windowSize()) {
            scene->d_ptr->setWindowSize(windowSize);
            win->update();
        }

        if (directRender) {
            // Origin mapping is needed when rendering directly to background
            QPointF point = QQuickItem::mapToScene(QPointF(0.0, 0.0));
            scene->d_ptr->setViewport(QRect(point.x() + 0.5f, point.y() + 0.5f,
                                            m_cachedGeometry.width() + 0.5f,
                                            m_cachedGeometry.height() + 0.5f));
        } else {
            // No translation needed when rendering to FBO
            scene->d_ptr->setViewport(QRect(0.0, 0.0, m_cachedGeometry.width() + 0.5f,
                                            m_cachedGeometry.height() + 0.5f));
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

    // If we're not rendering directly to the background, return
    if (m_renderMode != RenderDirectToBackground && m_renderMode != RenderDirectToBackground_NoClear)
        return;

    // Clear the background once per window as that is not done by default
    QQuickWindow *win = window();
    activateOpenGLContext(win);
    if (m_renderMode == RenderDirectToBackground && !clearList.contains(win)) {
        clearList.append(win);
        QColor clearColor = win->color();
        glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    if (isVisible()) {
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDisable(GL_BLEND);

        m_controller->render();

        glEnable(GL_BLEND);
    }
    doneOpenGLContext(win);
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

void AbstractDeclarative::checkWindowList(QQuickWindow *window)
{
    QQuickWindow *oldWindow = graphWindowList.value(this);

    graphWindowList[this] = window;

    if (oldWindow != window && oldWindow) {
        QObject::disconnect(oldWindow, &QObject::destroyed, this,
                            &AbstractDeclarative::windowDestroyed);
        QObject::disconnect(oldWindow, &QQuickWindow::beforeSynchronizing, this,
                            &AbstractDeclarative::synchDataToRenderer);
        QObject::disconnect(oldWindow, &QQuickWindow::beforeRendering, this,
                            &AbstractDeclarative::render);
        if (!m_controller.isNull()) {
            QObject::disconnect(m_controller.data(), &Abstract3DController::needRender,
                                oldWindow, &QQuickWindow::update);
        }
    }

    QList<QQuickWindow *> windowList;

    foreach (AbstractDeclarative *graph, graphWindowList.keys()) {
        if (graph->m_renderMode == RenderDirectToBackground
                || graph->m_renderMode == RenderDirectToBackground_NoClear) {
            windowList.append(graphWindowList.value(graph));
        }
    }

    if (oldWindow && !windowList.contains(oldWindow)
            && windowClearList.values(oldWindow).size() != 0) {
        // Return window clear value
        oldWindow->setClearBeforeRendering(windowClearList.value(oldWindow));
        windowClearList.remove(oldWindow);
    }

    if (!window) {
        graphWindowList.remove(this);
        return;
    }

    if ((m_renderMode == RenderDirectToBackground
         || m_renderMode == RenderDirectToBackground_NoClear)
            && windowClearList.values(window).size() == 0) {
        // Save old clear value
        windowClearList[window] = window->clearBeforeRendering();
        // Disable clearing of the window as we render underneath
        window->setClearBeforeRendering(false);
    }
}

void AbstractDeclarative::setMeasureFps(bool enable)
{
    m_controller->setMeasureFps(enable);
}

bool AbstractDeclarative::measureFps() const
{
    return m_controller->measureFps();
}

qreal AbstractDeclarative::currentFps() const
{
    return m_controller->currentFps();
}

void AbstractDeclarative::windowDestroyed(QObject *obj)
{
    // Remove destroyed window from window lists
    QQuickWindow *win = static_cast<QQuickWindow *>(obj);
    QQuickWindow *oldWindow = graphWindowList.value(this);

    if (win == oldWindow)
        graphWindowList.remove(this);

    windowClearList.remove(win);
}

QT_END_NAMESPACE_DATAVISUALIZATION
